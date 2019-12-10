//<LICENCE>

#include <string>
#include <sstream>
#include <QDebug>
#include "../../include/base/KiplException.h"

namespace kipl
{

namespace base
{

KiplException::~KiplException()
{
}

KiplException::KiplException(const std::string &message,const std::string &exname) :
    sExceptionName(exname),
    sMessage(message),
    nLineNumber(0)
{}

KiplException::KiplException(const std::string &message,
        const std::string &filename,
        size_t linenumber,
        const std::string &exname) :
    sExceptionName(exname),
    sMessage(message),
    sFileName(filename),
    nLineNumber(linenumber)
{
    std::ostringstream msg;
    msg<<"A "<<sExceptionName<<" was thrown in file "<<sFileName<<" on line "<<nLineNumber<<": \n"<<sMessage;

    sFormattedMessage = msg.str();
}

const char* KiplException::what() const noexcept
{
	
	if (nLineNumber==0) {
        return sMessage.c_str();
	}

    if (sFileName.empty())
        return sMessage.c_str();

    return sFormattedMessage.c_str();
}


DimsException::DimsException(std::string message) :
    KiplException(message, "DimsException")
{}

DimsException::DimsException(std::string message,
        std::string filename,
        const size_t linenumber) :
    KiplException(message,filename, linenumber, "DimsException")
{}
}}
