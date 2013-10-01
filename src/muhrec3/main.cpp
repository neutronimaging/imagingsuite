#include <QtGui/QApplication>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include "muhrecmainwindow.h"
#include <string>
#include <utilities/nodelocker.h>
#include <ReconException.h>
#include <ModuleException.h>

int RunGUI(QApplication *app);
int RunOffline(QApplication *app);


int main(int argc, char *argv[])
{
    kipl::logging::Logger logger("MuhRec3");
    kipl::logging::Logger::SetLogLevel(kipl::logging::Logger::LogMessage);

    std::ostringstream msg;

    QApplication app(argc, argv);

    std::string homedir = QDir::homePath().toStdString();

    kipl::strings::filenames::CheckPathSlashes(homedir,true);

    std::string application_path=app.applicationDirPath().toStdString();

    kipl::strings::filenames::CheckPathSlashes(application_path,true);

    kipl::utilities::NodeLocker license(homedir);
    bool licensefail=false;
    std::string errormsg;
    try {
        std::list<std::string> liclist;
        liclist.push_back(homedir+".imagingtools/license_muhrec.dat");
        liclist.push_back(application_path+"license_muhrec.dat");
        liclist.push_back(application_path+"license.dat");
        liclist.push_back(homedir+"license_muhrec.dat");

        license.Initialize(liclist,"muhrec");
    }
    catch (kipl::base::KiplException &e) {
        errormsg=e.what();
        licensefail=true;
    }

    if (licensefail || !license.AccessGranted()) {
        msg.str("");
        if (licensefail)
            msg<<"Could not locate the license file\n"<<errormsg<<"\n";
        else
            msg<<"MuhRec is not registered on this computer\n";

        msg<<"\nPlease contact Anders Kaestner (anders.kaestner@psi.ch) to activate MuhRec.\n";
        msg<<"\nActivation code: "<<*license.GetNodeString().begin();
        logger(kipl::logging::Logger::LogError,msg.str());
        QMessageBox mbox;

       // mbox.addButton("Have license file",QMessageBox::AcceptRole);
        mbox.addButton(QMessageBox::Save);
        mbox.addButton(QMessageBox::Abort);
        mbox.setText(QString::fromStdString(msg.str()));
        mbox.setWindowTitle("License error");
        mbox.setDetailedText(QString::fromStdString(license.GetMessage()));
        int res=mbox.exec();
        std::cout<<"Res ="<<res<<std::endl;
        if (res==QMessageBox::Save) {
            QDir dir;
            QString fname=QFileDialog::getOpenFileName(&mbox,"Select the license file",dir.homePath(),"*.dat");

            if (!fname.isEmpty()) {
                if (!dir.exists(dir.homePath()+"/.imagingtools")) {
                    dir.mkdir(QDir::homePath()+"/.imagingtools");
                }
                std::cout<<(dir.homePath()+"/.imagingtools/license_muhrec.dat").toStdString()<<std::endl;
                QFile::copy(fname,dir.homePath()+"/.imagingtools/license_muhrec.dat");
            }
        }

        return -1;
    }


    if (argc==1)
        return RunGUI(&app);
    else
        return RunOffline(&app);

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

    omp_set_nested(1);
    if (2<app->argc()) {
        if (!strcmp(app->argv()[1],"-f")) {
          logger(kipl::logging::Logger::LogMessage,"MuhRec2 is running in offline mode");
          try {
                  ReconFactory factory;
                  logger(kipl::logging::Logger::LogMessage, "Building a reconstructor");
                  ReconConfig config;
                  config.LoadConfigFile(app->argv()[2],"reconstructor");
                  config.MatrixInfo.bAutomaticSerialize=true;
                  ReconEngine *pEngine=factory.BuildEngine(config,NULL);
                  if (pEngine!=NULL) {
                          logger(kipl::logging::Logger::LogMessage, "Starting reconstruction");
                          pEngine->Run3D();
                          logger(kipl::logging::Logger::LogMessage, "Reconstruction done");
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


