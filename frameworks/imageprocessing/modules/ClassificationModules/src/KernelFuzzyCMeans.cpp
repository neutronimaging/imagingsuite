/*
 * DataScaler.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#include "KernelFuzzyCMeans.h"
#include <segmentation/kernelfuzzykmeans.h>
#include <strings/miscstring.h>

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>

KernelFuzzyCMeans::KernelFuzzyCMeans() :
KiplProcessModuleBase("KernelFuzzyCMeans"),
	m_nNClasses(3),
	m_fFuzziness(1.5f),
	m_fSigma(2.0f),
	m_nMaxIt(250),
	m_bParallel(false)
{

}

KernelFuzzyCMeans::~KernelFuzzyCMeans() {
}


int KernelFuzzyCMeans::Configure(std::map<std::string, std::string> parameters)
{
	m_nNClasses  = GetFloatParameter(parameters,"nclasses");
	m_fFuzziness = GetFloatParameter(parameters,"fuzziness");
	m_fSigma     = GetFloatParameter(parameters,"sigma");
	m_nMaxIt     = GetIntParameter(parameters,"maxiterations");
	m_bParallel  = kipl::strings::string2bool(GetStringParameter(parameters,"parallel"));

	return 0;
}

std::map<std::string, std::string> KernelFuzzyCMeans::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["nclasses"]      = kipl::strings::value2string(m_nNClasses);
	parameters["fuzziness"]     = kipl::strings::value2string(m_fFuzziness);
	parameters["sigma"]         = kipl::strings::value2string(m_fSigma);
	parameters["maxiterations"] = kipl::strings::value2string(m_nMaxIt);
	parameters["parallel"]	    = kipl::strings::bool2string(m_bParallel);
	return parameters;
}

int KernelFuzzyCMeans::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
//todo fix missing virtuals
//    akipl::segmentation::KernelFuzzyKMeans<float,unsigned char,3> segmenter;

//	segmenter.set(m_nNClasses, m_fFuzziness, m_nMaxIt);
//	kipl::base::TImage<unsigned char,3> seg;
//	if (!m_bParallel)
//		segmenter(img,seg);
//	else
//		segmenter.parallel(img,seg);

//	float *pImg=img.GetDataPtr();
//	unsigned char *pSeg=seg.GetDataPtr();
//	for (size_t i=0; i<img.Size(); i++)
//		pImg[i]=static_cast<float>(pSeg[i]);

	return 0;
}
