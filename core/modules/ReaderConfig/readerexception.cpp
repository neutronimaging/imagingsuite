#include "readerexception.h"

#include <string>
#include <sstream>

ReaderException::ReaderException(void) :
    kipl::base::KiplException("Undeclared Exception","ReaderException")
{
}

ReaderException::~ReaderException(void)
{
}

ReaderException::ReaderException(std::string msg) : kipl::base::KiplException(msg)
{}

ReaderException::ReaderException(std::string msg, std::string filename, size_t line) :
kipl::base::KiplException(msg,filename,line,"ReaderException")
{}

std::string ReaderException::what()
{
    if (sFileName.empty())
        return sMessage;
    else {
        std::stringstream str;
        str<<"Reader exception in "<<sFileName<<" on line "<<nLineNumber<<": \n"<<sMessage;
        return str.str();
    }
}
