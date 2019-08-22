//<LICENSE>
#include "stdafx.h"
#include "../include/ReconException.h"
#include <string>
#include <sstream>

ReconException::ReconException(void) :
    kipl::base::KiplException("Undeclared Exception","ReconException")
{
}

ReconException::~ReconException(void)
{
}

ReconException::ReconException(std::string msg) : kipl::base::KiplException(msg)
{}

ReconException::ReconException(std::string msg, std::string filename, size_t line) :
kipl::base::KiplException(msg,filename,line,"ReconException")
{}

const char* ReconException::what() const _NOEXCEPT
{
	if (sFileName.empty())
        return sMessage.c_str();
	else {
		std::stringstream str;
		str<<"Reconstructor exception in "<<sFileName<<" on line "<<nLineNumber<<": \n"<<sMessage;
        return str.str().c_str();
	}
}
