/*
 * BasicRingClean.cpp
 *
 *  Created on: Jun 30, 2011
 *      Author: kaestner
 */
//#include "stdafx.h"

#include <algorithm>

#include <io/io_tiff.h>
#include <filters/medianfilter.h>

#include "../include/BasicRingClean.h"


BasicRingClean::BasicRingClean() : 
	PreprocModuleBase("BasicRingClean")
{

}

BasicRingClean::~BasicRingClean() {

}


int BasicRingClean::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	return 0;
}

std::map<std::string, std::string> BasicRingClean::GetParameters()
{
	std::map<std::string, std::string> parameters;

	//parameters["type"]="box";
	//parameters["size"]="3";

	return parameters;
}

bool BasicRingClean::SetROI(size_t *roi)
{
	return false;
}

int BasicRingClean::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	kipl::base::TImage<float,2> meanproj(img.Dims());

	float *pMean=meanproj.GetDataPtr();
	float *pImg=img.GetDataPtr();

	size_t N=meanproj.Size();
	memcpy(pMean,pImg,sizeof(float)*N);

	for (size_t i=1; i<img.Size(2); i++) {
		pImg=img.GetLinePtr(0,i);
		for (size_t j=0; j<N; j++) {
			pMean[j]+=pImg[j];
		}
	}

	float scale=1.0f/static_cast<float>(img.Size(2));

	for (size_t j=0; j<N; j++) {
		pMean[j]*=scale;
	}

	kipl::io::WriteTIFF32(meanproj,"mean.tif");

	size_t filtdims[2]={3,3};
	kipl::filters::TMedianFilter<float,2> medfilt(filtdims);

	kipl::base::TImage<float,2> medproj=medfilt(meanproj);
	
	float *pMed=medproj.GetDataPtr();
	kipl::io::WriteTIFF32(medproj,"med.tif");

	for (size_t i=0 ; i<N; i++) {
		pMed[i] = pMean[i]-pMed[i];
	}

	kipl::io::WriteTIFF32(medproj,"diff.tif");

	for (size_t i=0; i<img.Size(2); i++) {
		pImg=img.GetLinePtr(0,i);
		for (size_t j=0; j<N; j++) {
			pImg[j]-=pMed[j];
		}
	}

	return 0;
}
