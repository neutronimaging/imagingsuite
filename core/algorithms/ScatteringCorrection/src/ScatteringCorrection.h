//<LICENSE>

#ifndef SCATTERINGCORRECTION_H_
#define SCATTERINGCORRECTION_H_

#include "ScatteringCorrection_global.h"

#include <map>
#include <vector>
#include <string>

#include <base/timage.h>
#include <logging/logger.h>

#include <averageimage.h>

#include "SegmentBB.h"
#include "ScatterEstimation.h"
#include "ImageNormalization.h"

class SCATTERINGCORRECTIONSHARED_EXPORT ScatteringCorrection
{
    kipl::logging:Logger logger;

    public:
        ScatteringCorrection();
        ScatteringCorrection(const std::map<std::string, std::string> &parameters);

        ~ScatteringCorrection();

        void setParameters(const std::map<std::string, std::string> &parameters);
        std::map<std::string, std::string> parameters();

        void setROI(const std::vector<size_t> &ROI);
        void setDoseROI(const std::vector<size_t> &ROI);
        void setAveragingMethod(ImagingAlgorithms::AverageImage::eAverageMethod method);
        void setNormalizationMethod(ImageNormalization::eNormalizationMethod method);
        void setFitMethod(ScatterEstimator::eFitMethod method);
        bool checkDataIntegrity();

        void setOpenBeam(   const kipl::base::TImage<float,2> &ob,   float dose);
        void setOpenBeam(   const kipl::base::TImage<float,2> &ob,   const std::vector<float> &dose);
        
        void setDarkCurrent(const kipl::base::TImage<float,2> &dc,   float dose);
        void setDarkCurrent(const kipl::base::TImage<float,2> &dc,   const std::vector<float> &dose);
        
        void setBBSample(   const kipl::base::TImage<float,2> &bbs,  float dose);
        void setBBSample(   const kipl::base::TImage<float,3> &bbs,  const std::vector<float> &dose);

        void setBBOpenBeam( const kipl::base::TImage<float,2> &bbob, float dose);
        void setBBOpenBeam( const kipl::base::TImage<float,3> &bbob, const std::vector<float> &dose);

        void calibrate();
    
        kipl::base::TImage<float,2> process(kipl::base::TImage<float,2> &img, float dose);
        void process(kipl::base::TImage<float,3> &img, std::vector<float> dose);

    private:
        bool m_bCalibrationNeeded;
        std::vector<size_t> m_ROI;
        std::vector<size_t> m_DoseROI;
        ImagingAlgorithms::AverageImage::eAverageMethod m_AverageMethod;
        ImageNormalization::eNormalizationMethod m_NormalizationMethod;
        ScatterEstimator::eFitMethod m_FitMethod;

}