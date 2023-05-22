//<LICENSE>

#include "ScatterEstimation.h"
#include <ImagingException.h>

ScatterEstimator::ScatterEstimator() :
    logger("ScatterEstimator")
{

}

ScatterEstimator::ScatterEstimator(const std::map<std::string,std::string> &params) :
    logger("ScatterEstimator")
{

}

void ScatterEstimator::setParameters(const std::map<std::string,std::string> &params)
{

}

std::map<std::string,std::string> ScatterEstimator::parameters()
{

}

void ScatterEstimator::setAverageMethod(eAverageMethod method)
{

}

void ScatterEstimator::fit(const std::vector<float> &x, const std::vector<float> &y, 
            const kipl::base::TImage<float,2> &bb, 
            float dotRadius, 
            int polyOrderX,
            int polyOrderY
            )
{
    if ((x.size() != y.size()) || x.empty())
        ImagingException("Size mismatch in the fitting coordinate data",__FILE__,__LINE__);

    if (dotRadius <= 0.0f)
        ImagingException("Bad dot radius <=0",__FILE__,__LINE__);

    m_fDotRadius = dotRadius;

    if ((polyOrderX<0) || (2<polyOrderX))
        ImagingException("Bad polynomial order x (must be 0-2)",__FILE__,__LINE__);
    
    m_nPolyOrderX = polyOrderX;

    if ((polyOrderY<0) || (2<polyOrderY))
        ImagingException("Bad polynomial order y (must be 0-2)",__FILE__,__LINE__);
    
    m_nPolyOrderY = polyOrderY;

    // Get dot intensity 
    std::vector<float> dotVals(x.size());
    arma::vec b(x.size());

    for (size_t i=0; i<x.size(); ++i) 
    {
        b(i) = dotVals[i] = dotValue(x[i],y[i],dotRadius);
    }

    auto A = buildA(x,y);

    m_fittedParameters = arma::solve(A, b);

    auto fit = A * m_fittedParameters;

    m_fFitError = fitError(b,fit);

}

arma::mat ScatterEstimator::buildA(const std::vector<float> &x, const std::vector<float>  &y)
{
    size_t nCol = m_nPolyOrderX + m_nPolyOrderY + 1 + (m_nPolyOrderX*m_nPolyOrderY != 0 ? 1 : 0);

    arma::mat A( x.size(), nCol );

    for (size_t i=0; i<x.size() ; ++i) 
    {
        int col = 0;
        switch (m_nPolyOrderX)
        {
            case 0: break;
            case 1: 
                A(i,col++) = x[i]; 
                break;
            case 2:
                A(i,col++) = x[i]*x[i];
                A(i,col++) = x[i];
            default:
                throw ImagingException("Poly order X out of bound",__FILE__,__LINE__);
        }

        switch (m_nPolyOrderY)
        {
            case 0: break;
            case 1: 
                A(i,col++) = y[i]; 
                break;
            case 2:
                A(i,col++) = y[i]*y[i];
                A(i,col++) = y[i];
            default:
                throw ImagingException("Poly order X out of bound",__FILE__,__LINE__);
        }

        if (m_nPolyOrderX*m_nPolyOrderY != 0)
            A(i,col++) = x[i]*y[i];

        A(i,col++)=1.0;
    }

    return A;
}

kipl::base::TImage<float,2> ScatterEstimator::scatterImage(const std::vector<size_t> &ROI)
{

}

float scatterDose(const std::vector<size_t> &ROI)
{

}
std::vector<float> ScatterEstimator::fitParameters()
{
   std::vector<float> parameters;

   return parameters; 
}

std::vector<int> ScatterEstimator::polyOrders()
{
    return std::vector<int>({m_nPolyOrderX, m_nPolyOrderY});
}

float ScatterEstimator::fitError()
{
    return m_fFitError;
}

std::vector<float> ScatterEstimator::dotPixels(float x, float y, float radius)
{

}

float ScatterEstimator::dotValue(float x, float y, float radius)
{

}

float ScatterEstimator::fitError(arma::vec a, arma::vec b)
{
    auto c = a-b;

    return arma::norm(c,2);
}
