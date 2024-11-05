

#ifndef KIPLEXCEPTION_H
#define KIPLEXCEPTION_H
#include "../kipl_global.h"
#include <exception>
#include <string>

namespace kipl { namespace base {

/// \brief Base class for execptions in the kipl environment
class KIPLSHARED_EXPORT KiplException : public std::exception
{
public :
    /// \brief Basic exception C'tor. Only creates a message.
    /// \param message The error message that is carried by the exception
    /// \param exname Name of the exception. This is to be assigned by the C'tor of the deriving class
    KiplException(const std::string &message, const std::string &exname="KiplException");

    /// \brief Exception C'tor with full information.
    /// \param message The error message that is carried by the exception
    /// \param filename Name of the file that throws the exception (provided by __FILE__)
    /// \param linenumber line number of the location where the exception was thrown.
    /// \param exname Name of the exception. This is to be assigned by the C'tor of the deriving class
    KiplException(const std::string & message, const std::string & filename, size_t linenumber, const std::string &exname="KiplException");
	virtual ~KiplException();

    /// \brief Getter for the error message
    /// \returns A string containing the error message information
    virtual const char* what() const noexcept;
protected :
    std::string sExceptionName;
	std::string sMessage;
	std::string sFileName;
	size_t nLineNumber;
    std::string sFormattedMessage;
	
};

/// \brief Exception to throw when there are problems with the image dimensions
class KIPLSHARED_EXPORT DimsException : public KiplException
{
public : 
    /// \brief Basic exception C'tor. Only creates a message.
    /// \param message The error message that is carried by the exception
    /// \param exname Name of the exception. This is to be assigned by the C'tor of the deriving class
    DimsException(std::string message);
    /// \brief Exception C'tor with full information.
    /// \param message The error message that is carried by the exception
    /// \param filename Name of the file that throws the exception (provided by __FILE__)
    /// \param linenumber line number of the location where the exception was thrown.
    /// \param exname Name of the exception. This is to be assigned by the C'tor of the deriving class
	DimsException(std::string message, 
			std::string filename, 
            const size_t linenumber) ;
};

}}

#endif /*KIPLEXCEPTION_H_*/
