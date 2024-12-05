

#include <ctype.h>
#include <string>
#include <algorithm>

#include "../../include/strings/miscstring.h"
#include "../../include/strings/filenames.h"
#include "../../include/base/KiplException.h"

namespace kipl { namespace strings {
std::string toLower(const std::string str)
{
	
	std::string sBuffer=str;
	std::transform( sBuffer.begin(), sBuffer.end(), sBuffer.begin(),::tolower );
	return sBuffer;
}

bool string2bool(std::string str)
{
	std::string s=toLower(str);

	if ((s=="true") || (s=="yes"))
		return true;

	if ((s=="false") || (s=="no"))
			return false;

	throw kipl::base::KiplException("string2bool could not parse string",__FILE__,__LINE__);

	return false;

}

std::string bool2string(bool val)
{
	return val==true ? std::string("true") : std::string("false");
}

std::string Byte2Hex(unsigned char val) 
{
	
	char lo=static_cast<char>(val & 15);
	lo += lo<=9 ? 48 : 55;


	char hi=static_cast<char>(val >> 4);
	hi += hi<=9 ? 48 : 55;

	std::string str;
	str.push_back(hi); str.push_back(lo);

	return str;
}

int VersionNumber(std::string vstr)
{
    std::string::size_type pos=vstr.find_first_of("0123456789");

	int ver=-1;
	if (pos!=std::string::npos) {
		ver = atoi( vstr.substr(pos).c_str());
	}

	return ver;
}

}}

