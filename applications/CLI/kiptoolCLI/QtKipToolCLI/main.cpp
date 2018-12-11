#include "stdafx.h"
#include <QCoreApplication>

#include "qtkiptoolcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtKipToolCLI kiptoolProcess(&a);

    return kiptoolProcess.exec();
}
