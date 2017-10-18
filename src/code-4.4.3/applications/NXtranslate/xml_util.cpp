#include "xml_util.h"
#include "string_util.h"

using std::string;
using std::vector;

/*
 * If len<0 then just return the whole string
 */
extern string xml_util::xmlChar_to_str(const xmlChar *ch, int len){
  string result((char *)ch);
  if( (len>0) && ((unsigned int)len<result.size()) )
    result.erase(result.begin()+len,result.end());

  return result;
  //  return string_util::trim(result);
}

extern vector<string> xml_util::xmlattr_to_strvec(const xmlChar* char_array[]){
  vector<string> result;
  int i=0;
  while(true){
    if((char_array+i)==NULL || *(char_array+i)==NULL)
      break;
    else{
      string str=xml_util::xmlChar_to_str(*(char_array+i),-1);
      result.push_back(str);
    }
    i++;
  }

  return result;
}
