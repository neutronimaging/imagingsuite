//<LICENSE>

#include <sstream>
#include <iostream>

#include <io/io_tiff.h>
#include <fft/zeropadding.h>
#include <fft/fftbase.h>
#include <base/trotate.h>
#include <base/tpermuteimage.h>

#include "../include/StripeFilter.h"
#include "../include/ImagingException.h"


namespace ImagingAlgorithms {


StripeFilter::StripeFilter(size_t const * const dims, std::string wname, size_t scale, float sigma) :
	logger("StripeFilter"),
	m_wt(wname),
	m_nScale(scale),
	m_fSigma(sigma),
	m_pLine(NULL),
	m_pCLine(NULL)
{
	std::ostringstream msg;
	for (size_t i=0; i<m_nScale; i++) {
		m_nFFTlength[i]=kipl::math::fft::NextPower2(static_cast<size_t>(1.25*dims[1])>>i);
		size_t N=2*m_nFFTlength[i];
		fft[i]=new kipl::math::fft::FFTBaseFloat(&N,1);
		m_pDamping[i] = new float[N];
	}

	size_t N=2*m_nFFTlength[0];

	m_pLine    = new float[N];
	m_pCLine   = new complex<float>[N];

	CreateFilterWindow();
}

StripeFilter::~StripeFilter() {
	if (m_pDamping[0]!=NULL)
		for (size_t i=0; i<m_nScale; i++)
			delete [] m_pDamping[i];

	if (m_pLine!=NULL)
		delete [] m_pLine;

	if (m_pCLine!=NULL)
		delete [] m_pCLine;

	if (fft[0]!=NULL) {
		for (size_t i=0; i<m_nScale; i++)
		delete fft[i];
	}
}


void StripeFilter::CreateFilterWindow()
{
	const float s=-1.0f/(2.0f*m_fSigma*m_fSigma);

	for (size_t j=0; j<m_nScale; j++) {
		const float scale=1.0f/(2.0f*m_nFFTlength[j]);
		for (size_t i=0; i<2*m_nFFTlength[j]; i++) {
			float w=static_cast<float>(i)*scale;
			m_pDamping[j][i]=(1.0f-exp(w*w*s))*scale;
		}
	}
}

void StripeFilter::Process(kipl::base::TImage<float,2> &img, StripeFilterOperation op)
{
	std::ostringstream msg;

	try {
		m_wt.transform(img,m_nScale);
	}
	catch (kipl::base::KiplException &e) {
		logger(kipl::logging::Logger::LogWarning,e.what());
		return;
		//throw ImagingException(e.what(),__FILE__,__LINE__);
	}

	list< kipl::wavelets::WaveletQuad<float> >::iterator it;
	size_t level=0;
	for (it=m_wt.data.begin(), level=0; it!=m_wt.data.end(); it++, level++) {
		switch (op) {
		case VerticalComponentZero : VerticalStripesZero(it->v); break;
		case VerticalComponentFFT  : FilterVerticalStripes(it->v, level); break;
		}
	}

	img=m_wt.synthesize();
}

void StripeFilter::FilterVerticalStripes(kipl::base::TImage<float,2> &img, size_t level)
{

	const size_t nLines=img.Size(0);

	float *pData=img.GetDataPtr();

	for (size_t line=0; line<nLines; line++) {
		memset(m_pLine,0,sizeof(float)*2*m_nFFTlength[level]);
		GetVerticalLine(pData,m_pLine,line,nLines,img.Size(1));

		(*fft[level])(m_pLine,m_pCLine);

		for (size_t i=0; i< 2*m_nFFTlength[level]; i++) {
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

	for (size_t i=0; i<len; i++, pData+=stride) {
		pLine[i]=*pData;
	}
}

void StripeFilter::SetVerticalLine(float *pLine, float *pDest, size_t pos, size_t stride, size_t len)
{
	float *pData=pDest+pos;

	for (size_t i=0; i<len; i++, pData+=stride) {
		*pData=pLine[i];
	}
}

}

std::string enum2string(ImagingAlgorithms::StripeFilterOperation op)
{
	std::string str;

	switch (op) {
	case ImagingAlgorithms::VerticalComponentZero :  return "verticalzero"; break;
	case ImagingAlgorithms::VerticalComponentFFT  :  return "verticalfft"; break;
	default : throw ImagingException("Unknown stripe filter operation", __FILE__, __LINE__);
	}
	return str;
}

void string2enum(std::string str, ImagingAlgorithms::StripeFilterOperation &op)
{
    std::ostringstream msg;
    if (str=="verticalzero")
        op=ImagingAlgorithms::VerticalComponentZero;
    else if (str=="verticalfft")
        op=ImagingAlgorithms::VerticalComponentFFT;
    else {
        msg<<"Could not translate string ("<<str<<") to stripe filter operation",
        throw ImagingException(msg.str(), __FILE__, __LINE__);
    }
}

std::ostream & operator<<(std::ostream & s, ImagingAlgorithms::StripeFilterOperation op)
{
	s<<enum2string(op);

	return s;
}
