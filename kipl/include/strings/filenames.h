//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2008-11-11 21:09:49 +0100 (Di, 11 Nov 2008) $
// $Rev: 13 $
//
#ifndef __MAKEFNAME_H
#define __MAKEFNAME_H
#include "../kipl_global.h"
#include <string>
#include <vector>

namespace kipl { namespace strings { namespace filenames {
#ifdef _MSC_VER
const char slash='\\';
const char wrong_slash='/';
#else
const char slash='/';
const char wrong_slash='\\';
#endif

/// \brief Creates a numbered file name from a given mask 
/// \param fname file name mask. The full path of the file with the number replaced by as many X's as the required zero padding
/// \param num number to use for the file name
/// \param name The created file name 
/// \param ext Extension of the file. If several extensions exist, only the last will be returned

/// \returns The function always returns 0.
int KIPLSHARED_EXPORT MakeFileName(const std::string filename,int num, std::string &name, std::string &ext, const char cWildCardChar ='X', const char cFillChar='0', bool bReversedIndex=false);

/// \brief Finds the first numeric information in a string
/// \param str String to be analyzed
///
/// \returns The function returns the first numeric value encountered 
/// in the string or -1 if no numbers are found
int KIPLSHARED_EXPORT GetStrNum(char const * const str);

/// \brief Splits a string into path, name, and a list of extensions
///	\param filestr input string with the full filename
///	\param path output string with the path (including a trailing slash). An empty path will be replaced by ./
///	\param name output string with the main name
///	\param extensions output vector containing the extensions including heading .
void KIPLSHARED_EXPORT StripFileName(const std::string filestr,
		std::string &path, 
		std::string & name, 
		std::vector<std::string> &extensions);

/// \brief Locates a * in a string and splits the string into two parts
/// \param src source string
/// \param prefix string with information before the *
/// \param suffix string with information behind the *
/// 
/// \returns -1 if no * was found
/// 
/// \note All strings must be allocated prior to use
int KIPLSHARED_EXPORT ExtractWildCard(const char *src, char *prefix, char *suffix);

/// \brief Parse the path string and replaces the slashes according to target os requirements
/// \param path the string to check, this is also the result string
/// \param bAddSlashAtEnd checks wether the last character is a slash and adds a slash if missing.
int KIPLSHARED_EXPORT CheckPathSlashes(std::string &path, bool bAddSlashAtEnd=true);

/// \brief Get the file extension from a file name (the remaining string after a '.')
///	\param filestr input string with the full filename
///	\returns the file extension
std::string KIPLSHARED_EXPORT GetFileExtension(const std::string filestr);

}}}

#endif
