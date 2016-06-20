#ifndef DATAMODULEBASE_H
#define DATAMODULEBASE_H

#include <logging/logger.h>
#include <string>

/// \brief A base function for data carrying modules. To be used as variables in a future framework.
class DataModuleBase
{
private:
    kipl::logging::Logger logger;       ///< Logger instance for the DataModules
    static int m_nIDCount;              ///< Instance counter to provide unique ids for each data module
    const int m_nID;                    ///< Unique id number for the instance
    const std::string m_nModuleType;    ///< Indicates which data type the module carries
    const std::string m_sModuleName;    ///< Variable name of the module

public:
    /// \brief C'tor to initalize a data module
    /// \param ModuleType Sets the type of module
    /// \param ModuleName Unique name of the module
    /// \param ID sets a id number to the module. An automatic number will assigned if ID=-1.
    DataModuleBase(std::string ModuleType="DataModuleBase", std::string ModuleName="NoName",int ID=-1);
    virtual ~DataModuleBase();

    /// \brief Getter for the id number
    /// \returns the id number of the object
    int GetID();

    /// \brief Getter for the object name
    /// \returns the name of the object
    std::string GetName();


};


#endif // DATAMODULEBASE_H

