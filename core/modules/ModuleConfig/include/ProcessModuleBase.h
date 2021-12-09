//<LICENSE>

#ifndef PROCESSMODULEBASE_H_
#define PROCESSMODULEBASE_H_

#include "ModuleConfig_global.h"

#include "publication.h"

#include <string>
#include <map>
#include <atomic>

#include <base/timage.h>
#include <profile/Timer.h>
#include <logging/logger.h>
#include <strings/miscstring.h>
#include <interactors/interactionbase.h>
#include <algorithm>

/// Base class for all processing modules. This class will allways be refined for each processing framework.
/// The class provides an interface to the processing engine.
class MODULECONFIGSHARED_EXPORT ProcessModuleBase {
protected:
    kipl::logging::Logger logger; ///< The logger for convenient logging.
public:
    /// Constructor to initialize basic information.
    /// \param name The name of the module. The name will be obtained from the constructor of the refining class. It is mainly used for the logging.
    ProcessModuleBase(std::string name="ProcessModuleBase", kipl::interactors::InteractionBase *interactor=nullptr);

    /// Destructor
    virtual ~ProcessModuleBase();

    /// Processing of 2D images, calls the Process core method for 2D images.
    /// \param img The image to process. The method acts inplace, i.e. the result is returned through this method.
    /// \param parameters A list of parameters provided by the framework.
    /// \returns The result of the process core method. Usually a 0 is returned.
	virtual int Process(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters);

    /// Processing of 3D images, calls the Process core method for 3D images.
    /// \param img The image to process. The method acts inplace, i.e. the result is returned through this method.
    /// \param parameters A list of parameters provided by the framework.
    /// \returns The result of the process core method. Usually a 0 is returned.
    virtual int Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);

    /// Configure the module using parameters from the list provided by ModuleConfig.
    /// \param parameters A list of parameters
    /// \returns an integer value that indicates the status. Usually only a 0.
    /// \throws A ModuleException when the a parameter is missing.
	virtual int Configure(std::map<std::string, std::string> parameters)=0;

    /// Initializes the module for processing.
    /// \returns It returns a zero.
	virtual int Initialize() {timer.reset(); return 0;}

    /// Provides a list of parameters that the module requires. This method is used by the module creation method as a guide.
	virtual std::map<std::string, std::string> GetParameters()=0;

    /// The name of the current module.
    /// \returns A string containing the module name
	std::string ModuleName() {return m_sModuleName;}

    /// The time used to process data wth the module.
    /// \returns The elapsed time in seconds.
    double execTime() {return timer.cumulativeTime();}
    void resetTimer();

    /// The repository verision of the module base and its module instance.
    /// \returns A string containing the version.
	virtual std::string Version() {
		ostringstream s;
    //	s<<"ProcessModuleBase ("<<std::max(kipl::strings::VersionNumber("$Rev$"), ProcessModuleBase::SourceVersion())<<")";

		return s.str();
	}

    const std::vector<Publication> & publicationList() const;
protected:

    /// Method that executes the algorithmic core of the processing for 2D images.
    /// \throws A module exception if the method is called without refinement.
	virtual int ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters);

    /// Method that executes the algorithmic core of the processing for 3D images.
    /// \throws A module exception if the method is called without refinement.
	virtual int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);

    /// Interface to a progress bar in a GUI.
    /// \param val a fraction value 0.0-1.0 to tell the progress of the back-projection.
    /// \param msg a message string to add information to the progress bar.
    /// \returns The abort status of interactor object. True means abort back-projection and false continue.
    bool UpdateStatus(float val, std::string msg);

    std::string m_sModuleName;   ///< The name of the module
    kipl::profile::Timer timer;  ///< Timer object to measure the execution time.
    virtual int SourceVersion(); ///< \returns the value of the repository version of the source code.

    kipl::interactors::InteractionBase *m_Interactor;               ///< Interface to a progress bar in the GUI.
    std::vector<Publication> publications;
    bool m_bThreading;
    std::atomic_int m_nCounter;
};


#endif /* PROCESSMODULEBASE_H_ */
