#include <QtGui/QApplication>
#include "muhrecmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MuhRecMainWindow w;
    w.show();
    
    return a.exec();
}
