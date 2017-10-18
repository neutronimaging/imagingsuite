#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "frm2_retriever.h"
#include "../node.h"
#include "../nexus_util.h"
#include "../string_util.h"
#include "../tree.hh"
#include <math.h>
#include <cstring>
#include <cstdlib>
#include <cctype>

using std::ifstream;
using std::invalid_argument;
using std::runtime_error;
using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::isspace;
using std::isdigit;
using std::isalpha;

#define MIN(a,b) ( (a)<(b)? (a):(b) )
#define MAX(a,b) ( (a)>(b)? (a):(b) )

class NXunits {
	public:
	static const std::string second;		 
	static const std::string millisecond;	
	static const std::string microsecond;
	static const std::string nanosecond;
	static const std::string minute;
	static const std::string degree;	
	static const std::string count;
	static const std::string volt;
	static const std::string ampere;
	static const std::string angstrom;
	static const std::string inverse_angstrom;
	static const std::string bar;
	static const std::string millibar;
	static const std::string meter;
	static const std::string millimeter;
	static const std::string rpm;
	static const std::string hertz;
	static const std::string megahertz;
	static const std::string gigahertz;
	static const std::string terahertz;
	static const std::string kelvin;
	// to be completed later
};


static const int BUFFER_SIZE=4096;
static const int LOG_BUFFER_SIZE=64000;

const std::string NXunits::second = "second";
const std::string NXunits::millisecond = "millisecond";
const std::string NXunits::microsecond = "microsecond";
const std::string NXunits::nanosecond = "nanosecond";
const std::string NXunits::minute = "minute";
const std::string NXunits::degree = "degree";
const std::string NXunits::count = "counts";
const std::string NXunits::volt = "volt";
const std::string NXunits::ampere = "ampere";
const std::string NXunits::angstrom = "angstrom";
const std::string NXunits::inverse_angstrom = "angstrom^-1";
const std::string NXunits::bar = "bar";
const std::string NXunits::millibar = "millibar";
const std::string NXunits::meter = "meter";
const std::string NXunits::millimeter = "millimeter";
const std::string NXunits::rpm = "rpm";
const std::string NXunits::hertz = "Hz";
const std::string NXunits::megahertz = "MHz";
const std::string NXunits::gigahertz = "GHz";
const std::string NXunits::terahertz = "THz";
const std::string NXunits::kelvin = "K";


void Frm2Retriever::initUnits() {
	unit_strings["seconds"] 		= NXunits::second;
	unit_strings["second"] 			= NXunits::second;
	unit_strings["secs"] 			= NXunits::second;
	unit_strings["sec"] 				= NXunits::second;
	
	unit_strings["milliseconds"] 	= NXunits::millisecond;
	unit_strings["msec"] 			= NXunits::millisecond;
	unit_strings["msecs"] 			= NXunits::millisecond;

	unit_strings["microseconds"] 	= NXunits::microsecond;
	unit_strings["usec"] 			= NXunits::microsecond;
	unit_strings["usecs"] 			= NXunits::microsecond;
	
	unit_strings["nanoseconds"] 	= NXunits::nanosecond;
	unit_strings["nsec"] 			= NXunits::nanosecond;
	unit_strings["ns"] 				= NXunits::nanosecond;
	
	unit_strings["minutes"]	= NXunits::minute;
	unit_strings["minute"]	= NXunits::minute;
	unit_strings["min"]		= NXunits::minute;
	
	unit_strings["deg"] 		= NXunits::degree;
	unit_strings["degs"] 	= NXunits::degree;
	unit_strings["degree"] 	= NXunits::degree;
	unit_strings["degrees"] = NXunits::degree;
	
	unit_strings["cts"] 		= NXunits::count;
	unit_strings["counts"] 	= NXunits::count;
	unit_strings["count"] 	= NXunits::count;
	unit_strings["cnt"] 		= NXunits::count;
	
	unit_strings["bar"] 		= NXunits::bar;
	unit_strings["bars"]		= NXunits::bar;
	
	unit_strings["mbar"] 	= NXunits::millibar;
	unit_strings["millibar"]= NXunits::millibar;
	
	unit_strings["rpm"] 		= NXunits::rpm;
	unit_strings["rpms"]		= NXunits::rpm;
	
	unit_strings["volt"] 	= NXunits::volt;
	unit_strings["v"] 		= NXunits::volt;
	unit_strings["volts"] 	= NXunits::volt;
	
	unit_strings["ampere"] 	= NXunits::ampere;
	unit_strings["amp"] 		= NXunits::ampere;
	unit_strings["amperes"] = NXunits::ampere;
	
	unit_strings["a"] 			= NXunits::angstrom;
	unit_strings["ang"] 			= NXunits::angstrom;
	unit_strings["angst"] 		= NXunits::angstrom;
	unit_strings["angstrom"] 	= NXunits::angstrom;
	unit_strings["angstroem"] 	= NXunits::angstrom;
	unit_strings["angström"] 	= NXunits::angstrom;
	unit_strings["ångström"] 	= NXunits::angstrom;
	
	unit_strings["a^-1"] 				= NXunits::inverse_angstrom;
	unit_strings["a-1"] 					= NXunits::inverse_angstrom;
	unit_strings["angst^-1"] 			= NXunits::inverse_angstrom;
	unit_strings["angst-1"] 			= NXunits::inverse_angstrom;
	unit_strings["angstrom-1"] 		= NXunits::inverse_angstrom;
	unit_strings["angstrom^-1"] 		= NXunits::inverse_angstrom;
	unit_strings["invangstrom"] 		= NXunits::inverse_angstrom;
	unit_strings["inverse angstrom"] = NXunits::inverse_angstrom;
	unit_strings["invangstroem"] 		= NXunits::inverse_angstrom;
	unit_strings["inverse angstroem"]= NXunits::inverse_angstrom;
	unit_strings["angstroem-1"] 		= NXunits::inverse_angstrom;
	unit_strings["angstroem^-1"] 		= NXunits::inverse_angstrom;
	unit_strings["invangström"] 		= NXunits::inverse_angstrom;
	unit_strings["inverse angström"] = NXunits::inverse_angstrom;
	unit_strings["angström^-1"] 		= NXunits::inverse_angstrom;
	unit_strings["angström-1"] 		= NXunits::inverse_angstrom;
	unit_strings["invångström"] 		= NXunits::inverse_angstrom;
	unit_strings["inverse ångström"] = NXunits::inverse_angstrom;
	unit_strings["ångström-1"] 		= NXunits::inverse_angstrom;
	unit_strings["ångström^-1"] 		= NXunits::inverse_angstrom;
	
	unit_strings["m"] 			= NXunits::meter;
	unit_strings["meter"]		= NXunits::meter;
	
	unit_strings["mm"] 			= NXunits::millimeter;
	unit_strings["millimeter"]	= NXunits::millimeter;
	
	unit_strings["hz"]			= NXunits::hertz;
	unit_strings["hertz"]		= NXunits::hertz;
	unit_strings["mhz"]			= NXunits::megahertz;
	unit_strings["megahertz"]	= NXunits::megahertz;
	unit_strings["ghz"]			= NXunits::gigahertz;
	unit_strings["gigahertz"]	= NXunits::gigahertz;
	unit_strings["thz"]			= NXunits::terahertz;
	unit_strings["terahertz"]	= NXunits::terahertz;
	
	unit_strings["k"]				= NXunits::kelvin;
	unit_strings["kelvin"]		= NXunits::kelvin;
	unit_strings["kelvins"]		= NXunits::kelvin;
	
	unit_strings["none"]			= "";
}


const std::string Frm2Retriever::MIME_TYPE = "text/frm2";
const std::string Frm2Retriever::COLUMN_TAG = "column";
const std::string Frm2Retriever::DICT_TAG = "dict";
const std::string Frm2Retriever::DATETIME_TAG = "datetime";
const std::string Frm2Retriever::FILENAME_TAG = "filename";
const std::string Frm2Retriever::DICTARRAY_TAG = "dict_array";
const std::string Frm2Retriever::DICTMULTI_TAG = "dict_multi";
const std::string Frm2Retriever::TOFCTS_TAG = "tof_cts";
const std::string Frm2Retriever::TOFTOF_TAG = "tof_tof";
const std::string Frm2Retriever::TOFDNR_TAG = "tof_dnr";
const std::string Frm2Retriever::TOFMONTOF_TAG = "tof_montof";
const std::string Frm2Retriever::TOFMONCTS_TAG = "tof_moncts";
const std::string Frm2Retriever::TOFLOG_TAG = "tof_log";
const std::string Frm2Retriever::LOGCPY_TAG = "log_cpy";
const std::string Frm2Retriever::DESC_TAG = "desc";
const std::string Frm2Retriever::FLINE_TAG = "fline";
const std::string Frm2Retriever::RANGE_OPEN_BRACKET="[";
const std::string Frm2Retriever::RANGE_CLOSE_BRACKET="]";
const std::string Frm2Retriever::RANGE_SEPARATOR=":";
const std::string Frm2Retriever::METHOD_OPEN_BRACKET="(";
const std::string Frm2Retriever::METHOD_CLOSE_BRACKET=")";
const std::string Frm2Retriever::TYPE_OPEN_BRACKET="{";
const std::string Frm2Retriever::TYPE_CLOSE_BRACKET="}";
const std::string Frm2Retriever::ARG_QUOTE="'";
const std::string Frm2Retriever::ARG_SEPARATOR="'";


const std::string Frm2Retriever::HEADER_TAG = "header";
const std::string Frm2Retriever::DATA_TAG = "data";
const std::string Frm2Retriever::HEIDICTS_TAG = "heidi_cts";
const std::string Frm2Retriever::HEIDIMON_TAG = "heidi_mon";
const std::string Frm2Retriever::HEIDIIDS_TAG = "heidi_ids";
const std::string Frm2Retriever::HEIDIOMG_TAG = "heidi_omg";
const std::string Frm2Retriever::HEIDISO_TAG = "heidi_so";
const std::string Frm2Retriever::ARRAY_OPEN_BRACKET="[";
const std::string Frm2Retriever::ARRAY_CLOSE_BRACKET="]";
const std::string Frm2Retriever::ARRAY_RANGE_SEPARATOR=":";
const std::string Frm2Retriever::ARRAY_SEPARATOR=",";

const unsigned int Frm2Retriever::HEIDI_LINES_PER_ENTRY=3;
const unsigned int Frm2Retriever::HEIDI_CHARS_PER_COUNT=5;
const unsigned int Frm2Retriever::HEIDI_COLS_PER_LINE=16;



static unsigned int convert_type(std::string nxtype) {
	if (nxtype =="NX_CHAR") {
		return NX_CHAR;
	}
	else if (nxtype =="NX_BOOLEAN") {
		return NX_UINT8;
	}
	else if (nxtype =="NX_BINARY") {
		return NX_BINARY;
	}
	else if (nxtype =="NX_INT8") {
		return NX_INT8;
	}
	else if (nxtype =="NX_UINT8") {
		return NX_UINT8;
	}
	else if (nxtype =="NX_INT16") {
		return NX_INT16;
	}
	else if (nxtype =="NX_UINT16") {
		return NX_UINT16;
	}
	else if (nxtype =="NX_INT32") {
		return NX_INT32;
	}
	else if (nxtype =="NX_UINT32") {
		return NX_UINT32;
	}
	else if (nxtype =="NX_FLOAT32") {
		return NX_FLOAT32;
	}
	else if (nxtype =="NX_FLOAT32") {
		return NX_FLOAT32;
	}
	else if (nxtype =="ISO8601") {
		return NX_CHAR;
	}
	return NX_FLOAT64;
}

static double arsin(double x)
{
  if (x>0)
    return(asin(MIN(x, 1.0)));
  if (x<0)
    return(asin(MAX(x,-1.0)));
  return(0);
}

static double sgn(double x)
{
  if (x>0)
    return(1);
  else if (x<0)
    return(-1);
  else
    return(0);
}


static void reset_file(ifstream &file) {
	file.clear();
	file.seekg(0,std::ios::beg);
	file.clear();
}

static string read_line(ifstream &file) {
	static char buffer[BUFFER_SIZE];
	file.get(buffer,BUFFER_SIZE);
	file.get();
	int state = file.rdstate();
	//std::cout << "iostate: " << state << " failbit:" << std::ios::failbit << " badbit:"<< std::ios::badbit<<" eofbit:" << std::ios::eofbit<< std::endl;
	while (file.fail() && (!file.eof())) {
		file.clear();
		file.get();
	}
	return string(buffer);
}

static int count_chars(std::string str, char c) {
	int n=0;
	for (int i=0; i<str.size(); i++) {
		if (str[i] == c) {
			n++;
		}
	}
	return n;
}

static char read_char(ifstream &file){
  return file.get();
}

