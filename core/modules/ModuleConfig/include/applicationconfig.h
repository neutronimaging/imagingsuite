#ifndef MUHRECCONFIG_H
#define MUHRECCONFIG_H

#include "ModuleConfig_global.h"

#include <string>

class MODULECONFIGSHARED_EXPORT ApplicationConfig
{
public:
    ApplicationConfig(std::string appname="application");

    bool parseXML(std::string &xml);
    std::string streamXML();

    std::string getApplicationVersion();
    std::string getConfigVersion();
    std::string getAppName();
    std::string updatePath(std::string path);

    std::string applicationPath;
    std::string dataPath;

    int memory;

private:
    int parseXML(std::string xml, std::map<std::string, std::string> &info);
    std::string appVersion;
    std::string cfgVersion;
    std::string appName;
};

#endif // MUHRECCONFIG_H
