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

