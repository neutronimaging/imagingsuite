//<LICENSE>

#ifndef PROCESSTIMINGLOGGER_H
#define PROCESSTIMINGLOGGER_H


#include "ReconFramework_global.h"
#include <fstream>
#include <string>

#include <logging/logger.h>

#include "ReconConfig.h"
#include "ModuleItem.h"
#include "BackProjectorModuleBase.h"

class RECONFRAMEWORKSHARED_EXPORT ProcessTimingLogger
{
    kipl::logging::Logger logger;
public:
    ProcessTimingLogger(const std::string &path);

//    void addLogEntry(ReconConfig &config,
//                     std::vector<ModuleItem *> & preprocList,
//                     BackProjectorModuleBase *backProjector);

    void addLogEntry(std::map<string, std::map<string, string>> &entryData);

private:
    std::string logPath;

    std::string timeString();
};

#endif // PROCESSTIMINGLOGGER_H
