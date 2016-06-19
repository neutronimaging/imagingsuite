//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $File$
// $Date: 2014-11-27 09:03:06 +0100 (Thu, 27 Nov 2014) $
// $Rev: 1215 $
// $Id: InteractionBase.h 1215 2014-11-27 08:03:06Z kaestner $
//

#ifndef __KIPLINTERACTIONBASE_H
#define __KIPLINTERACTIONBASE_H

#include "../kipl_global.h"
#include <string>

#include "../logging/logger.h"



namespace kipl {
/// \brief Provides interaction functionality between a user interface and processing components
namespace interactors {

/// The class provides a link between processing modules and a GUI. The base class shall be free of GUI code.
/// The interactor shall be implemented to be thread safe.
class KIPLSHARED_EXPORT InteractionBase
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

    /// Updates the status of the reconstruction, to be called by the processing engine.
    /// \param progress a fraction telling the main program about the progress of the process.
    /// \param msg An additional message to include in the progress dialog.
    /// \returns Tells how to proceed.
    /// \retval true The process should be aborted
    /// \retval false continue processing
    virtual bool SetProgress(float progress, std::string msg="");

    /// Indicator that tells if the process has been interupted by the user.
    /// \returns Finished status
    /// \retval true The user has called the process to abort
    /// \retval false No abort request
    virtual bool Aborted();
    /// Indicator that tells if the process has ended.
    /// \returns Finished status
    /// \retval true The process finished
    /// \retval false The process is still running
	virtual bool Finished();

    /// The current progress value.
    /// \returns The current progress as a value between 0 and 1.
	virtual float CurrentProgress();
    /// The current message.
	virtual std::string CurrentMessage();

protected:
    bool m_bAbort;          ///< State variable that holds the abort status (Set using Abort() and checked using Aborted()).
    bool m_bFinished;       ///< State variable that holds the finished status (Set using Done() and checked using Finished().
    float m_fProgress;      ///< Progress counter a value in the interval 0.0-1.0.
    std::string m_sMessage; ///< The progress message currently shown.
};
}}

#endif
