//<LICENSE>

#include <sstream>
#include "ScatterEstimation.h"
#include <ImagingException.h>
#include <stltools/stlvecmath.h>

ScatterEstimator::ScatterEstimator() :
    logger("ScatterEstimator"),
    m_imgDims({0,0}),
    m_nPolyOrderX(2),
    m_nPolyOrderY(2),
    m_fDotRadius(5),
    m_AvgMethod(avg_mean),
    m_FitMethod(fitmethod_polynomial)
{

}

ScatterEstimator::ScatterEstimator(const std::map<std::string,std::string> &params) :
    logger("ScatterEstimator"),
    m_imgDims({0,0}),
    m_nPolyOrderX(2),
    m_nPolyOrderY(2),
    m_fDotRadius(5),
    m_AvgMethod(avg_mean),
    m_FitMethod(fitmethod_polynomial)
{
    setParameters(params);
}

void ScatterEstimator::setParameters(const std::map<std::string,std::string> &params)
{
    m_nPolyOrderX = std::stoi(params.at("polyorderx"));
    m_nPolyOrderX = std::stoi(params.at("polyordery"));
    m_fDotRadius  = std::stof(params.at("dotradius"));
    string2enum(params.at("avgmethod"),m_AvgMethod);
    string2enum(params.at("fitmethod"),m_FitMethod);
}

std::map<std::string,std::string> ScatterEstimator::parameters()
{
    std::map<std::string,std::string> params = {
        {"polyorderx", to_string(m_nPolyOrderX)},
        {"polyordery", to_string(m_nPolyOrderY)},
        {"dotradius",  to_string(m_fDotRadius)},
        {"avgmethod",  enum2string(m_AvgMethod)},
        {"fitmethod",  enum2string(m_FitMethod)}
    };

    return params;
}

void ScatterEstimator::setAverageMethod(eAverageMethod method)
{
    m_AvgMethod = method;
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


    std::ostringstream msg;

    // Get dot intensity 
    std::vector<float> dotVals(x.size());
    arma::vec b(x.size());

    for (size_t i=0; i<x.size(); ++i) 
    {
        b(i) = dotVals[i] = dotValue(bb,x[i],y[i],dotRadius);
        msg.str("");
        msg<<"("<<x[i]<<", "<<y[i]<<") = "<<dotVals[i];
        logger.message(msg.str());
    }

    msg<<"dots = [";
    for (size_t i=0; i<dotVals.size(); ++i)
    {
        msg<<dotVals[i]<<", ";
    }
    msg<<"]";

    logger.message(msg.str());

    auto A = buildA(x,y);

    m_fittedParameters = arma::solve(A, b);

    msg.str("");
    msg<<"Parameters=[ ";
    for (const auto & par : m_fittedParameters)
        msg<<par<<" ";

    logger.message(msg.str());

    auto fit = A * m_fittedParameters;

    m_fFitError = fitError(b,fit);

    transferFitParameters();

}

arma::mat ScatterEstimator::buildA(const std::vector<float> &x, const std::vector<float>  &y)
{
    size_t nCol = m_nPolyOrderX + m_nPolyOrderY + 1 + (m_nPolyOrderX*m_nPolyOrderY != 0 ? 1 : 0);

    std::ostringstream msg;
    msg<<"pox="<<m_nPolyOrderX<<", poy="<<m_nPolyOrderY<<", nCol="<<nCol;
    logger.message(msg.str());
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
                break;
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
                break;
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
    if (ROI.size() != 4)
        throw ImagingException("The ROI vector must have length 4.",__FILE__,__LINE__);

    kipl::base::TImage<float,2> img({ROI[2]-ROI[0],ROI[3]-ROI[1]});

    size_t i = 0UL;

    for (size_t y=ROI[1]; y<ROI[3]; ++y)
        for (size_t x=ROI[1]; x<ROI[3]; ++x, ++i)
            img[i]=polyVal(x,y);  

}

float ScatterEstimator::scatterDose(const std::vector<size_t> &ROI)
{
    auto doseImg = scatterImage(ROI);

    float m = std::accumulate(doseImg.GetDataPtr(),doseImg.GetDataPtr()+doseImg.Size(),0.0f)/doseImg.Size();
}

