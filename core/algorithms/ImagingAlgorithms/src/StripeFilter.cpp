//<LICENSE>

#include <sstream>
#include <iostream>

#include <fft/zeropadding.h>
#include <fft/fftbase.h>
#include <base/trotate.h>
#include <base/tpermuteimage.h>
#include <io/io_tiff.h>

#include "../include/StripeFilter.h"
#include "../include/ImagingException.h"


namespace ImagingAlgorithms {


StripeFilter::StripeFilter(size_t const * const dims, const string &wname, int scale, float sigma) :
	logger("StripeFilter"),
	m_wt(wname),
    m_pLine(nullptr),
    m_pCLine(nullptr)
{
    std::copy_n(dims,2,wdims.begin());
    std::fill_n(fft,NLevels,nullptr);
    std::fill_n(m_pDamping,NLevels,nullptr);
    configure(wdims,wname,scale,sigma);
}

/// \brief The constructor initializes the filter
/// \note The filter is initialized for one image size only.
/// \param dims Size of the image to filter
/// \param wname Name of the wavelet base
/// \param scale number of decomosition levels
/// \param sigma High pass cut-off frequency
StripeFilter::StripeFilter(const std::vector<int> &dims, const std::string &wname, int scale, float sigma) :
    logger("StripeFilter"),
    m_wt(wname),
    m_pLine(nullptr),
    m_pCLine(nullptr)
{
    std::fill_n(fft,NLevels,nullptr);
    std::fill_n(m_pDamping,NLevels,nullptr);
    configure(dims,wname,scale,sigma);
}

StripeFilter::~StripeFilter() 
{
    if (m_pDamping[0]!=nullptr)
		for (size_t i=0; i<m_nScale; i++)
			delete [] m_pDamping[i];

    if (m_pLine!=nullptr)
		delete [] m_pLine;

    if (m_pCLine!=nullptr)
		delete [] m_pCLine;

    if (fft[0]!=nullptr) 
    {
		for (size_t i=0; i<m_nScale; i++)
		    delete fft[i];
	}
}


void StripeFilter::CreateFilterWindow()
{
	const float s=-1.0f/(2.0f*m_fSigma*m_fSigma);

	for (size_t j=0; j<m_nScale; j++) 
    {
		const float scale=1.0f/(2.0f*m_nFFTlength[j]);
		for (size_t i=0; i<2*m_nFFTlength[j]; i++) 
        {
			float w=static_cast<float>(i)*scale;
			m_pDamping[j][i]=(1.0f-exp(w*w*s))*scale;
		}
	}
}

void StripeFilter::process(kipl::base::TImage<float,2> &img, eStripeFilterOperation op)
{
	std::ostringstream msg;
   
	try 
    {
 		m_wt.transform(img,m_nScale);
 	}
	catch (kipl::base::KiplException &e) 
    {
		logger(kipl::logging::Logger::LogError,e.what());
        std::cerr<<"Error in the wavelet transform\n";
		throw ImagingException(e.what(),__FILE__,__LINE__);
	}

    size_t level=0;

	for (auto &q : m_wt.data) 
    {
		switch (op) 
        {
		case VerticalComponentZero : VerticalStripesZero(q.v); break;
		case VerticalComponentFFT  : FilterVerticalStripes(q.v, level); break;
		}

        level++;
	}

	img=m_wt.synthesize();
 }

void StripeFilter::FilterVerticalStripes(kipl::base::TImage<float,2> &img, size_t level)
{
	const size_t nLines=img.Size(0);

	float *pData=img.GetDataPtr();

	for (size_t line=0; line<nLines; line++) 
    {
        std::fill_n(m_pLine,2*m_nFFTlength[level],0.0f);
		GetVerticalLine(pData,m_pLine,line,nLines,img.Size(1));

		(*fft[level])(m_pLine,m_pCLine);

		for (size_t i=0; i< 2*m_nFFTlength[level]; i++) 
        {
			m_pCLine[i]*=m_pDamping[level][i];
		}

		(*fft[level])(m_pCLine, m_pLine);
		SetVerticalLine(m_pLine,pData,line,nLines,img.Size(1));
	}
}

void StripeFilter::VerticalStripesZero(kipl::base::TImage<float,2> &img)
{
	img=0.0f;
}

void StripeFilter::GetVerticalLine(float *pSrc, float *pLine, size_t pos, size_t stride, size_t len)
{
	float *pData=pSrc+pos;

	for (size_t i=0; i<len; i++, pData+=stride) 
    {
		pLine[i]=*pData;
	}
}

void StripeFilter::SetVerticalLine(float *pLine, float *pDest, size_t pos, size_t stride, size_t len)
{
	float *pData=pDest+pos;

	for (size_t i=0; i<len; i++, pData+=stride) 
    {
		*pData=pLine[i];
	}
}

std::vector<int> StripeFilter::dims()
{
    return wdims;
}

bool StripeFilter::checkDims(const size_t *dims)
{
    if ((dims[0]!=wdims[0]) || (dims[1]!=wdims[1]))
        throw ImagingException("Image size check failed",__FILE__,__LINE__);

    return true;
}

std::string StripeFilter::waveletName()
{
    return m_wt.Kernel().name();
}

int StripeFilter::decompositionLevels()
{
    return static_cast<int>(m_nScale);
}

float StripeFilter::sigma()
{
    return m_fSigma;
}

std::vector<float> StripeFilter::filterWindow(int level)
{
    std::vector<float> vec(2*m_nFFTlength[level]);

    std::copy_n(m_pDamping[level],vec.size(),vec.begin());

    return vec;
}
void StripeFilter::configure(const std::vector<int> &dims, const string &wname, int scale, float sigma)
{
    if (dims.size()<2)
        throw ImagingException("Stripe filter was configured with too few dimensions.",__FILE__, __LINE__);

    m_wt=kipl::wavelets::WaveletTransform<float>(wname);
    m_fSigma = sigma;
    m_nScale = scale;
    wdims=dims;

    std::ostringstream msg;
    for (int i=0; i<m_nScale; i++) 
    {
        m_nFFTlength[i]=kipl::math::fft::NextPower2(static_cast<size_t>(1.25*dims[1])>>i);
        size_t N=2*m_nFFTlength[i];
        if ( fft[i] != nullptr ) 
            delete fft[i];

        fft[i]=new kipl::math::fft::FFTBaseFloat(&N,1);

        if (m_pDamping[i]!= nullptr)
            delete [] m_pDamping[i];

        m_pDamping[i] = new float[N];
    }

    size_t N=2*m_nFFTlength[0];

    if (m_pLine != nullptr)
        delete [] m_pLine;
    m_pLine    = new float[N];

    if (m_pCLine != nullptr)
        delete [] m_pCLine;
    m_pCLine   = new complex<float>[N];

    CreateFilterWindow();
}

}

std::string enum2string(ImagingAlgorithms::eStripeFilterOperation op)
{
	std::string str;

	switch (op) {
	case ImagingAlgorithms::VerticalComponentZero :  return "verticalzero"; break;
	case ImagingAlgorithms::VerticalComponentFFT  :  return "verticalfft"; break;
	default : throw ImagingException("Unknown stripe filter operation", __FILE__, __LINE__);
	}
	return str;
}

void string2enum(std::string str, ImagingAlgorithms::eStripeFilterOperation &op)
{
    std::ostringstream msg;
    if (str=="verticalzero")
        op=ImagingAlgorithms::VerticalComponentZero;
    else if (str=="verticalfft")
        op=ImagingAlgorithms::VerticalComponentFFT;
    else {
        msg<<"Could not translate string ("<<str<<") to stripe filter operation";
        throw ImagingException(msg.str(), __FILE__, __LINE__);
    }
}

std::ostream & operator<<(std::ostream & s, ImagingAlgorithms::eStripeFilterOperation op)
{
	s<<enum2string(op);

	return s;
}


