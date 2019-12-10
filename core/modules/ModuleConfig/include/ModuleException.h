//<LICENSE>

#ifndef MODULEEXCEPTION_H
#define MODULEEXCEPTION_H
#include "../src/stdafx.h"
#include <string>

#include "ModuleConfig_global.h"
#include <base/KiplException.h>

/// Exception class that communicates module exceptions.
class  MODULECONFIGSHARED_EXPORT ModuleException : public kipl::base::KiplException
{
public:
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

};

#endif
