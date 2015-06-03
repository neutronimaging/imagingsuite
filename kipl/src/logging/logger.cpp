//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//
#include "../../include/logging/logger.h"
#include "../../include/base/KiplException.h"

#include <sstream>
#include <list>
#include <iostream>
#include <ctime>


namespace kipl { namespace logging {
using namespace std;
//std::ostream * Logger::LogTargets=& std::cout;
Logger::LogLevel Logger::CurrentLogLevel=Logger::LogMessage;
LogWriter ConsoleLogger;
#ifdef MULTITARGETS
#else
LogWriter * Logger::LogTarget = &ConsoleLogger;
#endif

size_t LogWriter::Write(std::string str){
	std::cout<<str;
	
	return 0;
}

LogStreamWriter::LogStreamWriter(std::string fname)
{
    fout.open(fname.c_str(),ios::out);
    if (fout.fail())
        throw kipl::base::KiplException("Failed to open log file",__FILE__,__LINE__);
}

size_t LogStreamWriter::Write(std::string str)
{
    fout<<str;

    return 0;
}

LogStreamWriter::~LogStreamWriter()
{
    fout.close();
}


Logger::Logger(std::string str, std::ostream & UNUSED(s))
{
#ifdef MULTITARGET
    if (Logger::LogTargets.empty())
        Logger::LogTargets.push_back(&ConsoleLogger);
#endif

	sLogOrigin=str;
}

size_t Logger::AddLogTarget(LogWriter & lw)
{
#ifdef MULTITARGET
    if (((*(Logger::LogTargets.begin())) == &ConsoleLogger) && (Logger::LogTargets.size()==1))
        Logger::LogTargets.clear();

    Logger::LogTargets.push_back(&lw);
#else
    Logger::LogTarget = &lw;
#endif
	return 0;
}

void Logger::SetLogLevel(LogLevel level)
{
	Logger::CurrentLogLevel=level;
}

void Logger::operator()(LogLevel severity, std::string message)
{
	
	std::ostringstream ostr;
	ostr<<sLogOrigin<<": "<<message;
	
	WriteMessage(severity, ostr.str());
	
}

void Logger::WriteMessage(LogLevel s, std::string message)
{
	if (s<=CurrentLogLevel) {
		stringstream msg;
		msg<<"["<<s<<"] "<<message<<std::endl;
#ifdef MULTITARGET
        std::list<kipl::logging::LogWriter *>::iterator it;
        for (it=Logger::LogTargets.begin(); it!=Logger::LogTargets.end(); it++) {
            (*it)->Write(msg.str());
        }
#else
        Logger::LogTarget->Write(msg.str());
#endif
	}
}
	

std::ostream & operator<<(std::ostream &os, kipl::logging::Logger::LogLevel level)
{
	switch (level) {
	case Logger::LogError   : os<<"error"   ; break;
	case Logger::LogWarning : os<<"warning" ; break;
	case Logger::LogMessage : os<<"message" ; break;
	case Logger::LogDebug   : os<<"debug"   ; break;
	case Logger::LogVerbose : os<<"verbose" ; break;
	}
	
	return os;
}

void string2enum(std::string s, kipl::logging::Logger::LogLevel &level)
{
	if (s=="error")
		level=kipl::logging::Logger::LogError;
	else if (s=="warning")
		level=kipl::logging::Logger::LogWarning;
	else if (s=="message")
		level=kipl::logging::Logger::LogMessage;
	else if (s=="debug")
		level=kipl::logging::Logger::LogDebug;
	else if (s=="verbose")
		level=kipl::logging::Logger::LogVerbose;
	else
		throw kipl::base::KiplException("Unknown log-level",__FILE__,__LINE__);

}
}}
