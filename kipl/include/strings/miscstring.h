#ifndef MISCSTRING_H_
#define MISCSTRING_H_

#include "../kipl_global.h"
#include <string>
#include <sstream>

namespace kipl { namespace strings {

/// \brief Converts the contents of a string to lower case 
/// \param str Input string
/// \returns the converted string
std::string KIPLSHARED_EXPORT toLower(const std::string str);

bool KIPLSHARED_EXPORT string2bool(std::string str);

std::string KIPLSHARED_EXPORT bool2string(bool val);

template<typename T>
std::string value2string(T val)
{
	std::ostringstream ss;

	ss<<val;

	std::string str;

	str=ss.str();
	return str;
}

std::string KIPLSHARED_EXPORT Byte2Hex(unsigned char val);

int KIPLSHARED_EXPORT VersionNumber(std::string vstr);
}}

#endif /*MISCSTRING_H_*/
