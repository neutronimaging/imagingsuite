#include "viewermainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ViewerMainWindow w;
    w.show();

    return a.exec();
}
