//<LICENSE>

#ifndef SINOSPOTCLEAN_H_
#define SINOSPOTCLEAN_H_

#include <PreprocModuleBase.h>

class SinoSpotClean: public PreprocModuleBase {
public:
	using PreprocModuleBase::Configure;
	SinoSpotClean();
	~SinoSpotClean();
	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
    bool SetROI(const std::vector<size_t> &/*roi*/) override {return false;}
	std::map<std::string, std::string> GetParameters() override;

	std::string Version() override {
		ostringstream s;
		s<<"SinoSpotClean ("<<std::max(kipl::strings::VersionNumber("$Rev: 1314 $"), PreprocModuleBase::SourceVersion())<<"), "<<ProcessModuleBase::Version();

		return s.str();
	}

protected:
	using PreprocModuleBase::ProcessCore;
	int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters) override;
	int SourceVersion() override;

	int CleanSinogramSpots(kipl::base::TImage<float,2> &img, int length, float th);

	int m_nFilterLength;
	float m_fThreshold;
};

#endif /* SINOSPOTCLEAN_H_ */
