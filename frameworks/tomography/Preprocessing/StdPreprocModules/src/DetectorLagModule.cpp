//<LICENSE>

#include <sstream>

#include "../include/DetectorLagModule.h"
#include <ParameterHandling.h>
#include <ModuleException.h>

DetectorLagModule::DetectorLagModule() :
PreprocModuleBase("DetectorLag"),
	fCorrectionFactor(0.0f),
	iMedianKernelSize(3)
{

}

DetectorLagModule::~DetectorLagModule() {
}


int DetectorLagModule::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    std::ostringstream msg;

    try {
        fCorrectionFactor = GetFloatParameter(parameters,"correctionFactor");
    }
    catch (ModuleException & e) {
        fCorrectionFactor = 0.0f;
        msg.str("");
        msg<<"No correction factor found setting default =0 (exception message:"<<e.what()<<")";
        logger(logger.LogWarning,msg.str());
    }

    try {
        iMedianKernelSize = GetFloatParameter(parameters,"medianKernelSize");
    }
    catch (ModuleException & e) {
        iMedianKernelSize = 3;
        msg.str("");
        msg<<"No median kernel size found setting default =3 (exception message:"<<e.what()<<")";
        logger(logger.LogWarning,msg.str());
    }

    // Pass configured values
    dl_correction.setup(fCorrectionFactor, iMedianKernelSize);

	return 0;
}

std::map<std::string, std::string> DetectorLagModule::GetParameters()
{
	std::map<std::string, std::string> parameters;

    parameters["correctionFactor"]=fCorrectionFactor;
    parameters["medianKernelSize"]=iMedianKernelSize;

	return parameters;
}

bool DetectorLagModule::SetROI(size_t *roi)
{
	return false;
}

int DetectorLagModule::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
    dl_correction.process(img);

	return 0;
}

int DetectorLagModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	return 0;
}
