//<LICENSE>

#ifndef NONLOCALMEANSMODULE_H
#define NONLOCALMEANSMODULE_H

#include "AdvancedFilterModules_global.h"
#include <KiplProcessModuleBase.h>

class ADVANCEDFILTERMODULES_EXPORT NonLocalMeansModule: public KiplProcessModuleBase {
public:
    NonLocalMeansModule();
    virtual ~NonLocalMeansModule();

    virtual int Configure(std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

    float m_fSensitivity;
    int m_nWidth;
};

#endif // NONLOCALMEANSMODULE_H
