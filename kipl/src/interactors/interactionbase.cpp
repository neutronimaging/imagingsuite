//<LICENCE>

#include "../../include/kipl_global.h"
#include "../include/interactors/interactionbase.h"

namespace kipl { namespace interactors {
InteractionBase::InteractionBase(std::string name) : 
	logger(name),
	m_bAbort(false),
	m_bFinished(false),
	m_fProgress(0.0f)
{}

void InteractionBase::Reset() 
{
	m_bAbort=false; 
	m_bFinished=false;
	m_fProgress=0.0f;
    m_fOverallProgress=0.0f;
    m_sMessage="";
}

void InteractionBase::Abort() 
{
	m_bAbort=true;
}

void InteractionBase::Done()
{
	m_bFinished=true;
}

bool InteractionBase::SetProgress(float progress, std::string msg)
{
	m_fProgress=progress;
	m_sMessage=msg;

	return m_bAbort;
}

bool InteractionBase::SetOverallProgress(float progress)
{
    m_fOverallProgress=progress;

    return m_bAbort;
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
	return m_bAbort;
}

bool InteractionBase::Finished()
{
	return m_bFinished;
}
}}
