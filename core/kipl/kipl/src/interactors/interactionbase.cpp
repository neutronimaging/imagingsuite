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
    std::lock_guard<std::mutex> guard(m_Mutex);

    m_Status=interactorIdle;
	m_fProgress=0.0f;
    m_fOverallProgress=0.0f;
    m_sMessage="";
}

void InteractionBase::Abort() 
{
    std::lock_guard<std::mutex> guard(m_Mutex);

    m_Status = interactorAborted;
}

void InteractionBase::Done()
{
    std::lock_guard<std::mutex> guard(m_Mutex);

    m_Status = interactorFinished;
}

bool InteractionBase::SetProgress(float progress, std::string msg)
{
    std::lock_guard<std::mutex> guard(m_Mutex);

	m_fProgress=progress;
	m_sMessage=msg;

    return m_Status == interactorAborted;
}

bool InteractionBase::SetOverallProgress(float progress)
{
    std::lock_guard<std::mutex> guard(m_Mutex);

    m_fOverallProgress=progress;

    return m_Status == interactorAborted;
}

float InteractionBase::CurrentProgress() 
{
    std::lock_guard<std::mutex> guard(m_Mutex);

    return m_fProgress;
}

float InteractionBase::CurrentOverallProgress()
{
    std::lock_guard<std::mutex> guard(m_Mutex);

    return m_fOverallProgress;
}

std::string InteractionBase::CurrentMessage()
{
    std::lock_guard<std::mutex> guard(m_Mutex);

	return m_sMessage;
}

bool InteractionBase::Aborted()
{
    std::lock_guard<std::mutex> guard(m_Mutex);
    return m_Status == interactorAborted;
}

bool InteractionBase::Finished()
{
    std::lock_guard<std::mutex> guard(m_Mutex);

    return m_Status == interactorFinished;
}
}}
