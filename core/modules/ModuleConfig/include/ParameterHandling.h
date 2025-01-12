//<LICENSE>

#ifndef PARAMETERHANDLING_H
#define PARAMETERHANDLING_H

#include "ModuleConfig_global.h"

#include <map>
#include <vector>
#include <string>

/// Get a string parameter from a list of parameters. This function is called by all parameter parsing functions.
/// \param parameters A list of parameters formatted with parameter name as first entry and the value as the second.
/// \param parameter The value of the parameter to retrieve.
/// \returns A string containing the parmeter value.
/// \throws ModuleException if the parameter is not found in the list
std::string MODULECONFIGSHARED_EXPORT GetStringParameter(std::map<std::string, std::string> parameters, std::string parameter);

/// Get a float parameter from a list of parameters. This function is called by all parameter parsing functions.
/// \param parameters A list of parameters formatted with parameter name as first entry and the value as the second.
/// \param parameter The value of the parameter to retrieve.
/// \returns A float containing the parmeter value.
/// \throws ModuleException if the parameter is not found in the list
float MODULECONFIGSHARED_EXPORT GetFloatParameter(std::map<std::string, std::string> parameters, std::string parameter);

/// Get an int parameter from a list of parameters. This function is called by all parameter parsing functions.
/// \param parameters A list of parameters formatted with parameter name as first entry and the value as the second.
/// \param parameter The value of the parameter to retrieve.
/// \returns An int containing the parmeter value.
/// \throws ModuleException if the parameter is not found in the list
int MODULECONFIGSHARED_EXPORT GetIntParameter(std::map<std::string, std::string> parameters, std::string parameter);

/// Get a vector of floats from a list of parameters. The values in the string shall be space separated to be interpreted correctly.
/// This function is called by all parameter parsing functions.
/// \param parameters A list of parameters formatted with parameter name as first entry and the value as the second.
/// \param parameter The value of the parameter to retrieve.
/// \param arr Pointer to an allocated array to contain the retrieved values.
/// \param N number of numerical elements in the string
/// \returns Number of elements retrieved.
/// \throws ModuleException if the parameter is not found in the list or if there are too few values in the string.
int MODULECONFIGSHARED_EXPORT GetFloatParameterVector(std::map<std::string, std::string> parameters,
                                                      std::string parameter,
                                                      float *arr,
                                                      int N);

/// Get a vector of floats from a list of parameters. The values in the string shall be space separated to be interpreted correctly.
/// This function is called by all parameter parsing functions.
/// \param parameters A list of parameters formatted with parameter name as first entry and the value as the second.
/// \param parameter The value of the parameter to retrieve.
/// \param arr Pointer to an allocated array to contain the retrieved values.
/// \param N number of numerical elements in the string
/// \returns Number of elements retrieved.
/// \throws ModuleException if the parameter is not found in the list or if there are too few values in the string.
int MODULECONFIGSHARED_EXPORT GetFloatParameterVector(std::map<std::string, std::string> parameters,
                                                     std::string parameter,
                                                     std::vector<float> &arr,
                                                     int N);

/// Get a vector of ints from a list of parameters. The values in the string shall be space separated to be interpreted correctly.
/// This function is called by all parameter parsing functions.
/// \param parameters A list of parameters formatted with parameter name as first entry and the value as the second.
/// \param parameter The value of the parameter to retrieve.
/// \param arr Pointer to an allocated array to contain the retrieved values.
/// \param N number of numerical elements in the string
/// \returns Number of elements retrieved.
/// \throws ModuleException if the parameter is not found in the list or if there are too few values in the string.
 int MODULECONFIGSHARED_EXPORT GetIntParameterVector(std::map<std::string, std::string> parameters, std::string parameter,int *arr, int N);

 /// Get a vector of uints from a list of parameters. The values in the string shall be space separated to be interpreted correctly.
 /// This function is called by all parameter parsing functions.
 /// \param parameters A list of parameters formatted with parameter name as first entry and the value as the second.
 /// \param parameter The value of the parameter to retrieve.
 /// \param arr Pointer to an allocated array to contain the retrieved values.
 /// \param N number of numerical elements in the string
 /// \returns Number of elements retrieved.
 /// \throws ModuleException if the parameter is not found in the list or if there are too few values in the string.
int MODULECONFIGSHARED_EXPORT GetUIntParameterVector(std::map<std::string, std::string> parameters,
                                                     std::string parameter,
                                                     size_t *arr,
                                                     int N);

int MODULECONFIGSHARED_EXPORT GetUIntParameterVector(std::map<std::string, std::string> parameters,
                                                     std::string parameter,
                                                     std::vector<size_t> &arr,
                                                     int N);

#endif
