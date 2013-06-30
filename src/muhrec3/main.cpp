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
    QApplication app(argc, argv);

    MuhRecMainWindow w(&app);
    w.show();
    
    return app.exec();
}

int RunOffline(int argc, char *argv[])
{

    return 0;
}
