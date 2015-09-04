//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//
#ifndef __MODULEA_H
#define __MODULEA_H

#include <PreprocModuleBase.h>

class ModuleA : public PreprocModuleBase
{
public:
	ModuleA(void);
	virtual ~ModuleA(void);


	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);

	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,float> &parameters);

private: 
	float parA;
	int parB;
};

#endif
