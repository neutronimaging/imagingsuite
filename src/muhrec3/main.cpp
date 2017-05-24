//<LICENSE>

#include <QtWidgets/QApplication>
#include <QDesktopServices>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QVector>
#include "muhrecmainwindow.h"
#include <string>
#include <strings/miscstring.h>
#include <utilities/nodelocker.h>
#include <ReconException.h>
#include <ModuleException.h>

int RunGUI(QApplication *app);
int RunOffline(QApplication *app);
void TestConfig();


int main(int argc, char *argv[])
{
    std::cout<<"Starting muhrec3"<<std::endl;
    kipl::logging::Logger logger("MuhRec3");
    kipl::logging::Logger::SetLogLevel(kipl::logging::Logger::LogMessage);

    std::ostringstream msg;

    QApplication app(argc, argv);

    std::string homedir = QDir::homePath().toStdString();

    kipl::strings::filenames::CheckPathSlashes(homedir,true);
    std::cout<<"home dir="<<homedir<<std::endl;

    std::string application_path=app.applicationDirPath().toStdString();

    kipl::strings::filenames::CheckPathSlashes(application_path,true);

    if (app.arguments().size()==1) {
        std::cout<<"Running MuhRec in GUI mode."<<std::endl;
        return RunGUI(&app);
    }
    else {
        std::cout<<"Running MuhRec in CLI mode."<<std::endl;
        return RunOffline(&app);
    }

    return 0;
}

int RunGUI(QApplication *app)
{
  std::ostringstream msg;
  kipl::logging::Logger logger("MuhRec3::RunGUI");

  try {
        MuhRecMainWindow w(app);
        w.show();

        return app->exec();
  }
  catch (ReconException &e) {
      msg<<"A recon exception was thrown during the main window initialization\n"<<e.what();
      logger(kipl::logging::Logger::LogError,msg.str());

      return -1;
  }
  catch (ModuleException &e) {
          msg<<"A module exception was thrown during the main window initialization\n"<<e.what();
          logger(kipl::logging::Logger::LogError,msg.str());

          return -6;
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
  catch (...) {
      msg<<"An unknown exception was thrown\n";
      logger(kipl::logging::Logger::LogError,msg.str());
      return -7;
  }
  return -100;
}

int RunOffline(QApplication *app)
{
    std::ostringstream msg;
    kipl::logging::Logger logger("MuhRec3::RunOffline");

#ifdef _OPENMP
    omp_set_nested(1);
#endif
    QVector<QString> qargs=app->arguments().toVector();
    std::vector<std::string> args;

    for (int i=0; i<qargs.size(); i++) {
        args.push_back(qargs[i].toStdString());
    }

    if (2<args.size()) {
        if (args[1]=="-f") {
          logger(kipl::logging::Logger::LogMessage,"MuhRec3 is running in offline mode");
          try {
                  ReconFactory factory;
                  logger(kipl::logging::Logger::LogMessage, "Building a reconstructor");
                  ReconConfig config;
                  config.LoadConfigFile(args[2],"reconstructor");
                  config.GetCommandLinePars(args);
                  config.MatrixInfo.bAutomaticSerialize=true;
                  ReconEngine *pEngine=factory.BuildEngine(config,NULL);
                  if (pEngine!=NULL) {
                          logger(kipl::logging::Logger::LogMessage, "Starting reconstruction");
                          pEngine->Run3D();
                          logger(kipl::logging::Logger::LogMessage, "Reconstruction done");

                          std::string confname=config.MatrixInfo.sDestinationPath;
                          kipl::strings::filenames::CheckPathSlashes(confname,true);
                          std::string basename=config.MatrixInfo.sFileMask.substr(0,config.MatrixInfo.sFileMask.find_first_of('#'));
                          confname+=basename+"_recon.xml";

                          ofstream conffile(confname.c_str());

                          conffile<<config.WriteXML();
                          conffile.close();
                  }
                  else {
                          logger(kipl::logging::Logger::LogMessage, "There is no reconstruction engine, skipping reconstruction");
                  }
              }  // Exception handling as last resort to report unhandled faults
              catch (ReconException &re) {
                  cerr<<"An unhandled reconstructor exception occurred"<<endl;
                  cerr<<"Trace :"<<endl<<re.what()<<endl;
                  return -1;
              }
              catch (ModuleException &e) {
                    msg<<"A module exception was thrown during the main window initialization\n"<<e.what();
                    logger(kipl::logging::Logger::LogError,msg.str());

                    return -3;
                }
              catch (kipl::base::KiplException &ke) {
                  cerr<<"An unhandled kipl exception occurred"<<endl;
                  cerr<<"Trace :"<<endl<<ke.what()<<endl;
                  return -2;
              }

              catch (std::exception &e) {
                  cerr<<"An unhandled STL exception occurred"<<endl;
                  cerr<<"Trace :"<<endl<<e.what()<<endl;
                  return -4;
              }

              catch (...) {
                  cerr<<"An unknown exception occurred"<<endl;
                  return -5;
              }
          }
      }

    return 0;
}


