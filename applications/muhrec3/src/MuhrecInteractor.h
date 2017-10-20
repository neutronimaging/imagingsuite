/*
 * MuhrecInteractor.h
 *
 *  Created on: Dec 6, 2011
 *      Author: anders
 */

#ifndef MUHRECINTERACTOR_H_
#define MUHRECINTERACTOR_H_
#include <QMutex>
#include <interactors/interactionbase.h>


class MuhrecInteractor: public kipl::interactors::InteractionBase {
public:
	MuhrecInteractor();
	virtual ~MuhrecInteractor();
	virtual void Reset();
	virtual void Abort();
	virtual bool Aborted();
	virtual void Done();
	virtual bool Finished();

	/// Updates the status of the reconstruction, to be called by the engine and back-projector
	///
	/// Returns true if the process should be aborted.
	virtual bool SetProgress(float progress, std::string msg="");

    /// Updates the status of the reconstruction, to be called by the engine and back-projector
    ///
    /// Returns true if the process should be aborted.
    virtual bool SetOverallProgress(float progress);

	virtual float CurrentProgress();
	virtual std::string CurrentMessage();

protected:
    QMutex m_Mutex;
};

#endif /* MUHRECINTERACTOR_H_ */