static void skip_to_line(ifstream &file,int &cur_line, int new_line){
   file.seekg(0,std::ios::beg);
  if(new_line==cur_line){ // skip out early if possible
    return;
  }else if(new_line<cur_line){  // go to the beginning if necessary
    file.seekg(0,std::ios::beg);
    cur_line=0;
  }

  // scan down to the right place
  while( (file.good()) && (cur_line<new_line) ){
    string text=read_line(file);
    //cout << "LINE" << cur_line << "[" << file.tellg() << "]:" << text << endl;
    cur_line++;
  }

  if(!(file.good()))
    throw invalid_argument("Could not reach line "
                           +string_util::int_to_str(new_line));
}




std::string Frm2Retriever::parse_method(std::string location){
	unsigned int pos = 0;
	static std::string sub_str;
	
	pos = location.find_first_of(Frm2Retriever::METHOD_OPEN_BRACKET);
	if (pos != std::string::npos) {
		sub_str = location.substr(0, pos);
		return sub_str;			
	}
	else {
		pos = location.find_first_of(Frm2Retriever::RANGE_OPEN_BRACKET);
		if (pos != std::string::npos) {
			sub_str = location.substr(0, pos);
			return sub_str;			
		}
		else {
			pos = location.find_first_of(Frm2Retriever::TYPE_OPEN_BRACKET);
			if (pos != std::string::npos) {
				sub_str = location.substr(0, pos);
				return sub_str;			
			}
		}
	}
	return location;
}


std::vector<unsigned int> Frm2Retriever::parse_dims(std::string dimstr) {
	unsigned int pos = 0;
	std::string sub_str;
	std::vector<unsigned int> dims;
	
	pos = dimstr.find_last_of(Frm2Retriever::ARRAY_OPEN_BRACKET);
	if (pos != std::string::npos) {
		sub_str = dimstr.substr(pos);
		sub_str = sub_str.substr(1);
		pos = sub_str.find_first_of(Frm2Retriever::ARRAY_CLOSE_BRACKET);
		if (pos != std::string::npos) {
			sub_str = sub_str.substr(0,pos);
			dims = string_util::split_uints(sub_str);	
		}
	}
	return dims;
}


std::vector<unsigned int> Frm2Retriever::parse_type_dims(std::string location, std::string &typestr){
	unsigned int pos = 0;
	std::string sub_str;
	std::vector<unsigned int> dims;
	
	pos = location.find_last_of(Frm2Retriever::TYPE_OPEN_BRACKET);
	if (pos != std::string::npos) {
		sub_str = location.substr(pos);
		sub_str = sub_str.substr(1);
		pos = sub_str.find_first_of(Frm2Retriever::TYPE_CLOSE_BRACKET);
		if (pos != std::string::npos) {
			sub_str = sub_str.substr(0,pos);
			pos = sub_str.find_last_of(Frm2Retriever::ARRAY_OPEN_BRACKET);
			if (pos != std::string::npos) {
				//std::cout << "dim brackets found: " << sub_str.substr(0,pos) << " " << sub_str << std::endl;
				typestr = sub_str.substr(0,pos);	
				return parse_dims(sub_str);
			}
			else {
				typestr = sub_str;			
			}
		}
	}
	return dims;
}

std::string Frm2Retriever::parse_type(std::string location){
	unsigned int pos = 0;
	static std::string sub_str;
	
	pos = location.find_last_of(Frm2Retriever::TYPE_OPEN_BRACKET);
	if (pos != std::string::npos) {
		sub_str = location.substr(pos);
		sub_str = sub_str.substr(1);
		pos = sub_str.find_first_of(Frm2Retriever::TYPE_CLOSE_BRACKET);
		if (pos != std::string::npos) {
			sub_str = sub_str.substr(0,pos);
			return sub_str;			
		}
	}
	return "";
}

std::vector<std::string> Frm2Retriever::parse_arg(std::string location){
	// check if column
	unsigned int start_pos = 0;
	unsigned int end_pos = 0;
	static std::string arg_str;
	static std::string item_str;
	std::vector<std::string> result;
	
	start_pos = location.find_first_of(Frm2Retriever::METHOD_OPEN_BRACKET);
	if (start_pos != std::string::npos) {
		arg_str = location.substr(start_pos);
		arg_str = arg_str.substr(1);
		arg_str = arg_str.substr(0, arg_str.size()-1);

		// TODO: check if range brackets are to be extracted
		// no need, they get cut automatically
		end_pos = arg_str.find_first_of(Frm2Retriever::METHOD_CLOSE_BRACKET);
		if (end_pos != std::string::npos) {
			arg_str = arg_str.substr(0, end_pos);
		}
		
		start_pos = arg_str.find_first_of(Frm2Retriever::ARG_QUOTE);
		if (start_pos == std::string::npos) {
			result = string_util::split_colons(arg_str);
			return result;
		}
		
		// check how many args 
		while (true) {
			start_pos = arg_str.find_first_of(Frm2Retriever::ARG_QUOTE);
			if (start_pos != std::string::npos) {
				
				arg_str = arg_str.substr(start_pos);
				arg_str = arg_str.substr(1);
			
				end_pos = arg_str.find_first_of(Frm2Retriever::ARG_QUOTE);
				if (end_pos != std::string::npos) {
					item_str = arg_str.substr(0, end_pos);
					arg_str = arg_str.substr(end_pos+1);
					result.push_back(item_str);
				}
			}
			else {
				break;
			}
		}
		
		/*/ check if quotes have to be removed 
		if (arg_str[0]=='\"' || arg_str[0] == '\'') {
			arg_str = arg_str.substr(1);
			arg_str = arg_str.substr(0, arg_str.size()-1);
		}*/
	}
		
	return result;
}


void Frm2Retriever::parse_range(std::string location, int &x, int &y){
	unsigned int pos = 0;
	static std::string sub_str;
	
	x = -1;
	y = -1;
	
	pos = location.find_last_of(Frm2Retriever::RANGE_OPEN_BRACKET);
	if (pos != std::string::npos) {
		sub_str = location.substr(pos);
		pos = sub_str.find_first_of(Frm2Retriever::RANGE_CLOSE_BRACKET);
		if (pos != std::string::npos) {
			sub_str = sub_str.substr(0,pos);
			
			pos = sub_str.find_first_of(Frm2Retriever::RANGE_SEPARATOR);
			if (pos != std::string::npos) {
				if (isNumber(sub_str.substr(0,pos))) {
					x = string_util::str_to_int(sub_str.substr(0,pos));
				}
				if (isNumber(sub_str.substr(pos))) {
					y = string_util::str_to_int(sub_str.substr(pos));
				}
				return;
			}
			else {
				// if no range separator is found -> go specified position to end of column
				if (isNumber(sub_str)) {
					x = string_util::str_to_int(sub_str);
				}
				return;
			}
		}
	}
}

void Frm2Retriever::parse_heidi_range(std::string location, int &line, int &word, int &wcount){
	unsigned int pos = 0;
	std::string sub_str;

	wcount = 0;
	// cut the type part as it may contain range info too ...
	pos = location.find_last_of(Frm2Retriever::TYPE_OPEN_BRACKET);
	if (pos != std::string::npos) {
		location = location.substr(0,pos);
	}
	
	pos = location.find_last_of(Frm2Retriever::ARRAY_OPEN_BRACKET);
	if (pos != std::string::npos) {
		sub_str = location.substr(pos+1);
		pos = sub_str.find_first_of(Frm2Retriever::ARRAY_CLOSE_BRACKET);
		if (pos != std::string::npos) {
			sub_str = sub_str.substr(0,pos);
			
			pos = sub_str.find_first_of(Frm2Retriever::ARRAY_SEPARATOR);
			if (pos != std::string::npos) {
				//std::cout << "conv to line: " << sub_str.substr(0,pos) << std::endl;
				line = string_util::str_to_int(sub_str.substr(0,pos));
				sub_str = sub_str.substr(pos+1);
				//std::cout << "XD sub_str: " << sub_str << std::endl;
				
				pos = sub_str.find_first_of(Frm2Retriever::ARRAY_RANGE_SEPARATOR);
				if (pos != std::string::npos) {
					//std::cout << "conv to word: " << sub_str.substr(0,pos) << std::endl;
					word = string_util::str_to_int(sub_str.substr(0,pos));
					//std::cout << "conv to wcount: " << sub_str.substr(pos+1) << std::endl;
					wcount = string_util::str_to_int(sub_str.substr(pos+1));
				}
				else {
					word = string_util::str_to_int(sub_str);
					wcount=0;
				}
				return;
			}
			else {
				line = 0;
				pos = sub_str.find_first_of(Frm2Retriever::ARRAY_RANGE_SEPARATOR);
				if (pos != std::string::npos) {
					//std::cout << "intstr: " << sub_str << sub_str.substr(0,pos) << std::endl;
					word = string_util::str_to_int(sub_str.substr(0,pos));
					int upperlimit = string_util::str_to_int(sub_str.substr(pos+1));
					wcount = upperlimit-word;
					//std::cout << "word: " << word << " wcount: " << wcount << std::endl;	
				}
				else {
					word = string_util::str_to_int(sub_str);
				}
				return;
			}
		}
	}
	line = -1;
	word = -1;
	wcount = 0;
}

bool Frm2Retriever::isdata(std::string line) {
	unsigned int i=0;
	int digit_count = 0;
	int alpha_count = 0;
	int space_count = 0;
	
	if (line.size()==0) {
		return false;
	}

	while(i<line.size()){
		if (isdigit(line[i])) {
			digit_count++;
		}
		else if (isalpha(line[i])) {
			alpha_count++;
		}
		else if (isspace(line[i])) {
			space_count++;
		}
		i++;
	}
	// if alpha count is greater than 75 percent -> assume its a data row

	if (line.size() > space_count) {
//cout << "chars: " << line.size()-space_count << " digits: " << digit_count <<" percent: " <<digit_count*100/(line.size()-space_count)<<endl;
		if (digit_count*100/(line.size()-space_count) > 70) {
			return true;
		}
	}
	else {
		return false;
	}
	return false;
}


bool Frm2Retriever::isunit(std::string line) {
	unsigned int unit_count = 0;
	std::vector<std::string> strings = string_util::split_whitespace(line);
	strings = string_util::strip_punct(strings);
	
	for (std::vector<std::string>::iterator its=strings.begin(); its!=strings.end(); its++) {
		std::string str_word = *its;
		for (std::map<std::string, std::string>::iterator it = unit_strings.begin(); it != unit_strings.end(); it++) {
			std::string str_unit= it->first;
			if (str_word == str_unit) {
				unit_count++;
				break;
			}
		}
	}
	//std::cout << " unit_count: " << unit_count << std::endl;
	if (unit_count > 1) {
		return true;
	}
	return false;
}

void Frm2Retriever::strip(std::string& str) {
	while (isspace(str[0])){
		str = str.substr(1);
	}
	while (isspace(str[str.size()-1])){
		str = str.substr(0, str.size()-1);
	}
}

