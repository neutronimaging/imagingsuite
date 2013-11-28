#include <QtGui/QApplication>
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QFileDialog>

#include <sstream>
#include <strings/filenames.h>
#include <strings/miscstring.h>
#include <base/KiplException.h>
#include <utilities/nodelocker.h>


#include <KiplEngine.h>
#include <KiplFactory.h>
#include <KiplFrameworkException.h>

#include <ModuleException.h>

#include "kiptoolmainwindow.h"
#include "ImageIO.h"

int RunGUI(QApplication * a);
int RunOffline(QApplication * a);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir dir;
    kipl::logging::Logger logger("KipTool");
    kipl::logging::Logger::SetLogLevel(kipl::logging::Logger::LogMessage);

    std::ostringstream msg;

    std::string homedir = dir.homePath().toStdString();
    kipl::strings::filenames::CheckPathSlashes(homedir,true);

    std::string errormsg;

    std::string application_path=a.applicationDirPath().toStdString();

    kipl::utilities::NodeLocker license(homedir);

    bool licensefail=false;
    try {
        std::list<std::string> liclist;

        liclist.push_back(application_path+"./license_kiptool.dat");
        liclist.push_back(application_path+"./license.dat");
        liclist.push_back(homedir+".imagingtools/license_kiptool.dat");
        liclist.push_back(homedir+"./license.dat");

        license.Initialize(liclist,"kiptool");
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
          msg<<"KipTool is not registered on this computer\n";

      msg<<"\nPlease contact Anders Kaestner (anders.kaestner@psi.ch) to activate KipTool.\n";
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
              std::cout<<(dir.homePath()+"/.imagingtools/license_kiptool.dat").toStdString()<<std::endl;
              QFile::copy(fname,dir.homePath()+"/.imagingtools/license_kiptool.dat");
          }
      }

      return -1;
  }


    if (argc<2) {
        return RunGUI(&a);
    }
    else
    {
        return RunOffline(&a);
    }

    return 0;
}

int RunGUI(QApplication * a)
{
    KipToolMainWindow w;
    w.show();

    return a->exec();
}

int RunOffline(QApplication * a)
{
    std::ostringstream msg;
    kipl::logging::Logger logger("QtKipTool::RunOffline");

    omp_set_nested(1);
    // Command line mode
    if ((2<a->argc()) && (!strcmp(a->argv()[1],"-f"))) {
        std::string fname(a->argv()[2]);

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

