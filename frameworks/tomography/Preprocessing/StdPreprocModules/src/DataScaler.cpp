//<LICENSE>
#include <sstream>

#include "../include/DataScaler.h"
#include <ParameterHandling.h>
#include <ModuleException.h>

DataScaler::DataScaler() :
PreprocModuleBase("DataScaler"),
	fOffset(0.0f),
	fSlope(1.0f)
{

}

DataScaler::~DataScaler() {
}


int DataScaler::Configure(ReconConfig /*config*/, std::map<std::string, std::string> parameters)
{
    std::ostringstream msg;

    try {
        fOffset = GetFloatParameter(parameters,"offset");
    }
    catch (ModuleException & e) {
        fOffset = 0.0f;
        msg.str("");
        msg<<"No offset found setting default =0 (exception message:"<<e.what()<<")";
        logger(logger.LogWarning,msg.str());
    }

    try {
        fSlope = GetFloatParameter(parameters,"slope");
    }
    catch (ModuleException & e) {
        fSlope = 1.0f;
        msg.str("");
        msg<<"No slope found setting default =0 (exception message:"<<e.what()<<")";
        logger(logger.LogWarning,msg.str());
    }
	return 0;
}

std::map<std::string, std::string> DataScaler::GetParameters()
{
	std::map<std::string, std::string> parameters;

    parameters["offset"]=fOffset;
    parameters["slope"]=fSlope;

	return parameters;
}

bool DataScaler::SetROI(const std::vector<size_t> & /*roi*/)
{
	return false;
}

int DataScaler::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & /*coeff*/)
{
	float *pImg = img.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++) {
		pImg[i]=fSlope*pImg[i]+fOffset;
	}

	return 0;
}

int DataScaler::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & /*coeff*/)
{
	float *pImg = img.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++) {
		pImg[i]=fSlope*pImg[i]+fOffset;
	}

	return 0;
}
