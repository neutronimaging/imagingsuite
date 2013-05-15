//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:24:22 +0200 (Mon, 21 May 2012) $
// $Rev: 1318 $
// $Id: ConfiguratorDialogBase.cpp 1318 2012-05-21 14:24:22Z kaestner $
//


// ModuleConfigurator.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../include/ConfiguratorDialogBase.h"

// This is the constructor of a class that has been exported.
// see ModuleConfigurator.h for the class definition
ConfiguratorDialogBase::ConfiguratorDialogBase(std::string name) :
	logger(name)
{
	set_title(name);
	add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
	add_button(Gtk::Stock::OK,Gtk::RESPONSE_OK);
}

ConfiguratorDialogBase::~ConfiguratorDialogBase()
{
}

kipl::base::TImage<float,2> ConfiguratorDialogBase::GetProjection(kipl::base::TImage<float,3> img, size_t n)
{
	kipl::base::TImage<float,2> proj(img.Dims());

	memcpy(proj.GetDataPtr(), img.GetLinePtr(0,n), sizeof(float)*proj.Size());

	return proj;
}

kipl::base::TImage<float,2> ConfiguratorDialogBase::GetSinogram(kipl::base::TImage<float,3> img, size_t n)
{
	size_t dims[2]={img.Size(0),img.Size(2)};
	kipl::base::TImage<float,2> sino(dims);

	for (size_t i=0; i<img.Size(2); i++)
		memcpy(sino.GetLinePtr(i),img.GetLinePtr(n,i),sizeof(float)*img.Size(0));

	return sino;
}
