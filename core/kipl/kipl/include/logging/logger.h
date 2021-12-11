//<LICENCE>

#ifndef LOGGER_H_
#define LOGGER_H_

#include "../kipl_global.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <mutex>

namespace kipl {
/// \brief This namespace collects classes related to the handling of log messages.
namespace logging {

/// \brief Base class to redirect the log messages to different targets. Instances of this class write to the standard stream.
class KIPLSHARED_EXPORT LogWriter {
public:
    /// \brief Writes a message string using cout
	virtual size_t Write(std::string str);
	virtual ~LogWriter() {}
}; 

/// \brief A writer class that streams the messages to a file
class KIPLSHARED_EXPORT LogStreamWriter : public LogWriter
{
public:
    /// \brief C'tor that open a file for message streaming.
    /// \param fname File name of the destination file.
    LogStreamWriter(std::string fname);
    /// \brief Writes a message to the stream
    /// \param str The message to write
    virtual size_t Write(std::string str);
    virtual ~LogStreamWriter();
protected:
    std::ofstream fout;
};

/// \brief A basic logging class that can take log messages from different origins and write then to the same destination.
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
    static kipl::logging::Logger::LogLevel GetLogLevel() { return CurrentLogLevel; }
	
	/// \brief Log a message
	/// \param severity The log level of the current log message
	/// \param message A string containing the message
    void operator()(LogLevel severity, const std::string &message, const std::string &functionName="");

    /// \brief Log a message
    /// \param severity The log level of the current log message
    /// \param message A string containing the message
    void operator()(LogLevel severity, std::stringstream & message, const std::string &functionName="");

    void error(  const std::string &message, const std::string &functionName="");
    void warning(const std::string &message, const std::string &functionName="");
    void message(const std::string &message, const std::string &functionName="");
    void verbose(const std::string &message, const std::string &functionName="");
    void debug(  const std::string &message, const std::string &functionName="");

protected:

	/// \brief Back-end of the log writer
	/// \param s The log level of the current log message
	/// \param message A string containing the message
    static void WriteMessage(LogLevel s, const std::string &message);
#ifdef MULTITARGETS
    /// \brief
    static std::list<LogWriter *> LogTargets;   //!< Refence to the global log target. Experimental approach to allow several log targets. It is still not stable
#else
    static LogWriter * LogTarget;   //!< Refence to the global log target single log target case
#endif
    static LogLevel CurrentLogLevel; //!< The current global log level
    static std::mutex m_LoggerMutex; ///< A mutex to protect against simultaneous writing from several threads.

    std::string sLogOrigin; //!< The name of the current log space. Every class that use a logger instance should tell what their name is to improve the quality of the message.
};



}}

/// \brief Interface for log enums to the stream class
/// \param os The target stream
/// \param level The log level to print out
std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &os, kipl::logging::Logger::LogLevel level);

/// \brief String to enum converter
/// \param s The string to convert
/// \param level The translated enum value for the entered string
/// \throws ReconException if the string translation failed
void KIPLSHARED_EXPORT string2enum(std::string s, kipl::logging::Logger::LogLevel &level);


/// \brief Enum to String converter
/// \param level The translated enum value for the entered string
/// \returns The enum name
/// \throws ReconException if the string translation failed
std::string  KIPLSHARED_EXPORT enum2string(kipl::logging::Logger::LogLevel level);

#endif /*LOGGER_H_*/
