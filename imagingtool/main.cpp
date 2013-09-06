#include <QtGui/QApplication>
#include "imagingtoolmain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImagingToolMain w;
    w.show();
    
    return a.exec();
}
