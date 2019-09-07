//<LICENSE>

#include <cstring>
#include <sstream>
#include <iomanip>

#include "../../include/strings/xmlstrings.h"
#include "../../include/strings/miscstring.h"

namespace kipl {
namespace strings {

std::string xmlString(std::string tag, std::string value, int indent)
{
    std::ostringstream str;

    str<<std::setw(indent)<<" "<<"<"<<tag<<">"<<value<<"</"<<tag<<">"<<std::endl;

    return str.str();
}

std::string xmlString(std::string tag, int value, int indent)
{
    return xmlString(tag,std::to_string(value),indent);
}

std::string xmlString(std::string tag, size_t value, int indent)
{
    return xmlString(tag,std::to_string(value),indent);
}

std::string xmlString(std::string tag, float value, int indent)
{
    return xmlString(tag,std::to_string(value),indent);
}

std::string xmlString(std::string tag, double value, int indent)
{
    return xmlString(tag,std::to_string(value),indent);
}

std::string xmlString(std::string tag, bool value, int indent)
{
    return xmlString(tag,kipl::strings::bool2string(value),indent);
}

}}
