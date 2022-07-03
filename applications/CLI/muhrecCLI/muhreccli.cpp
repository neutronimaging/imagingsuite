//<LICENSE>

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <vector>

//#include <QCoreApplication>
//#include <QVector>
//#include <QDebug>

#include <ReconFactory.h>
#include <ReconEngine.h>
#include <ReconConfig.h>
#include <ReconException.h>

#include <ModuleException.h>

#include <base/KiplException.h>
#include <strings/filenames.h>

#include "muhreccli.h"

//MuhRecCLI::MuhRecCLI(QCoreApplication *a) :
//    logger("MuhRecCLI"),
//    app(a)
//{
//    QVector<QString> qargs=app->arguments().toVector();

//    for (int i=0; i<qargs.size(); i++) {
//        args.push_back(qargs[i].toStdString());
//    }
//    applicationPath = QCoreApplication::applicationDirPath().toStdString();
//}

MuhRecCLI::MuhRecCLI(int argc, char ** argv) :
    logger("MuhRecCLI")
//    app(nullptr)
{
    for (int i=0; i<argc ; ++i)
        args.push_back(argv[i]);

    applicationPath = args[0];
    applicationPath = applicationPath.substr(0,applicationPath.find_last_of(kipl::strings::filenames::slash));

}

int MuhRecCLI::exec()
{
    std::ostringstream msg;

#ifdef _OPENMP
    omp_set_nested(1);
#endif
    logger(kipl::logging::Logger::LogMessage,"MuhRec is running in CLI mode");

    if (2<args.size())
    {
        if (args[1]=="-f")
        {

            try
            {
                logger(kipl::logging::Logger::LogMessage, "Building a reconstructor");
                ReconConfig config(applicationPath);

                config.LoadConfigFile(args[2],"reconstructor");
                config.GetCommandLinePars(args);
                config.MatrixInfo.bAutomaticSerialize=true;

                ReconFactory factory;
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
                    logger(kipl::logging::Logger::LogError, "There is no reconstruction engine, skipping reconstruction");
                    return -1;
                }
            }  // Exception handling as last resort to report unhandled faults
            catch (ReconException &re) {
                std::cerr<<"An unhandled reconstructor exception occurred"<< std::endl;
                std::cerr<<"Trace :"<< std::endl<<re.what()<< std::endl;
                return -2;
            }
            catch (ModuleException &e) {
                msg<<"A module exception was thrown during the main window initialization\n"<<e.what();
                logger(kipl::logging::Logger::LogError,msg.str());

                return -3;
            }
            catch (kipl::base::KiplException &ke) {
                 std::cerr<<"An unhandled kipl exception occurred"<< std::endl;
                 std::cerr<<"Trace :"<< std::endl<<ke.what()<< std::endl;
                return -4;
            }
            catch (std::exception &e) {
                 std::cerr<<"An unhandled STL exception occurred"<< std::endl;
                 std::cerr<<"Trace :"<< std::endl<<e.what()<< std::endl;
                return -5;
            }

            catch (...) {
                 std::cerr<<"An unknown exception occurred"<< std::endl;
                return -6;
            }
        }
    }
    else {
        logger(kipl::logging::Logger::LogError,"No arguments provided.");
        return -7;
    }

    return 0;
}
