//<LICENSE>

#ifndef SAVEPROJECTIONS_H_
#define SAVEPROJECTIONS_H_
#include "inspectormodules_global.h"
#include <string>

#include <PreprocModuleBase.h>
#include <ReconConfig.h>

#include <interactors/interactionbase.h>

class INSPECTORMODULESSHARED_EXPORT SaveProjections : public PreprocModuleBase
{
public:
	using PreprocModuleBase::Configure;

    SaveProjections(kipl::interactors::InteractionBase *interactor=nullptr);
	~SaveProjections();

	std::map<std::basic_string<char>, std::basic_string<char> > GetParameters() override;
	int Configure(ReconConfig config, std::map<std::basic_string<char>, std::basic_string<char> > parameters) override;
protected:
	using PreprocModuleBase::ProcessCore;
	int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters) override;

	ReconConfig m_config;
    std::string m_sPath;
	std::string m_sFileMask;
	ReconConfig::cProjections::eImageType m_eImageType;
    kipl::io::eFileType m_eFileType;
};

#endif /* PROJECTIONINSPECTOR_H_ */
