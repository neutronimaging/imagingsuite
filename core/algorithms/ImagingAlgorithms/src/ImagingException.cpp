//<LICENSE>

#include <sstream>
#include "../include/ImagingException.h"

ImagingException::~ImagingException()
{
}

ImagingException::ImagingException(std::string msg) :
    kipl::base::KiplException (msg,"ImagingException")
{
}

ImagingException::ImagingException(std::string msg, std::string filename, size_t line) :
    kipl::base::KiplException (msg,filename,line,"ImagingException")
{
}


