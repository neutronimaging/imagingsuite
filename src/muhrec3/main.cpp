#include <QtGui/QApplication>
#include "muhrecmainwindow.h"

int RunGUI(int argc, char *argv[]);
int RunOffline(int argc, char *argv[]);


int main(int argc, char *argv[])
{
    if (argc==1)
        return RunGUI(argc,argv);
    else
        return RunOffline(argc, argv);

    return 0;
}

int RunGUI(int argc, char *argv[])
{
    QApplication a(argc, argv);

    std::string application_path=argv[0];

    application_path.substr(0,application_path.find_last_of('/'));
    MuhRecMainWindow w(application_path);
    w.show();
    
    return a.exec();
}

int RunOffline(int argc, char *argv[])
{

    return 0;
}
