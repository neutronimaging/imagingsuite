// kiptool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#undef max
#include <gtkmm.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>

#include <ModuleConfig.h>
#include <ModuleException.h>

#include <KiplProcessConfig.h>
#include <KiplFrameworkException.h>
#include <KiplFactory.h>
#include <KiplEngine.h>

#include <base/KiplException.h>
#include <logging/logger.h>
#include <utilities/nodelocker.h>
#include <strings/filenames.h>

#include "ImageIO.h"

#include "KipToolMainWindow.h"

int RunGUI(int argc, char *argv[],std::string path);
int RunOffline(int argc, char *argv[], std::string path);

int main(int argc, char* argv[])
{
	kipl::logging::Logger logger("KipTool");
	kipl::logging::Logger::SetLogLevel(kipl::logging::Logger::LogMessage);

	std::ostringstream msg;
	
	std::string homedir = Glib::get_home_dir();
	kipl::strings::filenames::CheckPathSlashes(homedir,true);

	std::string errormsg;

	std::string application_path;
	char *tmpstr = getenv("KIPTOOL_BASE_PATH");
	if (tmpstr!=NULL) {
	  application_path=tmpstr;
	  kipl::strings::filenames::CheckPathSlashes(application_path,true);
	}

  kipl::utilities::NodeLocker license(homedir);

  bool licensefail=false;
  try {
	  std::list<std::string> liclist;

	  liclist.push_back(application_path+"./license_kiptool.dat");
	  liclist.push_back(application_path+"./license.dat");
	  liclist.push_back(homedir+"./license_kiptool.dat");
	  liclist.push_back(homedir+"./license.dat");
	  liclist.push_back(homedir+"./licenses/license_kiptool.dat");

	  license.Initialize(liclist,"kiptool");
  }
  catch (kipl::base::KiplException &e) {
	  errormsg=e.what();
	  licensefail=true;
  }


  if (licensefail || !license.AccessGranted()) {
	  Gtk::Main kit(argc, argv);

	  msg.str("");
	  if (licensefail)
		  msg<<"Could not locate the license file\n"<<errormsg<<"\n";
	  else
		  msg<<"KipTool is not registered on this computer\n";

	  msg<<"\nPlease contact Anders Kaestner (anders.kaestner@psi.ch) to activate KipTool.\n";
	  msg<<"\nActivation code: "<<*license.GetNodeString().begin();
	  logger(kipl::logging::Logger::LogError,msg.str());
	  Gtk::MessageDialog msgdlg(msg.str(),false,Gtk::MESSAGE_ERROR);
	  msgdlg.set_title("License error");
	  msgdlg.run();
	  return -1;
  }


	if (argc<2) {
		return RunGUI(argc,argv,application_path);
	}
	else 
	{
		return RunOffline(argc,argv,application_path);
	}

	return 0;
}

