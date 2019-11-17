#ifndef MODULELIBNAMEMANGER_H
#define MODULELIBNAMEMANGER_H

#include <string>
#include <logging/logger.h>
#include <base/kiplenums.h>

class ModuleLibNameManger
{
    kipl::logging::Logger logger;
public:
    ModuleLibNameManger(const std::string &path);
    std::string generateLibName(const std::string &name, const kipl::base::eOperatingSystem &os);
    std::string generateLibName(const std::string &name);
    std::string stripLibName(const std::string &libPath, const kipl::base::eOperatingSystem &os);
    std::string stripLibName(const std::string &libPath);

private:
    std::string generateWindowsLibName(const std::string &name);
    std::string generateMacOSLibName(const std::string &name);
    std::string generateLinuxLibName(const std::string &name);

    std::string stripWindowsLibName(const std::string &path);
    std::string stripMacOSLibName(const std::string &path);
    std::string stripLinuxLibName(const std::string &path);

    std::string m_sApplicationPath;

};

#endif // MODULELIBNAMEMANGER_H
