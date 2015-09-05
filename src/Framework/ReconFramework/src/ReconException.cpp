//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//
#include "stdafx.h"
#include "../include/ReconException.h"
#include <string>
#include <sstream>

ReconException::ReconException(void) :
    kipl::base::KiplException("Undeclared Exception","ReconException")
{
}

ReconException::~ReconException(void)
{
}

ReconException::ReconException(std::string msg) : kipl::base::KiplException(msg)
{}

ReconException::ReconException(std::string msg, std::string filename, size_t line) :
kipl::base::KiplException(msg,filename,line,"ReconException")
{}

std::string ReconException::what()
{
	if (sFileName.empty())
		return sMessage;
	else {
		std::stringstream str;
		str<<"Reconstructor exception in "<<sFileName<<" on line "<<nLineNumber<<": \n"<<sMessage;
		return str.str();
	}
}
