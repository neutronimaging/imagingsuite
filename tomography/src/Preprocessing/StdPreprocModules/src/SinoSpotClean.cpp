/*
 * SinoSpotClean.cpp
 *
 *  Created on: May 21, 2013
 *      Author: kaestner
 */
//#include "stdafx.h"
#include "../include/SinoSpotClean.h"
#include <filters/medianfilter.h>

SinoSpotClean::SinoSpotClean() :
	PreprocModuleBase("SinoSpotClean"),
	m_nFilterLength(5),
	m_fThreshold(1.0f)
{
	// TODO Auto-generated constructor stub

}

SinoSpotClean::~SinoSpotClean() {
	// TODO Auto-generated destructor stub
}

int SinoSpotClean::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{


	return 0;
}

std::map<std::string, std::string> SinoSpotClean::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["length"]=kipl::strings::value2string(m_nFilterLength);
	parameters["threshold"]=kipl::strings::value2string(m_fThreshold);


	return parameters;
}

int SinoSpotClean::ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
	kipl::base::TImage<float,2> sino;
	int N=static_cast<int>(img.Size(2));
	for (int i=0 ; i<N; i++) {
		ExtractSinogram(img,sino,i);
		CleanSinogramSpots(sino,m_nFilterLength,m_fThreshold);
		InsertSinogram(sino,img,i);
	}

	return 0;
}

int SinoSpotClean::SourceVersion()
{
	return kipl::strings::VersionNumber("$Rev: 1314 $");
}

int SinoSpotClean::CleanSinogramSpots(kipl::base::TImage<float,2> &img, int length, float th)
{
	int cnt=0;

    size_t dims[2]={1UL, static_cast<size_t>(2*length+1)};
	int weights[256];
	for (int i=0; i<dims[1]; i++)
		weights[i]=1;
	weights[length]=0;

	kipl::filters::TWeightedMedianFilter<float,2> medfilt(dims,weights);

	kipl::base::TImage<float,2> filtered;

	filtered=medfilt(img);
	int N=static_cast<int>(img.Size());
	float *pImg=img.GetDataPtr();
	float *pFilt=filtered.GetDataPtr();

	for (int i=0; i<N; i++) {
		if (th<fabs(pImg[i]-pFilt[i]))
			pImg[i]=pFilt[i];
	}


	return cnt;
}
