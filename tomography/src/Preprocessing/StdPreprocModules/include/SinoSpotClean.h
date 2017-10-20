/*
 * SinoSpotClean.h
 *
 *  Created on: May 21, 2013
 *      Author: kaestner
 */

#ifndef SINOSPOTCLEAN_H_
#define SINOSPOTCLEAN_H_

#include <PreprocModuleBase.h>

class SinoSpotClean: public PreprocModuleBase {
public:
	SinoSpotClean();
	virtual ~SinoSpotClean();
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual bool SetROI(size_t *roi) {return false;}
	virtual std::map<std::string, std::string> GetParameters();

	virtual std::string Version() {
		ostringstream s;
		s<<"SinoSpotClean ("<<std::max(kipl::strings::VersionNumber("$Rev: 1314 $"), PreprocModuleBase::SourceVersion())<<"), "<<ProcessModuleBase::Version();

		return s.str();
	}

protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);
	virtual int SourceVersion();

	int CleanSinogramSpots(kipl::base::TImage<float,2> &img, int length, float th);

	int m_nFilterLength;
	float m_fThreshold;
};

#endif /* SINOSPOTCLEAN_H_ */
