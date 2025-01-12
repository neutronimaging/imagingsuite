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
        kipl::logging::Logger logger;

    public:
        ScatteringCorrection();
        ScatteringCorrection(const std::map<std::string, std::string> &parameters);

        ~ScatteringCorrection();

        void setParameters(const std::map<std::string, std::string> &parameters);
        std::map<std::string, std::string> parameters();

        void setROI(const std::vector<size_t> &ROI);
        void setBBROI(const std::vector<size_t> &ROI);
        void setDoseROI(const std::vector<size_t> &ROI);
        void setAveragingMethod(ImagingAlgorithms::AverageImage::eAverageMethod method);
        void setNormalizationMethod(ImageNormalization::eNormalizationMethod method);
        void setFitMethod(ScatterEstimator::eFitMethod method);

        void setReferences( const std::map<std::string, kipl::base::TImage<float,3>> & refs,
                            const std::map<std::string, std::vector<float>>          & doses);

        void calibrate();
    
        kipl::base::TImage<float,2> process(kipl::base::TImage<float,2> &img, float dose);
        void process(kipl::base::TImage<float,3> &img, std::vector<float> dose);

    private:
        void checkDataIntegrity(const std::map<std::string, kipl::base::TImage<float,3>> & refs, 
                                const std::map<std::string, std::vector<float>>          & doses) ;

        // bool m_bCalibrationNeeded;
        std::vector<size_t> m_ROI;
        std::vector<size_t> m_BBROI;
        std::vector<size_t> m_DoseROI;

        float m_fDotRadius;
        std::vector<size_t> m_DotAreaLimits;
    
        ImagingAlgorithms::AverageImage::eAverageMethod m_AverageMethod;
        ImageNormalization::eNormalizationMethod        m_NormalizationMethod;
        SegmentBB::eSegmentationMethod                  m_SegmentationMethod;
        ScatterEstimator::eFitMethod                    m_FitMethod;

        std::map<std::string, kipl::base::TImage<float,3>> m_refs;
        std::map<std::string, std::vector<float>>          m_doses;

        float m_dcdose;
        kipl::base::TImage<float,2> ob;
        kipl::base::TImage<float,2> dc;
        kipl::base::TImage<float,2> bbob;
        kipl::base::TImage<float,2> mask;

        SegmentBB m_SegmentBB;

};

#endif