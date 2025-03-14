#ifndef DUMMYCONFIG_H
#define DUMMYCONFIG_H

#include <ConfigBase.h>

class DummyConfig : public ConfigBase
{
public:
    using ConfigBase::operator=;
    DummyConfig(const DummyConfig &other) : ConfigBase(other) {ProjectionInfo = other.ProjectionInfo;}
    DummyConfig(std::string name="DummyConfig", std::string path="");
    DummyConfig& operator=(const DummyConfig& other); 

    std::string WriteXML() override {return "";}  ///< Virtual method to used to stream an XML formatted string

    std::string SanitySlicesCheck() override {return "";}

    void ParseConfig(xmlTextReaderPtr /*reader*/, std::string /*sName*/) override {}

    std::string SanityMessage(bool /*mess*/) override {return "";}

    void ParseProcessChain(xmlTextReaderPtr /*reader*/) override {}



    class cProjectionInfo {
        public:
            cProjectionInfo();

            float fCenter;
        
            bool bCorrectTilt;
        
            std::vector<int> dose_roi;
    };

    cProjectionInfo ProjectionInfo;
};

#endif // DUMMYCONFIG_H
