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
#include "../include/TranslatedProjectionWeighting.h"
#include "../include/ReconException.h"
#include <io/io_matlab.h>
#include <math/mathfunctions.h>

#include <math/mathconstants.h>

#include <ParameterHandling.h>


TranslatedProjectionWeighting::TranslatedProjectionWeighting() : 
	PreprocModuleBase("TranslatedProjectionWeighting")
{
}

TranslatedProjectionWeighting::~TranslatedProjectionWeighting()
{

}

int TranslatedProjectionWeighting::Setup(ReconConfig *config, eWeightFunctionType wf, float width)
{
	mConfig=*config;

	mWidth=width;
	eWeightFunction=wf;

	return 0;
}

int TranslatedProjectionWeighting::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	mConfig=config;
// todo: fix parameters
	mWidth=GetFloatParameter(parameters,"width");
	string2enum(GetStringParameter(parameters,"weightfunction"),eWeightFunction);

	return 0;
}

std::map<std::string, std::string> TranslatedProjectionWeighting::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["width"]="5";
	parameters["weightfunction"]="sigmoid";

	return parameters;
}

bool TranslatedProjectionWeighting::SetROI(size_t *roi)
{
	mROI[0]=roi[0];
	mROI[1]=roi[1];
	mROI[2]=roi[2];
	mROI[3]=roi[3];

	size_t dims[2]={mROI[2]-mROI[0],mROI[3]-mROI[1]};
	fWeights.Resize(dims);

	switch (eWeightFunction) {
	case WeightFunction_Sigmoid : ComputeSigmoidWeights(); break;
	case WeightFunction_Linear  : ComputeLinearWeights();  break;
	default : throw ReconException("Unknown weighting method", __FILE__, __LINE__);
	}	

	return true;	
}

int TranslatedProjectionWeighting::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
	const size_t N=img.Size();
	if (N!=fWeights.Size()) 
		throw ReconException("Translation mix weight data does not match projection size",__FILE__,__LINE__);

	float *pImg=img.GetDataPtr();
	float *pWeight=fWeights.GetDataPtr();

	for (size_t i=0; i<N; i++) {
			pImg[i]*=pWeight[i];
	}
	
	return 0;
}

void TranslatedProjectionWeighting::ComputeSigmoidWeights()
{
// todo fix parameters

//	size_t nSizeU = fWeights.Size(0);
//	float fCenter = mConfig.Projections.fCenter;
//	float fPivot  = mConfig.Projections.fTiltPivotPosition;
//
//	if (mConfig.Projections.bCorrectTilt) {
//		float fTilt=tan(mConfig.Projections.fTiltAngle*fPi/180.0f);
//		for (size_t j=0; j<fWeights.Size(1); j++) {
//			float *pLine=fWeights.GetLinePtr(j);
//
//			if ((0.5*nSizeU)<fCenter)
//				for (size_t i=0; i<nSizeU; i++)
//					pLine[i]=1.0f-kipl::math::Sigmoid(static_cast<float>(i),
//							fCenter-(mROI[1]+j-fPivot)*fTilt,
//							mWidth);
//			else
//				for (size_t i=0; i<nSizeU; i++)
//					pLine[i]=kipl::math::Sigmoid(static_cast<float>(i),
//							fCenter-(mROI[1]+j-fPivot)*fTilt,
//							mWidth);
//		}
//
//	}
//	else {
//		float *pLine=fWeights.GetLinePtr(0);
//
//		if ((0.5*nSizeU)<fCenter)
//			for (size_t i=0; i<nSizeU; i++)
//				pLine[i]=1.0f-kipl::math::Sigmoid(static_cast<float>(i), fCenter, mWidth);
//		else
//			for (size_t i=0; i<nSizeU; i++)
//				pLine[i]=kipl::math::Sigmoid(static_cast<float>(i), fCenter, mWidth);
//
//		for (size_t i=1; i<fWeights.Size(1); i++) {
//			memcpy(fWeights.GetLinePtr(i),pLine,sizeof(float)*fWeights.Size(0));
//		}
//	}
	//kipl::io::WriteTIFF32(fWeights,"weights.tif");
}

void TranslatedProjectionWeighting::ComputeLinearWeights()
{

	throw ReconException("The linear weighting is not ready for use yet",__FILE__,__LINE__);
// todo fix parameters
//	size_t nSizeU=fWeights.Size(0);
//	float fCenter=mConfig.Projections.fCenter;
//
//	const float w2=mWidth/2;
//	const size_t startU=static_cast<size_t>(fCenter-w2);
//	const size_t stopU=static_cast<size_t>(std::min(nSizeU-1,static_cast<size_t>(fCenter+w2+0.5)));
//	const float k=1.0f/mWidth;
//
//	for (size_t i=0; i<startU; i++)
//		fWeights[i]=1.0f;
//
//	for (size_t i=startU; i<stopU; i++)
//		fWeights[i]=1.0f-(i-fCenter)*k;
//
//	for (size_t i=stopU; i<nSizeU; i++)
//		fWeights[i]=0.0f;
}
