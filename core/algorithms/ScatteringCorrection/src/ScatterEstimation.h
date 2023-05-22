//<LICENSE>
#ifndef SCATTERESTIMATION_H_
#define SCATTERESTIMATION_H_

#include "ScatteringCorrection_global.h"

#include <map>
#include <vector>
#include <string>

#include <base/timage.h>
#include <logging/logger.h>
#include <armadillo>

class SCATTERINGCORRECTIONSHARED_EXPORT ScatterEstimator
{
    kipl::logging::Logger logger;

    public:
        enum eAverageMethod 
        {
            avg_mean,
            avg_median,
            avg_min,
            avg_max
        };

        enum eFitMethod 
        {
            fitmethod_polynomial,
            fitmethod_thinplatesplines
        };

        ScatterEstimator();
        ScatterEstimator(const std::map<std::string,std::string> &params);

        void setParameters(const std::map<std::string,std::string> &params);
        std::map<std::string,std::string> parameters();

        void setAverageMethod(eAverageMethod method);

        void fit(const std::vector<float> &x, const std::vector<float> &y, 
                 const kipl::base::TImage<float,2> &bb, 
                 float dotRadius, 
                 int polyOrderX,
                 int polyOrderY
                 );
        kipl::base::TImage<float,2> scatterImage(const std::vector<size_t> &ROI={});
        float scatterDose(const std::vector<size_t> &ROI);
        std::vector<float> fitParameters();
        std::vector<int> polyOrders();
        float fitError();
    private:
        std::vector<float> dotPixels(float x, float y, float radius);
        float              dotValue(float x, float y, float radius);
        arma::mat          buildA(const std::vector<float> &x, const std::vector<float> &y);
        float              fitError(arma::vec a, arma::vec b);

        std::vector<size_t> m_imgDims;
        int m_nPolyOrderX;
        int m_nPolyOrderY;
        float m_fDotRadius;

        float m_fFitError;
        arma::vec m_fittedParameters;
};
#endif