std::vector<float> ScatterEstimator::fitParameters()
{
   std::vector<float> parameters{a,b,c,d,e,f};

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

std::vector<float> ScatterEstimator::dotPixels(const kipl::base::TImage<float,2> &img,
                                                     float x0, float y0, 
                                                     float radius)
{
    std::vector<float> v;
    for (float y=-ceil(radius); y<=ceil(radius); ++y )
    {
        float y2=y*y;
        for (float x=-ceil(radius); x<=ceil(radius); ++x )
        {
            float R=sqrt(x*x+y2);
            if (R<=radius)
            {
                v.push_back(img(static_cast<size_t>(x0+x+0.5),static_cast<size_t>(y0+y+0.5)));
            }
        } 
    }

    return v;
}

float ScatterEstimator::dotValue(const  kipl::base::TImage<float,2> &img, 
                                        float x, float y, 
                                        float radius)
{
    auto v=dotPixels(img,x,y,radius);

    std::ostringstream msg;

    // msg<<"#pixels="<<v.size();
    // logger.message(msg.str());
    float m=0.0f;
    switch (m_AvgMethod)
    {
        case avg_mean: 
            m=std::accumulate(v.begin(), v.end(),0.0f)/v.size();
            break;
        case avg_median: break;
            m = median(v);
        default:
            throw ImagingException("Unknown average method for dot pixels", __FILE__, __LINE__);
    }

    return m;
}

float ScatterEstimator::fitError(arma::vec x, arma::vec y)
{
    auto diff = x-y;

    return arma::norm(diff,2)/x.size();
}

float ScatterEstimator::polyVal(float x, float y)
{
    return x*(a*x + b) + y*(c*y + d + e*x) + f;
}

void ScatterEstimator::transferFitParameters()
{
    // float a; // x2
    // float b; // x
    // float c; // y2
    // float d; // y
    // float e; // xy
    // float f; // const
    int col = 0;
    switch (m_nPolyOrderX)
    {
        case 0 : 
            a = b = 0.0f; 
        case 1 : 
            a = 0.0f;
            b = m_fittedParameters[col++]; 
            break;
        case 2 : 
            a = m_fittedParameters[col++]; 
            b = m_fittedParameters[col++]; 
            break;
    }

    switch (m_nPolyOrderY)
    {
        case 0 : 
            c = d = 0.0f; 
        case 1 : 
            c = 0.0f;
            d = m_fittedParameters[col++]; 
            break;
        case 2 : 
            c = m_fittedParameters[col++]; 
            d = m_fittedParameters[col++]; 
            break;
    }

    if (m_nPolyOrderX * m_nPolyOrderY)
        e = m_fittedParameters[col++];

    f = m_fittedParameters[col];
}

std::string enum2string(ScatterEstimator::eAverageMethod m)
{
    std::map<ScatterEstimator::eAverageMethod, std::string> table={
            {ScatterEstimator::avg_mean,   "avgmean"},
            {ScatterEstimator::avg_median, "avgmedian"},
            {ScatterEstimator::avg_min,    "avgmin"},
            {ScatterEstimator::avg_max,    "avgmax"}};

    return table.at(m);
}

void string2enum(const std::string &s, ScatterEstimator::eAverageMethod &m)
{
    std::map<std::string, ScatterEstimator::eAverageMethod> table={
            {"avgmean",   ScatterEstimator::avg_mean},
            {"avgmedian", ScatterEstimator::avg_median},
            {"avgmin",    ScatterEstimator::avg_min},
            {"avgmax",    ScatterEstimator::avg_max}};

    m=table.at(s); 
}

std::ostream & operator<<(std::ostream & s, ScatterEstimator::eAverageMethod m)
{
    s<<enum2string(m);

    return s;
}

std::string enum2string(ScatterEstimator::eFitMethod m)
{
    std::map<ScatterEstimator::eFitMethod, std::string> table={
            {ScatterEstimator::fitmethod_polynomial,      "fitpolynimal"},
            {ScatterEstimator::fitmethod_thinplatesplines,"fitspline"}};

    return table.at(m);
}
void string2enum(const std::string &s, ScatterEstimator::eFitMethod &m)
{    
    std::map<std::string, ScatterEstimator::eFitMethod> table={
            {"fitpolynimal", ScatterEstimator::fitmethod_polynomial},
            {"fitspline",    ScatterEstimator::fitmethod_thinplatesplines}};

    m=table.at(s); 
}

std::ostream & operator<<(std::ostream & s, ScatterEstimator::eFitMethod m)
{
    s<<enum2string(m);

    return s;
}