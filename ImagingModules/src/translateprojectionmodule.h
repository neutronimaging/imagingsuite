//
// This file is part of the i KIPL image processing tool by Anders Kaestner
// (c) 2008,2009,2010,2011,2012,2013 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2013-08-15 21:58:23 +0200 (Thu, 15 Aug 2013) $
// $Rev: 1481 $
//

#ifndef TRANSLATEPROJECTIONMODULE_H
#define TRANSLATEPROJECTIONMODULE_H

#include "ImagingAlgorithms_global.h"
#include <KiplProcessModuleBase.h>
#include <TranslateProjections.h>

class IMAGINGALGORITHMSSHARED_EXPORT TranslateProjectionModule: public KiplProcessModuleBase {
public:
    TranslateProjectionModule();
    virtual ~TranslateProjectionModule();

    virtual int Configure(std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    float m_fSlope;
    float m_fIntercept;
    ImagingAlgorithms::eTranslateSampling m_eSampler;
    float m_fParameter;
};

#endif // TRANSLATEPROJECTIONMODULE_H
