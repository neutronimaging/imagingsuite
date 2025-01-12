//<LICENSE>

#include "../include/ReplaceUnderexposedModule.h"
#include <ModuleException.h>
#include <tuple>
#include <ParameterHandling.h>
#include <strings/miscstring.h>
ReplaceUnderexposedModule::ReplaceUnderexposedModule(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("ReplaceUnderexposed",interactor),
    threshold(1000.0f)
{

}

int ReplaceUnderexposedModule::Configure(ReconConfig config, std::map<string, string> parameters)
{
    std::ignore = config;

    threshold = std::stof(GetStringParameter(parameters,"threshold"));

    return 1;
}

std::map<string, string> ReplaceUnderexposedModule::GetParameters()
{
    std::map<string, string> parameters;
    parameters["threshold"]=std::to_string(threshold);

    return parameters;
}

bool ReplaceUnderexposedModule::SetROI(const std::vector<size_t> &roi)
{
    std::ignore = roi;
    return false;
}

int ReplaceUnderexposedModule::ProcessCore(kipl::base::TImage<float, 2> &img,
                                                  std::map<string, string> &coeff)
{
    std::ignore = img;
    std::ignore = coeff;

    throw ModuleException("Process core is not implemented for 2D images");

    return 0;
}

int ReplaceUnderexposedModule::ProcessCore(kipl::base::TImage<float, 3> &img, std::map<string, string> &coeff)
{
    std::ostringstream msg;
    std::vector<float> doses;
    GetFloatParameterVector(coeff,"dose",doses,img.Size(2));
    msg<<"Got "<<doses.size()<<" dose values";
    logger.message(msg.str());

    psc.process(img,doses,threshold);

    ostringstream dosestr;

    for (const auto & dose : doses)
        dosestr << dose << " ";

    coeff["dose"]=dosestr.str();

    return 1;
}
