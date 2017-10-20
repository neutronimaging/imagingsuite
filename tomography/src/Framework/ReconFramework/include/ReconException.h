//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//
#ifndef _RECONEXCEPTION_H_
#define _RECONEXCEPTION_H_
#include "ReconFramework_global.h"
//#include "stdafx.h"
#include <string>
#include <base/KiplException.h>

class RECONFRAMEWORKSHARED_EXPORT ReconException : public kipl::base::KiplException
{
public:
	ReconException(void);
	ReconException(std::string msg);
	ReconException(std::string msg, std::string filename, size_t line);
	virtual ~ReconException(void);

	virtual std::string what();
private:
};

#endif
