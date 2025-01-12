//<LICENSE>

#ifndef ADAPTIVEFILTER_H_
#define ADAPTIVEFILTER_H_
#include "StdPreprocModules_global.h"
#include <base/timage.h>
#include <math/LUTCollection.h>
#include <PreprocModuleBase.h>
#include <ReconConfig.h>
//#include "ReconEnums.h"

class STDPREPROCMODULESSHARED_EXPORT AdaptiveFilter : public PreprocModuleBase
{
public:
    AdaptiveFilter(kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~AdaptiveFilter();

	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
    bool SetROI(const std::vector<size_t> &roi) override;
	//virtual int Process(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);

	std::map<std::string, std::string> GetParameters() override;
    int ProcessSingle(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters); // moved here by Chiara
    int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters) override;

protected:
    int SimpleFilter(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters);
    void MaxProfile(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,2> &profile);
    void MinProfile(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,2> &profile);
    void MinMaxProfile(kipl::base::TImage<float,2> &img, std::vector<float> &minprofile, std::vector<float> &maxprofile);
    void Eccentricity(std::vector<float> &minprofile, std::vector<float> &maxprofile, std::vector<float> &eprofile);
    void GetColumn(kipl::base::TImage<float,2> &img, size_t idx, float *data);
	ReconConfig mConfig;
	kipl::math::SigmoidLUT *pLUT;

    int m_nFilterSize;
    float m_fEccentricityMin;
    float m_fEccentricityMax;

    float m_fFilterStrength;
    float m_fFmax;
    bool bNegative; /// boolean value that triggers the use on original sinograms (if true) or after the -log plugin (if false)
};

#endif /* ADAPTIVEFILTER_H_ */
