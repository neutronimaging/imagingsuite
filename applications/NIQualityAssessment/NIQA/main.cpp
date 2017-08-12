#include "niqamainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NIQAMainWindow w;
    w.show();

    return a.exec();
}
