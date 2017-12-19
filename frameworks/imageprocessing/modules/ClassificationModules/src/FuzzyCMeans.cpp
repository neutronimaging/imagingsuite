/*
 * DataScaler.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */
//#include "stdafx.h"
#include "FuzzyCMeans.h"
#include <segmentation/fuzzykmeans.h>
#include <strings/miscstring.h>

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>

FuzzyCMeans::FuzzyCMeans() :
KiplProcessModuleBase("FuzzyCMeans"),
	m_nNClasses(3),
	m_fFuzziness(1.5f),
	m_nMaxIt(250), 
	m_bParallel(false),
	m_bSaveIterations(false),
	m_sIterationFileName("cmeans_####.tif")
{

}


FuzzyCMeans::~FuzzyCMeans() {
}


int FuzzyCMeans::Configure(std::map<std::string, std::string> parameters)
{
	m_nNClasses  = GetFloatParameter(parameters,"nclasses");
	m_fFuzziness = GetFloatParameter(parameters,"fuzziness");
	m_nMaxIt     = GetFloatParameter(parameters,"maxiterations");
	m_bParallel  = kipl::strings::string2bool(GetStringParameter(parameters,"parallel"));
	m_bSaveIterations = kipl::strings::string2bool(GetStringParameter(parameters,"saveiterations"));
	m_sIterationFileName = GetStringParameter(parameters,"iterationsfilename");

	return 0;
}

std::map<std::string, std::string> FuzzyCMeans::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["nclasses"]       = kipl::strings::value2string(m_nNClasses);
	parameters["fuzziness"]      = kipl::strings::value2string(m_fFuzziness);
	parameters["maxiterations"]  = kipl::strings::value2string(m_nMaxIt);
	parameters["parallel"]       = kipl::strings::bool2string(m_bParallel);
	parameters["saveiterations"] = kipl::strings::bool2string(m_bSaveIterations);
	parameters["iterationsfilename"] = m_sIterationFileName;

	return parameters;
}

int FuzzyCMeans::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    kipl::segmentation::FuzzyKMeans<float,float,3> segmenter;

	segmenter.set(m_nNClasses, m_fFuzziness, m_nMaxIt,m_bSaveIterations,m_sIterationFileName);
	kipl::base::TImage<float,3> tmp;
	if (!m_bParallel) 
		segmenter(img,tmp);
	else
		segmenter.parallel(img,tmp);

	img=tmp;

	return 0;
}
