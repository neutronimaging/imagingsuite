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
	InteractionBase::Reset();
}

void MuhrecInteractor::Abort()
{
	InteractionBase::Abort();
}

void MuhrecInteractor::Done()
{

	InteractionBase::Done();
}

bool MuhrecInteractor::Finished()
{
	return InteractionBase::Finished();
}

bool MuhrecInteractor::SetProgress(float progress, std::string msg)
{
	return InteractionBase::SetProgress(progress,msg);
}

bool MuhrecInteractor::SetOverallProgress(float progress)
{
    return InteractionBase::SetOverallProgress(progress);
}

float MuhrecInteractor::CurrentProgress()
{
	return InteractionBase::CurrentProgress();
}

std::string MuhrecInteractor::CurrentMessage()
{
    return InteractionBase::CurrentMessage();
}

bool MuhrecInteractor::Aborted()
{
	return InteractionBase::Aborted();
}
