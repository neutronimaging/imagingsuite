#ifndef DUMMYCONFIG_H
#define DUMMYCONFIG_H

#include <ConfigBase.h>

class DummyConfig : public ConfigBase
{
public:
    DummyConfig(std::string name="DummyConfig", std::string path="");

    virtual std::string WriteXML() {return "";}  ///< Virtual method to used to stream an XML formatted string

    virtual std::string SanitySlicesCheck() {return "";}

    virtual void ParseConfig(xmlTextReaderPtr reader, std::string sName) {}

    virtual std::string SanityMessage(bool mess) {return "";}

    virtual void ParseProcessChain(xmlTextReaderPtr reader) {}
};

#endif // DUMMYCONFIG_H
