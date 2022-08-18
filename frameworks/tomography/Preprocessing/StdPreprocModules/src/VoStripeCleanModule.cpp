//<LICENSE>

#include <sstream>
#include <tuple>

#include "../include/VoStripeCleanModule.h"
#include <ParameterHandling.h>
#include <ModuleException.h>
#include <vostripeclean.h>

VoStripeCleanModule::VoStripeCleanModule(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("VoStripeClean",interactor),
    useUnresponsiveStripes(true),
    useStripeSorting(false),
    useLargeStripes(true),
    filterSize_unresponsive(21),
    filterSize_sorting(31),
    filterSize_large(21),
    snr_unresponsive(0.1),
    snr_large(0.1)
{
    m_bThreading = false;
    publications.push_back(Publication({"N.T. Vo","R.C. Atwood", "M. Drakopoulos"},
                                        "Superior techniques for eliminating ring artifacts in X-ray micro-tomography",
                                        "Optics express",
                                        2018,
                                        26,
                                        22,
                                        "342867",
                                        "10.1364/OE.26.028396"));
}

VoStripeCleanModule::~VoStripeCleanModule()
{
}


int VoStripeCleanModule::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    std::ignore = config;
    useUnresponsiveStripes  = kipl::strings::string2bool(GetStringParameter(parameters,"useUnresponsiveStripes"));
    useStripeSorting        = kipl::strings::string2bool(GetStringParameter(parameters,"useStripeSorting"));
    useLargeStripes         = kipl::strings::string2bool(GetStringParameter(parameters,"useLargeStripes"));
    filterSize_unresponsive = GetIntParameter(parameters,"filterSize_unresponsive");
    filterSize_sorting      = GetIntParameter(parameters,"filterSize_sorting");
    filterSize_large        = GetIntParameter(parameters,"filterSize_large");
    snr_unresponsive        = GetIntParameter(parameters,"snr_unresponsive");
    snr_large               = GetFloatParameter(parameters,"snr_large");
    m_bThreading            = kipl::strings::string2bool(GetStringParameter(parameters,"threading"));

	return 0;
}

std::map<std::string, std::string> VoStripeCleanModule::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["useUnresponsiveStripes"]  = kipl::strings::bool2string(useUnresponsiveStripes);
    parameters["useStripeSorting"]        = kipl::strings::bool2string(useStripeSorting);
    parameters["useLargeStripes"]         = kipl::strings::bool2string(useLargeStripes);
    parameters["filterSize_unresponsive"] = std::to_string(filterSize_unresponsive);
    parameters["filterSize_sorting"]      = std::to_string(filterSize_sorting);
    parameters["filterSize_large"]        = std::to_string(filterSize_large);
    parameters["snr_unresponsive"]        = std::to_string(snr_unresponsive);
    parameters["snr_large"]               = std::to_string(snr_large);
    parameters["threading"]               = kipl::strings::bool2string(m_bThreading);

	return parameters;
}

bool VoStripeCleanModule::SetROI(size_t *roi)
{
    std::ignore = roi;

	return false;
}

int VoStripeCleanModule::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
    std::ignore = img;
    std::ignore = coeff;
	return 0;
}

int VoStripeCleanModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    int res=0;
    if (m_bThreading)
    {
        res=processParallel(img,coeff);
    }
    else
    {
        res=processSequential(img,coeff);
    }

    return res;
}

int VoStripeCleanModule::processSequential(kipl::base::TImage<float, 3> &img, std::map<string, string> &coeff)
{
    std::ignore = coeff;
    ImagingAlgorithms::VoStripeClean vsc;
    kipl::base::TImage<float,2> sino;

    for (size_t i=0; (i<img.Size(2)) && !UpdateStatus(static_cast<float>(i)/static_cast<float>(img.Size(2)),"VoStripeRemoval") ; ++i)
    {
        ExtractSinogram(img,sino,i);
        if (useUnresponsiveStripes)
            sino = vsc.removeUnresponsiveAndFluctuatingStripe(sino,filterSize_unresponsive,snr_unresponsive);

        if (useStripeSorting)
            sino = vsc.removeStripeBasedSorting(sino,filterSize_sorting,false);

        if (useLargeStripes)
            sino = vsc.removeLargeStripe(sino,filterSize_large,snr_large,false);

        InsertSinogram(sino,img,i);
    }
    return 0;
}

int VoStripeCleanModule::processParallel(kipl::base::TImage<float, 3> &img, std::map<string, string> &coeff)
{
    return processSequential(img,coeff);
}
