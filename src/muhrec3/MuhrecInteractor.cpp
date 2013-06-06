/*
 * MuhrecInteractor.cpp
 *
 *  Created on: Dec 6, 2011
 *      Author: anders
 */
#include "stdafx.h"
#undef max
#include "MuhrecInteractor.h"

MuhrecInteractor::MuhrecInteractor() {

}

MuhrecInteractor::~MuhrecInteractor() {

}

void MuhrecInteractor::Reset()
{
    //Glib::Mutex::Lock lock(m_Mutex);
	InteractionBase::Reset();
}

void MuhrecInteractor::Abort()
{
    //Glib::Mutex::Lock lock(m_Mutex);
	InteractionBase::Abort();
}

void MuhrecInteractor::Done()
{
    //Glib::Mutex::Lock lock(m_Mutex);
	InteractionBase::Done();
}

bool MuhrecInteractor::Finished()
{
    //Glib::Mutex::Lock lock(m_Mutex);
	return InteractionBase::Finished();
}

bool MuhrecInteractor::SetProgress(float progress, std::string msg)
{
    //Glib::Mutex::Lock lock(m_Mutex);
	return InteractionBase::SetProgress(progress,msg);
}


float MuhrecInteractor::CurrentProgress()
{
    //Glib::Mutex::Lock lock(m_Mutex);
	return InteractionBase::CurrentProgress();
}

std::string MuhrecInteractor::CurrentMessage()
{
    //Glib::Mutex::Lock lock(m_Mutex);
	return InteractionBase::CurrentMessage();
}

bool MuhrecInteractor::Aborted()
{
    //Glib::Mutex::Lock lock(m_Mutex);
	return InteractionBase::Aborted();
}
