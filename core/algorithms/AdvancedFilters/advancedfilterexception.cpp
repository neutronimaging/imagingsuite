//<LICENSE>

#include <sstream>
#include "advancedfilterexception.h"

AdvancedFiltersException::~AdvancedFiltersException()
{
}

AdvancedFiltersException::AdvancedFiltersException(std::string msg) :
    kipl::base::KiplException(msg,"AdvancedFiltersException")
{
}

AdvancedFiltersException::AdvancedFiltersException(std::string msg, std::string filename, size_t line) :
    kipl::base::KiplException(msg,filename,line,"AdvancedFiltersException")
{
}

