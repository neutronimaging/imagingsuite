#include "../../include/base/KiplException.h"
#include <string>
#include <sstream>

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

KiplException::KiplException(const std::string message, 
		const std::string filename, 
        const size_t linenumber,
        const std::string exname) :
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

	std::ostringstream str;
	
    str<<sExceptionName<<" occured in file "<<sFileName<<" on line "<<nLineNumber<<": "<<std::endl<<sMessage;
	
	return str.str();
}


}}
