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

#include "ModuleConfig_global.h"
#include <base/KiplException.h>

/// Exception class that communicates module exceptions.
class  MODULECONFIGSHARED_EXPORT ModuleException : public kipl::base::KiplException
{
public:
    /// Basic constructor
    ModuleException(void);

    /// Constructor to create an exception carrying a message
    /// \param msg The message
	ModuleException(std::string msg);

    /// Constructor to create an exception carrying a message and tells the location of the exception.
    /// \param msg The message.
    /// \param filename The file name where the exception was thrown.
    /// \param line Number of the line where the exception was thrown.
	ModuleException(std::string msg, std::string filename, size_t line);

    /// Destructor
	virtual ~ModuleException(void);

    /// The message of the exception
    /// \returns A string contatining the exception message.
	virtual std::string what();
};

#endif
