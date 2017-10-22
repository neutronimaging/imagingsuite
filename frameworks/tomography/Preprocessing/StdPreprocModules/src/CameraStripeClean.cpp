//#include "stdafx.h"
#include "../include/StdPreprocModules_global.h"
#include "../include/CameraStripeClean.h"
#include <ParameterHandling.h>
#include <base/timage.h>
#include <strings/miscstring.h>
#include <filters/filter.h>

CameraStripeClean::CameraStripeClean() :
    m_fThreshold(50000.0f)
{
}

CameraStripeClean::~CameraStripeClean()
{}

int CameraStripeClean::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    m_fThreshold = GetFloatParameter(parameters,"threshold");
    return 0;
}

std::map<std::string, std::string> CameraStripeClean::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["threshold"]=kipl::strings::value2string(m_fThreshold);

    return parameters;
}

bool CameraStripeClean::SetROI(size_t *roi)
{
    return false;
}

int CameraStripeClean::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
    kipl::base::TImage<float,2> orig=img;
    orig.Clone();

    float *prev   = orig.GetLinePtr(0);
    float *center = img.GetLinePtr(1);
    float *next   = orig.GetLinePtr(2);


    int sx=img.Size(0);
    for (size_t i=1; i<img.Size(1); i++)
    {
        for (size_t j=0; j<img.Size(0); j++) {
            if (m_fThreshold<center[j]) {
                center[j]=0.5f*(prev[j]+next[j]);
            }
        }
        prev+=sx;
        center+=sx;
        next+=sx;
    }

    // First line
    center = img.GetLinePtr(0);
    prev   = orig.GetLinePtr(1);
    for (size_t j=0; j<img.Size(0); j++) {
        if (m_fThreshold<center[j]) {
            center[j]=prev[j];
        }
    }

    // First line
    center = img.GetLinePtr(img.Size(1)-1);
    prev   = orig.GetLinePtr(img.Size(1)-2);

    for (int j=0; j<img.Size(0); j++) {
        if (m_fThreshold<center[j]) {
            center[j]=prev[j];
        }
    }

    return 0;
}

int CameraStripeClean::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    kipl::base::TImage<float,2> proj(img.Dims());

    for (int j=0; j<img.Size(2); j++) {
        memcpy(proj.GetDataPtr(),img.GetLinePtr(0,j),proj.Size()*sizeof(float));
        ProcessCore(proj,coeff);
        memcpy(img.GetLinePtr(0,j),proj.GetDataPtr(),proj.Size()*sizeof(float));
    }

    return 0;
}
