//<LICENSE>

#include <sstream>
#include "../include/ModuleConfig_global.h"
#include "../include/ProcessModuleBase.h"

#include "../include/ModuleException.h"

ProcessModuleBase::ProcessModuleBase(std::string name,kipl::interactors::InteractionBase *interactor) :
 logger(name),
 m_sModuleName(name),
  m_Interactor(interactor),
  m_bThreading(false)
{
    std::ostringstream msg;

    msg<<"C'tor ProcessModuleBase for "<<m_sModuleName;

    logger(kipl::logging::Logger::LogMessage,msg.str());
    if (m_Interactor!=nullptr) {
        logger(kipl::logging::Logger::LogVerbose,"Got an interactor");
    }
    else {
        logger(kipl::logging::Logger::LogVerbose,"An interactor was not provided");

    }
}

ProcessModuleBase::~ProcessModuleBase() {

}

int ProcessModuleBase::Process(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters)
{
	int res=0;

	timer.Tic();
	res=ProcessCore(img,parameters);
	timer.Toc();

	return res;
}

int ProcessModuleBase::Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
	int res=0;

	timer.Tic();
	res=ProcessCore(img,parameters);
	timer.Toc();

    return res;
}

void ProcessModuleBase::resetTimer()
{
    timer.reset();
}

const std::vector<Publication> &ProcessModuleBase::publicationList() const
{
    return publications;
}

int ProcessModuleBase::ProcessCore(kipl::base::TImage<float,2> &UNUSED(img), std::map<std::string,std::string> & UNUSED(parameters))
{
	std::ostringstream msg;

	msg<<"Processing module "<<m_sModuleName<<" does not support 2D processing.";
	throw ModuleException(msg.str(),__FILE__,__LINE__);

	return 0;
}

int ProcessModuleBase::ProcessCore(kipl::base::TImage<float,3> &UNUSED(img), std::map<std::string,std::string> &UNUSED(parameters))
{
	std::ostringstream msg;

	msg<<"Processing module "<<m_sModuleName<<" does not support 3D processing.";
	throw ModuleException(msg.str(),__FILE__,__LINE__);

	return 0;
}

int ProcessModuleBase::SourceVersion()
{
	return kipl::strings::VersionNumber("$Rev$");
}

bool ProcessModuleBase::UpdateStatus(float val, std::string msg)
{
    if (m_Interactor!=nullptr) {
        return m_Interactor->SetProgress(val,msg);
    }

    return false;
}
