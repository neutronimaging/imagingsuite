#include "../include/modulelibnamemanger.h"
#include <base/kiplenums.h>
#include <base/KiplException.h>

ModuleLibNameManger::ModuleLibNameManger(const std::string &path) :
    logger("ModuleLibNameManger"),
    m_sApplicationPath(path)
{

}

std::string ModuleLibNameManger::generateLibName(std::string name)
{
    std::string fullName;

    switch (kipl::base::getOperatingSystem())
    {
        case kipl::base::OSUnknown : throw kipl::base::KiplException("OS not recognized",__FILE__,__LINE__);
        case kipl::base::OSWindows : fullName = generateWindowsLibName(name); break;
        case kipl::base::OSMacOS   : fullName = generateMacOSLibName(name);   break;
        case kipl::base::OSLinux   : fullName = generateLinuxLibName(name);   break;
    }

    return fullName;
}

std::string ModuleLibNameManger::stripLibName(std::string libPath)
{
    std::string fullName;

    return fullName;
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
