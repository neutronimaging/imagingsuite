#ifndef MUHRECCLI_H
#define MUHRECCLI_H

#include <string>
#include <vector>
#include <logging/logger.h>

//class QCoreApplication;

class MuhRecCLI
{
    kipl::logging::Logger logger;
public:
//    MuhRecCLI(QCoreApplication *a);

    MuhRecCLI(int argc, char ** argv);

    int exec();

private:
//    QCoreApplication *app;
    std::vector<std::string> args;
    std::string applicationPath;
};

#endif // MUHRECCLI_H
