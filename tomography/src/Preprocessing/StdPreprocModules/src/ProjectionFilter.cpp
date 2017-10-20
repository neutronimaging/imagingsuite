//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//

#include "../include/StdPreprocModules_global.h"

#include <ReconException.h>
#include <ReconHelpers.h>
#include <ParameterHandling.h>

#include <math/mathconstants.h>
#include <math/mathfunctions.h>
#include <strings/miscstring.h>
#include <math/compleximage.h>
#include <base/imagecast.h>
#include <io/io_matlab.h>
#include <visualization/GNUPlot.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <map>

#include "../include/ProjectionFilter.h"

ostream & operator<<(ostream & s, ProjectionFilterBase::FilterType ft)
{
    s<<enum2string(ft);

	return s;
}

void STDPREPROCMODULESSHARED_EXPORT string2enum(const std::string str, ProjectionFilterBase::FilterType &ft)
{
	std::string s=kipl::strings::toLower(str);
	
	if (s=="ram-lak")
		ft=ProjectionFilterBase::FilterRamLak;
	else if (s=="shepp-logan")
		ft=ProjectionFilterBase::FilterSheppLogan;
	else if (s=="hanning")
		ft=ProjectionFilterBase::FilterHanning;
	else if (s=="hamming")
		ft=ProjectionFilterBase::FilterHamming;
	else if (s=="butterworth")
		ft=ProjectionFilterBase::FilterButterworth;
	else
		throw ReconException("Could not convert filter string to enum.",__FILE__, __LINE__);
}

STDPREPROCMODULESSHARED_EXPORT std::string enum2string(const ProjectionFilterBase::FilterType &ft)
{
    switch (ft) {
        case ProjectionFilterBase::FilterRamLak			: return "Ram-Lak"; break;
        case ProjectionFilterBase::FilterSheppLogan		: return "Shepp-Logan"; break;
        case ProjectionFilterBase::FilterHanning		: return "Hanning"; break;
        case ProjectionFilterBase::FilterHamming		: return "Hamming"; break;
        case ProjectionFilterBase::FilterButterworth	: return "Butterworth"; break;
        default                                     	: return "Undefined filter type"; break;
    };

    return "Undefined filter type";
}


// Projection filter base
ProjectionFilterBase::ProjectionFilterBase(std::string name) : 
	PreprocModuleBase(name),
    m_FilterType(FilterHamming),
    m_fCutOff(0.5f),
    m_fOrder(1),
    m_bUseBias(true),
    m_fBiasWeight(0.1f),
    m_nPaddingDoubler(2)
{
}

int ProjectionFilterBase::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	size_t N=config.ProjectionInfo.roi[2]-config.ProjectionInfo.roi[0];
	string2enum(GetStringParameter(parameters,"filtertype"),m_FilterType);
	m_fCutOff=GetFloatParameter(parameters,"cutoff");
	m_fOrder=GetFloatParameter(parameters,"order");
	m_bUseBias=kipl::strings::string2bool(GetStringParameter(parameters,"usebias"));
    m_fBiasWeight=GetFloatParameter(parameters,"biasweight");
    m_nPaddingDoubler=GetIntParameter(parameters,"paddingdoubler");

    nImageSize=N;
    BuildFilter(N);

	return 0;
}

int ProjectionFilterBase::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
	if (img.Size()==0)
		throw ReconException("Empty projection image",__FILE__,__LINE__);

	FilterProjection(img);
	return 0;
}

int ProjectionFilterBase::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	if (img.Size()==0)
		throw ReconException("Empty projection image",__FILE__,__LINE__);

	kipl::base::TImage<float,2> proj(img.Dims());

	for (size_t i=0; i<img.Size(2); i++) {
		memcpy(proj.GetDataPtr(),img.GetLinePtr(0,i),sizeof(float)*proj.Size());
		FilterProjection(proj);
		memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),sizeof(float)*proj.Size());
	}
	return 0;

}

size_t ProjectionFilterBase::ComputeFilterSize(size_t len)
{
	double e=log(static_cast<double>(len))/log(2.0);

    return static_cast<size_t>(1)<<(static_cast<size_t>(ceil(e))+m_nPaddingDoubler); // Double the padding
}

std::map<std::string, std::string> ProjectionFilterBase::GetParameters()
{
// todo: fix parameter readout
	std::map<std::string, std::string> parameters;

    parameters["filtertype"]=enum2string(m_FilterType);
    parameters["cutoff"]=kipl::strings::value2string(m_fCutOff);
    parameters["order"]=kipl::strings::value2string(m_fOrder);
    parameters["usebias"]=m_bUseBias ? "true" : "false";
    parameters["biasweight"]=kipl::strings::value2string(m_fBiasWeight);
    parameters["paddingdoubler"]=kipl::strings::value2string(m_nPaddingDoubler);

	return parameters;

}

