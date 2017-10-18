#ifndef __XML_UTIL_H_GUARD
#define __XML_UTIL_H_GUARD

#include <libxml/parser.h>
#include <string>
#include <vector>

namespace xml_util{
  extern std::string xmlChar_to_str(const xmlChar *ch, int len);
  extern std::vector<std::string> 
                              xmlattr_to_strvec(const xmlChar* char_array[]);
}

#endif
