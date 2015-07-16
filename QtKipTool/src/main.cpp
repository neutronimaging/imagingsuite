#include <QtWidgets/QApplication>
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QVector>
#include <QDesktopServices>
#include <QPushButton>

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

#ifdef _OPENMP
#include <omp.h>
#endif

//#define USE_NODE_LOCK

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

#ifdef USE_NODE_LOCK
    std::string application_path=a.applicationDirPath().toStdString();
    kipl::utilities::NodeLocker license(homedir);

    bool licensefail=false;
    int res=0;
    std::string errormsg;
    do {
        std::cout<<"start lic loop"<<std::endl;
        licensefail=false;
        res=0;
        std::list<std::string> liclist;

        liclist.push_back(application_path+"./license_kiptool.dat");
        liclist.push_back(application_path+"./license.dat");
        liclist.push_back(homedir+".imagingtools/license_kiptool.dat");
        liclist.push_back(homedir+"./license.dat");
        try {
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

            msg<<"\nPlease press register to request an activation key for KipTool.\n";
            msg<<"\nActivation code: "<<*license.GetNodeString().begin()<<std::endl;
            msg<<"On computers with many network adapters, please deactivate the wifi and restart the application.\n";
            msg<<"When you obtained the email containing the key, press the save button and select the file containing the key.";

            logger(kipl::logging::Logger::LogError,msg.str());

            QMessageBox mbox;

            QPushButton *registerbutton=mbox.addButton("Register",QMessageBox::AcceptRole);
            mbox.addButton(QMessageBox::Save);
            mbox.addButton(QMessageBox::Abort);
            mbox.setText(QString::fromStdString(msg.str()));
            mbox.setWindowTitle("License error");
            mbox.setDetailedText(QString::fromStdString(license.GetLockerMessage()));
            res=mbox.exec();
            res=mbox.result();
            std::cout<<"Res ="<<res<<std::endl;
            if (res==QMessageBox::Save) {
                QDir dir;
                QString fname=QFileDialog::getOpenFileName(&mbox,"Select the license file",dir.homePath(),"*.dat");

                if (!fname.isEmpty()) {
                    if (!dir.exists(dir.homePath()+"/.imagingtools")) {
                        dir.mkdir(QDir::homePath()+"/.imagingtools");
                    }
                    QString licfname=dir.homePath()+"/.imagingtools/license_kiptool.dat";

                    if (QFile::exists(licfname)) {
                        QFile::remove(licfname);
                    }

                    std::cout<<licfname.toStdString()<<std::endl;
                    QFile::copy(fname,licfname);
                }
             }
             if (mbox.clickedButton() == registerbutton) {
                logger(kipl::logging::Logger::LogMessage,"Opening default web browser.");
                QDesktopServices::openUrl(QUrl("http://www.imagingscience.ch/usermanager/index.php?nodekey="+QString::fromStdString(*license.GetNodeString().begin())));
             }
         }
        cout<<"End lic loop granted="<<(license.AccessGranted() ? "true" : "false")<<", "<<res<<endl;
    } while ((!license.AccessGranted()) && (res!=QMessageBox::Abort));

    if (res!=QMessageBox::Abort) {
#endif
        #ifdef _OPENMP
            omp_set_nested(1);
        #endif

            if (argc<2) {
                return RunGUI(&a);
            }
            else
            {
                return RunOffline(&a);
            }
#ifdef USE_NODE_LOCK
    }
#endif
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
    QVector<QString> args=a->arguments().toVector();


    // Command line mode
    if ((2<args.size()) && (args[1]=="-f")) {
        std::string fname(args[2].toStdString());

        KiplProcessConfig config;
        KiplEngine *engine=NULL;
        KiplFactory factory;

        try {
            config.LoadConfigFile(fname,"kiplprocessing");
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