// Projection filter w. double
ProjectionFilter::ProjectionFilter(void) : 
	ProjectionFilterBase("ProjectionFilter"),
	fft(NULL)
{
}

ProjectionFilter::~ProjectionFilter(void)
{
	if (fft!=NULL)
		delete fft;
}


void ProjectionFilter::BuildFilter(const size_t N)
{	
	nFFTsize=ComputeFilterSize(N);
	const size_t N2=nFFTsize>>static_cast<size_t>(1);
	mFilter.Resize(&N2);
	mFilter=0.0f;
	
	std::stringstream msg;

	
	if (!((0<m_fCutOff) && (m_fCutOff<=1.0)))
		throw ReconException("Cut off frequency is not in valid range",__FILE__,__LINE__);
	
	float FilterOrder=0.0;
	if (m_FilterType==ProjectionFilterBase::FilterButterworth)
		FilterOrder=m_fOrder;

	const size_t cN2cutoff=static_cast<size_t>(2*m_fCutOff*N2);
	float step=1.0f/(N2*(N2-1.0f)); // The extra N2 is for scaling the FFT

	for (size_t i=0; i<N2; i++)
		mFilter[i]=i*step;

	step=1.0f/(cN2cutoff-1.0f); // The extra N2 is for scaling the FFT
	for (size_t i=0; i<cN2cutoff; i++) {
		switch (m_FilterType){
		case FilterRamLak: break;
		case FilterSheppLogan: mFilter[i]=sin(dPi*i*step*m_fCutOff)/N2;break;
		case FilterHanning: mFilter[i]*=(0.5+0.5*cos(i*step*dPi*m_fCutOff)); break;
		case FilterHamming: mFilter[i]*=(0.54+0.46*cos(i*step*dPi*m_fCutOff)); break;
		case FilterButterworth: mFilter[i]/=(1+pow(static_cast<double>(i*step*m_fCutOff),static_cast<double>(FilterOrder))); break;
		default: break;
		}
	}
	memset(mFilter.GetDataPtr()+cN2cutoff,0, sizeof(double)*(N2-cN2cutoff));
	if (m_bUseBias==true)
    //	mFilter[0]=m_fBiasWeight/N2;
            mFilter[0]=m_fBiasWeight*mFilter[1];

	if (fft!=NULL)
		delete fft;

    fft=new kipl::math::fft::FFTBase(&nFFTsize,1);
	logger(kipl::logging::Logger::LogVerbose,"Filter init done");
}

void ProjectionFilter::FilterProjection(kipl::base::TImage<float,2> & img)
{
	const size_t nLines=img.Size(1);

	kipl::base::TImage<double,2> dimg;
	dimg=kipl::base::ImageCaster<double,float,2>::cast(img);
	
	const size_t cnFilterLength=nFFTsize;
	kipl::base::TImage<complex<double>,1> ft1Dimg(&nFFTsize);

	double *pFilter=mFilter.GetDataPtr();
	complex<double> *pFTLine=ft1Dimg.GetDataPtr();

	double *pLine=new double[nFFTsize*2];

	for (size_t line=0; line<nLines; line++) {
		memset(pLine,0,2*sizeof(double)*nFFTsize);
		memcpy(pLine,dimg.GetLinePtr(line),sizeof(double)*dimg.Size(0));
		fft->operator()(pLine,pFTLine);

		for (size_t i=0; i<cnFilterLength; i++) {
			pFTLine[i]*=pFilter[i];
		}
		
		fft->operator ()(pFTLine, pLine);
		memcpy(dimg.GetLinePtr(line),pLine,sizeof(double)*dimg.Size(0));
	}
	
	delete [] pLine;
	
	img=kipl::base::ImageCaster<float,double,2>::cast(dimg);
}


//------------------------------------------------------------
// Projection filter w. float
ProjectionFilterSingle::ProjectionFilterSingle(void) : 
	ProjectionFilterBase("ProjectionFilterSingle"),
	fft(NULL)
{
}

ProjectionFilterSingle::~ProjectionFilterSingle(void)
{
	if (fft!=NULL)
		delete fft;
}