bool Frm2Retriever::isNumber(std::string str) {
	// eliminate whitespace on both ends
	while (isspace(str[0])){
		str = str.substr(1);
	}
	while (isspace(str[str.size()-1])){
		str = str.substr(0, str.size()-1);
	}
	
	// if string is empty -> nan
	if (str.size()<=0) {
		return false;
	}

	if (str[0]=='-' || str[0]=='+') {
		str = str.substr(1);
	}
	
	// if string is empty -> nan
	if (str.size()<=0) {
		return false;
	}

	for (unsigned int i=0; i < str.size(); i++) {
		if (!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}

bool Frm2Retriever::isheidiheader(std::string line) {
	unsigned int i=0;
	int digit_count = 0;
	int alpha_count = 0;
	int space_count = 0;
	
	while(i<line.size()){
		if (isdigit(line[i])) {
			digit_count++;
		}
		else if (isalpha(line[i])) {
			alpha_count++;
		}
		else if (isspace(line[i])) {
			space_count++;
		}
		i++;
	}

	if (alpha_count >=3 && string_util::contains(line, "=") ) {
		return true;
	}
	return false;
}


bool Frm2Retriever::isheidicountdata(std::string line) {
	unsigned int i=0;
	int digit_count = 0;
	int alpha_count = 0;
	int space_count = 0;
	
	while(i<line.size()){
		if (isdigit(line[i])) {
			digit_count++;
		}
		else if (isalpha(line[i])) {
			alpha_count++;
		}
		else if (isspace(line[i])) {
			space_count++;
		}
		i++;
	}

	if (digit_count > 38) {
		return true;
	}
	return false;
}



void Frm2Retriever::extract_headers(std::string line) {
	//std::cout << "header line: " << line << std::endl;
	headers = string_util::split_whitespace(line); 
	headers = string_util::strip_punct(headers);
	/*for (unsigned int i=0; i< headers.size(); i++) {
		std::cout << "headers["<<i<<"]: " << headers.at(i) << std::endl;
	}*/
}


void Frm2Retriever::extract_toflogheaders(std::ifstream &file) {
	int cur_line=0;
	int i=0;
	unsigned int pos=0;
	std::string str="";
	std::string value="";
	std::map<int, std::string> key_pairs; 
	
	reset_file(file);
	while (file.good() && (cur_line++)<=data_section) {
		
		std::string line = read_line(file);
		
		// eliminate whitespace at start of line
		while (isspace(line[0])){
			line = line.substr(1);
		}
		pos = line.find("#++");
		if (pos != std::string::npos && pos<2) {
			line = line.substr(pos+3);
			pos = line.find(":", pos);
			if (pos != std::string::npos) {
				value = line.substr(0, pos);
				while (isspace(value[0])){
					value = value.substr(1);
				}
				
				str = line.substr(pos+1);
				while (isspace(str[0])){
					str = str.substr(1);
				}
				while (isspace(str[str.size()-1])){
					str = str.substr(0, str.size()-1);
				}
				
				if (isNumber(str)) {
					int key = string_util::str_to_int(str);
					key_pairs[key] = value;
				}
			}
		}
	}

	headers.clear();
	// map map to header vector 
	for (unsigned int i=0; i< key_pairs.size(); i++) {
		//std::cout << "pushing " << key_pairs[i] << " into headers" << std::endl;
		headers.push_back(key_pairs[i]);
	}
}

std::vector<std::string>& Frm2Retriever::clear_units(std::vector<std::string> &units) {
	for (std::vector<std::string>::iterator it=units.begin(); it!=units.end(); it++) {
		std::string unit = *it;
		if (unit == "-" ) {
			units.erase(it);
			units.insert(it, "none");
		}
	}
	return units;
}
	
void Frm2Retriever::extract_units(std::string line) {
	units = string_util::split_whitespace(line);
	units = clear_units(units);
	units = string_util::strip_punct(units);
	for (unsigned int i=0; i< units.size(); i++) {
		std::cout << "units["<<i<<"]: " << units.at(i) << std::endl;
	}
}


std::string Frm2Retriever::extract_datetime(std::ifstream &file, std::vector<std::string> args, unsigned int nxtype) {
	std::string datetime;
	std::string date;
	std::string time;
	
	// size of args must be at least 1 (time)
	if (args.size() < 1) {
		return "";
	}
	if (args.size() < 2 ) {
		// we got only a timestring
		std::map<std::string, std::string> raw_map = extract_dictentry(file, args[0], nxtype);
		datetime = raw_map["values"];
	}
	else {
		// we got date and time string separately
		std::map<std::string, std::string> raw_map;
		raw_map = extract_dictentry(file, args[0], nxtype);
		date = raw_map["values"];
		raw_map = extract_dictentry(file, args[1], nxtype);
		time = raw_map["values"];
		
		datetime = date+" "+time;
		if (count_chars(datetime, '.') > 1) {
			datetime = toftof_datetime_2_iso(datetime);
		}
		else if (count_chars(datetime, '/') > 1) {
			datetime = nicos_datetime_2_iso(datetime);
		}
		else if (count_chars(datetime, '-') > 1) {
			datetime = heidi_datetime_2_iso(datetime);
		}
	}
	
	return datetime;	
}


std::map<std::string, std::string> Frm2Retriever::extract_dictmulti(std::ifstream &file, std::vector<std::string> args, unsigned int nxtype) {
	std::string values = "";
	std::string value = "";
	std::string units = "";
	std::map<std::string, std::string> result;
	
	for (std::vector<std::string>::iterator it=args.begin(); it!=args.end();it++) {
		std::map<std::string, std::string> raw_map = extract_dictentry(file, *it, nxtype);
		value = raw_map["values"];
		if (raw_map["units"] != "" && raw_map["units"] != "unknown") {
			units = raw_map["units"];
		}
		values = values + value; 
		if (it!=(args.begin()-1)) {
			values = values + " ";
		}
	}	
	result["values"] = values;
	result["units"] = units;
	reset_file(infile);
	return result;	
}


// only for numeric arrays
std::map<std::string, std::string> Frm2Retriever::extract_dictarray(std::ifstream &file, std::string arg, unsigned int nxtype) {
	int i=0;
	unsigned int k=0, l=0;
	unsigned int pos=0, pos2=0;
	bool newnum=false;
	std::map<std::string, std::string> result;
	std::string str="";
	std::string units="";
	std::string values="";

	if (nxtype == NX_CHAR) {
		return result;
	}
	
	reset_file(infile);
	while (infile.good()) {
		std::string line = read_line(infile);
		//std::cout << "current line: " << line << std::endl;	
		
		// eliminate whitespace at start of line
		while (isspace(line[0])){
			line = line.substr(1);
		}
		
		//std::cout << "to find: " << arg << " current line: " << line << std::endl;	
		pos = line.find(arg);
		if (pos != std::string::npos && pos<2) {
			line = line.substr(pos+arg.size());
			pos2 = line.find(":", pos);
			if (pos2 != std::string::npos) {
				
				str = line.substr(pos2+1);
				//std::cout << "found dict: " << str << std::endl;	
				
				while(k<str.size() && ((ispunct(str[k]) && str[k]!='+' && str[k]!='-' ) || isspace(str[k]))) {
					k++;
				}
				str = str.substr(k);
				
				while (isspace(str[str.size()-1]) || ispunct(str[str.size()-1])){
					str = str.substr(0, str.size()-1);
				}
				
				while(l<str.size()) {
					if (isdigit(str[l]) || ispunct(str[l]) || str[l]=='e' || str[l]=='E' || str[l]=='+' || str[l]=='-') {
						if (str[l]=='e' || str[l]=='E') {
							if (l>0) {
								if (isdigit(str[l-1])){
									if (newnum) {
										values = values + " ";
										newnum= false;
									}
									values = values+str[l];
								}
							}
						}
						/*else if (str[l]=='+' || str[l]=='-') {
							if (l>0) {
								if (str[l-1]=='e' || str[l-1]=='E'){
									if (newnum) {
										values = values + " ";
										newnum= false;
									}
									values = values+str[l];
								}
							}
						}*/
						else {	
							if (newnum) {
								values = values + " ";
								newnum= false;
							}
							values = values+str[l];
						}
					}
					else {
						if (units == "") {
							while (isalpha(str[l])) {
								units = units+str[l];
								l++;
							}
						}
						newnum = true;
					}
					l++;
				}
			}
			break;
		}
	}
	result["values"]=values;
	result["units"]=units;
	reset_file(infile);
	return result;
}


std::vector<std::vector<unsigned int> > Frm2Retriever::extract_tofcts(std::ifstream &file, std::string arg, unsigned int nxtype, bool is_monitor) {
	std::vector<std::vector<unsigned int> > result;
	std::string line = "";
	unsigned int pos=0;
	int i=0, count=0;
	int monitor_number=-1;
	
	std::map<std::string, std::string> raw_map = extract_dictentry(infile, "TOF_MonitorInput", NX_INT32);
	std::string raw_minput = raw_map["values"];
	std::string units= raw_map["units"];
	
	if (isNumber(raw_minput)) {
		monitor_number = string_util::str_to_int(raw_minput);
	}
	
	std::vector<unsigned int> dInfo = extract_desc(infile, "aData",NX_INT32);
	
	reset_file(infile);
	while (infile.good()) {
		line = read_line(infile);
		
		// eliminate whitespace at start of line
		while (isspace(line[0])){
			line = line.substr(1);
		}
		
		pos = line.find(arg);
		if (pos != std::string::npos && pos<2) {
			break;
		}
	}
	while (infile.good()) {
		line = read_line(infile);
		if (is_monitor) {
			if (count == monitor_number) {
				std::vector<unsigned int> counts = string_util::split_uints(line);
				result.push_back(counts);
			}
			count++;
		}
		else {
			monitor_number = -1;
			if (count != monitor_number) {
				std::vector<unsigned int> counts = string_util::split_uints(line);
				while (counts.size() < dInfo.at(1)) {
					// add missing .. zeros
					counts.push_back(0);
				}
				while (counts.size() > dInfo.at(1)) {
					// cut superflouos
					counts.pop_back();
				}
				if (result.size() < dInfo.at(0)) {
					result.push_back(counts);
				}
			}
			count++;
		}
	}
	
	// if some entrys are missing -> adapt
	while (result.size() < dInfo.at(0)) {
		// add empty vectors
		std::vector<unsigned int> counts;
		for (unsigned int i=0; i < dInfo.at(1); i++) {
			counts.push_back(0);
		}
		result.push_back(counts);
	}
	
	reset_file(infile);
	return result;
}


std::vector<double> Frm2Retriever::extract_toftof(std::ifstream &file, std::string arg, unsigned int nxtype, bool is_monitor) {
	std::vector<double> result;
	std::string line = "";
	unsigned int pos=0;
	int i=0;
	unsigned int channel_width = 1;
	
	std::vector<unsigned int> dInfo = extract_desc(infile, "aData",NX_INT32);

	std::map<std::string, std::string> raw_map = extract_dictentry(infile, "TOF_ChannelWidth", NX_INT32);
	std::string ch_width = raw_map["values"];
	
	if (isNumber(ch_width)) {
		channel_width = string_util::str_to_int(ch_width);
	}

	
	reset_file(infile);
	while (infile.good()) {
		line = read_line(infile);
		
		// eliminate whitespace at start of line
		while (isspace(line[0])){
			line = line.substr(1);
		}
		pos = line.find(arg);
		if (pos != std::string::npos && pos<2) {
			break;
		}
	}

	if (infile.good()) {
		line = read_line(infile);
		std::vector<unsigned int> counts = string_util::split_uints(line);
		while (counts.size() < dInfo.at(1)) {
			// add missing .. zeros
			counts.push_back(0);
		}
		while (counts.size() > dInfo.at(1)) {
			// cut superflouos
			counts.pop_back();
		}

		for (unsigned int i=0; i<counts.size(); i++) {
			result.push_back((((double)i)+0.5)*channel_width*50);
		}
	}

	reset_file(infile);
	return result;
}


std::string Frm2Retriever::extract_filename(std::ifstream &file, std::string source, std::string arg, unsigned int nxtype) {
	unsigned int pos=0;
	unsigned int from=0, to=source.size();
	std::string result="0";
	
	
	pos = source.rfind("/");
	if (pos != std::string::npos) {
		result = source.substr(pos+1);
		to = result.size();
	}
	pos = arg.find(":");
	if (pos != std::string::npos) {
		if (isNumber(arg.substr(0, pos)) && isNumber(arg.substr(pos+1))) {
			from 	= string_util::str_to_int(arg.substr(0, pos));
			to 	= string_util::str_to_int(arg.substr(pos+1)) - from;
		}
	}
	result = result.substr(from, to);
	return result;
}


std::vector<unsigned int> Frm2Retriever::extract_desc(std::ifstream &file, std::string arg, unsigned int nxtype) {
	int i=0;
	unsigned int pos=0;
	std::string line="";
	std::vector<unsigned int> result;
	
	reset_file(infile);
	while (infile.good()) {
		std::string line = read_line(infile);

		// eliminate whitespace at start of line
		while (isspace(line[0])){
			line = line.substr(1);
		}

		pos = line.find(arg);
		if (pos != std::string::npos && pos<2) {
			pos = line.find(Frm2Retriever::METHOD_OPEN_BRACKET, pos);
			if (pos != std::string::npos) {
				line = line.substr(pos+1);
				pos = line.find(Frm2Retriever::METHOD_CLOSE_BRACKET);
				if (pos != std::string::npos) {
					line = line.substr(0,pos);
					result = string_util::split_uints(line);	
				}
			}
			break;
		}
	}
	reset_file(infile);
	return result;
}


std::string Frm2Retriever::extract_logcpy(std::ifstream &file, std::string filename) {
	char ch;
	std::stringstream ss;
	
	reset_file(infile);
   while( infile.get(ch)) {
     ss << ch;
	}
	
	reset_file(infile);
	return ss.str();
}


std::vector<std::string> Frm2Retriever::extract_toflog(std::ifstream &file, std::string col_name, int from, int to) {
	bool use_col_number=true;
	int count = 0;
	int index = 0;
	std::vector<std::string> values;
	int cur_line=0;
	std::string line = ""; // read_line(infile);

	reset_file(infile);
	
	skip_to_line(infile, cur_line, data_section+((from<0)?0:from)); 
	
	use_col_number = isNumber(col_name);
	
	if (!use_col_number) {
		std::vector<std::string>::iterator it;
		for (it = headers.begin(); it !=headers.end(); it++) {
			if (*it != col_name)  {
				index++;
			}
			else {
				break;
			}
		}
		if (it== headers.end()) {
			std::cout << "no column '"<< col_name << "' found " << std::endl;
			return values;
		}
	}
	else {
		index = string_util::str_to_int(col_name);
		index--;
	}
	
	line = read_line(infile);
	while ((infile.good()) && ((to>=0 && to<count) || (to<0))) {			
		if (!isdata(line)) {
			break;
		}
		try {
			std::vector<std::string> line_values = string_util::split_values(line);
			/*for (std::vector<std::string>::iterator itt=values.begin(); itt!=values.end(); itt++) {
				std::cout << "column------- vector: " << *itt << std::endl;
			}*/
			/*std::cout << endl << "pushing data: " << (string_util::str_to_float(string_values.at(index))) << std::endl;	
			//double dbl_val = string_util::str_to_float(string_values.at(index));*/
			values.push_back(line_values.at(index));
		}
		catch(...) {
			cout << "exception in transforming column values " << endl;	
		}
		count++;
		line = read_line(infile);
	}
		
	reset_file(infile);
	return values;
}


std::string Frm2Retriever::extract_line_below(std::ifstream &file, std::string arg, unsigned int nxtype) {
	unsigned int pos=0;
	std::string line="";
	
	reset_file(infile);
	while (infile.good()) {
		line = read_line(infile);
		while (isspace(line[0])){
			line = line.substr(1);
		}
		pos = line.find(arg);
		if (pos != std::string::npos && pos<2) {
			line = read_line(infile);
			break;
		}
	}
	reset_file(infile);
	return line;
}


std::map<std::string,std::string > Frm2Retriever::extract_dictentry(std::ifstream &file, std::string arg, unsigned int nxtype) {
	int i=0;
	unsigned int k=0, l=0;
	unsigned int pos=0, pos2=0;
	bool newnum=false;
	std::string str="";
	
	std::string units="";
	std::string description="";
	std::string values="";

	std::map<std::string,std::string > result;
	
	reset_file(infile);
	while (infile.good()) {
		std::string line = read_line(infile);
		//std::cout << "current line: " << line << std::endl;	
	
		// eliminate whitespace at start of line
		while (isspace(line[0])){
			line = line.substr(1);
		}

		pos = line.find(arg);
		if (pos != std::string::npos && pos<2) {
			line = line.substr(pos+arg.size());
			//std::cout << "got line: " << 	line << std::endl;
			/* extract description*/
			description = line;
			pos = description.find("-");
			if (pos != std::string::npos) {
				description = description.substr(pos+1);
				pos=description.find(":");
				if (pos != std::string::npos) {
					description = description.substr(0, pos);			
				}
			}
			else {
				description ="";
			}
			while (isspace(description[0])){
				description=description.substr(1);
			}
			while (isspace(description[description.size()-1])){
				description = description.substr(0, description.size()-1);
			}
			result["description"] = description;
			/*end description*/
			
			pos2 = line.find(":", pos);
			if (pos2 != std::string::npos) {
				str = line.substr(pos2+1);
				
				while(k<str.size() && (ispunct(str[k]) || isspace(str[k])) && (str[k]!='-') && (str[k]!='+')) {
					k++;
				}
				str = str.substr(k);
				
				while(l<str.size()) {
					if (nxtype == NX_CHAR) {
						if (!isspace(str[l])) {
							if (newnum) {
								values = values + " ";
								newnum= false;
							}
							values = values+str[l];
						}
						else {
							if (units == "") {
								while ( isalpha(str[l]) && !isspace(str[l])  ) {
									units = units+str[l];
									l++;
								}
							}
							newnum = true;
						}
					}
					else {
						if (   (!isspace(str[l]) && !isalpha(str[l])) 
							 || ( (l>0) && isdigit(str[l-1]) && ((str[l]=='e') || (str[l]=='E')) ) 
							 || (str[l]=='-') || (str[l]=='+') ) {
							if (newnum) {
								//std::cout << "starting new word! " << std::endl;
								values = values + " ";
								newnum= false;
							}
							//std::cout << "appending: " << str[l] << std::endl;
							values = values+str[l];
						}
						else {
							if (units == "") {
								while (isalpha(str[l]) && !isspace(str[l])) {
									units = units+str[l];
									l++;
								}
							}
							newnum = true;
						}
					}
					l++;
				}
			}
			break;
		}
	}
	result["values"]=values;
	result["units"]=units;
	reset_file(infile);
	return result;
}

std::vector<std::string> Frm2Retriever::extract_column(ifstream &file, std::string col_name, int from, int to)
{
	bool use_col_number=true;
	int count = 0;
	int index = 0;
	std::vector<std::string> values;
	reset_file(infile);
	int cur_line=0;
	std::string line = ""; // read_line(infile);

	//std::cout << "datasection starts at line: " << data_section << std::endl;
	skip_to_line(infile, cur_line, data_section+((from<0)?0:from)); 
	//cout << endl << "counting headers: " << *(headers.begin()) << endl;	
	
	use_col_number = isNumber(col_name);
	
	if (!use_col_number) {
		std::vector<std::string>::iterator it = headers.begin();
		while (*it != col_name && it != headers.end()) {
			it++;
			index++;
		}
		if (it== headers.end()) {
			std::cout << "no column '"<< col_name << "' found " << std::endl;
			/*while ((infile.good()) && ((to>=0 && to<count) || (to<0))) {			
				if (!isdata(line)) {
					break;
				}
				values.push_back("0");
				count++;
				line = read_line(infile);
			}*/
			return values;
		}
	}
	else {
		index = string_util::str_to_int(col_name);
		index--;
	}
	

//cout << endl << "pushing data.. [" << to << ","<< count<<"] error:"<< infile.good()<< endl;	
	line = read_line(infile);
	while ((infile.good()) && ((to>=0 && to<count) || (to<0))) {			
		if (!isdata(line)) {
			//std::cout << "NO DATA LINE:" << line << std::endl;
			break;
		}
		try {
			//std::cout << "transforming..." << std::endl;
			std::vector<std::string> line_values = string_util::split_values(line);
			/*for (std::vector<std::string>::iterator itt=line_values.begin(); itt!=line_values.end(); itt++) {
				std::cout << "column------- vector: " << *itt << std::endl;
			}*/
			/*std::cout << endl << "pushing data: " << (string_util::str_to_float(string_values.at(index))) << std::endl;	
			//double dbl_val = string_util::str_to_float(string_values.at(index));*/
			values.push_back(line_values.at(index));
		}
		catch(...) {
			cout << "exception in transforming column values " << endl;	
		}
		count++;
		line = read_line(infile);
	}
	reset_file(infile);
	return values;
}


std::vector<unsigned int> Frm2Retriever::extract_dnr(std::ifstream &file, std::string arg, unsigned int nxtype) {
	std::vector<unsigned int> result;
	std::string line = "";
	unsigned int pos=0;
	unsigned int i=0, count=0;
	int monitor_number = -1;

	std::map<std::string, std::string> raw_map = extract_dictentry(infile, "TOF_MonitorInput", NX_INT32);
	std::string raw_minput = raw_map["values"];
	
	if (isNumber(raw_minput)) {
		monitor_number= string_util::str_to_int(raw_minput);
	}
	
	std::vector<unsigned int> dInfo = extract_desc(infile, "aData",NX_INT32);

	reset_file(infile);
	while (infile.good()) {
		line = read_line(infile);
		
		// eliminate whitespace at start of line
		while (isspace(line[0])){
			line = line.substr(1);
		}
		pos = line.find(arg);
		if (pos != std::string::npos && pos<2) {
			break;
		}
	}
	while (infile.good()) {
		line = read_line(infile);
		if (count != monitor_number) { 
			if (result.size() < dInfo.at(1)) {
				result.push_back(count);
			}
		}
		count++;
	}
	
	while (result.size() > dInfo.at(1)) {
		result.pop_back();
	}
	
	reset_file(infile);
	return result;
}

std::string Frm2Retriever::extract_header(std::ifstream &file, std::string arg, unsigned int nxtype, unsigned int word, unsigned int wcount) {
	int i=0, j=0;
	unsigned int k=0, l=0, u1=0, u2=0;
	unsigned int pos=0, pos2=0;
	std::string str="";
	std::string result="";

	reset_file(infile);
	if (arg[arg.size()-1]!='=') {
		arg.append("=");
	}
	
	//std::cout << "data_section: " << data_section << std::endl;
	while (i<data_section) {
		std::string line = read_line(infile);
		pos = line.find(arg);
		//std::cout << "line: " << line << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		if (pos != std::string::npos) {
			//got the right line in data_section !!
			str = line.substr(pos+arg.size());
			//std::cout << "getting val: " << str << std::endl;
			
			// scan for word ...
			std::vector<std::string> words = string_util::split_whitespace(str);
			
			// special case for heidi Omat extends over 3 lines
			if (arg.find("Omat")>=0) {
				line = read_line(infile);
				std::vector<std::string> words2 = string_util::split_whitespace(line);
				for (unsigned int i=0; i< words2.size(); i++) {
					words.push_back(words2.at(i)); 
				}
				line = read_line(infile);
				std::vector<std::string> words3 = string_util::split_whitespace(line);
				for (unsigned int i=0; i< words3.size(); i++) {
					words.push_back(words3.at(i)); 
				}
			}
			
			//std::cout << "words: " << word << " wcount: " << wcount <<std::endl;
			for (int j=word; j<word+wcount;j++) {
				result.append(words[j]).append(" ");
				//std::cout << "appending: " <<  words[j] <<std::endl;
			}
			break;
		}
		i++;
	}
	//std::cout << "extract_header returning: " << result << std::endl; //REMOVE
	return result;
}

#ifdef _WIN32
static double round(double x)
{
	if (x >= 0.0)
	{
		return (double)(long long)(x + 0.5);
	}
	else
	{
		return (double)(long long)(x - 0.5);
	}
}
#endif

std::vector<double> Frm2Retriever::extract_heidi_so(std::ifstream &file, int entry_num, unsigned int nxtype, unsigned int word, unsigned int wcount) {
	const double dlim=1e-8;
	int i=0, j=0;
	double x, y;
	double vl, wl;
	double psi;
	double xyz[3];
  
	double c;
	
	double schi0;
	double cchi0;
	double omg1;
	double phi1;
	double psr;
	
	double angle[5]; 
	
	std::vector<double> result;
	
	//unsigned int k=0, l=0, u1=0, u2=0;
	//unsigned int pos=0, pos2=0;
	//double omat[3][3]; 
	//std::string str="";
	//std::string result="";
	
	//reset_file(infile);
	
printf("extract the orientation matrix\n");	
	// extract the orientation matrix
	std::string omat_str = extract_header(file, "Omat", NX_FLOAT64, 0, 9);
printf("orientation matrix extracted\n");	
   std::vector<double> om = string_util::split_doubles(omat_str);	
printf("vector of doubles created: \n%f%f%f\n%f%f%f\n%f%f%f\n\n", om[0], om[1],om[2],om[3],om[4],om[5],om[6],om[7],om[8]);	
	
	// extract wavelength
	std::string wl_str = extract_header(file, "Wave", NX_FLOAT64, 0, 1);
   wl = string_util::str_to_float(wl_str);	
	
	// extract the hkls 
   std::string hkl_str = extract_data(file, entry_num, NX_INT32, 0, 0, 3);
printf("hkls extracted:%s\n",hkl_str.c_str());	
	std::vector<int> hkl = string_util::split_ints(hkl_str);	
printf("hkls extracted:%d %d %d\n",hkl[0], hkl[1], hkl[2]);	
	
	// extract psi
	std::string psi_str = extract_data(file, entry_num, NX_FLOAT64, 0, 6, 1); 
printf("psi extracted:%s\n",psi_str.c_str());	
	psi = (double)atoi(psi_str.c_str());

	// calculate vector length
	//vl=sqrt(pow(xyz[0], 2)+pow(xyz[1], 2)+pow(xyz[2], 2));


  // calculate direction vectors
  for (i=0; i<3; ++i)
  {
    xyz[i]=0.0;
    for (j=0; j<3; ++j)
      xyz[i]+=om[(i*3)+j]*hkl[j];
  }

  // calculate scalar product
  c=0.0;
  for (i=0; i<3; ++i) {
    c+=xyz[i]*xyz[i];
  }
  
  vl=sqrt(c);

  if (vl<dlim)
    vl=1.0;

  for (i=0; i<3; ++i) {
    xyz[i]/=vl;
  }

  x       =xyz[0];
  y       =xyz[1];
  schi0   =xyz[2];

  //printf("vl:%f, xyz: %f %f %f, psi:%f\n", vl, xyz[0], xyz[1], xyz[2]);
  printf("vl:%f, xyz: %f %f %f\n", vl, xyz[0], xyz[1], xyz[2]);
  angle[2]=arsin(0.5*vl*wl);
  angle[1]=2.0*angle[2];
  angle[3]=arsin(schi0);
  angle[4]=0;

  if ((x*x+y*y)>0) {
    angle[4]=atan2(-y, x);
  }

  // psi rotation
  if (psi!=0)
  {
    psr=psi*1.74532925199e-2;
    cchi0=cos(angle[3]);

    // chi0=0
    if (fabs(angle[3])==0)
    {
      omg1=90.0*1.74532925199e-2;
      angle[3]=psr+angle[3];
      phi1=sgn(omg1);
    }
    else
    {
      if (fabs(cchi0)==0)
      {
        omg1=0.0;
        phi1=psr*sgn(-angle[3]);
      }
      else
      {
        omg1=atan(sin(psr)*cchi0/schi0);
        x=cos(omg1);
        angle[3]=atan2(schi0/x, cchi0*cos(psr));
        phi1=atan2(-sin(omg1)/cchi0, x*cos(angle[3])/cchi0);
      }
    }
    angle[2]-=omg1;
    angle[4]-=phi1;
  }

  for (i=1; i<5; ++i) {
    angle[i]*=57.2957795130;
  }
  x=angle[3];

  if (fabs(x)>180) {
    angle[3]=x-sgn(360-x);
  }

  angle[4]+=round(-angle[4]/360)*360.0;
  if (/*(lphi==0) &&*/ (angle[4]<0)) {
      angle[4]=angle[4]+360.0;
  }

  result.push_back(angle[0]);
  result.push_back(angle[1]);
  result.push_back(angle[2]);
  result.push_back(angle[3]);
	// now we hopefully got the orientation matrix
   // re-calculate angles, omega, chi and phi  now ...
   // convert strings to floats
	// do matrix calculations (inverse from dif4)
	
	//std::cout << "extract_header returning: " << result << std::endl; //REMOVE
	
	/*while (i<data_section) {
		std::string line = read_line(infile);
		pos = line.find(arg);
		//std::cout << "line: " << line << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		if (pos != std::string::npos) {
			//got the right line in data_section !!
			str = line.substr(pos+arg.size());
			//std::cout << "getting val: " << str << std::endl;
			
			// scan for word ...
			std::vector<std::string> words = string_util::split_whitespace(str);
			for (unsigned int i=0; i< words.size(); i++) {
				omat[0][i] = string_util::str_to_float(words.at(i));	
			}
			
			// special case for heidi Omat extends over 3 lines
			if (arg.find("Omat")>=0) {
				line = read_line(infile);
				std::vector<std::string> words2 = string_util::split_whitespace(line);
				for (unsigned int i=0; i< words2.size(); i++) {
					words.push_back(words2.at(i)); 
					omat[1][i] = string_util::str_to_float(words2.at(i));	
			   }		
				line = read_line(infile);
				std::vector<std::string> words3 = string_util::split_whitespace(line);
				for (unsigned int i=0; i< words3.size(); i++) {
					words.push_back(words3.at(i)); 
					omat[2][i] = string_util::str_to_float(words2.at(i));	
				}
			}
			
			//std::cout << "words: " << word << " wcount: " << wcount <<std::endl;
			for (int j=word; j<word+wcount;j++) {
				result.append(words[j]).append(" ");
				//std::cout << "appending: " <<  words[j] <<std::endl;
			}
			break;
		}
		i++;
	 }*/
   
	return result;
}


std::vector<double> Frm2Retriever::extract_heidi_counts(ifstream &file, int entry_num, bool monitor_counts)
{
	int count = 0;
	int index = 0;
	std::vector<double> values;
	reset_file(infile);
	int cur_line=0;
	skip_to_line(infile, cur_line, data_section); 
// strategy: find datasection -> skip 3*entry_num lines -> read 8 param of first line -> 
// n = number of scans -> read n 
	std::string line = read_line(infile);
	std::vector<std::string> words = string_util::split_whitespace(line);
	int nitems = 1;
	int nlines = 0;
	if (isNumber(words.at(7))) {
		nitems =string_util::str_to_int(words.at(7));
		nlines = (int)ceil(((double)(nitems*2))/Frm2Retriever::HEIDI_COLS_PER_LINE);
	}

	if (entry_num>0) {
		for (unsigned int i=0; i<((entry_num*(nlines+2))-1); i++) {
			read_line(infile);
		}
		line = read_line(infile);
	}
	//std::cout << "line: " << line << std::endl;
	words = string_util::split_whitespace(line);
			
	//std::cout << "words: " << word << " wcount: " << wcount <<std::endl;
	int numcounts = 0;
	if (isNumber(words[7])) {
		numcounts = string_util::str_to_int(words[7]);
	}
	else {
		std::ios::pos_type fpos = infile.tellg();
		line = read_line(infile);
		line = read_line(infile);
		numcounts = line.size() / (Frm2Retriever::HEIDI_CHARS_PER_COUNT*2);
		infile.seekg(fpos);
	}
	line = read_line(infile);
	
	//std::cout << "CURRENT LINE: " << line << std::endl;
	char count_str[Frm2Retriever::HEIDI_CHARS_PER_COUNT+1];
	if (monitor_counts) {
		int num_line_breaks = (int)floor(((double)numcounts)/((double)Frm2Retriever::HEIDI_COLS_PER_LINE));
		infile.seekg(((Frm2Retriever::HEIDI_CHARS_PER_COUNT*numcounts)+num_line_breaks), std::ios_base::cur);
	}
	for (int i=0; i<numcounts; i++) {
		for (int j=0; j<Frm2Retriever::HEIDI_CHARS_PER_COUNT;j++) { 
			count_str[j] = read_char(infile);
			if (count_str[j] == '\n') {
				count_str[j] = read_char(infile);
			}
		}
		count_str[Frm2Retriever::HEIDI_CHARS_PER_COUNT]='\0';
		//std::cout << "pushing back: " << std::string(count_str) << std::endl;
		values.push_back(string_util::str_to_float(std::string(count_str)));
		//std::cout << "pushing back: " << string_util::str_to_float(std::string(count_str)) << std::endl;
	}
	
//cout << endl << "counting headers: " << *(headers.begin()) << endl;	

	/*std::vector<std::string>::iterator it = headers.begin();
	while (*it != col_name && it != headers.end()) {
		it++;
		index++;
	}

	std::string line = read_line(infile);

	if (it== headers.end()) {
		std::cout << "no column '"<< col_name << "' found in file ... filling with 0s" << std::endl;
		while ((infile.good()) && ((to>=0 && to<count) || (to<0))) {			
			if (!isdata(line)) {
				break;
			}
			values.push_back(0);
			count++;
			line = read_line(infile);
		}
		return values;
	}

//cout << endl << "pushing data: " << to << " "<< count<<" "<< infile.good()<< endl;	
	while ((infile.good()) && ((to>=0 && to<count) || (to<0))) {			
		if (!isdata(line)) {
			break;
		}
		try {
			std::vector<std::string> string_values = string_util::split_values(line);
	//cout << endl << "pushing data: " << (string_util::str_to_float(string_values.at(index))) << endl;	
			double dbl_val = string_util::str_to_float(string_values.at(index));
			values.push_back(dbl_val);
		}
		catch(...) {
			cout << "exception in transforming column values " << endl;	
					
		}
		count++;
		line = read_line(infile);
	}*/
	return values;
}

std::string Frm2Retriever::extract_data(std::ifstream &file, int entry_num, unsigned int nxtype, unsigned int lineno, unsigned int word, unsigned int wcount)
{
	int cur_line=0;
	std::string result="";

	std::string line;

	reset_file(infile);
	skip_to_line(infile, cur_line, data_section);

	line = read_line(infile);
	std::vector<std::string> words = string_util::split_whitespace(line);
	int nitems = 1;
	int nlines = 0;
	if (isNumber(words.at(7))) {
		nitems =string_util::str_to_int(words.at(7));
		nlines = (int)ceil(((double)(nitems*2))/Frm2Retriever::HEIDI_COLS_PER_LINE);
	}
	//std::cout << "nlines: " << nlines << std::endl;
	if (entry_num > 0) {
		for (unsigned int i=0; i<((entry_num*(nlines+2))+lineno)-1; i++) {
			read_line(infile);
		}
		line = read_line(infile);
	}
	//std::cout << "XD line: " << line << "lineno: " << lineno<< std::endl;
	words = string_util::split_whitespace(line);
	/*for (unsigned int i =0; i< words.size(); i++) {
		std::cout << "words["<<i<<"]: " << words[i] << std::endl;
	}*/
	//std::cout << "XD words: " << word << " wcount: " << wcount <<std::endl;
	for (int j=word; j<word+wcount;j++) {
		result.append(words[j]).append(" ");
		//std::cout << "XD appending: " <<  words[j] <<std::endl;
	}
	//std::cout << "XD result: " << result<< std::endl;
	return result;
}


std::string Frm2Retriever::extract_heidiids(std::ifstream &file, int entry_num, unsigned int nxtype, unsigned int lineno, unsigned int word)
{
	int cur_line=0;
	std::string result="";

	std::string line;

	reset_file(infile);
	skip_to_line(infile, cur_line, data_section);

	line = read_line(infile);
	std::vector<std::string> words = string_util::split_whitespace(line);
	int nitems = 1;
	int nlines = 0;
	if (isNumber(words.at(7))) {
		nitems =string_util::str_to_int(words.at(7));
		nlines = (int)ceil(((double)(nitems*2))/Frm2Retriever::HEIDI_COLS_PER_LINE);
	}
	//std::cout << "nlines: " << nlines << std::endl;
	if (entry_num > 0) {
		for (unsigned int i=0; i<((entry_num*(nlines+2))+lineno)-1; i++) {
			read_line(infile);
		}
		line = read_line(infile);
	}
	//std::cout << "XD line: " << line << "lineno: " << lineno<< std::endl;
	words = string_util::split_whitespace(line);
	/*for (unsigned int i =0; i< words.size(); i++) {
		std::cout << "words["<<i<<"]: " << words[i] << std::endl;
	}*/
	//std::cout << "XD words: " << word << " wcount: " << wcount <<std::endl;
	
	result.append(std::string(1,words[word][0])).append(" ");
	result.append(std::string(1,words[word][1]));
		//std::cout << "XD appending: " <<  words[j] <<std::endl;
	//std::cout << "XD result: " << result<< std::endl;
	return result;
}


std::string Frm2Retriever::extract_heidi_omg(std::ifstream &file, int entry_num, unsigned int nxtype, unsigned int word, unsigned int wcount)
{	
	int ncts=0;
	double domg=0.0;
	std::string result="";
	std::string line;
	
   line =  extract_data(file, entry_num, nxtype, 0, 7, 1);
	if (isNumber(line)) {
		strip(line);
		ncts = string_util::str_to_int(line);
	}
	line = extract_header(file, "Scan", nxtype, 0, 1);
	domg = string_util::str_to_float(line); 
	
	for (int i=0; i<ncts; i++) {
   	std::stringstream ss;
		ss << domg*((double)(i+1));
		printf("ss.str(): %s\n", ss.str().c_str());
		result.append(ss.str());
		if (i<(ncts-1)) {
			result.append(" ");
		}
	}
	return result;
}


int Frm2Retriever::number_of_columns(std::string header_line) {
	std::vector<std::string> entrys = string_util::split_whitespace(header_line);
	entrys = string_util::strip_punct(entrys);
	return entrys.size();
}

























/**
 * The factory will call the constructor with a string. The string
 * specifies where to locate the data (e.g. a filename), but
 * interpreting the string is left up to the implementing code.
 */
 
Frm2Retriever::Frm2Retriever(const string &str): source(str),current_line(0){
  //cout << "Frm2Retriever(" << source << ")" << endl; // REMOVE

  // open the file
  infile.open(source.c_str());

  // check that open was successful
  if(! infile.is_open()) {
    //throw invalid_argument("Could not open file: "+source);
	 return;
  }
	
	initUnits();

	this->number_of_cols = 0;
	this->number_of_entrys = 0;
	
	int cur_line = 0;
	std::string line; /* = read_line(infile);*/
	std::string prev1_line="";
	std::string prev2_line="";
	std::string prev3_line="";
	std::string prev4_line="";
	
 	//std::cout << "current line: " << line << std::endl;
	while (infile.good())  {
		line = read_line(infile);
		//std::cout << "current line: " << line << std::endl;
		//std::cout << "prev1_line: " << prev1_line << std::endl;
		//std::cout << "prev2_line: " << prev2_line << std::endl;
		if(isdata(line) && isdata(prev1_line) && isdata(prev2_line)) {
			break;
		}
		prev4_line = prev3_line;
		prev3_line = prev2_line;
		prev2_line = prev1_line;
		prev1_line=line;
		cur_line++;
	}

	//std::cout << "data line: " << line << "  isdata?: " << !isdata(line) << "   isfileok?: " << infile.good()<< std::endl;
	header_section = cur_line-3;
	// this is dangerous as it assumes the unit line to be present and always under the header line
	// alternative: check if in header line is some occurence of unit string -> treat is as unit line
	if (isunit(prev3_line)) {
//std::cout << "extracting units from: " << prev_line << std::endl;
		extract_units(prev3_line);
//std::cout << "extracting headers from: " << prev_prev_line << std::endl;
		extract_headers(prev4_line);
	}
	else {
		extract_headers(prev3_line);
	}

	data_section = cur_line-2;
	// check if we have a tof file ...
	reset_file(infile);
	cur_line=0;
	while (infile.good()) {
		line = read_line(infile);
		int pos = line.find("aDetInfo");
		if (pos != std::string::npos) {
			//std::cout << "seems that we got a tof file" << std::endl;
			data_section = cur_line+3;
			read_line(infile);
			extract_headers(read_line(infile));
			break;
		}
		cur_line++;
	}

	
	// check if we have a heidi file ...
	reset_file(infile);
	cur_line=0;
	std::string lastheader = "Omat";  	
	while (infile.good()) {
		line = read_line(infile);
		if (string_util::contains(line, lastheader)) {
			data_section = cur_line+3;
			break;
		}
		cur_line++;
	}

	//std::cout << "data_section: " << data_section << std::endl;

	// check if we have a tof continous log file ...
	reset_file(infile);
	cur_line=0;
	int pos = str.find("_");
	if (pos!=std::string::npos) {
		if (str[pos+1] == '5') {
			if (str[pos+2] == '0') {
				//std::cout << "seems that we got a tof log file " << data_section << std::endl;
				extract_toflogheaders(infile);
			}
		}
	}
	reset_file(infile);
	
	
	/*for (std::vector<std::string>::iterator it=headers.begin(); it!= headers.end(); it++) {
		std::cout << "headers: " << *it << std::endl;
	}*/
	
	/*int num_of_cols, num_of_cols2, num_of_cols3;
	//std::cout << "compare data line 1: " << line << std::endl;
	num_of_cols = number_of_columns(line);
	if (isdata(line)) {
		line = read_line(infile);
	//std::cout << "compare data line 2: " << line << std::endl;
		num_of_cols2 = number_of_columns(line);
		if (isdata(line)) {
			line = read_line(infile);
	//std::cout << "compare data line 3: " << line << std::endl;
			num_of_cols3 = number_of_columns(line);
		}
	}
	if ((num_of_cols == num_of_cols2) && (num_of_cols == num_of_cols3)) {
		number_of_cols = num_of_cols;
		number_of_headers = std::min(number_of_cols, number_of_headers); 
		number_of_cols = std::min(number_of_cols, number_of_headers);
	}
	else {
  		//cout << "numbers of cols: " << num_of_cols <<" "<< num_of_cols2 <<" "<< num_of_cols3 << endl; 
		printf("ERROR: number of columns must remain constant. check your ASCII file\n");
	}*/

}


Frm2Retriever::~Frm2Retriever(){
  //cout << "~Frm2Retriever()" << endl;

  // close the file
  if(infile)
    infile.close();
}




Node* Frm2Retriever::createEmptyNode(std::string nodename, unsigned int nxtype) {
	/*int* empty_dims = new int[1];
	empty_dims[0] = 1;
	void *data;
	
	if(NXmalloc(&data, 1, empty_dims, nxtype)!=NX_OK) {
		throw runtime_error("NXmalloc failed");
	}
	*((int*)data)=0;
	return (new Node(nodename, data, 1, empty_dims, nxtype));*/
	return NULL;
}


Node* Frm2Retriever::createNode(std::string nodename, std::vector<unsigned int> values, unsigned int nxtype) {
	int nxrank = 1;
	int* nxdims = new int[1];
	nxdims[0] = values.size();

	// allocate space for the data
	void *data;
	if(NXmalloc(&data, nxrank, nxdims, nxtype)!=NX_OK) {
		throw runtime_error("NXmalloc failed");
	}

	for( unsigned int i=0 ; i<values.size() ; i++ ) {
		switch (nxtype) {
			case NX_INT32: 
				*(((int*)data)+i)=static_cast<int>(values.at(i));
				break;
			case NX_UINT32: 
				*(((unsigned int*)data)+i)=static_cast<unsigned int>(values.at(i));
				break;
			case NX_INT16: 
				*(((short*)data)+i)=static_cast<short>(values.at(i));
				break;
			case NX_UINT16: 
				*(((unsigned short*)data)+i)=static_cast<unsigned short>(values.at(i));
				break;
			case NX_INT8: 
				*(((char*)data)+i)=static_cast<char>(values.at(i));
				break;
			case NX_UINT8: 
				*(((unsigned char*)data)+i)=static_cast<unsigned char>(values.at(i));
				break;
		}	
	}
	return (new Node("empty", data, nxrank, nxdims, nxtype));
	
}

Node* Frm2Retriever::createNode(std::string nodename, std::vector<double> values, unsigned int nxtype, std::string units, std::vector<unsigned int> dims) {
	int nxrank = 1;
	int* nxdims = new int[1];
	nxdims[0] = values.size();


	if (dims.size()>0) {
		nxrank = dims.size();
		nxdims = new int[nxrank];
		for (unsigned int i=0; i<dims.size();i++) {
			nxdims[i] = dims.at(i);  
		}
	}

	// allocate space for the data
	void *data;
	if(NXmalloc(&data, nxrank, nxdims, nxtype)!=NX_OK) {
		throw runtime_error("NXmalloc failed");
	}

	for( unsigned int i=0 ; i<values.size() ; i++ ) {
		switch (nxtype) {
			case NX_INT32: 
				*(((int*)data)+i)=static_cast<int>(values.at(i));
				break;
			case NX_UINT32: 
				*(((unsigned int*)data)+i)=static_cast<unsigned int>(values.at(i));
				break;
			case NX_INT16: 
				*(((short*)data)+i)=static_cast<short>(values.at(i));
				break;
			case NX_UINT16: 
				*(((unsigned short*)data)+i)=static_cast<unsigned short>(values.at(i));
				break;
			case NX_INT8: 
				*(((char*)data)+i)=static_cast<char>(values.at(i));
				break;
			case NX_UINT8: 
				*(((unsigned char*)data)+i)=static_cast<unsigned char>(values.at(i));
				break;
			case NX_FLOAT64:
				*(((double*)data)+i)=static_cast<double>(values.at(i));
				break;
			case NX_FLOAT32:
				*(((float*)data)+i)=static_cast<float>(values.at(i));
				break;
		}	
	}
	Node* node = new Node("empty", data, nxrank, nxdims, nxtype);
	if (units!="" && nxtype != NX_CHAR) {
		std::vector<Attr> attrs;
		Attr attr("units", units.c_str(), units.size(), NX_CHAR);
		attrs.push_back(attr);
		node->update_attrs(attrs);
	}
	return node;
}


Node* Frm2Retriever::createNode(std::string nodename, std::string value, unsigned int nxtype, std::string units) {
	int nxrank = 1;
	int* nxdims = new int[1];
	if (nxtype == NX_CHAR || nxtype==NX_BINARY) {
		nxdims[0] = value.size();
	}
	else {
		nxdims[0] = 1;
	}

	// allocate space for the data
	void *data;
	if(NXmalloc(&data, nxrank, nxdims, nxtype)!=NX_OK) {
		throw runtime_error("NXmalloc failed");
	}
	
	switch (nxtype) {
		case NX_CHAR:
			data = (char*)value.c_str();
			//nxdims[0] = value.size();
			break;
		case NX_INT32: 
			*((int*)data) = static_cast<int>(string_util::str_to_int(value.c_str()));
			break;
		case NX_UINT32: 
			*((unsigned int*)data) = static_cast<unsigned int>(string_util::str_to_int(value.c_str()));
			break;
		case NX_INT16: 
			*((short*)data) = static_cast<short>(string_util::str_to_int(value.c_str()));
			break;
		case NX_UINT16: 
			*((unsigned short*)data) = static_cast<unsigned short>(string_util::str_to_int(value.c_str()));
			break;
		case NX_INT8: 
			*((char*)data) = static_cast<char>(string_util::str_to_int(value.c_str()));
			break;
		case NX_UINT8:
                        data = const_cast<char*>(value.c_str());
			//nxdims[0] = value.size();
			//*((unsigned char*)data) = static_cast<unsigned char>(string_util::str_to_int(value.c_str()));
			break;
		case NX_FLOAT64:
			*((double*)data) = static_cast<double>(string_util::str_to_float(value.c_str()));
			break;
		case NX_FLOAT32:
			*((float*)data) = static_cast<float>(string_util::str_to_float(value.c_str()));
			break;
	}

	// create a data node
	Node* node = new Node("empty", data, nxrank, nxdims, nxtype);
	if (units!="" && nxtype != NX_CHAR) {
		std::vector<Attr> attrs;
		Attr attr("units", units.c_str(), units.size(), NX_CHAR);
		attrs.push_back(attr);
		node->update_attrs(attrs);
	}
	return node;
}

Node* Frm2Retriever::createNode(std::string nodename, std::vector<std::vector<double> >values, unsigned int nxtype) {
	int nxrank = 2;
	int* nxdims = new int[2];
	nxdims[0] = values.size();
	nxdims[1] = values.at(0).size();

	// allocate space for the data
	void *data;
	if(NXmalloc(&data, nxrank, nxdims, nxtype)!=NX_OK) {
		throw runtime_error("NXmalloc failed");
	}

	for( unsigned int i=0 ; i<values.size() ; i++ ) {
		for( unsigned int j=0 ; j<values.at(i).size() ; j++ ) {
			
			switch (nxtype) {
				case NX_INT32: 
					*(((int*)data)+(i*nxdims[1]+j))=static_cast<int>(values.at(i).at(j));
					break;
				case NX_UINT32: 
					*(((unsigned int*)data)+(i*nxdims[1]+j))=static_cast<unsigned int>(values.at(i).at(j));
					break;
				case NX_INT16: 
					*(((short*)data)+(i*nxdims[1]+j))=static_cast<short>(values.at(i).at(j));
					break;
				case NX_UINT16: 
					*(((unsigned short*)data)+(i*nxdims[1]+j))=static_cast<unsigned short>(values.at(i).at(j));
					break;
				case NX_INT8: 
					*(((char*)data)+(i*nxdims[1]+j))=static_cast<char>(values.at(i).at(j));
					break;
				case NX_UINT8: 
					*(((unsigned char*)data)+(i*nxdims[1]+j))=static_cast<unsigned char>(values.at(i).at(j));
					break;
				case NX_FLOAT64:
					*(((double*)data)+(i*nxdims[1]+j))=static_cast<double>(values.at(i).at(j));
					break;
				case NX_FLOAT32:
					*(((float*)data)+(i*nxdims[1]+j))=static_cast<float>(values.at(i).at(j));
					break;
			}	
		}
	}
	return (new Node("empty", data, nxrank, nxdims, nxtype));
}
Node* Frm2Retriever::createNode(std::string nodename, std::vector<std::vector<unsigned int> >values, unsigned int nxtype) {
	int nxrank = 2;
	int* nxdims = new int[2];
	nxdims[0] = values.size();
	nxdims[1] = values.at(0).size();

	// allocate space for the data
	void *data;
	if(NXmalloc(&data, nxrank, nxdims, nxtype)!=NX_OK) {
		throw runtime_error("NXmalloc failed");
	}

	for( unsigned int i=0 ; i<values.size() ; i++ ) {
		for( unsigned int j=0 ; j<values.at(i).size() ; j++ ) {
			
			switch (nxtype) {
				case NX_INT32: 
					*(((int*)data)+(i*nxdims[1]+j))=static_cast<int>(values.at(i).at(j));
					break;
				case NX_UINT32: 
					*(((unsigned int*)data)+(i*nxdims[1]+j))=static_cast<unsigned int>(values.at(i).at(j));
					break;
				case NX_INT16: 
					*(((short*)data)+(i*nxdims[1]+j))=static_cast<short>(values.at(i).at(j));
					break;
				case NX_UINT16: 
					*(((unsigned short*)data)+(i*nxdims[1]+j))=static_cast<unsigned short>(values.at(i).at(j));
					break;
				case NX_INT8: 
					*(((char*)data)+(i*nxdims[1]+j))=static_cast<char>(values.at(i).at(j));
					break;
				case NX_UINT8: 
					*(((unsigned char*)data)+(i*nxdims[1]+j))=static_cast<unsigned char>(values.at(i).at(j));
					break;
			}	
		}
	}
	return (new Node("empty", data, nxrank, nxdims, nxtype));
}


Node* Frm2Retriever::createNode(std::string nodename, std::vector<std::string> values, unsigned int nxtype, std::string units) {
	int nxrank = 1;
	int* nxdims = new int[1];
	nxdims[0] = values.size();
	
	std::string cvalues="";
	if (nxtype== NX_CHAR) {
		for( unsigned int i=0 ; i<values.size() ; i++ ) {
			cvalues.append(values.at(i));
			if (i!= values.size()-1) {
				cvalues.append(",");
			}
		}
	}
	// allocate space for the data
	void *data;
	if(NXmalloc(&data, nxrank, nxdims, nxtype)!=NX_OK) {
		throw runtime_error("NXmalloc failed");
	}
	
	for( unsigned int i=0 ; i<values.size() ; i++ ) {
		switch (nxtype) {
			case NX_CHAR:
				data = (char*)cvalues.c_str();
				nxdims[0] = cvalues.size();
				break;
			case NX_INT32: 
				*(((int*)data)+i) = static_cast<int>(string_util::str_to_int(values.at(i)));
				break;
			case NX_UINT32: 
				*(((unsigned int*)data)+i) = static_cast<unsigned int>(string_util::str_to_int(values.at(i)));
				break;
			case NX_INT16: 
				*(((short*)data)+i) = static_cast<short>(string_util::str_to_int(values.at(i)));
				break;
			case NX_UINT16: 
				*(((unsigned short*)data)+i) = static_cast<unsigned short>(string_util::str_to_int(values.at(i)));
				break;
			case NX_INT8: 
				*(((char*)data)+i) = static_cast<char>(string_util::str_to_int(values.at(i)));
				break;
			case NX_UINT8: 
				*(((unsigned char*)data)+i) = static_cast<unsigned char>(string_util::str_to_int(values.at(i)));
				break;
			case NX_FLOAT32:
				{
				float ft = static_cast<float>(string_util::str_to_float(values.at(i)));
				//std::cout << std::setprecision(9);
				//std::cout << "string: "<< values.at(i) << "logged float: " << string_util::str_to_float(values.at(i)) << "  " << ft << std::endl;
				*(((float*)data)+i) = static_cast<float>(string_util::str_to_float(values.at(i)));
				}
				break;
			case NX_FLOAT64:
				*(((double*)data)+i) = static_cast<double>(string_util::str_to_float(values.at(i)));
				break;
		}
	}

	// create a data node
	Node* node = new Node("empty", data, nxrank, nxdims, nxtype);
	if (units!="" && nxtype != NX_CHAR) {
		std::vector<Attr> attrs;
		Attr attr("units", units.c_str(), units.size(), NX_CHAR);
		attrs.push_back(attr);
		node->update_attrs(attrs);
	}
	return node;
}


/**
 * This is the method for retrieving data from a file. The whole
 * tree will be written to the new file immediately after being
 * called. Interpreting the string is left up to the implementing
 * code.
 */
void Frm2Retriever::getData(const string &location, tree<Node> &tr){
	cout << "Frm2Retriever::getData(" << location << ",tree)" << endl; // REMOVE
	// check that the argument is not an empty string
	//printf("extracting...%s", location.c_str()); 
	if(!infile) {
		//std::cout << "infile not valid returning: " << infile << std::endl;
		return;
	}
	if(location.size()<=0) {
		throw invalid_argument("cannot parse empty string");
	}

	// so far ... all locations must refer to a column name 
	std::string arg = "";
	std::string method 	= parse_method(location);
	std::vector<std::string> args	= parse_arg(location);
	
	std::string nxtype;
	//nxtype 	= parse_type(location);

	std::vector<unsigned int> vdims;
	vdims = parse_type_dims(location, nxtype);
	
	if (args.size() ==1 ) {
		arg = args[0];
	}
	if (nxtype == "") {
		// default type is NX_FLOAT64
		nxtype = "NX_FLOAT64";
	}
	
	// get range bounds if specified 
	int from, to;
	parse_range(location, from, to); 

	//start: this is heidi only stuff
	int line, word, wcount;
	parse_heidi_range(location, line, word, wcount); 
	wcount++; 	
	
	int heidi_entry_num = 0;
	if (args.size()>0) {
		if (isNumber(args.at(0))) {
			heidi_entry_num = string_util::str_to_int(args.at(0));
		}
	}
	//end: this is heidi only stuff
	
	//cout << "location: " << method << "(\'" << arg << "\')" << "[" << from << "," << to << "]{" << nxtype <<"}" << "[" << word << "," << wcount << "]" << endl << endl;	
 	Node* node;
	
	if (method == Frm2Retriever::COLUMN_TAG) { 
		std::vector<std::string> values = extract_column(infile, arg, from, to);
  		if (values.size() <= 0) {
			node = createEmptyNode(arg, convert_type(nxtype));
		}
		else {
			int unit_pos=0;
			std::string unit="";
			std::vector<std::string>::iterator it = headers.begin();
			while (*it != arg && it != headers.end()) {
				it++;
				unit_pos++;
			}
			if (it!= headers.end()) {
				if (units.size()>unit_pos) {
					if (unit_strings.find(string_util::lower_str(units.at(unit_pos)))!=unit_strings.end()) {
						unit = unit_strings[units.at(unit_pos)];
					}
					else {
						unit = units.at(unit_pos);
					}
				}
			}
			//std::cout << endl << "pushing data: " << (string_util::str_to_float(string_values.at(index))) << std::endl;	
			//double dbl_val = string_util::str_to_float(string_values.at(index));
			//values.push_back(dbl_val);
			
			/*for (std::vector<std::string>::iterator itt=values.begin(); itt!=values.end(); itt++) {
				std::cout << "column------- vector: " << *itt << std::endl;
			}*/
			node = createNode("empty", values, convert_type(nxtype), unit);
			
		}
	}
	else if (method == Frm2Retriever::DICT_TAG) { 
		std::map<std::string, std::string> raw_map = extract_dictentry(infile, arg, convert_type(nxtype));
		std::string raw_string=raw_map["values"];
		std::string units=raw_map["units"];
		std::string description=raw_map["description"];
		
		if (unit_strings.find(string_util::lower_str(units))!=unit_strings.end()) {
			units = unit_strings[units];
		}
  		if (raw_string.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			if (convert_type(nxtype) == NX_CHAR) {
				if (nxtype == "ISO8601") {
					// check if its in heidi form
					if (count_chars(raw_string, '.') > 1) {
						raw_string = toftof_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '/') > 1) {
						raw_string = nicos_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '-') > 1) {
						raw_string = heidi_datetime_2_iso(raw_string);
					}
				}
				node = createNode("empty", raw_string, convert_type(nxtype), units);
			}
			else {
				std::vector<double> values = string_util::split_doubles(raw_string);	
				node = createNode("empty", values, convert_type(nxtype), units);
			}
			//cout << "dict value: '" << entry << "'"<<std::endl;
			//node = createNode("empty", raw_string, convert_type(nxtype), units);
			if (description.size() > 0) {	
				std::vector<Attr> attrs;
				Attr attr("description", description.c_str(), description.size(), NX_CHAR);
				attrs.push_back(attr);
				node->update_attrs(attrs);
			}
		}
	}
	else if (method == Frm2Retriever::FLINE_TAG) { 
		std::string raw_string = extract_line_below(infile, arg, convert_type(nxtype));
  		if (raw_string.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			if (convert_type(nxtype) == NX_CHAR) {
				if (nxtype == "ISO8601") {
					// check if its in heidi form
					if (count_chars(raw_string, '.') > 1) {
						raw_string = toftof_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '/') > 1) {
						raw_string = nicos_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '-') > 1) {
						raw_string = heidi_datetime_2_iso(raw_string);
					}
				}
				node = createNode("empty", raw_string, convert_type(nxtype));
			}
			else {
				std::vector<double> values = string_util::split_doubles(raw_string);	
				node = createNode("empty", values, convert_type(nxtype));
			}
		}
	}
	else if (method == Frm2Retriever::DATETIME_TAG) { 
		std::string entry="";
		std::string units="";
		std::string raw_string = extract_datetime(infile, args, convert_type(nxtype));
 		//cout << "dict value: '" << raw_string << "'"<<std::endl;  
  		if (raw_string.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			unsigned int pos = raw_string.find("value:");
			if (pos != std::string::npos) {
				entry = raw_string.substr(pos+6);
			}
			pos = raw_string.find("units:");
			if (pos != std::string::npos) {
				units = raw_string.substr(pos+6);
				unsigned int pos2 = units.find(";", pos);
				if (pos2 != std::string::npos) {
					units = units.substr(0, pos2);
					//std::cout << " units extracted: " << units <<  std::endl;
				}
			}
			else {
				entry = raw_string;
			}
			node = createNode("empty", entry, convert_type(nxtype), units);
		}
	}
	else if (method == Frm2Retriever::DICTMULTI_TAG) { 
		std::map<std::string, std::string> raw_map = extract_dictmulti(infile, args, convert_type(nxtype));
		std::string raw_string = raw_map["values"];
		std::string units = raw_map["units"];
		
		if (unit_strings.find(string_util::lower_str(units))!=unit_strings.end()) {
			units = unit_strings[units];
		}
 		//cout << "dict value: '" << raw_string << "'"<<std::endl; 
		while (isspace(raw_string[0])) {
			raw_string = raw_string.substr(1);
		} 
  		if (raw_string.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			std::vector<std::string> vals = string_util::split_values(raw_string);
			/*for (std::vector<std::string>::iterator it=vals.begin(); it!= vals.end();it++) {
				std::cout << "vals: " << *it << std::endl;
			}*/
			node = createNode("empty", string_util::split_values(raw_string), convert_type(nxtype), units);
		}
	}
	else if (method == Frm2Retriever::DICTARRAY_TAG) { 
 		//cout << "calling dict_array '" <<  std::endl;  
		std::map<std::string, std::string> raw_map = extract_dictarray(infile, arg, convert_type(nxtype));
		std::string raw_string=raw_map["values"];
		std::string units=raw_map["units"];
		
		if (unit_strings.find(string_util::lower_str(units))!=unit_strings.end()) {
			units = unit_strings[units];
		}
 		//cout << "dict value: '" << raw_string << "'"<<std::endl;  
  		if (raw_string.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			node = createNode("empty", string_util::split_values(raw_string), convert_type(nxtype), units);
		}
	}
	else if (method == Frm2Retriever::TOFCTS_TAG) {
		std::vector<std::string> values;
		std::vector<std::vector<unsigned int> > ivalues= extract_tofcts(infile, arg, convert_type(nxtype), false);
  		if (ivalues.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			node = createNode("empty", ivalues, convert_type(nxtype));
		}
	}
	else if (method == Frm2Retriever::TOFMONCTS_TAG) {
		std::vector<std::string> values;
		std::vector<std::vector<unsigned int> > ivalues= extract_tofcts(infile, arg, convert_type(nxtype), true);
  		if (ivalues.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			node = createNode("empty", ivalues.at(0), convert_type(nxtype));
		}
	}
	else if (method == Frm2Retriever::TOFTOF_TAG) { 
		std::vector<double> dvalues= extract_toftof(infile, arg, convert_type(nxtype), false);
  		if (dvalues.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			node = createNode("empty", dvalues, convert_type(nxtype));
		}
	}
	else if (method == Frm2Retriever::TOFMONTOF_TAG) { 
		std::vector<double> dvalues= extract_toftof(infile, arg, convert_type(nxtype), true);
  		if (dvalues.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			node = createNode("empty", dvalues, convert_type(nxtype));
		}
	}
	else if (method == Frm2Retriever::TOFDNR_TAG) { 
		std::vector<unsigned int> ivalues= extract_dnr(infile, arg, convert_type(nxtype));
  		if (ivalues.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			node = createNode("empty", ivalues, convert_type(nxtype));
		}
	}
	else if (method == Frm2Retriever::DESC_TAG) { 
		std::vector<unsigned int> ivalues= extract_desc(infile, arg, convert_type(nxtype));
  		if (ivalues.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			node = createNode("empty", ivalues, convert_type(nxtype));
		}
	}
	else if (method == Frm2Retriever::FILENAME_TAG) {
		std::string entry = extract_filename(infile, source, arg,convert_type(nxtype));
		node = createNode("empty", entry, convert_type(nxtype));
	}
	else if (method == Frm2Retriever::TOFLOG_TAG) {
		std::vector<std::string> logs = extract_toflog(infile, arg);
  		if (logs.size() <= 0) {
			node = createEmptyNode("", convert_type(nxtype));
		}
		else {
			node = createNode("empty", logs, convert_type(nxtype));
			if (arg =="nicd_time") {
				std::map<std::string, std::string> raw_map = extract_dictentry(infile, "# File_Creation_Time", NX_CHAR);
				std::string tstr=raw_map["values"];
				tstr = toflog_datetime_2_iso(tstr);
				
				std::vector<Attr> attrs;
				Attr attr("start", tstr.c_str(), tstr.size(), NX_CHAR);
				attrs.push_back(attr);
				node->update_attrs(attrs);
			}
		}
		
	}
	else if (method == Frm2Retriever::LOGCPY_TAG) {
		std::string entry = extract_logcpy(infile, source);
		node = createNode("empty", entry, convert_type(nxtype));
	}
	else if (method == Frm2Retriever::HEIDICTS_TAG || method == Frm2Retriever::HEIDIMON_TAG) {
		std::cout << "creating heidi mon cts node now 0" << std::endl;
		std::vector<double> values = extract_heidi_counts(infile, heidi_entry_num, (method==Frm2Retriever::HEIDIMON_TAG));
		//std::cout << "creating heidi mon cts node now 1" << std::endl;
  		if (values.size() <= 0) {
			node = createEmptyNode(arg, convert_type(nxtype));
		}
		else {
			//std::cout << "creating heidi mon cts node now 2"  << std::endl;
			node = createNode("empty", values, convert_type(nxtype));
		}
	}
	else if (method == Frm2Retriever::HEADER_TAG) {
		std::cout << "getting header data now" << std::endl;
		std::string raw_string = extract_header(infile, arg, convert_type(nxtype), word, wcount);
		std::cout << "extracted header string: '" << raw_string << "'" << std::endl;
		std::vector<double> values;
  		if (raw_string.size() <= 0) {
			node = createEmptyNode(arg, convert_type(nxtype));
		}
		else {
			unsigned int pos = raw_string.find("value:");
			if (pos != std::string::npos) {
				raw_string = raw_string.substr(pos+6);
			}
			if (convert_type(nxtype) == NX_CHAR) {
				if (nxtype == "ISO8601") {
					// check if its in heidi form
					if (count_chars(raw_string, '.') > 1) {
						raw_string = toftof_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '/') > 1) {
						raw_string = nicos_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '-') > 1) {
						raw_string = heidi_datetime_2_iso(raw_string);
					}
				}
				node = createNode("empty", raw_string, convert_type(nxtype));
			}
			else {
				values = string_util::split_doubles(raw_string);	
				node = createNode("empty", values, convert_type(nxtype), "", vdims);
			}
		}
	}
	else if (method == Frm2Retriever::DATA_TAG) {
		std::cout << "extracting data now... '"  << "'" << std::endl;
		std::string raw_string = extract_data(infile, heidi_entry_num, convert_type(nxtype), line, word, wcount);
		std::cout << "extracted data string: '" << raw_string << "'" << std::endl;
		std::vector<double> values;
  		if (raw_string.size() <= 0) {
			node = createEmptyNode(arg, convert_type(nxtype));
		}
		else {
			unsigned int pos = raw_string.find("value:");
			if (pos != std::string::npos) {
				raw_string = raw_string.substr(pos+6);
			}
			if (convert_type(nxtype) == NX_CHAR) {
				std::cout << "bingo ... got a char ..." << std::endl;
				if (nxtype == "ISO8601") {
					std::cout << "bingo ... got a time value ..." << std::endl;
					// check if its in heidi form
					if (count_chars(raw_string, '.') > 1) {
						raw_string = toftof_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '/') > 1) {
						std::cout << "bingo ... convert ..." << std::endl;
						raw_string = nicos_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '-') > 1) {
						raw_string = heidi_datetime_2_iso(raw_string);
					}
				}
				node = createNode("empty", raw_string, convert_type(nxtype));
			}
			else {
				values = string_util::split_doubles(raw_string);	
				node = createNode("empty", values, convert_type(nxtype), "", vdims);
			}
		}
	}
	else if (method == Frm2Retriever::HEIDIIDS_TAG) {
		std::cout << "extracting ids now... '"  << "'" << std::endl;
		std::string raw_string = extract_heidiids(infile, heidi_entry_num, convert_type(nxtype), line, word);
		std::cout << "extracted ids string: '" << raw_string << "'" << std::endl;
		std::vector<double> values;
  		if (raw_string.size() <= 0) {
			node = createEmptyNode(arg, convert_type(nxtype));
		}
		else {
			unsigned int pos = raw_string.find("value:");
			if (pos != std::string::npos) {
				raw_string = raw_string.substr(pos+6);
			}
			if (convert_type(nxtype) == NX_CHAR) {
				std::cout << "bingo ... got a char ..." << std::endl;
				if (nxtype == "ISO8601") {
					std::cout << "bingo ... got a time value ..." << std::endl;
					// check if its in heidi form
					if (count_chars(raw_string, '.') > 1) {
						raw_string = toftof_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '/') > 1) {
						std::cout << "bingo ... convert ..." << std::endl;
						raw_string = nicos_datetime_2_iso(raw_string);
					}
					else if (count_chars(raw_string, '-') > 1) {
						raw_string = heidi_datetime_2_iso(raw_string);
					}
				}
				node = createNode("empty", raw_string, convert_type(nxtype));
			}
			else {
				values = string_util::split_doubles(raw_string);	
				node = createNode("empty", values, convert_type(nxtype), "", vdims);
			}
		}
	}
	else if (method == Frm2Retriever::HEIDISO_TAG) {
		std::cout << "getting heidiso data now" << std::endl;
		std::vector<double> values = extract_heidi_so(infile, heidi_entry_num, convert_type(nxtype), word, wcount);
		for (int ii=0; ii< values.size(); ii++) {
			printf("values[%d]:%f\n", ii, values.at(ii));
		}
  		if (values.size() <= 0) {
			node = createEmptyNode(arg, convert_type(nxtype));
		}
		else {
			// remove first value (2theta)
			values.erase(values.begin());
			node = createNode("empty", values, convert_type(nxtype), "", vdims);
		}
	}
	else if (method == Frm2Retriever::HEIDIOMG_TAG) {
		std::string raw_string = extract_heidi_omg(infile, heidi_entry_num, convert_type(nxtype), word, wcount);
		std::vector<double> values;
  		if (raw_string.size() <= 0) {
			node = createEmptyNode(arg, convert_type(nxtype));
		}
		else {
			unsigned int pos = raw_string.find("value:");
			if (pos != std::string::npos) {
				raw_string = raw_string.substr(pos+6);
			}
			values = string_util::split_doubles(raw_string);	
			node = createNode("empty", values, convert_type(nxtype), "", vdims);
		}
	}
	else {
		// unknown method
		node = createEmptyNode("", NX_INT32);
		node = NULL;
	}
	if (node != NULL) {
		tr.insert(tr.begin(),*node);
	}
}



