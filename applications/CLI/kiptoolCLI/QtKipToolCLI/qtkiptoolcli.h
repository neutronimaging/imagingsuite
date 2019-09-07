#ifndef QTKIPTOOLCLI_H
#define QTKIPTOOLCLI_H

#include <string>
#include <vector>
#include <logging/logger.h>

class QCoreApplication;

class QtKipToolCLI
{
    kipl::logging::Logger logger;

public:
    QtKipToolCLI(QCoreApplication *a);
    int exec();

private:
     QCoreApplication *app;
     std::vector<std::string> args;
};

#endif // QTKIPTOOLCLI_H
