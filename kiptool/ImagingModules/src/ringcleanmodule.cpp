#include <string>
#include <map>

#include <base/textractor.h>
#include <ParameterHandling.h>
#include <strings/miscstring.h>

#include "ringcleanmodule.h"

RingCleanModule::RingCleanModule() : KiplProcessModuleBase("RingCleanModule",false),
    m_StripeFilter(nullptr),
    wname("daub15"),
    scale(3),
    sigma(0.05),
    op(ImagingAlgorithms::VerticalComponentFFT),
    plane(kipl::base::ImagePlaneXY)
{

}

RingCleanModule::~RingCleanModule()
{}

int RingCleanModule::Configure(std::map<std::string, std::string> parameters)
{
    wname=GetStringParameter(parameters,"waveletname");
    scale=GetIntParameter(parameters,"scale");
    sigma=GetFloatParameter(parameters,"sigma");
    string2enum(GetStringParameter(parameters,"filtertype"), op);
    string2enum(GetStringParameter(parameters,"plane"), plane);

    return 0;
}

std::map<std::string, std::string> RingCleanModule::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["waveletname"]=wname;
    parameters["scale"]=kipl::strings::value2string(scale);
    parameters["sigma"]=kipl::strings::value2string(sigma);
    parameters["filtertype"]=enum2string(op);
    parameters["plane"]=enum2string(plane);

    return parameters;
}

int RingCleanModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    size_t Nslices=0;

    switch (plane) {
    case kipl::base::ImagePlaneXY:
        Nslices=img.Size(2);
        break;
    case kipl::base::ImagePlaneXZ:
        Nslices=img.Size(1);
        break;
    case kipl::base::ImagePlaneYZ:
        Nslices=img.Size(0);
        break;
    default:
        break;
    }

    kipl::base::TImage<float,2> slice;
    slice=kipl::base::ExtractSlice(img,0,plane,nullptr);

    m_StripeFilter = new ImagingAlgorithms::StripeFilter(slice.Dims(),wname,scale,sigma);

    for (size_t i=0; i<Nslices; i++) {
        slice=kipl::base::ExtractSlice(img,i,plane,nullptr);
        m_StripeFilter->Process(slice,op);
        kipl::base::InsertSlice(slice,img,i,plane);
    }

    delete m_StripeFilter;

    m_StripeFilter=nullptr;

    return 0;
}

