

#ifndef __PARENC_H
#define __PARENC_H
#include <cstdio>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>

#include "../base/KiplException.h"


namespace kipl { namespace strings {
/// \brief Support functions to parse parameter files.
///
/// These functions are probably deprecated by the ones in module config and should not be used in new code
namespace parenc {

/// \brief Exception to be thrown when a parameter could not be handled correctly.
class parenc_exception : public kipl::base::KiplException {
public:
    /// \brief C'tor for simple exception, not recommended to use.
    parenc_exception() : KiplException("parenc_exception") {}

    /// \brief Basic exception C'tor. Only creates a message.
    /// \param message The error message that is carried by the exception
    /// \param exname Name of the exception. This is to be assigned by the C'tor of the deriving class
    parenc_exception(std::string message) : KiplException(message, "parenc_exception")
    {}

    /// \brief Exception C'tor with full information.
    /// \param message The error message that is carried by the exception
    /// \param filename Name of the file that throws the exception (provided by __FILE__)
    /// \param linenumber line number of the location where the exception was thrown.
    /// \param exname Name of the exception. This is to be assigned by the C'tor of the deriving class
    parenc_exception(std::string message,
            std::string filename,
            const size_t linenumber) : KiplException(message,filename, linenumber, "parenc_exception")
    {}
};

/// \brief Finds a parameter key and returns an STL string associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param str string associated with the key
/// \returns The status of the search
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getStringPar(const std::string data, const std::string par_key, std::string  & str); // -ak- get the string corresponding to par_key

/// \brief Finds a parameter key and returns c string associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param str string associated with the key
/// \returns The status of the search
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getChStringPar(char *data,char *par_key, char * str); // -ak- 

/// \brief Finds a parameter key and returns a double value associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param val value associated with the key
/// \returns The status of the search
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getDoublePar(const std::string data,const std::string par_key,double &val);

/// \brief Finds a parameter key and returns an integer value associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param val value associated with the key
/// \returns The status of the search
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getIntPar(const std::string data,const std::string par_key, int &val); // -ak- get the float value corresponding to par_key


/// \brief Finds a parameter key and returns an integer value associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param val value associated with the key
/// \returns The status of the search
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getUIntPar(const std::string data,const std::string par_key, unsigned int &val);

/// \brief Finds a parameter key and returns an float value associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param val value associated with the key
/// \returns The status of the search
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getFloatPar(const std::string data,const std::string par_key, float &val); // -ak- get the float value corresponding to par_key

/// \brief Finds a parameter key and returns an array with integer associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param val array with values associated with the key
/// \param n number of expected values
///
/// \returns The status of the search. If the search fails a -1 is placed in the first element of the array
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getIntVecPar(char *data,char *par_key, int *val, int n); 

/// \brief Finds a parameter key and returns an STL vector<int> associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param val string associated with the key
/// \returns The status of the search
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getIntSTLVecPar(const char *data,const char *par_key, std::vector<int> &val);

/// \brief Finds a parameter key and returns an STL vector<float> associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param val string associated with the key
/// \returns The status of the search
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getFloatSTLVecPar(const char *data,const char *par_key, std::vector<float> &val);

/// \brief Finds a parameter key and returns an STL vector<char> associated with it
/// \param data String containing the contents of a parameter file
/// \param par_key String containing the key name of the parameter
/// \param val string associated with the key
/// \returns The status of the search
/// \retval 0 Successful 
/// \retval -1 Parameter not found
int getCharSTLVecPar(char *data,char *par_key, std::vector<char> &val);

}}}

#endif
