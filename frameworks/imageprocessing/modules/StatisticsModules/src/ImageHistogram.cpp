/*
 * ImageHistogram.cpp
 *
 *  Created on: Jan 7, 2013
 *      Author: kaestner
 */
#include "stdafx.h"
#include "ImageHistogram.h"
#include <base/thistogram.h>
#include <strings/miscstring.h>

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>
#include <fstream>

ImageHistogram::ImageHistogram() :
m_nSize(512),
m_fMaskValue(0),
m_bUseMask(false),
m_sFileName("histogram.csv")
{
	// TODO Auto-generated constructor stub

}

ImageHistogram::~ImageHistogram() {
	// TODO Auto-generated destructor stub
}

int ImageHistogram::Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters)
{
	m_nSize=GetIntParameter(parameters,"bins");

	m_sFileName=GetStringParameter(parameters,"filename");

	return 0;
}

std::map<std::string, std::string> ImageHistogram::GetParameters()
{
	std::map<std::string, std::string> parameters;
	parameters["bins"]=m_nSize;
	parameters["filename"]=m_sFileName;

	return parameters;
}

int ImageHistogram::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	logger(kipl::logging::Logger::LogMessage,"Computing the histogram of the image.");

	std::ofstream histogramfile(m_sFileName.c_str());

	if (m_nSize!=0) {
		ComputeHistogram(img,m_nSize);

		size_t *bins=m_Histogram.GetY();
		float *axis=m_Histogram.GetX();

		for (size_t i=0; i<m_Histogram.Size(); i++) {
			histogramfile<<axis[i]<<"\t"<<bins[i]<<"\n";
		}
	}
	else {
		std::map<float,size_t> hist;

		hist=kipl::base::ExactHistogram(img.GetDataPtr(), img.Size());

		std::map<float,size_t>::iterator it;

		for (it=hist.begin(); it!=hist.end(); it++)
			histogramfile<<(it->first)<<"\t"<<it->second<<"\n";
	}

	histogramfile.close();

	return 0;
}


