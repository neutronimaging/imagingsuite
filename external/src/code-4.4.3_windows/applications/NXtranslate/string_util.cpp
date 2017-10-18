#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "string_util.h"

using std::cout;
using std::endl;
using std::find;
using std::find_if;
using std::invalid_argument;
using std::runtime_error;
using std::isspace;
using std::string;
using std::vector;

typedef vector<string> StrVec;
typedef vector<string>::const_iterator StrVecIter;

static bool my_isnotdigit(char c){
  return !isdigit(c);
}

static bool my_isnotfloatdigit(char c){
  if(c=='.')
    return false;
  else
    return my_isnotdigit(c);
}

static bool has_non_zero(const string &str){
  string::size_type size=str.size();

  if(str.find("1")<size || str.find("2")<size || str.find("3")<size || str.find("4")<size
     || str.find("5")<size || str.find("6")<size || str.find("7")<size || str.find("8")<size
     || str.find("9")<size )
    return true;

  return false;
}

extern bool string_util::starts_with(const string &str1, const string &str2){
  // empty string can't start with anything
  if(str1.empty())
    return false;

  // can't start with something that is longer
  if(str2.size()>str1.size())
    return false;

  // if they are the same then just return true
  if(str1==str2)
    return true;

  // do the actual comparison
  string cmp_str=str1.substr(0,str2.size());
  return (cmp_str==str2);
}

/*
 * strip leading and trailing spaces from the string.
 */
extern string string_util::trim (const string &str) {
  typedef string::size_type string_size;
  string new_str="";
  string_size i=0;
  while(i<str.size()){
    //skip initial whitespace
    while(i<str.size() && isspace(str[i])) {
      i++;
    }

    // find the extent of ending whitespace
    string_size  j=str.size();
    while(j>i && isspace(str[j-1])) {
      j--;
      }

    //copy the non-whitespace into the new string
    
    if (i!=j){
      new_str+=str.substr(i,j-i);
      i=j;
    }
  }
  return new_str;
}

extern long string_util::str_to_int(const string &str){
  if(str.substr(0,1)=="-")
    return -1*str_to_int(str.substr(1,str.size()));

  string::const_iterator it=str.begin();
  it=find_if(it,str.end(),my_isnotdigit);

  if(it!=str.end())
    throw invalid_argument("str_to_int(string) argument is not an integer");

  return atol(str.c_str());
}

extern long long string_util::str_to_int64(const string &str){
  if(str.substr(0,1)=="-")
    return -1*str_to_int64(str.substr(1,str.size()));

  string::const_iterator it=str.begin();
  it=find_if(it,str.end(),my_isnotdigit);

  if(it!=str.end())
    throw invalid_argument("str_to_int(string) argument is not an integer");
#ifdef _WIN32
  return _atoi64(str.c_str());
#else
  return atoll(str.c_str());
#endif
}

extern unsigned long string_util::str_to_uint(const string &str){
  long num=str_to_int(str);
  if(num<0)
    throw invalid_argument("str_to_uint(string) argument is not an integer");
  return num;
}

extern unsigned long long string_util::str_to_uint64(const string &str){
  long long num=str_to_int64(str);
  if(num<0)
    throw invalid_argument("str_to_uint(string) argument is not an integer");
  return num;
}

extern double string_util::str_to_float(const string &str){
  double num=atof(str.c_str());

  // check if the return is bad
  if((num==0.0) || (!num)){
    string::const_iterator it=str.begin();
    it=find_if(it,str.end(),my_isnotfloatdigit);
    if(it!=str.end() || has_non_zero(str)){
      throw invalid_argument("str_to_float(string) argument is not a float");
    }
  }

  return num;
}

static bool is_bracket(char c){
  static const string BRACKETS="[]";
  return find(BRACKETS.begin(),BRACKETS.end(),c)!=BRACKETS.end();
}

extern bool string_util::is_comma(char c){
  static const string COMMA=",";
  return find(COMMA.begin(),COMMA.end(),c)!=COMMA.end();
}

extern StrVec string_util::split(const string &source,const string &split)
{
  string::size_type number=count_occur(source,split);
  if(number==0)
    {
      StrVec result;
      result.push_back(source);
      return result;
    }

  vector<string> result;
  string::size_type start=0;
  string::size_type stop=0;
  string inner;
  while(true)
    {
      stop=source.find(split,start);
      if(stop==string::npos)
        {
          result.push_back(source.substr(start));
          break;
        }
      else
        {
          result.push_back(source.substr(start,stop-start));
          start=stop+split.size();
        }
    }
  return result;
}