int RunGUI(int argc, char *argv[], std::string path)
{
	kipl::logging::Logger logger("KipTool");
	std::ostringstream msg;
	
	std::string tempdir = Glib::get_home_dir();
	std::string errormsg;

	Gtk::Main kit(argc, argv);

	Glib::thread_init();

  	Glib::RefPtr<Gtk::Builder> refBuilder =Gtk::Builder::create();
	try {
		std::string gladefile=path+"resources/kiptool.glade";
		kipl::strings::filenames::CheckPathSlashes(gladefile,false);

		refBuilder->add_from_file(gladefile);
	}
	catch (const Glib::FileError &ex){
		std::cerr<<"File error: "<<ex.what()<<std::endl;
		return 1;
	}
	catch (const Gtk::BuilderError &ex) {
		std::cerr<<"BuilderError: "<<ex.what()<<std::endl;
		return -1;
	}
  catch (kipl::base::KiplException &e) {
	  msg<<"A kipl exception was thrown during the main window initialization\n"<<e.what();
	  logger(kipl::logging::Logger::LogError,msg.str());

	  return -2;
  }
  catch (std::exception &e) {
	msg<<"An STL exception was thrown during the main window initialization\n"<<e.what();
	logger(kipl::logging::Logger::LogError,msg.str());

	return -3;
  }
  catch (Glib::Exception & e) {
	msg<<"An unhandled Glib exception occurred\n"<<e.what();
	logger(kipl::logging::Logger::LogError,msg.str());
	return -4;
  }
  catch (ModuleException &e) {
  		  msg<<"A module exception was thrown during the main window initialization\n"<<e.what();
  		  logger(kipl::logging::Logger::LogError,msg.str());

  		  return -1;
  }
  catch (...) {
	  msg<<"An unknown exception was thrown\n";
	  logger(kipl::logging::Logger::LogError,msg.str());
	  return -5;
  }

  logger(kipl::logging::Logger::LogMessage,"Got glade file");
  try {
	KipToolMainWindow mainwindow(refBuilder,path);

	kit.run(mainwindow.get_window());
  }
  catch (kipl::base::KiplException &e) {
	  msg<<"A kipl exception was thrown during the main window initialization\n"<<e.what();
	  logger(kipl::logging::Logger::LogError,msg.str());

	  return -2;
  }
  catch (std::exception & e) {
	msg<<"An STL exception was thrown during the main window initialization\n"<<e.what();
	logger(kipl::logging::Logger::LogError,msg.str());

	return -3;
  }
  catch (Glib::Exception & e) {
	msg<<"An unhandled Glib exception occurred\n"<<e.what();
	logger(kipl::logging::Logger::LogError,msg.str());
	return -4;
  }
  catch (ModuleException &e) {
  		  msg<<"A module exception was thrown during the main window initialization\n"<<e.what();
  		  logger(kipl::logging::Logger::LogError,msg.str());

  		  return -6;
  }
  catch (...) {
	  msg<<"An unknown exception was thrown\n";
	  logger(kipl::logging::Logger::LogError,msg.str());
	  return -7;
  }

	return 0;
}

int RunOffline(int argc, char *argv[], std::string path)
{
	kipl::logging::Logger logger("KipTool");
	std::ostringstream msg;
	
	std::string tempdir = Glib::get_home_dir();
	std::string errormsg;

		// Command line mode
		if ((2<argc) && (!strcmp(argv[1],"-f"))) {
			std::string fname(argv[2]);

			KiplProcessConfig config;
			KiplEngine *engine=NULL;
			KiplFactory factory;

			try {
				config.LoadConfigFile(fname);
				engine=factory.BuildEngine(config);
			}
			catch (ModuleException & e) {
				cerr<<"Failed to initialize config struct "<<e.what()<<endl;
				return -1;	
			}
			catch (KiplFrameworkException &e) {
				cerr<<"Failed to initialize config struct "<<e.what()<<endl;
				return -2;
			}
			catch (kipl::base::KiplException &e) {
				cerr<<"Failed to initialize config struct "<<e.what()<<endl;
				return -3;
			}

			try {
				kipl::base::TImage<float,3> img = LoadVolumeImage(config);
				engine->Run(&img);
				engine->SaveImage();
//				m_PlotList=m_Engine->GetPlots();
//				m_HistogramList=m_Engine->GetHistograms();
			}
			catch (ModuleException &e) {
				cerr<<"ModuleException: "<<e.what();
				return -4;
			}
			catch (KiplFrameworkException &e) {
				cerr<<"KiplFrameworkException: "<<e.what();
				return -5;
			}
			catch (kipl::base::KiplException &e) {
				cerr<<"KiplException: "<<e.what();
				return -6;
			}
			catch (std::exception &e) {
				cerr<<"STL Exception: "<<e.what();
				return -7;
			}
			catch (...) {
				cerr<<"Unhandled exception";
				return -8;
			}
		}

	return 0;
}
