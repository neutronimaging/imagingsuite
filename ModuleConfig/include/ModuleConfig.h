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

#ifndef MODULECONFIG_H_
#define MODULECONFIG_H_
#include "ModuleConfig_global.h"
#include <map>
#include <list>
#include <string>

#include <libxml/xmlreader.h>
#include <logging/logger.h>

class MODULECONFIGSHARED_EXPORT ModuleConfig
{
protected:
	kipl::logging::Logger logger;

public:
	ModuleConfig(void);
	std::string m_sSharedObject;
	std::string m_sModule;
	bool m_bActive;

	std::map<std::string,std::string> parameters;

	std::string WriteXML(int indent);
	void ParseModule(xmlTextReaderPtr reader);
	std::string PrintParameters();
};

#endif /* MODULECONFIG_H_ */
