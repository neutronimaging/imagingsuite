//#LICENSE

#include <fstream>
#include <iostream>
#include <ctime>
#include <map>
#include <string>

#include <strings/filenames.h>

#include "../include/processtiminglogger.h"
#include "../include/ReconException.h"

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

    ofs.seekp(0,ios::end);

    long pos = ofs.tellp();

    if (pos==0L) {
        ofs<<"{\n";
    }
//    else
//    {
//        ofs.seekp(-2,ios::end);
//        ofs<<",\n";
//    }

    ofs << "    \"" << timeString() << "\" : {\n";
    for (const auto & category : entryData)
    {
        ofs << "        \""<< category.first <<"\":{ ";
        for (const auto & item : category.second)
        {
            ofs<< "\""<< item.first<<"\":";
            try
            {
                ofs<<std::stod(item.second);
            }  catch ( std::invalid_argument)
            {
                ofs<<"\""<<std::stod(item.second)<<"\"";
            }
            if (item.first != category.second.rbegin()->first)
                ofs<<", ";
        }
        ofs<<"}";
        if (category.first != entryData.rbegin()->first)
            ofs<<",";
        ofs<<"\n";

    }
    ofs << "    },\n";

    ofs.close();

}

string ProcessTimingLogger::timeString()
{
    char mbstr[100];

    std::time_t t = std::time(nullptr);
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%dT%H:%M:%S", std::localtime(&t));

    return std::string(mbstr);
}
