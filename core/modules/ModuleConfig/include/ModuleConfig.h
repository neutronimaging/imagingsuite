//<LICENSE>

#ifndef MODULECONFIG_H_
#define MODULECONFIG_H_
#include "ModuleConfig_global.h"
#include <map>
#include <list>
#include <array>
#include <string>
#include <iostream>

#include <libxml/xmlreader.h>
#include <logging/logger.h>
#include "publication.h"

#include "modulelibnamemanager.h"

/// Container for module configuration information.
/// This information is used in the configuration struct based on ConfigBase.
class MODULECONFIGSHARED_EXPORT ModuleConfig
{
protected:
    kipl::logging::Logger logger; ///< Logging for the Module config objects

public:
    ModuleConfig(const std::string &modulePath, const std::string &sCategory);           ///< Default constructor
    std::string m_sSharedObject;  ///< File name of the shared object file containing the module
    std::string m_sModule;        ///< Name of the module
	bool m_bActive;
    bool m_bThreading;

    std::map<std::string,std::string> parameters; ///< List of parameters to configure the module

    /// Serialization of the module description
    /// \param indent number of spaces used for the formatting indent of the configuration block.
    const std::string WriteXML(int indent);
    /// Parser for a module block in an XML formatted configuration file.
    /// \param Reference to an xml reader struct for the opened file.
    void ParseModule(xmlTextReaderPtr reader);

    /// Dumps the module parameters on the console.
	std::string PrintParameters();
    void setAppPath(const std::string &path, const std::string &category);
    std::string modulePath();
    std::string moduleSummary();
protected:

    ModuleLibNameManager m_NameManager;
    std::string m_CategoryName;
};


std::ostream & operator<<(std::ostream &s, ModuleConfig &mc);

#endif /* MODULECONFIG_H_ */
