//
// This file is part of the kiptool library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $File$
// $Date: 2011-08-17 16:22:51 +0200 (Mi, 17 Aug 2011) $
// $Rev: 1020 $
// $Id: KiplProcessModuleBase.cpp 1020 2011-08-17 14:22:51Z kaestner $
//

#ifndef PORESIZEMAPMODULE_H_
#define PORESIZEMAPMODULE_H_

#include "PorespaceModules_global.h"
#include <KiplProcessModuleBase.h>
#include <map>
#include <string>

class PORESPACEMODULES_EXPORT PoreSizeMapModule: public KiplProcessModuleBase
{
public:
	PoreSizeMapModule();
	virtual ~PoreSizeMapModule();
	
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	bool m_bComplement;
	float m_fMaxRadius;
};

#endif /* DATASCALER_H_ */
