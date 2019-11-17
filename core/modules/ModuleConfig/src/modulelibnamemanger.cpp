#include "../include/modulelibnamemanger.h"
#include <base/kiplenums.h>
#include <base/KiplException.h>

ModuleLibNameManger::ModuleLibNameManger(const std::string &path) :
    logger("ModuleLibNameManger"),
    m_sApplicationPath(path)
{

}

std::string ModuleLibNameManger::generateLibName(const std::string &name, const kipl::base::eOperatingSystem &os)
{
    std::string fullName;

    switch (os)
    {
        case kipl::base::OSUnknown : throw kipl::base::KiplException("OS not recognized",__FILE__,__LINE__);
        case kipl::base::OSWindows : fullName = generateWindowsLibName(name); break;
        case kipl::base::OSMacOS   : fullName = generateMacOSLibName(name);   break;
        case kipl::base::OSLinux   : fullName = generateLinuxLibName(name);   break;
    }

    return fullName;
}

std::string ModuleLibNameManger::generateLibName(const std::string &name)
{
    return generateLibName(name,kipl::base::getOperatingSystem());
}

std::string ModuleLibNameManger::stripLibName(const std::string &libPath, const kipl::base::eOperatingSystem &os)
{
    std::string moduleName;

    switch (os)
    {
        case kipl::base::OSUnknown : throw kipl::base::KiplException("OS not recognized",__FILE__,__LINE__);
        case kipl::base::OSWindows : moduleName = stripWindowsLibName(libPath); break;
        case kipl::base::OSMacOS   : moduleName = stripMacOSLibName(libPath);   break;
        case kipl::base::OSLinux   : moduleName = stripLinuxLibName(libPath);   break;
    }

    return moduleName;
}

std::string ModuleLibNameManger::stripLibName(const std::string &libPath)
{
    return stripLibName(libPath,kipl::base::getOperatingSystem());
}

std::string ModuleLibNameManger::generateWindowsLibName(const std::string &name)
{
    std::string fullName;

    return fullName;
}

std::string ModuleLibNameManger::generateMacOSLibName(const std::string &name)
{
    std::string fullName;

    return fullName;
}

std::string ModuleLibNameManger::generateLinuxLibName(const std::string &name)
{
    std::string fullName;

    return fullName;
}

std::string ModuleLibNameManger::stripWindowsLibName(const std::string &path)
{
    std::string libName;

    return libName;
}

std::string ModuleLibNameManger::stripMacOSLibName(const std::string &path)
{
    std::string libName;

    return libName;
}

std::string ModuleLibNameManger::stripLinuxLibName(const std::string &path)
{
    std::string libName;

    return libName;
}
