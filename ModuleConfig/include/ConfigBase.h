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

#ifndef __CONFIGBASE_H
#define __CONFIGBASE_H
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#define DLL_EXPORT __declspec(dllexport)

#else
#define DLL_EXPORT
#endif

#include <map>
#include <list>
#include <set>
#include <string>

#include <libxml/xmlreader.h>

#include "ModuleConfig.h"

#include <logging/logger.h>
//#include <io/io_stack.h>

class DLL_EXPORT ConfigBase
{
protected:
	kipl::logging::Logger logger;
	std::string m_sName;
    std::string m_sApplicationPath;

public:
	struct DLL_EXPORT cUserInformation {
		cUserInformation() ;
		cUserInformation(const cUserInformation &info);
		cUserInformation & operator=(const cUserInformation & info);
		std::string WriteXML(size_t indent=0);

		std::string sOperator;
		std::string sInstrument;
		std::string sProjectNumber;
		std::string sSample;
		std::string sComment;
        std::string sDate;
	};

    ConfigBase(std::string name="ConfigBase", std::string path="");
    ConfigBase(const ConfigBase &config);
    virtual const ConfigBase & operator=(const ConfigBase &config);

	virtual ~ConfigBase(void);

	cUserInformation UserInformation;
	std::list<ModuleConfig> modules;
	virtual std::string WriteXML()=0;
	void LoadConfigFile(std::string configfile, std::string ProjectName);

protected:
	virtual void ParseConfig(xmlTextReaderPtr reader, std::string sName)=0;
	void ParseUserInformation(xmlTextReaderPtr reader);
	virtual void ParseProcessChain(xmlTextReaderPtr reader)=0;
};

#endif
