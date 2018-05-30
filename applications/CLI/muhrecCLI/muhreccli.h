#ifndef MUHRECCLI_H
#define MUHRECCLI_H

#include <string>
#include <vector>
#include <logging/logger.h>

class QCoreApplication;

class MuhRecCLI
{
    kipl::logging::Logger logger;
public:
    MuhRecCLI(QCoreApplication *a);

    int exec();

private:
    QCoreApplication *app;
    std::vector<std::string> args;
};

#endif // MUHRECCLI_H
