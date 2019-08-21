//<LICENSE>
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

    virtual const char* what() const;
private:
};

#endif
