//<LICENSE>

#include <sstream>

#include "getimagesize.h"

GetImageSize::GetImageSize() :
    PreprocModuleBase("GetImageSize")
{

}

GetImageSize::~GetImageSize()
{

}

std::map<std::basic_string<char>, std::basic_string<char> > GetImageSize::GetParameters()
{
    std::map<std::basic_string<char>, std::basic_string<char> > parameters;

    return parameters;
}

int GetImageSize::Configure(ReconConfig config, std::map<std::basic_string<char>, std::basic_string<char> > parameters)
{
    return 0;
}


int GetImageSize::ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters)
{
    std::ostringstream msg;

    msg<<"Image info: "<<img;

    logger(logger.LogMessage,msg.str());

    return 0;
}

int GetImageSize::ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
    std::ostringstream msg;

    msg<<"Image info: "<<img;

    logger(logger.LogMessage,msg.str());

    return 0;
}
