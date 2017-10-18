#ifndef __XML_PARSER_H_GUARD
#define __XML_PARSER_H_GUARD

#include <string>
#include <napi.h>
#include <map>

#ifdef USE_TIMING
#include <time.h>
#include <string>
extern std::string print_time(const time_t & start,
                         const  time_t & stop = time(NULL));
#endif


namespace xml_parser{
  extern bool parse_xml_file(const std::map<std::string,std::string>&map,
                             const std::string &filename, NXhandle *handle,
                             const bool timing);
};
#endif
