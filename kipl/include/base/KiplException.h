//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//

#ifndef KIPLEXCEPTION_H_
#define KIPLEXCEPTION_H_
#include "../kipl_global.h"
#include <string>

namespace kipl { namespace base {

class KIPLSHARED_EXPORT KiplException
{
public :
 //   KiplException(std::string exname="KiplException");
    KiplException(std::string message, std::string exname="KiplException");
    KiplException(std::string message, std::string filename, size_t linenumber, std::string exname="KiplException");
	virtual ~KiplException();
	virtual std::string what();
protected :
    std::string sExceptionName;
	std::string sMessage;
	std::string sFileName;
	size_t nLineNumber;
	
};

class KIPLSHARED_EXPORT DimsException : public KiplException
{
public : 
	DimsException();
    DimsException(std::string message) : KiplException(message, "DimsException")
	{}
	DimsException(std::string message, 
			std::string filename, 
            const size_t linenumber) : KiplException(message,filename, linenumber, "DimsException")
	{}
};

}}

#endif /*KIPLEXCEPTION_H_*/
