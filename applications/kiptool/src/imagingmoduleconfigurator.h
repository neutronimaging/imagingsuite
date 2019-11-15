#ifndef IMAGINGMODULECONFIGURATOR_H
#define IMAGINGMODULECONFIGURATOR_H

#include <ModuleConfigurator.h>
#include <KiplProcessConfig.h>

class ImagingModuleConfigurator : public ModuleConfigurator
{
public:
    ImagingModuleConfigurator(KiplProcessConfig *config);
    virtual ~ImagingModuleConfigurator();
protected:
    virtual int GetImage(std::string sSelectedModule);

};

#endif // IMAGINGMODULECONFIGURATOR_H
