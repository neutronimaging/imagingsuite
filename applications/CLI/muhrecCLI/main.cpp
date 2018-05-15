#include <QCoreApplication>

#include "muhreccli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MuhRecCLI reconstructor(&a);

    reconstructor.exec();

    return a.exec();
}
