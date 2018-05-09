#include <QDebug>
#include <sstream>
#include <iomanip>
#include <map>
#include <regex>

#include <strings/filenames.h>
#include "../include/applicationconfig.h"
#include "../include/ModuleException.h"


ApplicationConfig::ApplicationConfig(std::string appname) :
    applicationPath("/"),
    dataPath("/"),
    memory(8192),
    appVersion(VERSION),
    cfgVersion(VERSION),
    appName(appname)
{
    kipl::strings::filenames::CheckPathSlashes(applicationPath,true);
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);
}

bool ApplicationConfig::parseXML(std::string &xml)
{
    std::map<std::string, std::string> info;

    if (parseXML(xml,info)==0) {
        throw ModuleException("Corrupt config data, contains no xml information",__FILE__,__LINE__);
    }

// todo this code should verify the parent block. The block is currently not detected...
//    if (info.find("appconfig")==info.end()) {
//        throw ModuleException("Corrupt config data, contains no appconfig block",__FILE__,__LINE__);
//    }

//    parseXML(info["appconfig"],info);

    std::map<std::string, std::string>::iterator pos;
    if ((pos=info.find("appversion"))!=info.end()) {
        cfgVersion=(*pos).second;
    }

    if ((pos=info.find("applicationpath"))!=info.end()) {
        applicationPath=(*pos).second;
    }

    if ((pos=info.find("datapath"))!=info.end()) {
        dataPath=(*pos).second;
    }

    if ((pos=info.find("memory"))!=info.end()) {
        memory=atoi((*pos).second.c_str());
    }

    if ((pos=info.find("appname"))!=info.end()) {
        appName=(*pos).second;
    }

    bool sameVersion = (appVersion == cfgVersion);

    return sameVersion;
}

std::string ApplicationConfig::streamXML()
{
    std::ostringstream xml;
    int indent=0;

    xml<<std::setw(indent)<<"<appconfig>\n";
    xml<<std::setw(indent+4)<<"<appversion>"<<appVersion<<"</appversion>\n";
    xml<<std::setw(indent+4)<<"<applicationpath>"<<applicationPath<<"</applicationpath>\n";
    xml<<std::setw(indent+4)<<"<datapath>"<<dataPath<<"</datapath>\n";
    xml<<std::setw(indent+4)<<"<memory>"<<memory<<"</memory>\n";
    xml<<"</appconfig>\n";

    return xml.str();
}

std::string ApplicationConfig::getApplicationVersion()
{
    return std::string(VERSION);
}

std::string ApplicationConfig::getConfigVersion()
{
    return std::string(VERSION);
}

std::string ApplicationConfig::getAppName()
{
    return appName;
}

std::string ApplicationConfig::updatePath(std::string path)
{
    kipl::strings::filenames::CheckPathSlashes(path,false);
    size_t pos=0UL;
    if ((pos=path.find('@')) == std::string::npos)
        return path;

    size_t pos2=path.find(kipl::strings::filenames::slash,pos);
    std::map<std::string,int> tags;
    tags["@applicationpath"]=0;
    tags["@datapath"]=1;
    std::string updatedPath=path.substr(0,pos);

    switch (tags[path.substr(pos,pos2-pos)]) {
    case 0 : updatedPath+=applicationPath; break;
    case 1 : updatedPath+=dataPath; break;
    }

    updatedPath+=path.substr(pos2+1);

    return updatedPath;
}

int ApplicationConfig::parseXML(std::string xml, std::map<std::string, std::string> &info)
{
    std::regex reg("<(.*)>(.*)</(\\1)>");

    auto pos=xml.cbegin();
    auto end=xml.cend();

    std::smatch m;
    info.clear();
    for (; regex_search(pos,end,m,reg); pos=m.suffix().first) {
        info.insert(std::make_pair(m.str(1),m.str(2)));
    }

    return (int)info.size();
}


