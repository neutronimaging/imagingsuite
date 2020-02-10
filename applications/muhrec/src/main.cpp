//<LICENSE>

#include <string>
#include <iostream>

#include <QtWidgets/QApplication>
#include <QDesktopServices>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QVector>
#include <QDebug>
#include <QStyleFactory>

#include <facestyles.h>
#include <strings/miscstring.h>
#include <strings/filenames.h>
#include <utilities/nodelocker.h>

#include <ReconException.h>
#include <ModuleException.h>

#include "muhrecmainwindow.h"

int RunGUI(QApplication *app);
int RunOffline(QApplication *app);
void TestConfig();


int main(int argc, char *argv[])
{
    kipl::logging::Logger logger("MuhRec");
    kipl::logging::Logger::SetLogLevel(kipl::logging::Logger::LogMessage);
    logger.message("Starting MuhRec");

    std::ostringstream msg;

    QApplication app(argc, argv);
    app.setApplicationVersion(VERSION);

    std::string homedir = QDir::homePath().toStdString();

    kipl::strings::filenames::CheckPathSlashes(homedir,true);
    msg.str(""); msg<<"Home dir: "<<homedir;
    logger.message(msg.str());

    std::string application_path=app.applicationDirPath().toStdString();

    kipl::strings::filenames::CheckPathSlashes(application_path,true);

    if (app.arguments().size()==1) {
        logger.message("Running MuhRec in GUI mode.");
        return RunGUI(&app);
    }
    else {
        logger.message("Running MuhRec in CLI mode.");
        return RunOffline(&app);
    }

    return 0;
}

int RunGUI(QApplication *app)
{
    std::ostringstream msg;
    kipl::logging::Logger logger("MuhRec3::RunGUI");

    QtAddons::setDarkFace(app);
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
          logger(kipl::logging::Logger::LogMessage,"MuhRec is running in CLI mode");
          try {
                  ReconFactory factory;
                  logger(kipl::logging::Logger::LogMessage, "Building a reconstructor");
                  ReconConfig config("");
                  config.LoadConfigFile(args[2],"reconstructor");
                  config.GetCommandLinePars(args);
                  config.MatrixInfo.bAutomaticSerialize=true;
                  ReconEngine *pEngine=factory.BuildEngine(config,nullptr);
                  if (pEngine!=nullptr) {
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
                  std::cerr<<"An unhandled reconstructor exception occurred"<<std::endl;
                  std::cerr<<"Trace :"<<std::endl<<re.what()<<std::endl;
                  return -1;
              }
              catch (ModuleException &e) {
                    msg<<"A module exception was thrown during the main window initialization\n"<<e.what();
                    logger(kipl::logging::Logger::LogError,msg.str());

                    return -3;
                }
              catch (kipl::base::KiplException &ke) {
                  std::cerr<<"An unhandled kipl exception occurred"<<std::endl;
                  std::cerr<<"Trace :"<<std::endl<<ke.what()<<std::endl;
                  return -2;
              }

              catch (std::exception &e) {
                  std::cerr<<"An unhandled STL exception occurred"<<std::endl;
                  std::cerr<<"Trace :"<<std::endl<<e.what()<<std::endl;
                  return -4;
              }

              catch (...) {
                  std::cerr<<"An unknown exception occurred"<<std::endl;
                  return -5;
              }
          }
      }

    return 0;
}


