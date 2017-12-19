#ifndef REFERENCEIMAGECORRECTION_H
#define REFERENCEIMAGECORRECTION_H

#include "ImagingModules_global.h"
#include <string>
#include <map>

#include <base/timage.h>
#include <base/kiplenums.h>

#include <KiplProcessModuleBase.h>

class IMAGINGMODULESSHARED_EXPORT ReferenceImageCorrectionModule : public KiplProcessModuleBase
{
public:
    ReferenceImageCorrectionModule();
    ~ReferenceImageCorrectionModule();

    virtual int Configure(std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    void GetDose(kipl::base::TImage<float,3> & img, float *dose);

    bool m_computeLogarithm;
    size_t m_doseROI[4];
    float m_tau;

    std::string m_OBname;
    size_t m_OBfirst;
    size_t m_OBcount;

    std::string m_BBname;
    size_t m_BBfirst;
    size_t m_BBcount;

    std::string m_DCname;
    size_t m_DCfirst;
    size_t m_DCcount;
};

#endif // REFERENCEIMAGECORRECTION_H
