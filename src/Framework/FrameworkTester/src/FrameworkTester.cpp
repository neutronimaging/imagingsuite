//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//

#include "stdafx.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <utilities/nodelocker.h>
//#include <ReconFactory.h>
//#include <ReconEngine.h>
//#include <ReconConfig.h>
//#include <ReconException.h>

#include <logging/logger.h>

using namespace std;

int main(int argc, char *argv[])
{
	

//	kipl::logging::Logger logger("ReconFrameworkTester");
//	std::ostringstream msg;
//
//	msg<<"Starting the tester";
//
//	logger(kipl::logging::Logger::LogMessage,msg.str());
//
//	ReconFactory factory;
//
//	ReconConfig config;
//	ReconEngine *engine=NULL;
//	try{
//		config.LoadConfigFile("config.xml");
//
//		engine=factory.BuildEngine(config);
//	}
//	catch (ReconException &e) {
//		msg<<"Failed to build the engine: \n"<<e.what();
//		logger(kipl::logging::Logger::LogError,msg.str());
//		return -1;
//	}
//
//	try {
//		engine->Run();
//		if (!config.MatrixInfo.bAutomaticSerialize)
//			engine->Serialize(&config.MatrixInfo);
//
//	}
//	catch (ReconException &e) {
//		msg<<"Failed to run the engine: \n"<<e.what();
//		logger(kipl::logging::Logger::LogError,msg.str());
//		return -1;
//	}
//
//	ofstream cfile("test.xml");
//	cfile<<config.WriteXML();
//
//	delete engine;

	return 0;
}


