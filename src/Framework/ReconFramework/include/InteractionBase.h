//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//

#ifndef __INTERACTIONBASE_H
#define __INTERACTIONBASE_H

#include "ReconFramework_global.h"
//#include "stdafx.h"
#include <string>

#include <logging/logger.h>
/// The class provides a link between processing modules and a GUI. The base class shall be free of GUI code.
/// The interactor shall be thread safe.
class RECONFRAMEWORKSHARED_EXPORT InteractionBase
{
    kipl::logging::Logger logger; ///< The logger for messages from the interactor
public:
    /// Constructor to initialize the interactor class
    /// \param name The name of the interactor class. Mostly set throgh the refining class constructor.
	InteractionBase(std::string name="InteractorBase");

    /// Resets the status of the interactor, to be called when a session is started.
	virtual void Reset();
    /// Sets the abort flag to terminate the running process, to be called from the GUI side.
	virtual void Abort();
    /// Sets the done flag to indicate that the processing has finished, to be called from the processing side.
	virtual void Done();
    /// Updates the status of the reconstruction, to be called by the engine and back-projector.
    /// \param progress a fraction telling the progress of the process.
    /// \param msg An additional message to include in the progress dialog.
    /// \returns true if the process should be aborted.
	virtual bool SetProgress(float progress, std::string msg="");
    /// Indicator that tells if the process has been interupted by the user.
	virtual bool Aborted();
    /// Indicator that tells if the process has ended.
	virtual bool Finished();

    /// The current progress value.
	virtual float CurrentProgress();
    /// The current message.
	virtual std::string CurrentMessage();

protected:
    bool m_bAbort;  ///< State variable that holds the abort status (Set using Abort() and checked using Aborted()).
    bool m_bFinished;   ///< State variable that holds the finished status (Set using Done() and checked using Finished().
    float m_fProgress;  ///< Progress counter a value in the interval 0.0-1.0.
    std::string m_sMessage; ///< The progress message currently shown.
};


#endif
