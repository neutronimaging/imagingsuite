/*
 * ImageHistogram.cpp
 *
 *  Created on: Jan 7, 2013
 *      Author: kaestner
 */
#include "stdafx.h"
#include "DistanceStatistics.h"
#include <base/thistogram.h>
#include <strings/miscstring.h>
#include <math/statistics.h>
#include <morphology/morphdist.h>

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>
#include <fstream>

DistanceStatistics::DistanceStatistics() :
m_fMaskValue(0),
m_sFileName("distance_statistics.csv")
{
	// TODO Auto-generated constructor stub

}

DistanceStatistics::~DistanceStatistics() {
	// TODO Auto-generated destructor stub
}

int DistanceStatistics::Configure(std::map<std::string, std::string> parameters)
{
	m_sFileName=GetStringParameter(parameters,"filename");

	return 0;
}

std::map<std::string, std::string> DistanceStatistics::GetParameters()
{
	std::map<std::string, std::string> parameters;
	parameters["filename"]=m_sFileName;

	return parameters;
}

int DistanceStatistics::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	logger(kipl::logging::Logger::LogMessage,"Computing the distance statistics of the image.");

	std::ofstream statfile(m_sFileName.c_str());

	map<float,kipl::math::Statistics> statisticslist;

	kipl::base::TImage<char,3> mask;
	float *pImg=img.GetDataPtr();
	char *pMask=mask.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++)
		pMask[i]= pImg[i]==m_fMaskValue ? 0 : 1;

	kipl::base::TImage<float,3> dist;

	kipl::morphology::EuclideanDistance(mask,dist);
	float *pDist=dist.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++) {
		statisticslist[pDist[i]].put(pImg[i]);
	}

	map<float,kipl::math::Statistics>::iterator it;

	statfile<<"Distance\t"
			<<"Count\t"
			<<"Sum\t"
			<<"Mean\t"
			<<"Std Dev\t"
			<<"Min\t"
			<<"Max\n";

	for (statisticslist.begin(); it!=statisticslist.end(); it++) {
		statfile<<it->first<<"\t"
				<<(it->second.n())<<"\t"
				<<(it->second.Sum())<<"\t"
				<<(it->second.E())<<"\t"
				<<(it->second.s())<<"\t"
				<<(it->second.Min())<<"\t"
				<<(it->second.Max())<<"\n";
	}

	statfile.close();

	return 0;
}


