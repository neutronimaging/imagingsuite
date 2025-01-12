//<LICENSE>
#include "../include/modulelibnamemanger.h"
#include <base/kiplenums.h>
#include <base/KiplException.h>
#include <strings/filenames.h>

ModuleLibNameManger::ModuleLibNameManger(const std::string &path) :
    logger("ModuleLibNameManger"),
    m_sApplicationPath(path)
{
//   kipl::strings::filenames::CheckPathSlashes(m_sApplicationPath,true);
}

std::string ModuleLibNameManger::generateLibName(const std::string &name, const kipl::base::eOperatingSystem &os)
{
    if (name.find_first_of("/\\")!=std::string::npos)
        return name;

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

void ModuleLibNameManger::setAppPath(const std::string &path)
{
    m_sApplicationPath = path;
    kipl::strings::filenames::CheckPathSlashes(m_sApplicationPath,true);
}

std::string ModuleLibNameManger::generateWindowsLibName(const std::string &name)
{
    std::string fullName=m_sApplicationPath;

    fullName = fullName+name+".dll";

    return fullName;
}

std::string ModuleLibNameManger::generateMacOSLibName(const std::string &name)
{
    std::string fullName=m_sApplicationPath.substr(0,m_sApplicationPath.size() - 6 - (*m_sApplicationPath.rbegin()=='/' ? 1 : 0));

    fullName = fullName+"/Frameworks/lib"+name+".dylib";

    return fullName;
}

std::string ModuleLibNameManger::generateLinuxLibName(const std::string &name)
{
    std::string fullName=m_sApplicationPath.substr(0,m_sApplicationPath.size() - 3 - (*m_sApplicationPath.rbegin()=='/' ? 1 : 0));

    fullName = fullName+"lib/lib"+name+".so";

    return fullName;
}

std::string ModuleLibNameManger::stripWindowsLibName(const std::string &path)
{
    if (!libInAppPath(path,m_sApplicationPath))
    {
        return path;
    }
    std::string libName;

    libName = path.substr(path.find_last_of("/\\")+1);
    libName = libName.substr(0,libName.find_first_of('.'));
    return libName;
}

std::string ModuleLibNameManger::stripMacOSLibName(const std::string &path)
{
    std::ostringstream msg;
    if (!libInAppPath(path,m_sApplicationPath.substr(0,m_sApplicationPath.size()-7)+"/Frameworks") &&
        !libInAppPath(path,m_sApplicationPath+"../Frameworks")   )
    {
        msg << path.c_str()<<" is not in lib path";
        logger.verbose(msg.str());
        return path;
    }

    std::string libName;

    libName = path.substr(path.find_last_of('/')+1);
    libName = libName.substr(3,libName.find_first_of('.')-3);
    logger.verbose(std::string("result: ")+path+" "+libName);

    return libName;
}

std::string ModuleLibNameManger::stripLinuxLibName(const std::string &path)
{
    std::ostringstream msg;


    if (!libInAppPath(path,m_sApplicationPath.substr(0,m_sApplicationPath.size()-4)+"/lib") &&
        !libInAppPath(path,m_sApplicationPath+"../lib/")   )
    {
        msg << path.c_str()<<" is not in lib path";
        logger.verbose(msg.str());
        return path;
    }

    std::string libName;

    libName = path.substr(path.find_last_of('/')+1);
    libName = libName.substr(3,libName.find_first_of('.')-3);

    return libName;
}

bool ModuleLibNameManger::libInAppPath(const std::string &path, const std::string &appPath)
{
    if (path.size()<appPath.size())
    {
        logger.message(path +" < "+appPath);
        return false;
    }

    auto truncated = path.substr(0,appPath.size());

    logger.message(truncated +" == "+appPath);

    if ((truncated != appPath) || path.find_first_of("/\\",appPath.size()+1)!=std::string::npos)
    {
        logger.warning(truncated +" != "+appPath);
        return false;
    }
    return true;
}
