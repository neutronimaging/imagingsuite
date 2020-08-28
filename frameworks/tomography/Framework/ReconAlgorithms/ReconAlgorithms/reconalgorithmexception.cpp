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
