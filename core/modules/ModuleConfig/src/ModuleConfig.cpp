//<LICENSE>

#include "../include/ModuleConfig_global.h"
#include "../include/ModuleConfig.h"
#include "../include/ModuleException.h"
#include "../include/modulelibnamemanger.h"
#include <sstream>
#include <iomanip>

#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <strings/filenames.h>


ModuleConfig::ModuleConfig(const std::string &appPath) :
	logger("ModuleConfig"),
    m_sSharedObject("NoObjectFile"),
    m_sModule("Empty"),
    m_bActive(true),
    m_bThreading(false),
    m_NameManager(appPath)
{
}

const std::string ModuleConfig::WriteXML(int indent)
{
	std::ostringstream str;

    kipl::strings::filenames::CheckPathSlashes(m_sSharedObject,false);
	str<<std::setw(indent)<<" "<<"<module>\n"
		<<std::setw(indent+4)<<" "<<"<modulename>"<<m_sModule<<"</modulename>\n"
        <<std::setw(indent+4)<<" "<<"<sharedobject>"<<m_NameManager.stripLibName(m_sSharedObject)<<"</sharedobject>\n"
        <<std::setw(indent+4)<<" "<<"<active>"<<(m_bActive ? "true":"false")<<"</active>\n"
        <<std::setw(indent+4)<<" "<<"<threading>"<<(m_bThreading ? "true":"false")<<"</threading>\n";

	if (!parameters.empty()) {
		str<<std::setw(indent+4)<<" "<<"<parameters>\n";
		std::map<std::string,std::string>::iterator it;

        for (auto &parameter :parameters) {
            str<<std::setw(indent+8)<<" "<<"<"<<parameter.first<<">"<<parameter.second<<"</"<<parameter.first<<">\n";
		}

		str<<std::setw(indent+4)<<" "<<"</parameters>\n";
	}
	str<<std::setw(indent)<<" "<<"</module>"<<std::endl;

	return str.str();
}

void ModuleConfig::ParseModule(xmlTextReaderPtr reader)
{
	std::ostringstream msg;

	parameters.clear();

	const xmlChar *name, *value;
    int ret = xmlTextReaderRead(reader);
    std::string sName, sValue;
    msg.str("");
    msg<<"Parsing module "<<m_sModule<<".";
	logger(kipl::logging::Logger::LogVerbose,msg.str());
	int depth=xmlTextReaderDepth(reader);
    while (ret == 1) {
		if (xmlTextReaderNodeType(reader)==1) {
			name = xmlTextReaderConstName(reader);
			ret=xmlTextReaderRead(reader);

			value = xmlTextReaderConstValue(reader);
            if (name==nullptr) {
				throw ModuleException("Unexpected contents in parameter file",__FILE__,__LINE__);
			}
            if (value!=nullptr)
				sValue=reinterpret_cast<const char *>(value);
			else {
				msg.str("");
				msg<<m_sModule<<"::Parameter "<<sValue<<" does not have a value";
				logger(kipl::logging::Logger::LogWarning,msg.str());
				sValue="Empty";
			}
			sName=reinterpret_cast<const char *>(name);
			if (sName=="modulename") {
				m_sModule=sValue;
			}
			if (sName=="sharedobject") {
                m_sSharedObject=m_NameManager.generateLibName(sValue);
			}
			if (sName=="active") {
				m_bActive=kipl::strings::string2bool(sValue);
			}

            if (sName=="threading") {
                m_bThreading=kipl::strings::string2bool(sValue);
            }
			if (sName=="parameters") {
				int depth2=xmlTextReaderDepth(reader);
				while (ret == 1) {
					if (xmlTextReaderNodeType(reader)==1) {
						name = xmlTextReaderConstName(reader);
						ret=xmlTextReaderRead(reader);

						value = xmlTextReaderConstValue(reader);
                        if (name==nullptr) {
							throw ModuleException("Unexpected contents in parameter file",__FILE__,__LINE__);
						}
                        if (value!=nullptr)
							sValue=reinterpret_cast<const char *>(value);
						else
							sValue="Empty";
						sName=reinterpret_cast<const char *>(name);

						parameters[sName]=sValue;
					}
					ret = xmlTextReaderRead(reader);
					if (xmlTextReaderDepth(reader)<depth2)
						ret=0;
				}
			}
		}
		ret = xmlTextReaderRead(reader);
		if (xmlTextReaderDepth(reader)<depth)
			ret=0;
	}
}

std::string ModuleConfig::PrintParameters()
{
	std::ostringstream s;

	std::map<std::string,std::string>::iterator it;

	for (it=parameters.begin(); it!=parameters.end(); it++) {
		s<<std::setw(16)<<it->first<<" = "<<it->second<<"\n";
	}

    return s.str();
}

void ModuleConfig::setAppPath(const std::string &path)
{
    m_NameManager.setAppPath(path);
}

std::string ModuleConfig::modulePath()
{
    return m_NameManager.generateLibName(m_sModule);
}

std::string ModuleConfig::moduleSummary()
{
   std::ostringstream summary;

   summary << "path:"          << m_sSharedObject
           <<", module:"       << m_sModule
           <<", "              << (m_bActive ?    "active" : "disabled")
           <<", threading is " << (m_bThreading ? "active" : "disabled");

   return summary.str();
}

std::ostream &operator<<(std::ostream &s, ModuleConfig &mc)
{
    s << mc.moduleSummary();

    return s;
}
