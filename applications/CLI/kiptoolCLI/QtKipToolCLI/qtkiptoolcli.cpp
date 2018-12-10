#include "qtkiptoolcli.h"
#include "stdafx.h"

#include <KiplFrameworkException.h>
#include <ModuleException.h>
#include <utilities/TimeDate.h>
#include <math/image_statistics.h>
//#include <base/thistogram.h>
//#include <base/textractor.h>
//#include <io/DirAnalyzer.h>
#include <base/KiplException.h>
#include <KiplEngine.h>
#include <KiplFactory.h>
#include <KiplProcessConfig.h>

#include <sstream>
#include <strings/filenames.h>
#include <strings/miscstring.h>
#include <base/KiplException.h>
#include <utilities/nodelocker.h>

#include "ImageIO.h"


#include <iostream>
#include <sstream>
#include <vector>

#include <QCoreApplication>
#include <QVector>
#include <QDebug>


QtKipToolCLI::QtKipToolCLI(QCoreApplication *a) :
        logger("KipToolCLI"),
        app(a)
{
    QVector<QString> qargs=app->arguments().toVector();

    for (int i=0; i<qargs.size(); i++) {
        args.push_back(qargs[i].toStdString());
    }

}

int QtKipToolCLI::exec()
{
    std::ostringstream msg;
//    kipl::logging::Logger logger("QtKipTool::RunOffline");
//    QVector<QString> args=a->arguments().toVector();

#ifdef _OPENMP
    omp_set_nested(1);
#endif

    // Command line mode
    if ((2<args.size()) && (args[1]=="-f")) {
        logger(kipl::logging::Logger::LogMessage,"KipTool is running in CLI mode");
//        std::string fname(args[2].toStdString());

        KiplProcessConfig config;
        KiplEngine *engine=nullptr;
        KiplFactory factory;

        try {
            config.LoadConfigFile(args[2],"kiplprocessing");
            config.GetCommandLinePars(args);
            engine=factory.BuildEngine(config,nullptr);

              if (engine!=nullptr) {
                    logger(kipl::logging::Logger::LogMessage, "Loading image");

                    kipl::base::TImage<float,3> img = LoadVolumeImage(config);
                    logger(kipl::logging::Logger::LogMessage, "Starting processing");
                    engine->Run(&img);
                    logger(kipl::logging::Logger::LogMessage, "Processing done");
                    engine->SaveImage();
                    logger(kipl::logging::Logger::LogMessage, "Image saved");

              }
              else{
                  logger(kipl::logging::Logger::LogError, "There is no processing engine, skipping");
                  return -1;
              }
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