std::string Frm2Retriever::month_strtonum(std::string &monthstr) {
	std::string result="Jan";
	
	if (monthstr == "Jan") {
		result = "01";	
	}
	else if (monthstr == "Feb") {
		result = "02";	
	}
	else if (monthstr=="Mar") {
		result = "03";	
	}
	else if (monthstr=="Apr") {
		result = "04";	
	}
	else if (monthstr=="May" || monthstr=="Mai") {
		result = "05";	
	}
	else if (monthstr=="Jun") {
		result = "06";	
	}
	else if (monthstr=="Jul") {
		result = "07";	
	}
	else if (monthstr=="Aug") {
		result = "08";	
	}
	else if (monthstr=="Sep") {
		result = "09";	
	}
	else if (monthstr=="Oct" || monthstr=="Oct") {
		result = "10";	
	}
	else if (monthstr=="Nov") {
		result = "11";	
	}
	else if (monthstr=="Dec" || monthstr=="Dez") {
		result = "12";	
	}

	return result;
}

std::string Frm2Retriever::toflog_datetime_2_iso(std::string &datetimestr) {
	/* toflog: Mon Aug 29 12:05:12 2005 */
	/* iso: 2005-05-17 03:13:37 */
	unsigned int pos, pos2;
	std::string datestr, timestr;
	std::string day="";
	std::string month="";
	std::string year="";
	
	// eliminate whitespace 
	while (isspace(datetimestr[0])){
		datetimestr = datetimestr.substr(1);
	}
	while (isspace(datetimestr[datetimestr.size()-1])){
		datetimestr = datetimestr.substr(0, datetimestr.size()-1);
	}

	pos = datetimestr.find(" ");
	if (pos != std::string::npos) {
		datetimestr = datetimestr.substr(pos+1);
		pos = datetimestr.find(" ");
		if (pos != std::string::npos) {
			month = datetimestr.substr(0, pos);
			month = month_strtonum(month);
			datetimestr = datetimestr.substr(pos+1);
			
			pos = datetimestr.find(" ");
			if (pos != std::string::npos) {
				day = datetimestr.substr(0, pos);
				datetimestr = datetimestr.substr(pos+1);
				
				pos2 = datetimestr.rfind(" ");  
				if (pos2 != std::string::npos) {
					year = datetimestr.substr(pos2+1);
					timestr = datetimestr.substr(0, pos2);
				}
			}
		}
	}
	//sample together new string
	if (timestr!="" && day!="" && month!="" && year !="") {
		datetimestr = year+"-"+month+"-"+day+" "+timestr;	
	}
	//std::cout << "year: " << year << ", month: " << month << ", day: "<< day << ", datetime:" << datetimestr <<  std::endl;
	return datetimestr;
}