/*
 * split a string up using commas as the delimiter
 */
extern StrVec string_util::split(const string &str){
  static const string COMMA=",";

  return split(str,COMMA);
}

static bool is_slash(char c){
  static const string SLASH="/";
  return find(SLASH.begin(),SLASH.end(),c)!=SLASH.end();
}

static bool is_colon(char c){
  static const string COLON=":";
  return find(COLON.begin(),COLON.end(),c)!=COLON.end();
}

extern vector<string> string_util::string_to_path(const string &str){
  vector<string> result;
  typedef string::size_type string_size;

  string_size i=0;
  while(i<str.size()){
    // skip seperators at the end of the last word
    while(i<str.size() && (is_slash(str[i]) || is_colon(str[i])) )
      i++;

    // find end of a "word"
    string_size j=i;
    while(j<str.size() && (!is_slash(str[j])) && (!is_colon(str[j])) )
      j++;

    if(i!=j){
      result.push_back(str.substr(i,j-i));
      i=j;
    }
  }

  return result;
}

static bool global_replace(string &str, const string &search, 
                           const string &replace){
  string::size_type index=str.find(search);

  if(index==string::npos){
    return false;
  }

  string::size_type search_size=search.size();
  string::size_type replace_size=replace.size();
  while(index!=string::npos){
    // replace the search string with the replace string
    str.replace(index,search_size,replace);
    // the next place to look is just past where we last found the
    // search string (which is now the replace string)
    index=str.find(search,index+replace_size);
  }

  return true;
}

static StrVec shrink_and_split(string &str){
  static const char *COMMA=",";
  typedef string::size_type string_size;

  // replace brackets with commas
  for( string::iterator ch=str.begin() ; ch!=str.end() ; ch++ ){
    if(is_bracket(*ch))
      *ch=*COMMA;
  }

  // replace the first space with a comma
  bool space=false;
  for( string::iterator ch=str.begin() ; ch!=str.end() ; ch++ ){
    if(isspace(*ch)){
      if(!space){
        space=true;
        *ch=*COMMA;
      }
    }else{
      space=false;
    }
  }

  // remove spaces
  {
    string new_str="";
    string_size i=0;
    while(i<str.size()){
      // skip initial whitespace
      while(i<str.size() && isspace(str[i]))
        i++;

      // find the end of the non-whitespace section
      string_size j=i;
      while(j<str.size() && !isspace(str[j]))
        j++;

      // copy the non-whitespace into the new string
      if(i!=j){
        new_str+=str.substr(i,j-i);
        i=j;
      }
    }
    str=new_str;
  }

  // remove commas that are next to each other
  while(global_replace(str,",,",",")){
    // the test does the work
  }

  // trim extra commas off of the beginning
  while(str.substr(0,1)==COMMA)
    str.erase(0,1);

  // trim extra commas off of the end
  while(str.substr(str.size()-1,str.size())==COMMA)
    str.erase(str.size()-1,str.size());

  return string_util::split(str);
}

extern void string_util::str_to_shortArray(std::string & str,short *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(short)str_to_int(*(strIt+i));
}

extern void string_util::str_to_int64Array(std::string & str,long long *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(long long)str_to_int64(*(strIt+i));
}

extern void string_util::str_to_intArray(std::string & str,int *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(int)str_to_int(*(strIt+i));
}

extern vector<int> string_util::str_to_intVec(string &str){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  // turn each string into an integer
  vector<int> result;
  for( StrVec::const_iterator str=splitted.begin() ; str!=splitted.end() ; str++ )
    result.push_back(str_to_int(*str));

  // return the vector<int>
  return result;
}

extern void string_util::str_to_longArray(std::string & str,long *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(long)str_to_int(*(strIt+i));
}

extern void string_util::str_to_ushortArray(std::string & str,unsigned short *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(unsigned short)str_to_uint(*(strIt+i));
}

extern void string_util::str_to_uint64Array(std::string & str,unsigned long long *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(unsigned long long)str_to_uint64(*(strIt+i));
}

extern void string_util::str_to_uintArray(std::string & str,unsigned int *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(unsigned int)str_to_uint(*(strIt+i));
}

extern void string_util::str_to_ulongArray(std::string & str,unsigned long *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(unsigned long)str_to_uint(*(strIt+i));
}

extern void string_util::str_to_floatArray(std::string & str,float *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size ("+string_util::int_to_str(splitted.size())+"!="+string_util::int_to_str(len)+")");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(float)str_to_float(*(strIt+i));
}

