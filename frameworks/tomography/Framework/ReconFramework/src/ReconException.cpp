//<LICENSE>

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

