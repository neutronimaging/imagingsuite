//#LICENSE

#include <fstream>
#include <iostream>
#include <ctime>

#include "../include/processtiminglogger.h"
#include "../include/ReconException.h"

ProcessTimingLogger::ProcessTimingLogger(const std::string &path) :
    logger("ProcessTimingLogger"),
    logPath(path)
{
}


//"2007-03-01T13:00:00" : {
//    "data":{"projections":625, "ROIx":100, "ROIy":200,"MROIx":100, "MROIy":100, "MROIz":100 },
//    "hardware":{"CPU":"Intel i7 @ 3.8GHz"},
//    "timing" : {"FullLogNorm":10.3, "MorphSpotClean":4.4, "BPRecon":20.3}
//    },

//void ProcessTimingLogger::addLogEntry(ReconConfig &config, std::vector<ModuleItem *> &preprocList, BackProjectorModuleBase *backProjector)
void ProcessTimingLogger::addLogEntry()
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
    ofs << "    \"data\":{ \"projections\":625, \"ROIx\":100},\n";
    ofs << "    }";

    ofs.flush();
    ofs.close();

}

string ProcessTimingLogger::timeString()
{
    char mbstr[100];

    std::time_t t = std::time(nullptr);
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%dT%H:%M:%S", std::localtime(&t));

    return std::string(mbstr);
}
