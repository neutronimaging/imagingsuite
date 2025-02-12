//<LICENSE>
#ifndef MODULELIBNAMEMANGER_H
#define MODULELIBNAMEMANGER_H
#include "ModuleConfig_global.h"
#include <string>
#include <logging/logger.h>
#include <base/kiplenums.h>

class MODULECONFIGSHARED_EXPORT ModuleLibNameManger
{
    kipl::logging::Logger logger;
public:
    ModuleLibNameManger(const std::string &path, const std::string &category);
    std::string generateLibName(const std::string &name, const kipl::base::eOperatingSystem &os, bool isPlugin = true);
    std::string generateLibName(const std::string &name, bool isPlugin = true);
    std::string stripLibName(const std::string &libPath, const kipl::base::eOperatingSystem &os);
    std::string stripLibName(const std::string &libPath);
    std::string generateLibPath(const kipl::base::eOperatingSystem &os);
    std::string generateLibPath();
    void setAppPath(const std::string &path, const std::string &category);

private:
    std::string generateWindowsLibName(const std::string &name, bool isPlugin);
    std::string generateMacOSLibName(  const std::string &name, bool isPlugin);
    std::string generateLinuxLibName(  const std::string &name, bool isPlugin);

    std::string generateWindowsLibPath();
    std::string generateMacOSLibPath();
    std::string generateLinuxLibPath();

    std::string stripWindowsLibName(const std::string &path);
    std::string stripMacOSLibName(  const std::string &path);
    std::string stripLinuxLibName(  const std::string &path);

    bool libInAppPath(const std::string & path, const std::string &appPath);

    std::string m_sApplicationPath;
    std::string m_sCategoryName;

};

#endif // MODULELIBNAMEMANGER_H