extern void string_util::str_to_doubleArray(std::string & str,double *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(double)str_to_float(*(strIt+i));
}

extern void string_util::str_to_ucharArray(std::string &str,unsigned char *array, const unsigned int len){
  // break it up into a string vector
  StrVec splitted=shrink_and_split(str);

  if(splitted.size()!=len)
    throw runtime_error("array and string not same size");

  // turn each string into a short
  StrVecIter strIt=splitted.begin();
  for( unsigned int i=0 ; i<len ; i++ )
    *(array+i)=(unsigned char)str_to_int(*(strIt+i));
}

/*
 * This method takes a string as an argument and attempts to convert this 
 * string into a vector of integers.  The string is in the form of a comma 
 * separated list.  Each element in the list is a single integer or a range
 * of integers specified by a starting value and ending value separated by
 * a colon.  i.e. 1,2,4:7 translates to a vector that contains the values
 * 1,2,4,5,6,7. Note that ranges must be increasing
 */
extern vector<int> string_util::int_list_str_to_intVect(std::string &intListStr){
  vector<int> intListVect;
  intListStr_to_intVec(intListStr,intListVect);
  return intListVect;
}

extern string string_util::int_to_str(const int num){
  char temp[20];
  sprintf(temp,"%d",num);
  return string(temp);
}

extern string string_util::intVec_to_str(const vector<int> &vec){

  string result("[");
  for( vector<int>::const_iterator it=vec.begin() ; it!=vec.end() ; it++ ){
    result+=int_to_str(*it);
    if(it+1!=vec.end())
      result+=",";
  }
  result+="]";

  return result;
}

extern std::string string_util::erase(const std::string &in_str,const std::string &match_str){
  string result=in_str;
  string::size_type match_size=match_str.size();
  string::size_type index=0;

  while(true)
    {
      index=result.find(match_str,index);
      if(index==string::npos)
        {
          break;
        }
      result.erase(index,match_size);
    }

  return result;
}

extern string::size_type string_util::count_occur(const string &str, const string &ch)
{
  string::size_type count=0;
  string::size_type index=0;

  // infinite loop to make sure that the entire string is parsed.
  while(true)
    {
      index=str.find(ch,index+1);
      if(index==string::npos)
        {
          break;
        }
      count++;
    }
  return count;
}

// these were previously in string_util_ext.cpp from test_collist and FRM2


/*
 * split a line of numeric entrys using whitespace, commas, semicolons, ... as delimiter 
 */
extern std::vector<std::string> string_util::split_values(const string &str){
	std::vector<std::string> result;
	typedef std::string::size_type string_size;

	string_size i=0;

	while(i<str.size()){

		// skip seperators at end of last word
		while(i<str.size() && (isspace(str[i]) || (ispunct(str[i]) && (str[i]!='.' && str[i]!='-' && str[i]!='+' && str[i]!='\'' && str[i]!='\"')) )) {
			i++;
		}
		// find end of "word"
		string_size j=i;
		//std::cout << "str["<<i<<"]: " << str[i] << std::endl;  
		if (str[i] == '\'') {
			while(j<str.size()) {
				j++;
				if (str[j]=='\'') {
					j++;
					break;
				}
			}
		}
		else if (str[i] == '\"') {
			while(j<str.size()) {
				j++;
				if (str[j]=='\"') {
					j++;
					break;
				}
			}
		}
		else {
			while(j<str.size() && !isspace(str[j]) && !( ispunct(str[j]) && str[j]!='.' && str[j]!='-' && str[j]!='+') ){
				j++;
			}
		}

		if(i!=j){
			result.push_back(str.substr(i,j-i));
			i=j;
		}
	}
	
	/*std::cout << "STARTTTTTTTTTTTTTTTTTTTTTTT: " <<std::endl;
	for (std::vector<std::string>::iterator itt=result.begin(); itt!=result.end(); itt++) {
		std::cout << "resulting vector: " << *itt << std::endl;
	}*/
	return result;
}

/*
 * split a line of numeric entrys using whitespace, commas, semicolons, ... as delimiter 
 */
