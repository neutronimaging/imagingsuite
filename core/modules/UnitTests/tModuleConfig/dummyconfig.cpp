#include "dummyconfig.h"

DummyConfig::cProjectionInfo::cProjectionInfo() :
    dose_roi(4,0)
{}
DummyConfig::DummyConfig(std::string name, std::string path) :
    ConfigBase(name,path)
{

}

DummyConfig& DummyConfig::operator=(const DummyConfig& other)
{
    if (this != &other) {
        ConfigBase::operator=(other);
        ProjectionInfo = other.ProjectionInfo;
    }
    return *this;
}