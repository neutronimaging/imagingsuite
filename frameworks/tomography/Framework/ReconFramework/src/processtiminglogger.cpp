//#LICENSE

#include <fstream>
#include <iostream>
#include <ctime>
#include <map>
#include <string>

#include "../include/processtiminglogger.h"
#include "../include/ReconException.h"
#include <strings/filenames.h>

ProcessTimingLogger::ProcessTimingLogger(const std::string &path) :
    logger("ProcessTimingLogger"),
    logPath(path)
{
    kipl::strings::filenames::CheckPathSlashes(logPath,false);
}

void ProcessTimingLogger::addLogEntry(std::map<std::string,std::map<std::string,std::string>> & entryData)
{
    std::ostringstream msg;
    std::ofstream ofs;
    try
    {
        ofs.open (logPath, std::ofstream::out | std::ofstream::app);
    }
    catch (std::exception &e)
    {
        msg.str(""); msg<<"Failed to open timing log file.\n"<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }

    long pos = ofs.tellp();
    std::cout<<pos<<std::endl;
    ofs.seekp(0,ios::end);

    if (pos==0L) {
        ofs<<"{\n";
    }
    else
    {
        ofs.seekp(-1,ios::end);
        pos = ofs.tellp();
        std::cout<<pos<<std::endl;
        //ofs<<",\n";
        ofs.write(",",1);
        pos = ofs.tellp();
        std::cout<<pos<<std::endl;
        ofs<<"\n";
    }

    ofs << "    \"" << timeString() << "\" : {\n";
    for (const auto & category : entryData)
    {
        ofs << "    \""<< category.first <<"\":{ ";
        for (const auto & item : category.second)
        {
            ofs<< "\""<< item.first<<"\":\""<<item.second<<"\",";
        }
        ofs<<"},\n";

    }
    ofs << "    }";

    ofs.close();

}

string ProcessTimingLogger::timeString()
{
    char mbstr[100];

    std::time_t t = std::time(nullptr);
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%dT%H:%M:%S", std::localtime(&t));

    return std::string(mbstr);
}
