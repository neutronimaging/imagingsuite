//============================================================================
// Name        : Tests.cpp
// Author      : Anders Kaestner
// Version     :
// Copyright   : This is my code stay away
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "TestStripeFilter.h"
#include "TestFFT.h"

using namespace std;

int main(int argc, char *argv[]) {
	cout << "Test platform for Imaging algorithms." << endl; // prints !!!Hello World!!!

	TestStripeFilter(argc,argv);
	//TestFFT(argc,argv);

	return 0;
}
