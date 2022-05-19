//<LICENSE>

#include "../include/projectionseriescorrectionmodule.h"
#include <ModuleException.h>
#include <tuple>
#include <ParameterHandling.h>
#include <strings/miscstring.h>
ProjectionSeriesCorrectionModule::ProjectionSeriesCorrectionModule(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("ProjectionSeriesCorrection",interactor),
    threshold(1000.0f)
{

}

int ProjectionSeriesCorrectionModule::Configure(ReconConfig config, std::map<string, string> parameters)
{
    std::ignore = config;

    threshold = std::stof(GetStringParameter(parameters,"threshold"));

    return 1;
}

std::map<string, string> ProjectionSeriesCorrectionModule::GetParameters()
{
    std::map<string, string> parameters;
    parameters["threshold"]=std::to_string(threshold);

    return parameters;
}

bool ProjectionSeriesCorrectionModule::SetROI(const std::vector<size_t> &roi)
{
    std::ignore = roi;
    return false;
}

int ProjectionSeriesCorrectionModule::ProcessCore(kipl::base::TImage<float, 2> &img,
                                                  std::map<string, string> &coeff)
{
    std::ignore = img;
    std::ignore = coeff;

    throw ModuleException("Process core is not implemented for 2D images");

    return 0;
}

int ProjectionSeriesCorrectionModule::ProcessCore(kipl::base::TImage<float, 3> &img, std::map<string, string> &coeff)
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
