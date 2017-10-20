//<LICENSE>

#ifndef SAVEPROJECTIONS_H_
#define SAVEPROJECTIONS_H_

#include <string>

#include <PreprocModuleBase.h>
#include <ReconConfig.h>

class SaveProjections : public PreprocModuleBase
{
public:
	SaveProjections();
	virtual ~SaveProjections();

	virtual std::map<std::basic_string<char>, std::basic_string<char> > GetParameters();
	virtual int Configure(ReconConfig config, std::map<std::basic_string<char>, std::basic_string<char> > parameters);
protected:
	
	virtual int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);

	ReconConfig m_config;
    std::string m_sPath;
	std::string m_sFileMask;
	ReconConfig::cProjections::eImageType m_eImageType;
    kipl::io::eFileType m_eFileType;
};

#endif /* PROJECTIONINSPECTOR_H_ */
