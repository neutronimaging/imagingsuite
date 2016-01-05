#include <QtWidgets/QApplication>
#include "imagingtoolmain.h"
#include "genericconversion.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
 //   ImagingToolMain w;
    GenericConversion w;

    w.show();
    
    return a.exec();
}

