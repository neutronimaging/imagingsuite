//<LICENCE>

#include "stdafx.h"
#undef max
#include "MuhrecInteractor.h"
#include <QMutexLocker>

MuhrecInteractor::MuhrecInteractor() {

}

MuhrecInteractor::~MuhrecInteractor() {

}

void MuhrecInteractor::Reset()
{
    QMutexLocker ml(&m_Mutex);
	InteractionBase::Reset();
}

void MuhrecInteractor::Abort()
{
    QMutexLocker ml(&m_Mutex);
	InteractionBase::Abort();
}

void MuhrecInteractor::Done()
{
    QMutexLocker ml(&m_Mutex);
	InteractionBase::Done();
}

bool MuhrecInteractor::Finished()
{
    QMutexLocker ml(&m_Mutex);
	return InteractionBase::Finished();
}

bool MuhrecInteractor::SetProgress(float progress, std::string msg)
{
    QMutexLocker ml(&m_Mutex);
	return InteractionBase::SetProgress(progress,msg);
}

bool MuhrecInteractor::SetOverallProgress(float progress)
{
    QMutexLocker ml(&m_Mutex);
    return InteractionBase::SetOverallProgress(progress);
}

float MuhrecInteractor::CurrentProgress()
{
    QMutexLocker ml(&m_Mutex);
	return InteractionBase::CurrentProgress();
}

std::string MuhrecInteractor::CurrentMessage()
{
    QMutexLocker ml(&m_Mutex);
    return InteractionBase::CurrentMessage();
}

bool MuhrecInteractor::Aborted()
{
    QMutexLocker ml(&m_Mutex);
	return InteractionBase::Aborted();
}
