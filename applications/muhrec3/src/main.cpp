//<LICENSE>

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
#include "muhrecmainwindow.h"
#include <string>
#include <strings/miscstring.h>
#include <utilities/nodelocker.h>
#include <ReconException.h>
#include <ModuleException.h>
#include <iostream>

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

    app->setStyle(QStyleFactory::create("Fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, QColor(250,250,250));
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    palette.setColor(QPalette::ToolTipBase, QColor("lemonchiffon"));
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Background, QColor("darkgray").darker());
    palette.setColor(QPalette::Highlight, QColor("#6db3f7"));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    app->setPalette(palette);
    app->setStyleSheet("QLineEdit {background : white; color:black}"
                       "QSpinBox {background : white; color:black} "
                       "QTextEdit {background : white; color:black} "
                       "QDoubleSpinBox{background : white; color:black} "
                       "QTabBar::tab {"
                       "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 1.0 #D3D3D3);"
                       "border: 1px solid #C4C4C3;"
                       "border-bottom-color: #C2C7CB; /* same as the pane color */"
                       "border-top-left-radius: 4px;"
                       "border-top-right-radius: 4px;"
                       "min-width: 8ex; padding: 2px;}"
                       "QTabBar::tab:selected, QTabBar::tab:hover {"
                       "color:white;"
                       "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6db3f7, stop: 1.0 #1b82fb);}"
                       "QTabBar::tab:selected {"
                       "border-color: #9B9B9B;"
                       "border-bottom-color: #C2C7CB; /* same as pane color */}"
                       "QTabBar::tab:!selected {color: #353535;margin-top: 2px; /* make non-selected tabs look smaller */}");
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
    kipl::logging::Logger::SetLogLevel(kipl::logging::Logger::LogMessage);
    kipl::logging::Logger logger("MuhRec3::RunOffline");

#ifdef _OPENMP
    omp_set_nested(1);
#endif
    QVector<QString> qargs=app->arguments().toVector();
    std::vector<std::string> args;

    std::cout<<"Arguments"<<std::endl;
    for (int i=0; i<qargs.size(); i++) {
        args.push_back(qargs[i].toStdString());
        qDebug()<<args[i].c_str();
    }

    std::cout<<args.size()<<", "<<qargs.size()<<", "<<args[1].c_str()<<std::endl;
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


