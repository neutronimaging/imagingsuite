//<LICENSE>

#ifndef __KIPLFRAMEWORKEXCEPTION_H
#define __KIPLFRAMEWORKEXCEPTION_H

#include <base/KiplException.h>
class KiplFrameworkException :
	public kipl::base::KiplException
{
public:
    KiplFrameworkException(void) : kipl::base::KiplException("","KiplFrameworkException") {}
    KiplFrameworkException(std::string message) : kipl::base::KiplException(message,"KiplFrameworkException") {}
    KiplFrameworkException(std::string message, std::string filename, const size_t linenumber) : kipl::base::KiplException(message,filename,linenumber,"KiplFrameworkException") {}

	virtual ~KiplFrameworkException(void) {}
};


#endif
