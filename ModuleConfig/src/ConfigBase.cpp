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
#include "../include/ConfigBase.h"
#include "../include/ModuleException.h"
#include <sstream>
#include <iomanip>

#include <utilities/TimeDate.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>

ConfigBase::ConfigBase(std::string name, std::string path) :
	logger(name),
    m_sName(name),
    m_sApplicationPath(path)
{

}

ConfigBase::~ConfigBase(void)
{
}

ConfigBase::ConfigBase(const ConfigBase &config) :
	logger(config.m_sName),
	m_sName(config.m_sName),
    m_sApplicationPath(config.m_sApplicationPath),
    modules(config.modules)
{

}


const ConfigBase & ConfigBase::operator=(const ConfigBase &config)
{
	m_sName=config.m_sName;
    m_sApplicationPath=config.m_sApplicationPath;
	modules=config.modules;

	return *this;
}

void ConfigBase::LoadConfigFile(std::string configfile, std::string ProjectName)
{
    xmlTextReaderPtr reader;
    const xmlChar *name;
    int ret;
    std::string sName;
    std::ostringstream msg;

    modules.clear();
    reader = xmlReaderForFile(configfile.c_str(), NULL, 0);
    if (reader != NULL) {
    	ret = xmlTextReaderRead(reader);
        name = xmlTextReaderConstName(reader);


        if (name==NULL) {
            throw ModuleException("Unexpected contents in parameter file",__FILE__,__LINE__);
        }

        sName=reinterpret_cast<const char *>(name);

        logger(kipl::logging::Logger::LogError,sName);
        if (sName!=ProjectName) {
            msg.str();
            msg<<"Unexpected project contents in parameter file ("<<sName<<"!="<<ProjectName<<")";
            throw ModuleException(msg.str(),__FILE__,__LINE__);
        }
        
        logger(kipl::logging::Logger::LogVerbose,"Got project");
    	
        ret = xmlTextReaderRead(reader);
        
        while (ret == 1) {
        	if (xmlTextReaderNodeType(reader)==1) {
	            name = xmlTextReaderConstName(reader);
	            
	            if (name==NULL) {
	                throw ModuleException("Unexpected contents in parameter file",__FILE__,__LINE__);
	            }
	            sName=reinterpret_cast<const char *>(name);

				if (sName=="userinformation")
					ParseUserInformation(reader);

				if (sName=="processchain")
					ParseProcessChain(reader);

				ParseConfig(reader,sName);
        	}
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
        	std::stringstream str;
        	str<<"Module config failed to parse "<<configfile;
        	throw ModuleException(str.str(),__FILE__,__LINE__);
        }
    } else {
    	std::stringstream str;
    	str<<"Module config could not open "<<configfile;
    	throw ModuleException(str.str(),__FILE__,__LINE__);
    }
    logger(kipl::logging::Logger::LogVerbose,"Parsing parameter file done");
}


void ConfigBase::ParseUserInformation(xmlTextReaderPtr reader)
{
	const xmlChar *name, *value;
    int ret = xmlTextReaderRead(reader);
    std::string sName, sValue;
    int depth=xmlTextReaderDepth(reader);

    logger(kipl::logging::Logger::LogVerbose,"Enter parse UserInfo");
  
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

	        if (sName=="operator") {
				UserInformation.sOperator=reinterpret_cast<const char *>(value);
	        }

	        if (sName=="instrument") {
				UserInformation.sInstrument=reinterpret_cast<const char *>(value);
	        }

			if (sName=="projectnumber") {
				UserInformation.sProjectNumber=reinterpret_cast<const char *>(value);
	        }

			if (sName=="sample") {
				UserInformation.sSample=reinterpret_cast<const char *>(value);
	        }

			if (sName=="comment") {
				if (value!=NULL)
					UserInformation.sComment=reinterpret_cast<const char *>(value);
	        }
            if (sName=="date") {
                if (value!=NULL)
                    UserInformation.sDate=reinterpret_cast<const char *>(value);
            }

		}
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
        	ret=0;
	}
  
    logger(kipl::logging::Logger::LogVerbose,"Done parse UserInfo");
}
	

//----------------------
ConfigBase::cUserInformation::cUserInformation() :
	sOperator("Anders Kaestner"),
	sInstrument("ICON"),
	sProjectNumber("P11001"),
	sSample("Unknown item"),
    sComment("No comment"),
    sDate(kipl::utilities::TimeStamp())
{
}

ConfigBase::cUserInformation::cUserInformation(const cUserInformation &info) :
	sOperator(info.sOperator),
	sInstrument(info.sInstrument),
	sProjectNumber(info.sProjectNumber),
	sSample(info.sSample),
    sComment(info.sComment),
    sDate(info.sDate)
{
}

ConfigBase::cUserInformation & ConfigBase::cUserInformation::operator = (const cUserInformation &info)
{
	sOperator      = info.sOperator;
	sInstrument    = info.sInstrument;
	sProjectNumber = info.sProjectNumber;
	sSample        = info.sSample;
	sComment       = info.sComment;
    sDate          = info.sDate;
	return * this;
}

std::string ConfigBase::cUserInformation::WriteXML(size_t indent)
{
	using namespace std;
	ostringstream str;

	str<<setw(indent)  <<" "<<"<userinformation>"<<endl;
		str<<setw(indent+4)  <<" "<<"<operator>"<<sOperator<<"</operator>\n";
		str<<setw(indent+4)  <<" "<<"<instrument>"<<sInstrument<<"</instrument>\n";
		str<<setw(indent+4)  <<" "<<"<projectnumber>"<<sProjectNumber<<"</projectnumber>\n";
		str<<setw(indent+4)  <<" "<<"<sample>"<<sSample<<"</sample>\n";
		str<<setw(indent+4)  <<" "<<"<comment>"<<sComment<<"</comment>\n";
        str<<setw(indent+4)  <<" "<<"<date>"<<sDate<<"</date>\n";
	str<<setw(indent)  <<" "<<"</userinformation>"<<endl;

	return str.str();
}


