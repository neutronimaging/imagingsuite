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

// ReconFramework.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../include/ReconFramework.h"
#include "../include/ReconFactory.h"

// This is an example of an exported function.
void * GetReconFactory(void)
{
	return new ReconFactory;
}
