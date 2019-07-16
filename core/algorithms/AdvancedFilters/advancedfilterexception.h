//<LICENSE>

#ifndef ADVANCEDFILTEREXCEPTION_H
#define ADVANCEDFILTEREXCEPTION_H

#include "advancedfilters_global.h"
#include <string>

class ADVANCEDFILTERSSHARED_EXPORT AdvancedFiltersException {
public:
    virtual ~AdvancedFiltersException();
    AdvancedFiltersException(std::string msg);
    AdvancedFiltersException(std::string msg, std::string filename, size_t line);

    const char* what() const;
protected :
    std::string sExceptionName;
    std::string sMessage;
    std::string sFileName;
    size_t nLineNumber;
};


#endif // ADVANCEDFILTEREXCEPTION_H
