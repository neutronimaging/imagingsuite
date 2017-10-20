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


#ifndef _SPOTCLEAN2_H_
#define _SPOTCLEAN2_H_

//#include "../src/stdafx.h"
#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <base/timage.h>
#include <filters/filterbase.h>
#include <math/LUTCollection.h>
#include <containers/ArrayBuffer.h>
#include <map>
#include <list>
#include "PreprocEnums.h"
#include <ReconConfig.h>
#include <SpotClean.h>


class STDPREPROCMODULESSHARED_EXPORT SpotClean2 :
	public PreprocModuleBase
{
public:
		const float mark;
	SpotClean2(std::string name="SpotClean2");
	virtual ~SpotClean2(void);
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);

	virtual std::map<std::string, std::string> GetParameters();

	int Setup(size_t iterations,
			float threshold, float width,
			float minlevel, float maxlevel,
			int maxarea,
			ImagingAlgorithms::DetectionMethod method);

	kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> img, ImagingAlgorithms::DetectionMethod method, size_t dims);
	kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> img);

	double ChangeStatistics(kipl::base::TImage<float,2> img);

	virtual std::string Version() {
		ostringstream s;
		s<<m_sModuleName<<" ("<<std::max(kipl::strings::VersionNumber("$Rev$"), SourceVersion())<<"), "<<PreprocModuleBase::Version();

		return s.str();
	}
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> &coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> &coeff);

	ImagingAlgorithms::SpotClean m_SpotClean;

	ReconConfig m_Config;
	float m_fGamma;
	float m_fSigma;
	int m_nIterations;
	float m_fMaxLevel;
	float m_fMinLevel;
	size_t m_nMaxArea;
	ImagingAlgorithms::DetectionMethod m_eDetectionMethod;
};

#endif
