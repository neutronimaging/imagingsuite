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

std::mutex Logger::m_LoggerMutex;

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

/// \brief Log a message
/// \param severity The log level of the current log message
/// \param message A string containing the message
void Logger::operator()(LogLevel severity, std::stringstream & message)
{
    operator ()(severity,message.str());
}

void Logger::WriteMessage(LogLevel s, std::string message)
{
    std::lock_guard<std::mutex> lock(m_LoggerMutex);

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
	
}}

std::ostream & operator<<(std::ostream &os, kipl::logging::Logger::LogLevel level)
{
    os<<enum2string(level);

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

std::string enum2string(kipl::logging::Logger::LogLevel &level)
{
    switch (level) {
    case kipl::logging::Logger::LogError   : return "error"   ; break;
    case kipl::logging::Logger::LogWarning : return "warning" ; break;
    case kipl::logging::Logger::LogMessage : return "message" ; break;
    case kipl::logging::Logger::LogDebug   : return "debug"   ; break;
    case kipl::logging::Logger::LogVerbose : return "verbose" ; break;
    default :
        throw kipl::base::KiplException("Unknown log-level",__FILE__,__LINE__); break;
    }

    return "bad level";
}
