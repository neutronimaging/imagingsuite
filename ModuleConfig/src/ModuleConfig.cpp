//
// This file is part of the ModuleConfig library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//

#include "stdafx.h"
#include "../include/ModuleConfig.h"
#include "../include/ModuleException.h"
#include <sstream>
#include <iomanip>

#include <strings/miscstring.h>
#include <strings/string2array.h>


ModuleConfig::ModuleConfig(void) :
	logger("ModuleConfig"),
    m_sSharedObject("NoObjectFile"),
    m_sModule("Empty"),
    m_bActive(true)
{
}

std::string ModuleConfig::WriteXML(int indent)
{
	std::ostringstream str;

	str<<std::setw(indent)<<" "<<"<module>\n"
		<<std::setw(indent+4)<<" "<<"<modulename>"<<m_sModule<<"</modulename>\n"
		<<std::setw(indent+4)<<" "<<"<sharedobject>"<<m_sSharedObject<<"</sharedobject>\n"
		<<std::setw(indent+4)<<" "<<"<active>"<<(m_bActive ? "true":"false")<<"</active>\n";
	if (!parameters.empty()) {
		str<<std::setw(indent+4)<<" "<<"<parameters>\n";
		std::map<std::string,std::string>::iterator it;

		for (it=parameters.begin(); it!=parameters.end(); it++) {
			str<<std::setw(indent+8)<<" "<<"<"<<it->first<<">"<<it->second<<"</"<<it->first<<">\n";
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
			if (name==NULL) {
				throw ModuleException("Unexpected contents in parameter file",__FILE__,__LINE__);
			}
			if (value!=NULL)
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
				m_sSharedObject=sValue;
			}
			if (sName=="active") {
				m_bActive=kipl::strings::string2bool(sValue);
			}
			if (sName=="parameters") {
				int depth2=xmlTextReaderDepth(reader);
				while (ret == 1) {
					if (xmlTextReaderNodeType(reader)==1) {
						name = xmlTextReaderConstName(reader);
						ret=xmlTextReaderRead(reader);

						value = xmlTextReaderConstValue(reader);
						if (name==NULL) {
							throw ModuleException("Unexpected contents in parameter file",__FILE__,__LINE__);
						}
						if (value!=NULL)
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
