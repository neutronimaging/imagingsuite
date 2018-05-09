#ifndef MUHRECCONFIG_H
#define MUHRECCONFIG_H

#include <string>

class ApplicationConfig
{
public:
    ApplicationConfig();

    void parseXML(std::string &xml);
    std::string streamXML();

    std::string updatePath(std::string path);

    std::string applicationPath;
    std::string dataPath;
    std::string version;
    int memory;
};

#endif // MUHRECCONFIG_H
