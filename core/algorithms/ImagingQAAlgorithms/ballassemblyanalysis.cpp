//<LICENSE>
#include "ballassemblyanalysis.h"

#include <sstream>
#include <list>

#include <math/basicprojector.h>
#include <math/statistics.h>
#include <base/marginsetter.h>
#include <segmentation/thresholds.h>
#include <morphology/morphology.h>
#include <morphology/label.h>
#include <morphology/morphfilters.h>
#include <morphology/morphdist.h>

namespace ImagingQAAlgorithms {

BallAssemblyAnalysis::BallAssemblyAnalysis() :
    logger("BallAssemblyAnalysis"),
    pixelSize(0.1f),
    strelRadius(25.0f)
{

}

void BallAssemblyAnalysis::analyzeImage(kipl::base::TImage<float,3> &img)
{
   projection=kipl::math::BasicProjector<float>::project(img,kipl::base::ImagePlaneXY);

   createLabelledMask(img);
}

void BallAssemblyAnalysis::analyzeImage(kipl::base::TImage<float,3> &img, std::list<kipl::base::RectROI> &roiList)
{
    assemblyStats.clear();

    foreach (auto roi, roiList) {
        kipl::math::Statistics stats=kipl::math::imageStatistics(img,roi);
        assemblyStats.push_back(stats);
    }
}

std::list<kipl::math::Statistics> BallAssemblyAnalysis::getStatistics()
{
    return assemblyStats;
}

void BallAssemblyAnalysis::createLabelledMask(kipl::base::TImage<float,3> &img)
{
    std::ostringstream msg;
    const int nBins=1024;
    size_t hist[nBins+1];
    float axis[nBins+1];
    kipl::base::Histogram(img.GetDataPtr(), img.Size(), hist, nBins, 0.0f, 0.0f, axis);
    int idx=kipl::segmentation::Threshold_Otsu(hist,nBins);

    float threshold=axis[idx];
    mask.Resize(img.Dims());

    kipl::segmentation::Threshold(img.GetDataPtr(),mask.GetDataPtr(),img.Size(),threshold);
    kipl::base::setMarginValue(mask,3,0.0f);

    msg.str(""); msg<<"Image thresholded using t="<<threshold;
    logger(logger.LogMessage,msg.str());

    kipl::base::TImage<float,3> tmp;
//    size_t dims[3]={3,3,3};
//    kipl::base::TImage<float,3> strel(dims);
//    strel=1.0f;
//    kipl::morphology::TDilate<float,3> dilate(strel.GetDataPtr(),strel.Dims());
//    kipl::morphology::TErode<float,3>  erode(strel.GetDataPtr(),strel.Dims());
//    tmp=erode(mask,kipl::filters::FilterBase::EdgeValid);
//    mask=dilate(tmp,kipl::filters::FilterBase::EdgeValid);
//    logger(logger.LogMessage,"Morph opening to remove small miss classified pixels");
    kipl::morphology::CMetric26conn metric;
    metric.initialize(mask.Dims());
    kipl::morphology::DistanceTransform3D(mask,tmp,metric,true);

    kipl::segmentation::Threshold(tmp.GetDataPtr(),mask.GetDataPtr(),tmp.Size(),strelRadius,kipl::segmentation::cmp_less);

    kipl::morphology::DistanceTransform3D(mask,tmp,metric,false);
    dist=tmp;
    kipl::segmentation::Threshold(tmp.GetDataPtr(),mask.GetDataPtr(),tmp.Size(),strelRadius,kipl::segmentation::cmp_greatereq);
    logger(logger.LogMessage,"Distance driven closing for pores in assembly");

    size_t cnt=kipl::morphology::LabelImage(mask,labels,kipl::base::conn6);
    //kipl::morphology::LabelArea(labels,cnt,labelarea);
    logger(logger.LogMessage,"Labelled");
}

kipl::base::TImage<float,3> & BallAssemblyAnalysis::getMask()
{
    return mask;
}

kipl::base::TImage<int,3> & BallAssemblyAnalysis::getLabels()
{
    return labels;
}

kipl::base::TImage<float,3> & BallAssemblyAnalysis::getDist()
{
    return dist;
}

kipl::base::TImage<float,2> & BallAssemblyAnalysis::getZProjection()
{
    return projection;
}

}
