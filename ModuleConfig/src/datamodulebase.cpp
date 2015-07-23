#include "../../include/datamodulebase.h"

int DataModuleBase::m_nIDCount=0;

DataModuleBase::DataModuleBase(std::string ModuleType, std::string ModuleName, int ID) :
    logger(ModuleType),
    m_nID(ID < 0 ? m_nIDCount++ : ID),
    m_nModuleType(ModuleType),
    m_sModuleName(ModuleName)
{

}
DataModuleBase::~DataModuleBase()
{

}

int DataModuleBase::GetID()
{
    return m_nID;
}

std::string DataModuleBase::GetName()
{
    return m_sModuleName;
}
