//<LICENCE>

#include <string>
#include <sstream>

#include "../../include/base/KiplException.h"

namespace kipl
{

namespace base
{

//KiplException::KiplException(std::string exname)
//{
//}

KiplException::~KiplException()
{
}

KiplException::KiplException(const std::string message,const std::string exname) :
    sExceptionName(exname),
    sMessage(message),
    nLineNumber(0)
{}

KiplException::KiplException(std::string message,
        std::string filename,
        size_t linenumber,
        std::string exname) :
    sExceptionName(exname),
    sMessage(message),
    sFileName(filename),
    nLineNumber(linenumber)
{
}

std::string KiplException::what()
{
	
	if (nLineNumber==0) {
		return sMessage;	
	}

    if (sFileName.empty())
        return sMessage;

	std::ostringstream str;
	
    str<<"An "<<sExceptionName<<" was thrown in file "<<sFileName<<" on line "<<nLineNumber<<": "<<std::endl<<sMessage;
	
	return str.str();
}


}}
