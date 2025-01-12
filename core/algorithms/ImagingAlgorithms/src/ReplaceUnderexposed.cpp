#include "../include/ReplaceUnderexposed.h"
#include "../include/ImagingException.h"

#include <map>
#include <string>

namespace ImagingAlgorithms
{

ReplaceUnderexposed::ReplaceUnderexposed() :
    logger("ReplaceUnderexposed"),
    // detectionMethod(ReplaceUnderexposed::FixedThreshold),
    replacementMethod(ReplaceUnderexposed::NeighborAverage)
{

}

ReplaceUnderexposed::ReplaceUnderexposed(float /*th*/,
                                                       ReplaceUnderexposed::eDoseOutlierDetection  /*dmethod*/,
                                                       ReplaceUnderexposed::eDoseOutlierReplacement rmethod) :
    logger("ReplaceUnderexposed"),
    // detectionMethod(dmethod),
    replacementMethod(rmethod)
{

}

void ReplaceUnderexposed::process(kipl::base::TImage<float, 3> &img, vector<float> &dose, float threshold)
{
    if (img.Size(2) != dose.size())
        throw ImagingException("#images != #dose",__FILE__,__LINE__);
    replacementMethod = NeighborAverage;

    switch (replacementMethod)
    {
    case NeighborAverage : processNeighborAverage(img,dose,threshold); break;
    case WeightedAverage : processWeightedAverage(img,dose,threshold); break;
    }

}

vector<size_t> ReplaceUnderexposed::detectUnderExposure(std::vector<float> &doses, float threshold)
{
    size_t idx=0UL;
    vector<size_t> underExposed;

    for (const auto &dose : doses)
    {
        if (dose<threshold)
            underExposed.push_back(idx);

        ++idx;
    }

    return underExposed;
}

void ReplaceUnderexposed::processNeighborAverage(kipl::base::TImage<float, 3> &img, vector<float> &dose, float threshold)
{
    std::ostringstream msg;
    auto underExposured = detectUnderExposure(dose,threshold);

    msg<<"Detected "<<underExposured.size()<<" underexposed projections";
    logger.message(msg.str());
    size_t cnt=0;
    for (const auto &idx: underExposured )
    {
        if ((0<idx) && (idx<(dose.size()-1)))
        {
            ++cnt;
            auto d0 = dose[idx-1];
            auto d1 = dose[idx+1];
            dose[idx] = 0.5f*(d0+d1);

            float *pImg0 = img.GetLinePtr(0,idx-1);
            float *pImg1 = img.GetLinePtr(0,idx+1);
            float *pRes  = img.GetLinePtr(0,idx);
            size_t N=img.Size(0)*img.Size(1);
            for (size_t i = 0; i<N; ++i)
                pRes[i]=0.5*(pImg0[i]+pImg1[i]);
        }
    }
    msg.str("");
    msg<<cnt<<" under exposed projections are corrected";
    logger.message(msg.str());

}

void ReplaceUnderexposed::processWeightedAverage(kipl::base::TImage<float, 3> &/*img*/, vector<float> &/*dose*/, float /*threshold*/)
{
    throw ImagingException("processWeightedAverage is not implemented",__FILE__,__LINE__);
}

} // End of namespace ImagingAlgorithms

void string2enum(const std::string &str, ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement &dor)
{
    std::map<std::string,ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement> methods;

    methods["neighboraverage"]  = ImagingAlgorithms::ReplaceUnderexposed::NeighborAverage;
    methods["weightedaverage"]  = ImagingAlgorithms::ReplaceUnderexposed::WeightedAverage;

    if (methods.count(str)==0)
        throw ImagingException("The key string does not exist for eDoseOutlierReplacement",__FILE__,__LINE__);

    dor=methods[str];
}

std::string enum2string(ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement dor)
{
    std::map<ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement,std::string> methods;

    methods[ImagingAlgorithms::ReplaceUnderexposed::NeighborAverage] = "neighboraverage";
    methods[ImagingAlgorithms::ReplaceUnderexposed::WeightedAverage] = "weightedaverage";

    if (methods.count(dor)==0)
        throw ImagingException("The key string does not exist for eDoseOutlierReplacement",__FILE__,__LINE__);

    return methods[dor];
}

std::ostream &operator<<(std::ostream &s, ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement &dor)
{
    return s<<enum2string(dor);
}

void string2enum(const std::string &str, ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection &dod)
{
    std::map<std::string,ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection> methods;

    methods["fixedthreshold"]  = ImagingAlgorithms::ReplaceUnderexposed::FixedThreshold;
    methods["mad"]             = ImagingAlgorithms::ReplaceUnderexposed::MAD;

    if (methods.count(str)==0)
        throw ImagingException("The key string does not exist for eDoseOutlierDetection",__FILE__,__LINE__);

    dod=methods[str];
}

std::string enum2string(ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection dod)
{
    std::map<ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection,std::string> methods;

    methods[ImagingAlgorithms::ReplaceUnderexposed::FixedThreshold] = "fixedthreshold";
    methods[ImagingAlgorithms::ReplaceUnderexposed::MAD]            = "mad";

    if (methods.count(dod)==0)
        throw ImagingException("The key string does not exist for eDoseOutlierDetection",__FILE__,__LINE__);

    return methods[dod];
}

std::ostream &operator<<(std::ostream &s, ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection &dod)
{
    return s<<enum2string(dod);
}
