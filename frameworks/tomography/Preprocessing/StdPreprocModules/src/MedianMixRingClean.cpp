/*
 * MedianMixRingClean.cpp
 *
 *  Created on: Aug 4, 2011
 *      Author: kaestner
 */

#include "../include/StdPreprocModules_global.h"

#include <algorithm>

#include <io/io_tiff.h>
#include <filters/medianfilter.h>
#include <math/LUTCollection.h>
#include <morphology/morphfilters.h>

#include <ParameterHandling.h>

#include "../include/MedianMixRingClean.h"

MedianMixRingClean::MedianMixRingClean() :
PreprocModuleBase("MedianMixRingClean"),
	m_fSigma(0.0025f),
	m_fLambda(0.025f)
{

}

MedianMixRingClean::~MedianMixRingClean() {
}

int MedianMixRingClean::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	m_fSigma=GetFloatParameter(parameters,"width");
	m_fLambda=GetFloatParameter(parameters,"threshold");


	return 0;
}

std::map<std::string, std::string> MedianMixRingClean::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["threshold"]="0.025";
	parameters["width"]="0.0025";

	return parameters;
}

bool MedianMixRingClean::SetROI(size_t *roi)
{
	return false;
}

int MedianMixRingClean::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	std::ostringstream msg;

	ComputeProfile(img);

	kipl::math::SigmoidLUT mLUT(1024,m_fLambda,m_fSigma);
	msg.str("");
	msg<<"Preparing LUT with L="<<m_fLambda<<" and S="<<m_fSigma;
	logger(kipl::logging::Logger::LogMessage,msg.str());

	weights=profile;
	weights.Clone();

	float *pImg=NULL;
	float *pWeights=weights.GetDataPtr();
	mLUT.InPlace(pWeights,weights.Size());


	kipl::base::TImage<float,2> proj(img.Dims());
	kipl::base::TImage<float,2> med(img.Dims());

    size_t filtdims[2]={5UL, img.Size(1)<5UL ? 1UL : 5UL};

	kipl::filters::TMedianFilter<float,2> medfilt(filtdims);
	size_t N=proj.Size();

	for (size_t i=0; i<img.Size(2); i++) {
		pImg=img.GetLinePtr(0,i);

		memcpy(proj.GetDataPtr(),pImg,N*sizeof(float));
		med=medfilt(proj);
		float *pMed=med.GetDataPtr();

		for (size_t j=0; j<N; j++) {
			pImg[j]=(1-pWeights[j])*pImg[j]+pWeights[j]*pMed[j];
		}
	}

	return 0;
}

int MedianMixRingClean::ComputeProfile(kipl::base::TImage<float,3> &img)
{
	kipl::base::TImage<float,2> meanproj(img.Dims());

	float *pMean=meanproj.GetDataPtr();
	float *pImg=img.GetDataPtr();

	size_t N=meanproj.Size();

	memset(pMean,0,sizeof(float)*N);

	for (size_t i=0; i<img.Size(2); i++) {
		pImg=img.GetLinePtr(0,i);
		for (size_t j=0; j<N; j++) {
			pMean[j]+=pImg[j];
		}
	}

	float scale=1.0f/static_cast<float>(img.Size(2));

	for (size_t j=0; j<N; j++) {
		pMean[j]*=scale;
	}

    size_t filtdims[2]={meanproj.Size(1)<3UL ? 5UL: 3UL,meanproj.Size(1)<3UL ? 1UL : 3UL};
	kipl::filters::TMedianFilter<float,2> medfilt(filtdims);


	profile=medfilt(meanproj);


	float *pWeights=profile.GetDataPtr();


	for (size_t i=0 ; i<N; i++) {
		pWeights[i] = fabs(pMean[i]-pWeights[i]);
	}

	return 0;
}