std::string Frm2Retriever::heidi_datetime_2_iso(std::string &datetimestr) {
	/* heidi: 14-Jun-05 15:57 */
	/* iso: 2005-05-17 03:13:37 */
	unsigned int pos, pos2;
	std::string datestr, timestr;
	std::string day="";
	std::string month="";
	std::string year="";
	
	pos = datetimestr.find(" ");
	if (pos != std::string::npos) {
		datestr = datetimestr.substr(0, pos);
		timestr = datetimestr.substr(pos+1);
		
		pos = datestr.find("-");
		if (pos != std::string::npos) {
			day = datestr.substr(0, pos);
			pos2 = datestr.rfind("-");  
			if (pos2 != std::string::npos) {
				year = datestr.substr(pos2+1);
				month = datestr.substr(0,pos2);
				month = month.substr(pos+1);
				month = month_strtonum(month);
				year = string("20").append(year);
			}
		}
	}
	//sample together new string
	if (timestr!="" && day!="" && month!="" && year !="") {
		datetimestr = year+"-"+month+"-"+day+" "+timestr;	
	}
	//std::cout << "year: " << year << ", month: " << month << ", day: "<< day << ", datetime:" << datetimestr <<  std::endl;
	return datetimestr;
}



std::string Frm2Retriever::toftof_datetime_2_iso(std::string &datetimestr) {
	/* toftof: 17.05.2005 03:13:37 */
	/* iso: 2005-05-17 03:13:37 */
	unsigned int pos, pos2;
	std::string datestr, timestr;
	std::string day="";
	std::string month="";
	std::string year="";
	pos = datetimestr.find(" ");
	if (pos != std::string::npos) {
		datestr = datetimestr.substr(0, pos);
		timestr = datetimestr.substr(pos+1);
		
		pos = datestr.find(".");
		if (pos != std::string::npos) {
			day = datestr.substr(0, pos);
			pos2 = datestr.rfind(".");  
			if (pos2 != std::string::npos) {
				year = datestr.substr(pos2+1);
				month = datestr.substr(0, pos2);
				month = month.substr(pos+1);
			}
		}
	}
	//sample together new string
	if (timestr!="" && day!="" && month!="" && year !="") {
		datetimestr = year+"-"+month+"-"+day+" "+timestr;	
	}
	//std::cout << "year: " << year << ", month: " << month << ", day: "<< day << ", datetime:" << datetimestr <<  std::endl;
	return datetimestr;
}

