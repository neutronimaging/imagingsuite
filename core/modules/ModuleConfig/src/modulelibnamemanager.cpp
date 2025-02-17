//<LICENSE>
#include "../include/modulelibnamemanager.h"
#include <base/kiplenums.h>
#include <base/KiplException.h>
#include <strings/filenames.h>

ModuleLibNameManager::ModuleLibNameManager(const std::string &path, const bool usePluginPath, const std::string &category) :
    logger("ModuleLibNameManager"),
    m_sApplicationPath(path),
    m_usePluginPath(usePluginPath),
    m_sCategoryName(category)
{
    // kipl::strings::filenames::CheckPathSlashes(m_sApplicationPath,true);
    // kipl::strings::filenames::CheckPathSlashes(m_sCategoryName,true);
}

std::string ModuleLibNameManager::generateLibName(const std::string &name,const kipl::base::eOperatingSystem &os)
{
    if (name.find_first_of("/\\")!=std::string::npos)
        return name;

    std::string fullName;

    switch (os)
    {
        case kipl::base::OSUnknown : throw kipl::base::KiplException("OS not recognized",__FILE__,__LINE__);
        case kipl::base::OSWindows : fullName = generateWindowsLibName( name); break;
        case kipl::base::OSMacOS   : fullName = generateMacOSLibName(   name); break;
        case kipl::base::OSLinux   : fullName = generateLinuxLibName(   name); break;
    }

    return fullName;
}

std::string ModuleLibNameManager::generateLibName(const std::string &name)
{
    return generateLibName(name,kipl::base::getOperatingSystem());
}

std::string ModuleLibNameManager::stripLibName(const std::string &libPath, const kipl::base::eOperatingSystem &os)
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

std::string ModuleLibNameManager::generateLibPath(const kipl::base::eOperatingSystem &os)
{
    std::string libPath;

    switch (os)
    {
        case kipl::base::OSUnknown : throw kipl::base::KiplException("OS not recognized",__FILE__,__LINE__);
        case kipl::base::OSWindows : libPath = generateWindowsLibPath(); break;
        case kipl::base::OSMacOS   : libPath = generateMacOSLibPath();   break;
        case kipl::base::OSLinux   : libPath = generateLinuxLibPath();   break;
    }

    return libPath;
}

std::string ModuleLibNameManager::generateLibPath(const bool usePluginPath)
{
    return generateLibPath(kipl::base::getOperatingSystem());
}

std::string ModuleLibNameManager::stripLibName(const std::string &libPath)
{
    return stripLibName(libPath,kipl::base::getOperatingSystem());
}

void ModuleLibNameManager::setAppPath(const std::string &path, const std::string &category)
{
    m_sApplicationPath = path;
    kipl::strings::filenames::CheckPathSlashes(m_sApplicationPath,true);
    
    m_sCategoryName = category;
    kipl::strings::filenames::CheckPathSlashes(m_sCategoryName,true);
}   

std::string ModuleLibNameManager::generateWindowsLibName(const std::string &name)
{
    std::string fullName=m_sApplicationPath;

    if (m_usePluginPath)
        fullName = fullName+"PlugIns\\"+m_sCategoryName+"\\"+name+".dll";
    else
        fullName = fullName+name+".dll";

    return fullName;
}

std::string ModuleLibNameManager::generateMacOSLibName(const std::string &name)
{
    std::string fullName=m_sApplicationPath.substr(0,m_sApplicationPath.size() - 6 - (*m_sApplicationPath.rbegin()=='/' ? 1 : 0));

    if (m_usePluginPath)
        fullName = fullName+"/PlugIns/"+m_sCategoryName+"/lib"+name+".dylib";
    else
        fullName = fullName+"/Frameworks/lib"+name+".dylib";

    return fullName;
}

std::string ModuleLibNameManager::generateLinuxLibName(const std::string &name)
{
    std::string fullName=m_sApplicationPath.substr(0,m_sApplicationPath.size() - 3 - (*m_sApplicationPath.rbegin()=='/' ? 1 : 0));

    if (m_usePluginPath)
        fullName = fullName+"PlugIns/"+m_sCategoryName+"/lib"+name+".so";
    else
        fullName = fullName+"lib/lib"+name+".so";

    return fullName;
}

