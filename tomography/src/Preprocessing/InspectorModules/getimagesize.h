//<LICENSE>

#ifndef GETIMAGESIZE_H
#define GETIMAGESIZE_H

#include <PreprocModuleBase.h>
#include <ReconConfig.h>

class GetImageSize : public PreprocModuleBase
{
public:
    GetImageSize();
    ~GetImageSize();

    virtual std::map<std::basic_string<char>, std::basic_string<char> > GetParameters();
    virtual int Configure(ReconConfig config, std::map<std::basic_string<char>, std::basic_string<char> > parameters);
protected:

    virtual int ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters);
    virtual int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);

};

#endif // GETIMAGESIZE_H
