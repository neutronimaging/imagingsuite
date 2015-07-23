#ifndef DATAMODULEBASE_H
#define DATAMODULEBASE_H

#include <logging/logger.h>
#include <string>

class DataModuleBase
{
private:
    kipl::logging::Logger logger;
    static int m_nIDCount;
    const int m_nID;
    const std::string m_nModuleType;
    const std::string m_sModuleName;

public:
    DataModuleBase(std::string ModuleType="DataModuleBase", std::string ModuleName="NoName",int ID=-1);
    virtual ~DataModuleBase();
    int GetID();
    std::string GetName();


};


#endif // DATAMODULEBASE_H