std::string ModuleLibNameManager::generateWindowsLibPath()
{
    if (m_usePluginPath)
        return m_sApplicationPath+"PlugIns\\"+m_sCategoryName+"\\";
    else
        return m_sApplicationPath;
}

std::string ModuleLibNameManager::generateMacOSLibPath()
{
    if (m_usePluginPath)
        return m_sApplicationPath.substr(0,m_sApplicationPath.size()-7)+"/PlugIns/"+m_sCategoryName+"/";
    else
        return m_sApplicationPath.substr(0,m_sApplicationPath.size()-7)+"/Frameworks/";

}

std::string ModuleLibNameManager::generateLinuxLibPath()
{
    if (m_usePluginPath)
        return m_sApplicationPath.substr(0,m_sApplicationPath.size()-4)+"/PlugIns/"+m_sCategoryName+"/";
    else
        return m_sApplicationPath.substr(0,m_sApplicationPath.size()-4)+"/lib/";
}

std::string ModuleLibNameManager::stripWindowsLibName(const std::string &path)
{
    if (m_usePluginPath) 
    {
        if (!libInAppPath(path,m_sApplicationPath+"PlugIns\\"+m_sCategoryName))
        {
            logger.message(path+" is not in the plugin path");
            return path;
        }
    }
    else
    {
        if (!libInAppPath(path,m_sApplicationPath))
        {
            logger.message(path+" is not in the plugin path");
            return path;
        }
    }

    std::string libName;

    libName = path.substr(path.find_last_of("/\\")+1);
    libName = libName.substr(0,libName.find_first_of('.'));
    return libName;
}

std::string ModuleLibNameManager::stripMacOSLibName(const std::string &path)
{
    if (m_usePluginPath)
    {
        if (!libInAppPath(path,m_sApplicationPath.substr(0,m_sApplicationPath.size()-7)+"/PlugIns/"+m_sCategoryName) &&
            !libInAppPath(path,m_sApplicationPath+"../PlugIns/"+m_sCategoryName)   )
        {
            logger.message(path+" is not in the plugin path");
            return path;
        }
    }
    else
    {
        if (!libInAppPath(path,m_sApplicationPath.substr(0,m_sApplicationPath.size()-7)+"/Frameworks") &&
            !libInAppPath(path,m_sApplicationPath+"../Frameworks")   )
        {
            logger.message(path+" is not in the plugin path");
            return path;
        }
    }
    
    std::string libName;

    libName = path.substr(path.find_last_of('/')+1);
    libName = libName.substr(3,libName.find_first_of('.')-3);
    logger.verbose(std::string("result: ")+path+" "+libName);

    return libName;
}

std::string ModuleLibNameManager::stripLinuxLibName(const std::string &path)
{
    if (m_usePluginPath)
    {
        if (!libInAppPath(path,m_sApplicationPath.substr(0,m_sApplicationPath.size()-4)+"/PlugIns/"+m_sCategoryName) &&
            !libInAppPath(path,m_sApplicationPath+"/../PlugIns/"+m_sCategoryName)   )
        {
            logger.message(path+" is not in the plugin path");
            return path;
        }
    }
    else
    {
        if (!libInAppPath(path,m_sApplicationPath.substr(0,m_sApplicationPath.size()- (*m_sApplicationPath.rbegin()=='/' ? 5 : 4))+"/lib") &&
            !libInAppPath(path,m_sApplicationPath+"/../lib")   )
        {
            logger.message(path+" is not in the plugin path");
            return path;
        }
    }

    std::string libName;

    libName = path.substr(path.find_last_of('/')+1);
    libName = libName.substr(3,libName.find_first_of('.')-3);

    return libName;
}

bool ModuleLibNameManager::libInAppPath(const std::string &path, const std::string &appPath)
{
    if (path.size()<appPath.size())
    {
        logger.message("The module path " + path +" < "+appPath);
        return false;
    }

    auto truncated = path.substr(0,appPath.size());

    logger.message("After truncation("+truncated +") == AppPath("+appPath +")?");

    if ((truncated != appPath) || path.find_first_of("/\\",appPath.size()+1)!=std::string::npos)
    {
        logger.warning(truncated +" != "+appPath);
        return false;
    }
    return true;
}
