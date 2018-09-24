//<LICENSE>

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

    return 0;
}

int RunGUI(QApplication * a)
{
    KipToolMainWindow w(a);
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
        KiplEngine *engine=nullptr;
        KiplFactory factory;

        try {
            config.LoadConfigFile(fname,"kiplprocessing");
            engine=factory.BuildEngine(config);
        }
        catch (ModuleException & e) {
            std::cerr<<"Failed to initialize config struct "<<e.what()<<std::endl;
            return -1;
        }
        catch (KiplFrameworkException &e) {
            std::cerr<<"Failed to initialize config struct "<<e.what()<<std::endl;
            return -2;
        }
        catch (kipl::base::KiplException &e) {
            std::cerr<<"Failed to initialize config struct "<<e.what()<<std::endl;
            return -3;
        }

        try {
            kipl::base::TImage<float,3> img = LoadVolumeImage(config);
            engine->Run(&img);
            engine->SaveImage();
        }
        catch (ModuleException &e) {
            std::cerr<<"ModuleException: "<<e.what();
            return -4;
        }
        catch (KiplFrameworkException &e) {
            std::cerr<<"KiplFrameworkException: "<<e.what();
            return -5;
        }
        catch (kipl::base::KiplException &e) {
            std::cerr<<"KiplException: "<<e.what();
            return -6;
        }
        catch (std::exception &e) {
            std::cerr<<"STL Exception: "<<e.what();
            return -7;
        }
        catch (...) {
            std::cerr<<"Unhandled exception";
            return -8;
        }
    }

    return 0;
}

