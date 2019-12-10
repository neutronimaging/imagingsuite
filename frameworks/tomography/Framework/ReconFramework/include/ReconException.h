//<LICENSE>
#ifndef RECONEXCEPTION_H
#define RECONEXCEPTION_H
#include "ReconFramework_global.h"
#include <string>
#include <base/KiplException.h>

class RECONFRAMEWORKSHARED_EXPORT ReconException : public kipl::base::KiplException
{
public:
	ReconException(void);
	ReconException(std::string msg);
	ReconException(std::string msg, std::string filename, size_t line);
	virtual ~ReconException(void);
private:
};

#endif
