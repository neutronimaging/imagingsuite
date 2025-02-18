//<LICENCE>

#include <sstream>
#include <list>
#include <iostream>
#include <ctime>

#include "../../include/kipl_global.h"
#include "../../include/logging/logger.h"
#include "../../include/base/KiplException.h"

namespace kipl { namespace logging {
using namespace std;
//std::ostream * Logger::LogTargets=& std::cout;

Logger::LogLevel Logger::CurrentLogLevel=Logger::LogMessage;

LogWriter ConsoleLogger("ConsoleLogger");

LogWriter * Logger::LogTarget = &ConsoleLogger;
std::vector<LogWriter *> Logger::LogTargets = {};

std::mutex Logger::m_LoggerMutex;

LogWriter::LogWriter(std::string name) :
    sName(name),
    bActive(true)
{

}

size_t LogWriter::write(const std::string &str)
{
	std::cout<<str;
	
    return 0;
}

bool LogWriter::isValid()
{
    return true;
}

bool LogWriter::isActive()
{
    return bActive;
}

void LogWriter::active(bool state)
{
    bActive = state;
}

string LogWriter::loggerName()
{
    return sName;
}

LogStreamWriter::LogStreamWriter(const std::string & fname) :
    LogWriter("LogStreamWriter")
{

    fout.open(fname.c_str(),ios::out);
    if (fout.fail())
        throw kipl::base::KiplException("Failed to open log file",__FILE__,__LINE__);
}

size_t LogStreamWriter::write(const std::string &str)
{
    fout<<str;
    fout.flush();

    return 0;
}

bool LogStreamWriter::isValid()
{
    return fout.is_open() && fout.good();
}

LogStreamWriter::~LogStreamWriter()
{
    fout.close();
}


Logger::Logger(std::string str)//, std::ostream & UNUSED(s))
{
	sLogOrigin=str;
}

size_t Logger::addLogTarget(LogWriter * lw)
{
    Logger::LogTargets.push_back(lw);

	return 0;
}

void Logger::SetLogLevel(LogLevel level)
{
	Logger::CurrentLogLevel=level;
}

void Logger::operator()(LogLevel severity, const string &message, const std::string &functionName) const
{	
	std::ostringstream ostr;
    if (functionName.empty())
        ostr<<sLogOrigin<<" -- "<<message;
    else
        ostr<<sLogOrigin<<":"<<functionName<<" -- "<<message;

    writeMessage(severity, ostr.str());
}

/// \brief Log a message
/// \param severity The log level of the current log message
/// \param message A string containing the message
void Logger::operator()(LogLevel severity, std::stringstream & message, const std::string &functionName) const
{
    operator ()(severity,message.str(),functionName);
}

void Logger::error(const std::string &message, const string &functionName) const
{
    operator()(LogError,message,functionName);
}

void Logger::warning(const std::string &message, const string &functionName) const
{
    operator()(LogWarning,message,functionName);
}

void Logger::message(const std::string &message, const string &functionName) const 
{
    operator()(LogMessage,message,functionName);
}

void Logger::verbose(const std::string &message, const string &functionName) const
{
    operator()(LogVerbose,message,functionName);
}

void Logger::debug(const string &message, const string &functionName) const
{
    operator()(LogDebug,message,functionName);
}

void Logger::writeMessage(LogLevel s, const std::string &message)
{
    std::lock_guard<std::mutex> lock(m_LoggerMutex);

    if (s<=CurrentLogLevel)
    {
		stringstream msg;
		msg<<"["<<s<<"] "<<message<<std::endl;

        for (auto & item : LogTargets)
        {
            if (item->isValid())
                item->write(msg.str());
        }

        Logger::LogTarget->write(msg.str());
	}
}
	
}}

std::ostream  KIPLSHARED_EXPORT & operator<<(std::ostream &os, kipl::logging::Logger::LogLevel level)
{
    os<<enum2string(level);

    return os;
}

void  KIPLSHARED_EXPORT string2enum(std::string s, kipl::logging::Logger::LogLevel &level)
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

std::string  KIPLSHARED_EXPORT enum2string(kipl::logging::Logger::LogLevel level)
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
