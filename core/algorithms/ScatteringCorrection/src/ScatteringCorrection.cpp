#include "ScatteringCorrection.h"

ScatteringCorrection::ScatteringCorrection() :
    logger("ScatteringCorrection"),
    m_bCalibrationNeeded(true),
    m_ROI(4,0UL),
    m_DoseROI(4,0UL),
    m_AverageMethod(ImagingAlgorithms::AverageImage::ImageAverage),
    m_NormalizationMethod(ImageNormalization::scatterNormalization),
    m_FitMethod(ScatterEstimator::fitmethod_polynomial)
{ 

}

ScatteringCorrection::ScatteringCorrection(const std::map<std::string, std::string> &parameters) :
    logger("ScatteringCorrection"),
    m_bCalibrationNeeded(true),
    m_ROI(4,0UL),
    m_DoseROI(4,0UL),
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

void setFitMethod(ScatterEstimation::eFitMethod method)
{
    m_FitMethod = method;
}

bool ScatteringCorrection::checkDataIntegrity() 
{ }

void ScatteringCorrection::setOpenBeam(   const kipl::base::TImage<float,2> &ob,   float dose) 
{ 
    m_bCalibrationNeed = true;
}

void ScatteringCorrection::setOpenBeam(   const kipl::base::TImage<float,3> &ob,   const std::vector<float> &dose) 
{ 
    m_bCalibrationNeed = true;
}

void ScatteringCorrection::setDarkCurrent(const kipl::base::TImage<float,2> &dc,   float dose) 
{ 
    m_bCalibrationNeed = true;
}

void ScatteringCorrection::setDarkCurrent(const kipl::base::TImage<float,3> &dc,   const std::vector<float> &dose) 
{ 
    m_bCalibrationNeed = true;
}

void ScatteringCorrection::setBBSample(   const kipl::base::TImage<float,2> &bbs,  float dose) 
{ 
    m_bCalibrationNeed = true;
}

void ScatteringCorrection::setBBSample(   const kipl::base::TImage<float,3> &bbs,  const std::vector<float> &dose) 
{ 
    m_bCalibrationNeed = true;
}

void ScatteringCorrection::setBBOpenBeam( const kipl::base::TImage<float,2> &bbob, float dose) 
{ 
    m_bCalibrationNeed = true;
}

void ScatteringCorrection::setBBOpenBeam( const kipl::base::TImage<float,3> &bbob, const std::vector<float> &dose) 
{ 
    m_bCalibrationNeed = true;
}

void ScatteringCorrection::calibrate() 
{ 

}

kipl::base::TImage<float,2> ScatteringCorrection::process(kipl::base::TImage<float,2> &img, float dose) 
{ 
    
}

void ScatteringCorrection::process(kipl::base::TImage<float,3> &img, std::vector<float> dose) 
{ }