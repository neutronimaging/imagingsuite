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
        /// @brief Enum to select averaging method for the dots
        enum eAverageMethod 
        {
            avg_mean,
            avg_median,
            avg_min,
            avg_max
        };

        /// @brief Enum to select the fitting method
        enum eFitMethod 
        {
            fitmethod_polynomial,
            fitmethod_thinplatesplines
        };

        /// @brief Base C'tor for default initialization
        ScatterEstimator();

        /// @brief  C'tor for initialialization with parameter list
        /// @param params A list of parameters to configure the object
        ScatterEstimator(const std::map<std::string,std::string> &params);

        /// @brief Configure the object with a parameter list
        /// @param params A list of parameters
        void setParameters(const std::map<std::string,std::string> &params);

        /// @brief  Get the current set of parameters
        /// @return The parameter list
        std::map<std::string,std::string> parameters();

        /// @brief  Define how the grayvalues under the dots are combined
        /// @param method The averaging method
        void setAverageMethod(eAverageMethod method);

        /// @brief Fits the intensities under the bbs to a polynomial
        /// @param x Dot x coordinates
        /// @param y Dot y coordinates
        /// @param bb intensity at x,y
        /// @param dotRadius Radius of the dot to measure
        /// @param polyOrderX Polynomial order in x
        /// @param polyOrderY Polynomial order in y
        void fit(const std::vector<float> &x, const std::vector<float> &y, 
                 const kipl::base::TImage<float,2> &bb, 
                 float dotRadius, 
                 int polyOrderX,
                 int polyOrderY
                 );

        /// @brief The mean squared error of the fit
        /// @return the error
        float fitError();
        
        /// @brief Renders the scatter image in the defined region
        /// @param ROI Region of the image
        /// @return The scatter estimate
        kipl::base::TImage<float,2> scatterImage(const std::vector<size_t> &ROI={});

        /// @brief Computes the dose of the scatter image
        /// @param ROI Region used to compute the scatter dose
        /// @return The dose
        float              scatterDose(const std::vector<size_t> &ROI);

        /// @brief Use predefined scatter image parameters
        /// @param pars Polynom coefficients, the vector must have 6 elements
        /// @param dims Image dimensions
        void               setFitParameters(const std::vector<float> &pars, 
                                            const std::vector<size_t> &dims = {2048UL,2048UL});

        /// @brief Provides the parameters of the current fit
        /// @return A vector with the parameters
        std::vector<float> fitParameters();

        /// @brief Provides the order of the polynomial in x and y
        /// @return A vector with the polynomial orders
        std::vector<int>   polyOrders();
        
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
        eFitMethod m_FitMethod;

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