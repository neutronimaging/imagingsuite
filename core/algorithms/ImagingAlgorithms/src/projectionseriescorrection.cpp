#include "../include/projectionseriescorrection.h"
#include "../include/ImagingException.h"

namespace ImagingAlgorithms
{
ImagingAlgorithms::ProjectionSeriesCorrection::ProjectionSeriesCorrection(DoseOutlierDetection dmethod, DoseOutlierReplacement rmethod) :
    detectionMethod(dmethod),
    replacementMethod(replacementMethod)
{

}

void ImagingAlgorithms::ProjectionSeriesCorrection::process(kipl::base::TImage<float, 3> &img, vector<float> &dose)
{
    if (img.Size(2) != dose.size())
        throw ImagingException("#images != #dose",__FILE__,__LINE__);

    switch (replacementMethod)
    {
    case NeighborAverage : processNeighborAverage(img,dose); break;
    case WeightedAverage : processWeightedAverage(img,dose); break;
    }

}

void ImagingAlgorithms::ProjectionSeriesCorrection::processNeighborAverage(kipl::base::TImage<float, 3> &img, vector<float> &dose)
{

}

void ImagingAlgorithms::ProjectionSeriesCorrection::processWeightedAverage(kipl::base::TImage<float, 3> &img, vector<float> &dose)
{

}

void string2enum(const string &str, ProjectionSeriesCorrection::DoseOutlierReplacement &dor)
{

}

string enum2string(const ProjectionSeriesCorrection::DoseOutlierReplacement &dor)
{

}

}
void ImagingAlgorithms::string2enum(const string &str, ProjectionSeriesCorrection::DoseOutlierDetection &dod)
{

}

string ImagingAlgorithms::enum2string(const ProjectionSeriesCorrection::DoseOutlierDetection &dod)
{

}

ostream &ImagingAlgorithms::operator<<(ostream &s, ProjectionSeriesCorrection::DoseOutlierDetection dod)
{

}
