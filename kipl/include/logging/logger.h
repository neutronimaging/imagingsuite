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
#ifndef LOGGER_H_
#define LOGGER_H_
#include "../kipl_global.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <mutex>

namespace kipl { namespace logging {
class KIPLSHARED_EXPORT LogWriter {
public:
	virtual size_t Write(std::string str);
	virtual ~LogWriter() {}
}; 

class KIPLSHARED_EXPORT LogStreamWriter : public LogWriter
{
public:
    LogStreamWriter(std::string fname);
    virtual size_t Write(std::string str);
    virtual ~LogStreamWriter();
protected:
    std::ofstream fout;
};

class KIPLSHARED_EXPORT Logger {
public:

	/// \brief Enum used to select the current minimum log level
	enum LogLevel {
		LogError,    //!< Report errors
		LogWarning,  //!< Report at least Warnings
		LogMessage,  //!< Report at least Messages
		LogVerbose,  //!< Report at least Verbose messages
		LogDebug     //!< Report at least Debug information
	};
	
	/// \brief C'tor that initializes the logger with a name and a stream for the output
	/// \param str Name used to indicate the message source
	/// \param s stream for the output
	Logger(std::string str, std::ostream &s=std::cout); 
	
	/// \brief Set the global log target for all logging objects
	/// \param lw A log writer object
	static size_t AddLogTarget(LogWriter & lw);


	/// \brief Set the global log level
	static void SetLogLevel(LogLevel level);
	static LogLevel GetLogLevel() { return CurrentLogLevel; }
	
	/// \brief Log a message
	/// \param severity The log level of the current log message
	/// \param message A string containing the message
	void operator()(LogLevel severity, std::string message);

    /// \brief Log a message
    /// \param severity The log level of the current log message
    /// \param message A string containing the message
    void operator()(LogLevel severity, std::stringstream & message);
protected:

	/// \brief Back-end of the log writer
	/// \param s The log level of the current log message
	/// \param message A string containing the message
	static void WriteMessage(LogLevel s, std::string message);
#ifdef MULTITARGETS
    static std::list<LogWriter *> LogTargets;   //!< Refence to the global log target
#else
    static LogWriter * LogTarget;   //!< Refence to the global log target
#endif
    static LogLevel CurrentLogLevel; //!< The current global log level
    static std::mutex m_LoggerMutex;

	std::string sLogOrigin; //!< The name of the current log space
};

/// \brief Interface for log enums to the stream class
/// \param os The target stream
/// \param level The log level to print out
std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &os, kipl::logging::Logger::LogLevel level);

/// \brief String to enum converter
/// \param s The string to convert
/// \param level The translated enum value for the entered string
/// \throws ReconException if the string translation failed
void KIPLSHARED_EXPORT string2enum(std::string s, kipl::logging::Logger::LogLevel &level);
}}

#endif /*LOGGER_H_*/
