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

#ifndef PROCESSMODULEBASE_H_
#define PROCESSMODULEBASE_H_

#include "../src/stdafx.h"

#include <string>
#include <map>

#include <base/timage.h>
#include <profile/Timer.h>
#include <logging/logger.h>
#include <strings/miscstring.h>

class DLL_EXPORT ProcessModuleBase {
protected:
	kipl::logging::Logger logger;
public:
	ProcessModuleBase(std::string name="ProcessModuleBase");
	virtual ~ProcessModuleBase();
	virtual int Process(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters);
	virtual int Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);
	virtual int Configure(std::map<std::string, std::string> parameters)=0;
	virtual int Initialize() {timer.Reset(); return 0;}
	virtual std::map<std::string, std::string> GetParameters()=0;

	std::string ModuleName() {return m_sModuleName;}
	double ExecTime() {return timer.ElapsedSeconds();}
	virtual std::string Version() {
		ostringstream s;
		s<<"ProcessModuleBase ("<<std::max(kipl::strings::VersionNumber("$Rev$"), ProcessModuleBase::SourceVersion())<<")";

		return s.str();
	}

protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters);
	virtual int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);

	std::string m_sModuleName;
	kipl::profile::Timer timer;
	virtual int SourceVersion();

};


#endif /* PROCESSMODULEBASE_H_ */
