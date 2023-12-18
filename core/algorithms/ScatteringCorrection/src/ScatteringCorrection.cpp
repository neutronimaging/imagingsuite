//<LICENSE>

#include "ScatteringCorrection.h"

#include <algorithm>

#include <ImagingException.h>

ScatteringCorrection::ScatteringCorrection() :
    logger("ScatteringCorrection"),
    m_bCalibrationNeeded(true),
    // m_ROI(4,0UL), // These should probably be empty vectors unless they get a value
    // m_BBROI(4,0UL),
    // m_DoseROI(4,0UL),
    m_fDotRadius(4.0f),
    m_DotAreaLimits({20UL,250UL}),
    m_dcdose(0.0f),
    m_AverageMethod(ImagingAlgorithms::AverageImage::ImageAverage),
    m_NormalizationMethod(ImageNormalization::scatterNormalization),
    m_SegmentationMethod(SegmentBB::method_otsu),
    m_FitMethod(ScatterEstimator::fitmethod_polynomial)
{ 

}

ScatteringCorrection::ScatteringCorrection(const std::map<std::string, std::string> &parameters) :
    logger("ScatteringCorrection"),
    m_bCalibrationNeeded(true),
    m_ROI(4,0UL),
    m_BBROI(4,0UL),
    m_DoseROI(4,0UL),
    m_fDotRadius(4.0f),
    m_DotAreaLimits({20UL,250UL}),
    m_dcdose(0.0f),
    m_AverageMethod(ImagingAlgorithms::AverageImage::ImageAverage),
    m_NormalizationMethod(ImageNormalization::scatterNormalization),
    m_FitMethod(ScatterEstimator::fitmethod_polynomial)
{ 
    setParameters(parameters);
}

ScatteringCorrection::~ScatteringCorrection() 
{ }

void ScatteringCorrection::setParameters(const std::map<std::string, std::string> &parameters) 
{ }

std::map<std::string, std::string> ScatteringCorrection::parameters() 
{ }

void ScatteringCorrection::setROI(const std::vector<size_t> &ROI) 
{ 
    m_ROI = ROI;
}

void ScatteringCorrection::setBBROI(const std::vector<size_t> &ROI) 
{ 
    m_BBROI = ROI;
}

void ScatteringCorrection::setDoseROI(const std::vector<size_t> &ROI) 
{ 
    m_DoseROI = ROI;
}

void ScatteringCorrection::setAveragingMethod(ImagingAlgorithms::AverageImage::eAverageMethod method) 
{
    m_AverageMethod = method;
}

void ScatteringCorrection::setNormalizationMethod(ImageNormalization::eNormalizationMethod method) 
{ 
    m_NormalizationMethod = method;
}

void ScatteringCorrection::setFitMethod(ScatterEstimator::eFitMethod method)
{
    m_FitMethod = method;
}

void ScatteringCorrection::checkDataIntegrity(const std::map<std::string, kipl::base::TImage<float,3>> & refs, const std::map<std::string, std::vector<float>> & doses) 
{ 
    ostringstream msg;
    for (const auto & item : refs) 
    {
        if (doses.count(item.first)) 
        {
            if (doses.at(item.first).size()!=item.second.Size(2)) 
            {
                msg.str(""); msg<<"Size of dose vector doesn't match the number of reference images ("<<item.first<<").";
                logger.error(msg.str());
                throw ImagingException(msg.str(),__FILE__,__LINE__); 
            }
               
        }
        else 
        {
            msg.str(""); msg<<"Reference images have no dose vector ("<<item.first<<").";
            logger.error(msg.str());
            throw ImagingException(msg.str(),__FILE__,__LINE__);
        }

    }

    auto dims = refs.begin()->second.dims();
    auto name = refs.begin()->first;

    for (const auto & item : refs) 
    {
        auto dims_item=item.second.dims();

        if ( (dims[0]!= dims_item[0]) || (dims[1]!= dims_item[1]) )
        {
            msg.str(""); msg<<"Reference image dimensions don't match ("<<name<<" and "<<item.first<<").";
            logger.error(msg.str());
            throw ImagingException(msg.str(),__FILE__,__LINE__);
        }
    }
}

void ScatteringCorrection::setReferences(const std::map<std::string, kipl::base::TImage<float,3>> & refs, const std::map<std::string, std::vector<float>> & doses)
{
    checkDataIntegrity(refs,doses);
    m_refs  = refs;
    m_doses = doses;

    calibrate();
}

void ScatteringCorrection::calibrate() 
{ 
    std::ostringstream msg;
    ImagingAlgorithms::AverageImage avg;

    if (m_refs.count("dc"))
    {
        m_dcdose = std::accumulate(m_doses["dc"].begin(),m_doses["dc"].end(),0.0f)/m_doses["dc"].size();

        dc = avg(m_refs["dc"],m_AverageMethod,m_doses["dc"]);
    }
    else if (m_refs.count("ob"))
    {
        dc.resize(m_refs["ob"].dims());
        dc=0.0f;
    }
    else 
    {
        logger.error("Open beam image missing, calibration failed");
        throw ImagingException("Open beam image missing, calibration failed",__FILE__,__LINE__);
    }

    if (m_refs.count("ob"))
    {
        auto doses = m_doses["ob"];

        for (auto & d : doses)
            d-=m_dcdose;

        ob  = avg(m_refs["ob"],m_AverageMethod,doses); // OB doses can be improved by subtracting dc dose
        ob -= dc;
    }

    if (m_refs.count("bbob"))
    {
        auto doses = m_doses["bbob"];

        for (auto & d : doses)
            d-=m_dcdose;

        bbob  = avg(m_refs["bbob"],m_AverageMethod,doses); // OB doses can be improved by subtracting dc dose
        bbob -= dc;
    }

    m_SegmentBB.setSegmentationMethod(m_SegmentationMethod);
    m_SegmentBB.setDotRadius(m_fDotRadius);
    m_SegmentBB.setDotAreaLimits(m_DotAreaLimits);
    m_SegmentBB.setSegmentationMethod(m_SegmentationMethod);

    mask.resize(m_refs["bbob"].dims());
    mask = 0.0f;
    if (m_SegmentationMethod == SegmentBB::method_usermask) 
    {    
        if (m_refs.count("mask")) 
        {
             std::copy_n(m_refs["mask"].GetDataPtr(),mask.Size(),mask.GetDataPtr());
        }
    }

    m_SegmentBB.exec(bbob,mask,m_BBROI);
}

kipl::base::TImage<float,2> ScatteringCorrection::process(kipl::base::TImage<float,2> &img, float dose) 
{ 
    
}

void ScatteringCorrection::process(kipl::base::TImage<float,3> &img, std::vector<float> dose) 
{ }