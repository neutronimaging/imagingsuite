#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <vector>
#include <cctype>
#include "collist_retriever.h"
#include "../node.h"
#include "../nexus_util.h"
#include "../string_util.h"
#include "../tree.hh"

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

class NXunits {
	public:
	static const std::string milliseconds;	
	static const std::string seconds;		 
	static const std::string degrees;	
	static const std::string counts;	
	// to be completed later
};

static const int BUFFER_SIZE=256;

const std::string NXunits::milliseconds = "milliseconds";
const std::string NXunits::seconds = "seconds";
const std::string NXunits::degrees = "degrees";
const std::string NXunits::counts = "counts";


const std::string TextCollistRetriever::MIME_TYPE = "text/collist";
const std::string TextCollistRetriever::COLUMN_TAG = "column";
const std::string TextCollistRetriever::DICT_TAG = "dict";
const std::string TextCollistRetriever::RANGE_OPEN_BRACKET="[";
const std::string TextCollistRetriever::RANGE_CLOSE_BRACKET="]";
const std::string TextCollistRetriever::RANGE_SEPARATOR=":";
const std::string TextCollistRetriever::METHOD_OPEN_BRACKET="(";
const std::string TextCollistRetriever::METHOD_CLOSE_BRACKET=")";
const std::string TextCollistRetriever::TYPE_OPEN_BRACKET="{";
const std::string TextCollistRetriever::TYPE_CLOSE_BRACKET="}";



