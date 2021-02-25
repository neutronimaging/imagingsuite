//<LICENSE>

#include "stdafx.h"
#include <QCoreApplication>
#include <QDir>

#include <strings/filenames.h>

#include <ReconException.h>
#include <ReconConfig.h>

#include "muhreccli.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion(VERSION);

    std::string homedir = QDir::homePath().toStdString();

    kipl::strings::filenames::CheckPathSlashes(homedir,true);
    ReconConfig::setHomePath(homedir);


    MuhRecCLI reconstructor(&app);

    return reconstructor.exec();
}