void ProjectionFilterSingle::BuildFilter(const size_t N)
{	
	nFFTsize=ComputeFilterSize(N);
	const size_t N2=nFFTsize/2;
	mFilter.Resize(&N2);
	mFilter=0.0f;
	
	std::stringstream msg;
	msg<<"Filter :"<<m_FilterType<<" filter size="<<mFilter.Size();
	logger(kipl::logging::Logger::LogVerbose, msg.str());

	if (!((0<m_fCutOff) && (m_fCutOff<=0.5)))
		throw ReconException("Cut off frequency is not in valid range",__FILE__,__LINE__);
	
	float FilterOrder=0.0;
	if (m_FilterType==ProjectionFilterBase::FilterButterworth)
		FilterOrder=m_fOrder;

	const size_t cN2cutoff=static_cast<size_t>(2*m_fCutOff*N2);

	float rampstep=1.0f/((N2-1.0f)); // The extra N2 is for scaling the FFT
	
	for (size_t i=0; i<N2; i++)
		mFilter[i]=i*rampstep;

	float fstep=1.0f/((cN2cutoff-1.0f)); // The extra N2 is for scaling the FFT
	for (size_t i=0; i<cN2cutoff; i++) {
		switch (m_FilterType){
		case FilterRamLak: break;
		case FilterSheppLogan: mFilter[i]=static_cast<float>(sin(dPi*i*fstep*m_fCutOff))/N2;break;
		case FilterHanning: mFilter[i]*=static_cast<float>(0.5f+0.5f*cos(i*fstep*fPi*m_fCutOff)); break;
		case FilterHamming: mFilter[i]*=static_cast<float>(0.54+0.46*cos(i*fstep*dPi*m_fCutOff)); break;
		case FilterButterworth: mFilter[i]/=static_cast<float>(1+pow(i*fstep*m_fCutOff,FilterOrder)); break;
		default: break;
		}
	}
	memset(mFilter.GetDataPtr()+cN2cutoff,0, sizeof(float)*(N2-cN2cutoff));
	if (m_bUseBias==true)
        mFilter[0]=m_fBiasWeight*mFilter[1];

	if (fft!=NULL)
		delete fft;

    fft=new kipl::math::fft::FFTBaseFloat(&nFFTsize,1);
	
	PreparePadding(nImageSize,nFFTsize);
	logger(kipl::logging::Logger::LogVerbose,"Filter init done");
}

void ProjectionFilterSingle::FilterProjection(kipl::base::TImage<float,2> & img)
{
	const size_t nLines=img.Size(1);

	const size_t cnFilterLength=mFilter.Size(0);
	const size_t cnLoopCnt=nFFTsize/2;
	kipl::base::TImage<complex<float>,1> ft1Dimg(&nFFTsize);

	float *pFilter=mFilter.GetDataPtr();
	complex<float> *pFTLine=ft1Dimg.GetDataPtr();
	const size_t nLenPad=nFFTsize+16;
	float *pLine=new float[nLenPad];

	for (size_t line=0; line<(nLines); line++) {
        memset(pLine,0,sizeof(float)*nLenPad);
		size_t insert=Pad(img.GetLinePtr(line),img.Size(0),pLine,nLenPad);
		fft->operator()(pLine,pFTLine);

		for (size_t i=0; i< cnFilterLength; i++) { //todo: find cross talk and overwrite here!!!
			pFTLine[i]*=pFilter[i];
		}
		
		fft->operator ()(pFTLine, pLine);
		
		float *pImg=img.GetLinePtr(line);
		const float scale=fPi/(4.0f*cnLoopCnt);
		for (size_t i=0; i<img.Size(0); i++) {
			pImg[i]=pLine[i+insert]*scale;
		}

	}

	delete [] pLine;
}

size_t ProjectionFilterSingle::Pad(float const * const pSrc, 
								   const size_t nSrcLen, 
								   float *pDest, 
								   const size_t nDestLen)
{
	memset(pDest,0,nDestLen*sizeof(float));
	memcpy(pDest+nInsert,pSrc,nSrcLen*sizeof(float));

	for (size_t i=0; i<nInsert; i++) {
		pDest[i]=pSrc[0]*mPadData[i];
		pDest[nDestLen-1-i]=pSrc[nSrcLen-1]*mPadData[i];
	}

	return nInsert;
}

void ProjectionFilterSingle::PreparePadding(const size_t nImage, const size_t nFilter)
{
	nInsert=(nFilter/2)-(nImage/2);
	
	mPadData.Resize(&nInsert);

	for (size_t i=0; i<nInsert; i++) {
		float x=(float) i/ (float)nInsert - 0.5f;
		mPadData[i]=kipl::math::Sigmoid(x,0.0f,0.07f);
	}
	

}
