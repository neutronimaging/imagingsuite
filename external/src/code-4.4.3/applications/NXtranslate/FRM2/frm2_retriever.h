#ifndef __FRM2_RETRIEVER
#define __FRM2_RETRIEVER

#include "../retriever.h"
#include <fstream>
#include <map>


// this is not intended to be inherited from
class Frm2Retriever: public Retriever{
 public:
  Frm2Retriever(const std::string &);
  ~Frm2Retriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private:
  Frm2Retriever(const Frm2Retriever&);
  Frm2Retriever& operator=(const Frm2Retriever&);
  std::string source;

protected:
  int current_line;
  std::ifstream infile;
  int number_of_cols;
  int number_of_headers;
  int number_of_entrys;
  int header_section;
  int data_section;
  
  std::vector<std::string> headers;
  std::vector<std::string> units;
  
  std::vector<std::string> detector_counts;
  std::vector<std::string> monitor_counts;
  
  
	void extract_headers(std::string line);
	void extract_toflogheaders(std::ifstream &file);
	void extract_units(std::string line);
	std::vector<std::string>& clear_units(std::vector<std::string> &units);
	int number_of_columns(std::string line);


  std::string 										extract_header			(std::ifstream &file, std::string arg, unsigned int nxtype, unsigned int word=0, unsigned int wcount=1);
  std::string 										extract_data			(std::ifstream &file, int entry_num, unsigned int nxtype, unsigned int lineno, unsigned int word=0, unsigned int wcount=1);
  std::vector<double>							extract_heidi_so		(std::ifstream &file, int entry_num, unsigned int nxtype, unsigned int word=0, unsigned int wcount=1);
  std::string 										extract_heidi_omg		(std::ifstream &file, int entry_num, unsigned int nxtype, unsigned int word=0, unsigned int wcount=1);
  std::string 										extract_heidiids			(std::ifstream &file, int entry_num, unsigned int nxtype, unsigned int lineno, unsigned int word=0);
  std::vector<double> 							extract_heidi_counts	(std::ifstream &file, int entry_num, bool monitor_counts=false);
  std::vector<std::vector<unsigned int> > extract_tofcts			(std::ifstream &file, std::string arg, unsigned int nxtype, bool is_monitor=false);
  std::vector<double> 							extract_toftof			(std::ifstream &file, std::string arg, unsigned int nxtype, bool is_monitor=false);
  std::string 										extract_filename		(std::ifstream &file, std::string source, std::string arg, unsigned int nxtype);
  std::string 										extract_line_below	(std::ifstream &file, std::string arg, unsigned int nxtype);
  std::vector<unsigned int> 					extract_desc			(std::ifstream &file, std::string arg, unsigned int nxtype);
  std::vector<unsigned int> 					extract_dnr				(std::ifstream &file, std::string arg, unsigned int nxtype);
  std::map<std::string, std::string>		extract_dictentry		(std::ifstream &file, std::string arg, unsigned int nxtype);
  std::map<std::string, std::string>		extract_dictmulti		(std::ifstream &file, std::vector<std::string> args, unsigned int nxtype);
  std::map<std::string, std::string>		extract_dictarray		(std::ifstream &file, std::string arg, unsigned int nxtype);
  std::string 										extract_datetime		(std::ifstream &file, std::vector<std::string> args, unsigned int nxtype);
  std::string 										extract_logcpy			(std::ifstream &file, std::string filename);
  
  std::vector<std::string> 					extract_toflog			(std::ifstream &file, std::string col_name, int from=-1, int to=-1);
  std::vector<std::string> 					extract_column			(std::ifstream &file, std::string col_name, int from=-1, int to=-1);
  
  
  
  std::vector<std::string> parse_arg(std::string location);
  std::string parse_method(std::string location);
  std::string parse_type(std::string location);
  void parse_range(std::string location, int &x, int &y);
  void parse_heidi_range(std::string location, int &line, int &word, int &wcount);
  std::vector<unsigned int> parse_dims(std::string dimstr);
  std::vector<unsigned int> parse_type_dims(std::string location, std::string &typestr);
  
  std::string nicos_datetime_2_iso(std::string &timestr);
  std::string toftof_datetime_2_iso(std::string &timestr);
  std::string toflog_datetime_2_iso(std::string &timestr);
  std::string heidi_datetime_2_iso(std::string &timestr);
  std::string month_strtonum(std::string &monthstr);
  
  void initUnits();
  
  Node* createEmptyNode(std::string nodename, unsigned int nxtype);
  Node* createNode(std::string nodename, std::vector<std::vector<double> >values, unsigned int nxtype);
  Node* createNode(std::string nodename, std::vector<std::vector<unsigned int> >values, unsigned int nxtype);
  Node* createNode(std::string nodename, std::vector<unsigned int>values, unsigned int nxtype);
  Node* createNode(std::string nodename, std::vector<double>values, unsigned int nxtype, std::string units="", std::vector<unsigned int> dims= (std::vector<unsigned int, std::allocator<unsigned int> >)(0));
  Node* createNode(std::string nodename, std::vector<std::string>values, unsigned int nxtype, std::string units="");
  Node* createNode(std::string nodename, std::string value, unsigned int nxtype, std::string units="");
  
  
public: 
  static bool isdata(std::string line);
  static void strip(std::string& line);
  bool isunit(std::string line);
  static bool isNumber(std::string str);
  static bool isheidicountdata(std::string line);
  static bool isheidiheader(std::string line);
  static const std::string COLUMN_TAG;
  static const std::string DICT_TAG;
  static const std::string DATETIME_TAG;
  static const std::string DICTARRAY_TAG;
  static const std::string DICTMULTI_TAG;
  static const std::string HEADER_TAG;
  static const std::string DATA_TAG;
  static const std::string HEIDIMON_TAG;
  static const std::string HEIDICTS_TAG;
  static const std::string HEIDIIDS_TAG;
  static const std::string HEIDIOMG_TAG;
  static const std::string HEIDISO_TAG;
  static const std::string TOFCTS_TAG;
  static const std::string TOFTOF_TAG;
  static const std::string TOFDNR_TAG;
  static const std::string TOFMONTOF_TAG;
  static const std::string TOFMONCTS_TAG;
  static const std::string TOFLOG_TAG;
  static const std::string LOGCPY_TAG;
  static const std::string DESC_TAG;
  static const std::string FILENAME_TAG;
  static const std::string FLINE_TAG;
  static const std::string RANGE_OPEN_BRACKET;
  static const std::string RANGE_CLOSE_BRACKET;
  static const std::string RANGE_SEPARATOR;
  static const std::string METHOD_OPEN_BRACKET;
  static const std::string METHOD_CLOSE_BRACKET;
  static const std::string TYPE_OPEN_BRACKET;
  static const std::string TYPE_CLOSE_BRACKET;
  static const std::string ARRAY_OPEN_BRACKET;
  static const std::string ARRAY_CLOSE_BRACKET;
  static const std::string ARRAY_SEPARATOR;
  static const std::string ARRAY_RANGE_SEPARATOR;
  static const std::string ARG_QUOTE;
  static const std::string ARG_SEPARATOR;
  
  static const unsigned int HEIDI_LINES_PER_ENTRY;
  static const unsigned int HEIDI_CHARS_PER_COUNT;
  static const unsigned int HEIDI_COLS_PER_LINE;
  
  std::map<std::string, std::string> unit_strings; 
  

};
#endif
