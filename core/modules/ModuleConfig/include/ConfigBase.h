//<LICENSE>

#ifndef CONFIGBASE_H
#define CONFIGBASE_H

#include "ModuleConfig_global.h"

#include <map>
#include <list>
#include <set>
#include <string>
#include <vector>

#include <libxml/xmlreader.h>

#include "ModuleConfig.h"

#include <logging/logger.h>

/// Pure virtual base class with basic interface for processing configuartion
/// It provides an interface the load and save configurations formatted as XML.
/// The class requires libXML2 to compile.
class MODULECONFIGSHARED_EXPORT ConfigBase
{
    friend class ConfigBaseTest;
protected:
    kipl::logging::Logger logger;   ///< The logger instance for the config objects
    std::string m_sName;            ///< Name of the application
    std::string m_sApplicationPath; ///< Path to the application binary
                                    ///< Can be used to set default paths

public:
    /// Configuration containing basic project information.
    /// This information is mainly for documentation purposes.
    /// Some fields will be put in the resulting data file headers others can be used in the report generation.
    struct MODULECONFIGSHARED_EXPORT cUserInformation {
        /// \brief Initializes the information to defaults
		cUserInformation() ;
        /// \brief Copy C'tor
        /// \param info the info item to copy
		cUserInformation(const cUserInformation &info);

        /// \brief Assignment operator to copy the contents of another info object
        /// \param info the info object to copy
        /// \return a reference to itself.
		cUserInformation & operator=(const cUserInformation & info);
        std::string WriteXML(int indent=0);

        std::string sOperator;      ///< Name of the user who processed the data of did the experiment
        std::string sInstrument;    ///< Name of the instrument used to acquire the data
        std::string sProjectNumber; ///< Project identification number
        std::string sSample;        ///< Short sample description
        std::string sComment;       ///< Optional comment
        std::string sDate;          ///< Processing date
        std::string sVersion;       ///< Application version
	};

    /// Main constructor used to initialize a new configuration set
    /// \param name Name of the class, mainly used to help the logging system by identifying the class.
    /// \param path The application binary path.
    ConfigBase(std::string name="ConfigBase", std::string path="");
    /// Copy constructor
    /// \param config The instance to copy
    ConfigBase(const ConfigBase &config);
    /// Assignment operator
    /// \param config The instance to use for assignment
    virtual const ConfigBase & operator=(const ConfigBase &config);


    /// Destructor for cleaning up
	virtual ~ConfigBase(void);

    cUserInformation UserInformation;  ///< Instance of the configuration information
    std::vector<ModuleConfig> modules;   ///< List of processing module descriptions. Used to set up a processing engine.
    virtual std::string WriteXML()=0;  ///< Virtual method to used to stream an XML formatted string
                                       ///< \returns A string with an XML formatted configuration
    /// Loads an XML formatted configuration file. It calls a set of virutal parsing methods.
    /// \param configfile The file name of the parameter file to load.
    /// \param ProjectName Name to identify of the application to avoid loading the wrong configuration for the current application.
    void LoadConfigFile(std::string configfile, std::string ProjectName);

    /// \brief Compares if there are differences between the current config and a second one
    ///
    /// The comparision is done using the xml formatted strings
    /// \param freelist a list of parameters to exclude from the comparison
    bool ConfigChanged(ConfigBase & config, std::list<std::string> freelist);

    /// \brief Adopts the config using parameters provided form the CLI using the main function argument style
    /// This is a convenience function that calls GetCommandLinePars(std::vector<std::string> &args)
    /// \param argc Number of arguments in the list
    /// \param argv list of argument strings. The list starts at item 1 since 0 is taken by the application name.
    void GetCommandLinePars(int argc, char *argv[]);


    /// \brief Adopts the config using parameters provided form the CLI using an STL vector. This is used in combination with the QApplication object.
    /// \param args Number of arguments in the list
    void GetCommandLinePars(std::vector<std::string> &args);

    /// Sanity check on the number of slices to be reconstruct during Config
    virtual std::string SanitySlicesCheck()=0;

    void setAppPath(const std::string & path);
    std::string appPath();
protected:
    /// Parser for an opened XML formatted input file.
    /// \param reader Reference to an xml reader struct for the opened configuration file.
    /// \param sName Name of the config block to parse.
	virtual void ParseConfig(xmlTextReaderPtr reader, std::string sName)=0;

    /// Sanity message that warns about the number of slices that are being configured
    virtual std::string SanityMessage(bool mess)=0;


    /// Parse a list of arguments as they come from the CLI
    /// \param args The argument list
    virtual void ParseArgv(std::vector<std::string> &args);

    void EvalArg(std::string arg, std::string &group, std::string &var, std::string &value);

    /// Parse the User information config block.
    /// \param reader Reference to an xml reader struct for the opened configuration file.
	void ParseUserInformation(xmlTextReaderPtr reader);

    /// Parse the process chain configuration
    /// \param Reference to an xml reader struct for the opened configuration file.
	virtual void ParseProcessChain(xmlTextReaderPtr reader)=0;
};

#endif
