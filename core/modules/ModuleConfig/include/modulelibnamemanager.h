//<LICENSE>
#ifndef ModuleLibNameManager_H
#define ModuleLibNameManager_H
#include "ModuleConfig_global.h"
#include <string>
#include <logging/logger.h>
#include <base/kiplenums.h>

class MODULECONFIGSHARED_EXPORT ModuleLibNameManager
{
    kipl::logging::Logger logger;
public:
    ModuleLibNameManager(const std::string &path, const bool usePluginPath, const std::string &category);
    std::string generateLibName(const std::string &name, const kipl::base::eOperatingSystem &os);
    std::string generateLibName(const std::string &name);
    std::string stripLibName(const std::string &libPath, const kipl::base::eOperatingSystem &os);
    std::string stripLibName(const std::string &libPath);
    std::string generateLibPath(const kipl::base::eOperatingSystem &os);
    std::string generateLibPath(const bool usePluginPath = false);
    void setAppPath(const std::string &path, const std::string &category);

private:
    std::string generateWindowsLibName( const std::string &name);
    std::string generateMacOSLibName(   const std::string &name);
    std::string generateLinuxLibName(   const std::string &name);

    std::string generateWindowsLibPath();
    std::string generateMacOSLibPath();
    std::string generateLinuxLibPath();

    std::string stripWindowsLibName(const std::string &path);
    std::string stripMacOSLibName(  const std::string &path);
    std::string stripLinuxLibName(  const std::string &path);

    bool libInAppPath(const std::string & path, const std::string &appPath);

    std::string m_sApplicationPath;
    bool m_usePluginPath;
    std::string m_sCategoryName;


};

#endif // ModuleLibNameManager_H
