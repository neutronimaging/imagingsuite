#ifndef MISCSTRING_H_
#define MISCSTRING_H_

#include <string>
#include <sstream>

namespace kipl { namespace strings {

/// \brief Converts the contents of a string to lower case 
/// \param str Input string
/// \returns the converted string
std::string toLower(const std::string str);

bool string2bool(std::string str);

std::string bool2string(bool val);

template<typename T>
std::string value2string(T val)
{
	std::ostringstream ss;

	ss<<val;

	std::string str;

	str=ss.str();
	return str;
}

std::string Byte2Hex(unsigned char val);

int VersionNumber(std::string vstr);
}}

#endif /*MISCSTRING_H_*/
