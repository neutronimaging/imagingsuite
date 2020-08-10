//<LICENSE>

#include "../include/ModuleConfig_global.h"
#include "../include/ModuleException.h"
#include <string>
#include <sstream>

ModuleException::~ModuleException(void)
{
}

ModuleException::ModuleException(std::string msg) : kipl::base::KiplException(msg,"ModuleException")
{}

ModuleException::ModuleException(std::string msg, std::string filename, size_t line) :
        kipl::base::KiplException(msg,filename,line,"ModuleException")
{}

