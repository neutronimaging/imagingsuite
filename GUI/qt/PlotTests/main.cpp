#include "mainwindow.h"
#include <QApplication>
#include <facestyles.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

   // QtAddons::setDarkFace(&a);
    return a.exec();
}
