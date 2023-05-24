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
        std::vector<float> dotPixels(const kipl::base::TImage<float,2> &img, float x, float y, float radius);
        float              dotValue(const kipl::base::TImage<float,2> &img, float x, float y, float radius);
        arma::mat          buildA(const std::vector<float> &x, const std::vector<float> &y);
        float              fitError(arma::vec a, arma::vec b);
        float              polyVal(float x, float y);
        void               transferFitParameters();

        std::vector<size_t> m_imgDims;
        int m_nPolyOrderX;
        int m_nPolyOrderY;
        float m_fDotRadius;
        eAverageMethod m_AvgMethod;

        float m_fFitError;
        arma::vec m_fittedParameters;
        float a; // x2
        float b; // x
        float c; // y2
        float d; // y
        float e; // xy
        float f; // const
};

std::string    SCATTERINGCORRECTIONSHARED_EXPORT enum2string(ScatterEstimator::eAverageMethod m);
void           SCATTERINGCORRECTIONSHARED_EXPORT string2enum(const std::string &s, ScatterEstimator::eAverageMethod &m);
std::ostream & SCATTERINGCORRECTIONSHARED_EXPORT operator<<(std::ostream & s, ScatterEstimator::eAverageMethod m);

std::string    SCATTERINGCORRECTIONSHARED_EXPORT enum2string(ScatterEstimator::eFitMethod m);
void           SCATTERINGCORRECTIONSHARED_EXPORT string2enum(const std::string &s, ScatterEstimator::eFitMethod &m);
std::ostream & SCATTERINGCORRECTIONSHARED_EXPORT operator<<(std::ostream & s, ScatterEstimator::eFitMethod m);

#endif