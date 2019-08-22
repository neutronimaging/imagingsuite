#include <sstream>

#include "reconalgorithmexception.h"


ReconAlgorithmException::~ReconAlgorithmException()
{}

ReconAlgorithmException::ReconAlgorithmException(std::string msg) :
    kipl::base::KiplException(msg,"ReconAlgorithmException")
{

}

ReconAlgorithmException::ReconAlgorithmException(std::string msg, std::string filename, size_t line) :
    kipl::base::KiplException(msg, filename, line,"ReconAlgorithmException")
{

}

const char* ReconAlgorithmException::what() const _NOEXCEPT
{
    if (sFileName.empty())
        return sMessage.c_str();
    else {
        std::ostringstream str;
        str<<"ReconAlgorithm exception in "<<sFileName<<" on line "<<nLineNumber<<": \n"<<sMessage;
        return str.str().c_str();
    }
}
