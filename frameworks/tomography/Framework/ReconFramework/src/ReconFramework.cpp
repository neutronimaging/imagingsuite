//<LICENSE>
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
