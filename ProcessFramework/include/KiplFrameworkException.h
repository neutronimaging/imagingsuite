//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2008-11-11 21:09:49 +0100 (Di, 11 Nov 2008) $
// $Rev: 13 $
//

#ifndef __KIPLFRAMEWORKEXCEPTION_H
#define __KIPLFRAMEWORKEXCEPTION_H

#include <base/KiplException.h>
class KiplFrameworkException :
	public kipl::base::KiplException
{
public:
	KiplFrameworkException(void) {}
	KiplFrameworkException(std::string message) : kipl::base::KiplException(message) {}
	KiplFrameworkException(std::string message, std::string filename, const size_t linenumber) : kipl::base::KiplException(message,filename,linenumber) {}

	virtual ~KiplFrameworkException(void) {}
};


#endif