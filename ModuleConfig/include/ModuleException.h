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

#ifndef _MODULEEXCEPTION_H_
#define _MODULEEXCEPTION_H_
#include <string>

#include "../src/stdafx.h"
#include <base/KiplException.h>

class  DLL_EXPORT ModuleException : public kipl::base::KiplException
{
public:
	ModuleException(void);
	ModuleException(std::string msg);
	ModuleException(std::string msg, std::string filename, size_t line);
	virtual ~ModuleException(void);

	virtual std::string what();
};

#endif
