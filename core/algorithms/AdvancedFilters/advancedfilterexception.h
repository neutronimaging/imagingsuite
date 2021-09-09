//<LICENSE>

#ifndef ADVANCEDFILTEREXCEPTION_H
#define ADVANCEDFILTEREXCEPTION_H

#include "advancedfilters_global.h"
#include <base/KiplException.h>
#include <string>

class ADVANCEDFILTERSSHARED_EXPORT AdvancedFiltersException : public kipl::base::KiplException
{
public:
    virtual ~AdvancedFiltersException();
    AdvancedFiltersException(std::string msg);
    AdvancedFiltersException(std::string msg, std::string filename, size_t line);
};


#endif // ADVANCEDFILTEREXCEPTION_H