std::string Frm2Retriever::nicos_datetime_2_iso(std::string &datetimestr) {
	/* nicos: 05/17/2005 03:13:37 */
	/* iso: 2005-05-17 03:13:37 */
	unsigned int pos, pos2;
	std::string datestr, timestr;
	std::string day="";
	std::string month="";
	std::string year="";
	pos = datetimestr.find(" ");
	if (pos != std::string::npos) {
		datestr = datetimestr.substr(0, pos);
		timestr = datetimestr.substr(pos+1);
		
		pos = datestr.find("/");
		if (pos != std::string::npos) {
			month = datestr.substr(0, pos);
			pos2 = datestr.rfind("/");  
			if (pos2 != std::string::npos) {
				year = datestr.substr(pos2+1);
				day = datestr.substr(0, pos2);
				day = day.substr(pos+1);
			}
		}
	}
	//sample together new string
	if (timestr!="" && day!="" && month!="" && year !="") {
		datetimestr = year+"-"+month+"-"+day+" "+timestr;	
	}
	//std::cout << "year: " << year << ", month: " << month << ", day: "<< day << ", datetime:" << datetimestr <<  std::endl;
	return datetimestr;
}


std::string Frm2Retriever::toString() const{
  return "["+MIME_TYPE+"] "+source;
}
