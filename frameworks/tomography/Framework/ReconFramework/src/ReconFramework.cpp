//<LICENSE>

#include "../include/ReconFramework.h"
#include "../include/ReconFactory.h"

// This is an example of an exported function.
void * GetReconFactory(void)
{
	return new ReconFactory;
}