static unsigned int convert_type(std::string nxtype) {
	if (nxtype =="NX_CHAR") {
		return NX_CHAR;
	}
	else if (nxtype =="NX_BOOLEAN") {
		return NX_UINT8;
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

static void reset_file(ifstream &file){
	file.clear();
	file.seekg(0,std::ios::beg);
	file.clear();
}

static string read_line(ifstream &file){
  static char buffer[BUFFER_SIZE];
  file.get(buffer,BUFFER_SIZE);
  file.get();
  return string(buffer);
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


std::string TextCollistRetriever::parse_method(std::string location){
	unsigned int pos = 0;
	static std::string sub_str;
	
	pos = location.find_first_of(TextCollistRetriever::METHOD_OPEN_BRACKET);
	if (pos != std::string::npos) {
		sub_str = location.substr(0, pos);
		return sub_str;			
	}
	else {
		pos = location.find_first_of(TextCollistRetriever::RANGE_OPEN_BRACKET);
		if (pos != std::string::npos) {
			sub_str = location.substr(0, pos);
			return sub_str;			
		}
		else {
			pos = location.find_first_of(TextCollistRetriever::TYPE_OPEN_BRACKET);
			if (pos != std::string::npos) {
				sub_str = location.substr(0, pos);
				return sub_str;			
			}
		}
	}
	return location;
}


std::string TextCollistRetriever::parse_type(std::string location){
	unsigned int pos = 0;
	static std::string sub_str;
	
	pos = location.find_last_of(TextCollistRetriever::TYPE_OPEN_BRACKET);
	if (pos != std::string::npos) {
		sub_str = location.substr(pos);
		sub_str = sub_str.substr(1);
		pos = sub_str.find_first_of(TextCollistRetriever::TYPE_CLOSE_BRACKET);
		if (pos != std::string::npos) {
			sub_str = sub_str.substr(0,pos);
			return sub_str;			
		}
	}
	return "";
}

std::string TextCollistRetriever::parse_arg(std::string location){
	// check if column
	unsigned int start_pos = 0;
	unsigned int end_pos = 0;
	static std::string sub_str;
	
	if (location.find_first_of(TextCollistRetriever::COLUMN_TAG) != std::string::npos) {
		start_pos = location.find_first_of(TextCollistRetriever::METHOD_OPEN_BRACKET, location.find_first_of(TextCollistRetriever::COLUMN_TAG));
		if (start_pos != std::string::npos) {
			sub_str = location.substr(start_pos);
			sub_str = sub_str.substr(1);
			sub_str = sub_str.substr(0, sub_str.size()-1);

			// TODO: check if range brackets are to be extracted
			// no need, they get cut automatically
			
			end_pos = sub_str.find_first_of(TextCollistRetriever::METHOD_CLOSE_BRACKET);
			if (end_pos != std::string::npos) {
				sub_str = sub_str.substr(0, end_pos);
			}

			// check if quotes have to be removed 
			if (sub_str[0]=='\"' || sub_str[0] == '\'') {
				sub_str = sub_str.substr(1);
				sub_str = sub_str.substr(0, sub_str.size()-1);
			}
			return sub_str;
		}
	}
	return "";
}

void TextCollistRetriever::parse_range(std::string location, int &x, int &y){
	unsigned int pos = 0;
	static std::string sub_str;
	
	pos = location.find_last_of(TextCollistRetriever::RANGE_OPEN_BRACKET);
	if (pos != std::string::npos) {
		sub_str = location.substr(pos);
		pos = sub_str.find_first_of(TextCollistRetriever::RANGE_CLOSE_BRACKET);
		if (pos != std::string::npos) {
			sub_str = sub_str.substr(0,pos);
			
			pos = sub_str.find_first_of(TextCollistRetriever::RANGE_SEPARATOR);
			if (pos != std::string::npos) {
				x = string_util::str_to_int(sub_str.substr(0,pos));
				y = string_util::str_to_int(sub_str.substr(pos));
				return;
			}
			else {
				// if no range separator is found -> go specified position to end of column
				x = string_util::str_to_int(sub_str);
				y = -1;
				return;
			}
		}
	}
	x = -1;
	y = -1;
}

bool TextCollistRetriever::isdata(std::string line) {
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
	// if alpha count is greater than 75 percent -> assume its a data row

//cout << "percent: " << line.size()-space_count << " " << digit_count <<" " <<digit_count*100/(line.size()-space_count)<<endl;

	if (digit_count*100/(line.size()-space_count) > 75) {
		return true;
	}
	return false;
}

bool TextCollistRetriever::isunit(std::string line) {
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


void TextCollistRetriever::extract_headers(std::string line) {
	headers = string_util::split_whitespace(line); 
	headers = string_util::strip_punct(headers);
}

void TextCollistRetriever::extract_units(std::string line) {
	units = string_util::split_whitespace(line);
	units = string_util::strip_punct(units);
}

std::string TextCollistRetriever::extract_dictentry(std::ifstream &file, std::string arg, unsigned int nxtype) {
	int i=0, j=0;
	unsigned int k=0, l=0, u1=0, u2=0;
	unsigned int pos=0, pos2=0;
	std::string str="";
	std::string ustr="";

	reset_file(infile);
	while (i<data_section) {
		std::string line = read_line(infile);
		//std::cout << "current line: " << line << std::endl;	

		while (isspace(line[j])){
			j++;
		}
		line = line.substr(j, line.size());
		pos = line.find(arg);
		if (pos != std::string::npos && pos<2) {
			pos2 = line.find(":", pos);
			if (pos2 != std::string::npos) {
				str = line.substr(pos2+1);
				
				while(k<str.size() && (ispunct(str[k]) || isspace(str[k]))) {
					k++;
				}

				l=str.size();
				while(l>=k && (ispunct(str[l-1]) || isspace(str[l-1]))){
					l--;
				}
				if (nxtype != NX_CHAR && l>=k) {
					//scan for unit
					if (isalpha((str[l-1]))) {
						u2=l-1;
						while(l>=k && (!isspace(str[l-1]))){
							l--;
						}
						u1=l-1;
						if (u1<u2) {
							ustr = std::string("units:")+str.substr(u1+1,u2)+std::string(";value:");
						}
						if (l>=k) {
							l--;
						}
					}
				}
				if(l!=k){
					std::string word = str.substr(k,l-k);
					if (word.size()>=2 && isalnum(word[0])) {
						str=word;
					}
				}
				break;
			}
		}
		i++;
	}
	return ustr+str;
}

std::vector<double> TextCollistRetriever::extract_column(ifstream &file, std::string col_name, int from, int to)
{
	int count = 0;
	int index = 0;
	std::vector<double> values;
	reset_file(infile);
	int cur_line=0;
	skip_to_line(infile, cur_line, data_section+((from<0)?0:from)); 
	
//cout << endl << "counting headers: " << *(headers.begin()) << endl;	

	std::vector<std::string>::iterator it = headers.begin();
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
	}
	return values;
}


int TextCollistRetriever::number_of_columns(std::string header_line) {
	std::vector<std::string> entrys = string_util::split_whitespace(header_line);
	entrys = string_util::strip_punct(entrys);
	return entrys.size();
}

/**
 * The factory will call the constructor with a string. The string
 * specifies where to locate the data (e.g. a filename), but
 * interpreting the string is left up to the implementing code.
 */
TextCollistRetriever::TextCollistRetriever(const string &str): source(str),current_line(0){
  //cout << "TextCollistRetriever(" << source << ")" << endl; // REMOVE

  // open the file
  infile.open(source.c_str());

  // check that open was successful
  if(! infile.is_open()) {
    throw invalid_argument("Could not open file: "+source);
  }

	unit_strings["seconds"] = NXunits::seconds;
	unit_strings["second"] 	= NXunits::seconds;
	unit_strings["secs"] 	= NXunits::seconds;
	unit_strings["sec"] 		= NXunits::seconds;
	
	unit_strings["deg"] 		= NXunits::degrees;
	unit_strings["degs"] 	= NXunits::degrees;
	unit_strings["degree"] 	= NXunits::degrees;
	unit_strings["degrees"] = NXunits::degrees;
	
	unit_strings["cts"] 		= NXunits::counts;
	unit_strings["counts"] 	= NXunits::counts;
	unit_strings["count"] 	= NXunits::counts;
	unit_strings["cnt"] 		= NXunits::counts;

	this->number_of_cols = 0;
	this->number_of_entrys = 0;
	
	int cur_line = 0;
	std::string line = read_line(infile);
	std::string prev_line="";
	std::string prev_prev_line="";
	
 	//std::cout << "current line: " << line << std::endl;
	while (infile.good() && !isdata(line)) {
		prev_prev_line = prev_line;
		prev_line=line;
		line = read_line(infile);
		//std::cout << "current line: " << line << std::endl;
		cur_line++;
	}
	//std::cout << "data line: " << line << std::endl;
	header_section = cur_line-1;
	// this is dangerous as it assumes the unit line to be present and always under the header line
	// alternative: check if in header line is some occurence of unit string -> treat is as unit line
	if (isunit(prev_line)) {
//std::cout << "extracting units from: " << prev_line << std::endl;
		extract_units(prev_line);
//std::cout << "extracting headers from: " << prev_prev_line << std::endl;
		extract_headers(prev_prev_line);
	}
	else {
		extract_headers(prev_line);
	}
	
	data_section = cur_line;
	
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


TextCollistRetriever::~TextCollistRetriever(){
  //cout << "~TextCollistRetriever()" << endl;

  // close the file
  if(infile)
    infile.close();
}

/**
 * This is the method for retrieving data from a file. The whole
 * tree will be written to the new file immediately after being
 * called. Interpreting the string is left up to the implementing
 * code.
 */
void TextCollistRetriever::getData(const string &location, tree<Node> &tr){
	//cout << "TextCollistRetriever::getData(" << location << ",tree)" << endl; // REMOVE
	// check that the argument is not an empty string
	//printf("extracting...%s", location.c_str()); 
	if(location.size()<=0) {
		throw invalid_argument("cannot parse empty string");
	}

	// so far ... all locations must refer to a column name 
	std::string method 	= parse_method(location);
	std::string arg 		= parse_arg(location);
	std::string nxtype 	= parse_type(location);
	if (nxtype == "") {
		// default type is NX_FLOAT64
		nxtype = "NX_FLOAT64";
	}
	
	// get range bounds if specified 
	int from, to;
	parse_range(location, from, to); 
	
	//cout << "location: " << method << "(\'" << arg << "\')" << "[" << from << "," << to << "]{" << nxtype <<"}" << endl << endl;	
 
	if (method == TextCollistRetriever::COLUMN_TAG) { 
		std::vector<double> values = extract_column(infile, arg, from, to);
  		if (values.size() <= 0) {
			// if we get no values, just dont create a node
			std::cout << "creating an empty node, as we got no values" << std::endl;
			// create an empty data node
			int* empty_dims = new int[1];
			empty_dims[0] = 1;
			void *data;
			if(NXmalloc(&data, 1, empty_dims, NX_INT32)!=NX_OK) {
				throw runtime_error("NXmalloc failed");
			}
			*((int*)data)=0;
			Node node(arg, data, 1, empty_dims, NX_INT32);
			// put the node into the tree 
			tr.insert(tr.begin(),node);
			return;
		}

	
		int nxrank = 1;
		int* nxdims = new int[1];
		nxdims[0] = values.size();

		// allocate space for the data
		void *data;
		if(NXmalloc(&data, nxrank, nxdims, convert_type(nxtype))!=NX_OK) {
			throw runtime_error("NXmalloc failed");
		}

		for( unsigned int i=0 ; i<values.size() ; i++ ) {
			switch (convert_type(nxtype)) {
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
				case NX_FLOAT64:
					*(((double*)data)+i)=static_cast<double>(values.at(i));
					break;
				case NX_FLOAT32:
					*(((float*)data)+i)=static_cast<float>(values.at(i));
					break;
					
			}
		}

		//cout << endl << "info: " << ((double*)data)[0] << " "<<((double*)data)[1]<<" "<<((double*)data)[2]<< endl;	
		// create a data node
		Node node("empty", data, nxrank, nxdims, convert_type(nxtype));
		// put the data in the node
		tr.insert(tr.begin(),node);
	}
	else if (method == TextCollistRetriever::DICT_TAG) { 
		std::string entry="";
		std::string units="";
		std::string raw_string = extract_dictentry(infile, arg, convert_type(nxtype));
 		//cout << "dict value: '" << raw_string << "'"<<std::endl;  
  		if (raw_string.size() <= 0) {
			// if we get no values, just dont create a node
			std::cout << "creating an empty node, as we got no values" << std::endl;
			// create an empty data node
			int* empty_dims = new int[1];
			empty_dims[0] = 1;
			void *data;
                        char *pPtr;

			if(NXmalloc(&data, 1, empty_dims, NX_CHAR)!=NX_OK) {
				throw runtime_error("NXmalloc failed");
			}
			pPtr = (char *)data;
			strcpy(pPtr,"");
			Node node(arg, data, 1, empty_dims, NX_CHAR);
			// put the node into the tree 
			tr.insert(tr.begin(),node);
			return;
		}
		
		
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
		//std::cout << "value extracted: " << entry << std::endl;

		int nxrank = 1;
		int* nxdims = new int[1];
		nxdims[0] = 1;

		// allocate space for the data
		void *data;
		if(NXmalloc(&data, nxrank, nxdims, convert_type(nxtype))!=NX_OK) {
			throw runtime_error("NXmalloc failed");
		}
			switch (convert_type(nxtype)) {
				case NX_CHAR:
					if (nxtype == "ISO8601") {
						data = (char*)mira_datetime_2_iso(entry).c_str();
					}
					else {
						data = (char*)entry.c_str();
					}
					nxdims[0] = entry.size();
					break;
				case NX_INT32: 
					*((int*)data) = static_cast<int>(string_util::str_to_int(entry.c_str()));
					break;
				case NX_UINT32: 
					*((unsigned int*)data) = static_cast<unsigned int>(string_util::str_to_int(entry.c_str()));
					break;
				case NX_INT16: 
					*((short*)data) = static_cast<short>(string_util::str_to_int(entry.c_str()));
					break;
				case NX_UINT16: 
					*((unsigned short*)data) = static_cast<unsigned short>(string_util::str_to_int(entry.c_str()));
					break;
				case NX_FLOAT64:
					*((double*)data) = static_cast<double>(string_util::str_to_float(entry.c_str()));
					break;
				case NX_FLOAT32:
					*((float*)data) = static_cast<float>(string_util::str_to_float(entry.c_str()));
					break;
			}

		// create a data node
		Node node("empty", data, nxrank, nxdims, convert_type(nxtype));
		if (units!="" && convert_type(nxtype) != NX_CHAR) {
			std::vector<Attr> attrs;
			Attr attr("units", units.c_str(), units.size(), NX_CHAR);
			attrs.push_back(attr);
			node.update_attrs(attrs);
		}
		// put the node into the tree 
		tr.insert(tr.begin(),node);
		
	}
	else {
		printf("unknown method !!");
	}
}


std::string TextCollistRetriever::mira_datetime_2_iso(std::string &datetimestr) {
	/* mira: 05/17/2005 03:13:37 */
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


std::string TextCollistRetriever::toString() const{
  return "["+MIME_TYPE+"] "+source;
}
