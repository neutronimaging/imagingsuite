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

#include "stdafx.h"
#include "../include/ProcessModuleBase.h"

#include "../include/ModuleException.h"

ProcessModuleBase::ProcessModuleBase(std::string name) :
 logger(name),
 m_sModuleName(name)
{

}

ProcessModuleBase::~ProcessModuleBase() {

}

int ProcessModuleBase::Process(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters)
{
	int res=0;

	timer.Tic();
	res=ProcessCore(img,parameters);
	timer.Toc();

	return res;
}

int ProcessModuleBase::Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
	int res=0;

	timer.Tic();
	res=ProcessCore(img,parameters);
	timer.Toc();

	return res;
}

int ProcessModuleBase::ProcessCore(kipl::base::TImage<float,2> &UNUSED(img), std::map<std::string,std::string> & UNUSED(parameters))
{
	std::ostringstream msg;

	msg<<"Processing module "<<m_sModuleName<<" does not support 2D processing.";
	throw ModuleException(msg.str(),__FILE__,__LINE__);

	return 0;
}

int ProcessModuleBase::ProcessCore(kipl::base::TImage<float,3> &UNUSED(img), std::map<std::string,std::string> &UNUSED(parameters))
{
	std::ostringstream msg;

	msg<<"Processing module "<<m_sModuleName<<" does not support 3D processing.";
	throw ModuleException(msg.str(),__FILE__,__LINE__);

	return 0;
}

int ProcessModuleBase::SourceVersion()
{
	return kipl::strings::VersionNumber("$Rev$");
}

