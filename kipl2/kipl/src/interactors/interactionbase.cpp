//<LICENCE>

#include "../../include/kipl_global.h"
#include "../include/interactors/interactionbase.h"

namespace kipl { namespace interactors {
InteractionBase::InteractionBase(std::string name) : 
	logger(name),
    m_Status(interactorIdle),
	m_fProgress(0.0f)
{}

void InteractionBase::Reset() 
{
    m_Status=interactorIdle;
	m_fProgress=0.0f;
    m_fOverallProgress=0.0f;
    m_sMessage="";
}

void InteractionBase::Abort() 
{
//    std::cout<<"Aborting"<<std::endl;
    m_Status = interactorAborted;
}

void InteractionBase::Done()
{
//    std::cout<<"Done"<<std::endl;
    m_Status = interactorFinished;
}

bool InteractionBase::SetProgress(float progress, std::string msg)
{
	m_fProgress=progress;
	m_sMessage=msg;
//    std::cout<<"Progress msg="<<m_sMessage<<", progress="<<m_fProgress<<std::endl;

    return m_Status == interactorAborted;
}

bool InteractionBase::SetOverallProgress(float progress)
{
//    std::cout<<"Overall Progress "<<m_fProgress<<std::endl;
    m_fOverallProgress=progress;

    return m_Status == interactorAborted;
}

float InteractionBase::CurrentProgress() 
{
	return m_fProgress;
}

float InteractionBase::CurrentOverallProgress()
{
    return m_fOverallProgress;
}

std::string InteractionBase::CurrentMessage()
{
	return m_sMessage;
}

bool InteractionBase::Aborted()
{
    return m_Status == interactorAborted;
}

bool InteractionBase::Finished()
{
    return m_Status == interactorFinished;
}
}}