extern std::vector<int> string_util::split_ints(const std::string &str){
  std::vector<int> result;
  typedef std::string::size_type string_size;

  string_size i=0;
  while(i<str.size()){
    // skip seperators at end of last word
    while(i<str.size() && (isspace(str[i]) || (ispunct(str[i]) && str[i]!='.' && str[i]!='-') )) {
      i++;
	 }

    // find end of "word"
    string_size j=i;
    while(j<str.size() && !isspace(str[j]) && !(ispunct(str[j]) && str[j]!='.' && str[i]!='-') ){
      j++;
    }

    if(i!=j){
      result.push_back(string_util::str_to_int(str.substr(i,j-i)));
      i=j;
    }
  }

  return result;
}
/*
 * split a line of numeric entrys using whitespace, commas, semicolons, ... as delimiter 
 */
extern std::vector<unsigned int> string_util::split_uints(const std::string &str){
  std::vector<unsigned int> result;
  typedef std::string::size_type string_size;

  string_size i=0;
  while(i<str.size()){
    // skip seperators at end of last word
    while(i<str.size() && (isspace(str[i]) || (ispunct(str[i]) && str[i]!='.') )) {
      i++;
	 }

    // find end of "word"
    string_size j=i;
    while(j<str.size() && !isspace(str[j]) && !(ispunct(str[j]) && str[j]!='.') ){
      j++;
    }

    if(i!=j){
      result.push_back(string_util::str_to_int(str.substr(i,j-i)));
      i=j;
    }
  }

  return result;
}


/*
 * split a string into doubles using whitespace as delimiter 
 */
extern std::vector<double> string_util::split_doubles(const string &str){
  std::vector<double> result;
  typedef std::string::size_type string_size;
  std::string tempstr;
  
  string_size i=0;

  while(i<str.size()){
    // skip seperators at end of last word
    while(i<str.size() && isspace(str[i]) )
      i++;

    // find end of "word"
    string_size j=i;
    while(j<str.size() && !isspace(str[j])){
      j++;
    }

    if(i!=j){
		 tempstr = str.substr(i,j-i);
		 
		while (isspace(tempstr[0])){
			tempstr = tempstr.substr(1);
		}
		while (isspace(tempstr[tempstr.size()-1])){
			tempstr = tempstr.substr(0, tempstr.size()-1);
		}
		 
      result.push_back(string_util::str_to_float(tempstr));
      i=j;
    }
  }

  return result;
}

/*
 * split a string up using colon as delimiter 
 */
extern std::vector<std::string> string_util::split_colons(const string &str){
  std::vector<std::string> result;
  typedef std::string::size_type string_size;

  string_size i=0;
  while(i<str.size()){
    // skip seperators at end of last word
    while(i<str.size() && (str[i]==',') )
      i++;

    // find end of "word"
    string_size j=i;
    while(j<str.size() && str[j]!=','){
      j++;
    }

    if(i!=j){
      result.push_back(str.substr(i,j-i));
      i=j;
    }
  }

  return result;
}

/*
 * split a string up using whitespace as delimiter 
 */
extern std::vector<std::string> string_util::split_whitespace(const string &str){
  std::vector<std::string> result;
  typedef std::string::size_type string_size;

  string_size i=0;
  while(i<str.size()){
    // skip seperators at end of last word
    while(i<str.size() && isspace(str[i]) )
      i++;

    // find end of "word"
    string_size j=i;
    while(j<str.size() && !isspace(str[j])){
      j++;
    }

    if(i!=j){
      result.push_back(str.substr(i,j-i));
      i=j;
    }
  }

  return result;
}

/*
 *  strip punctuation characters in front of and at end of strings  
 */
extern std::vector<std::string> string_util::strip_punct(std::vector<std::string> &strvec){
	std::vector<std::string> result;
	typedef std::string::size_type string_size;

	for (std::vector<std::string>::iterator it = strvec.begin(); it!=strvec.end(); it++) {
		string_size i=0;
	   std::string str = *it;
		// skip punctuation characters in front of word
		while (ispunct(str[i])){
			str = str.substr(1);
		}
		// skip punctuation characters at end of word
		while (ispunct(str[str.size()-1])){
			str = str.substr(0, str.size()-1);
		}
		
		if ((str.size()>0) && (isalpha(str[0]) || isdigit(str[0]))) {
			result.push_back(str);
		}
	}
	return result;
}

/*
 *  strip punctuation characters in front of at end of strings  
 */
extern bool string_util::contains (std::string &str, std::string substr) {
   unsigned int loc = str.find(substr, 0 );
   if( loc != string::npos )
     return true;
   else
     return false;	
}

extern std::string string_util::lower_str(std::string str) {
	for(unsigned int i=0; i<str.size(); i++) {
		char c = (char)tolower((char)str[i]);
		str[i] = c;
	}
	return str;
}
