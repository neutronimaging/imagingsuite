#ifndef __TEXT_COLLIST_RETRIEVER
#define __TEXT_COLLIST_RETRIEVER

#include "../retriever.h"
#include <fstream>
#include <map>

// this is not intended to be inherited from
class TextCollistRetriever: public Retriever{
 public:
  TextCollistRetriever(const std::string &);
  ~TextCollistRetriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private:
  TextCollistRetriever(const TextCollistRetriever&);
  TextCollistRetriever& operator=(const TextCollistRetriever&);
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
  
  void extract_headers(std::string line);
  void extract_units(std::string line);

  int number_of_columns(std::string line);
  std::string extract_dictentry(std::ifstream &file, std::string arg, unsigned int nxtype);
  std::vector<double> extract_column(std::ifstream &file, std::string col_name, int from=-1, int to=-1);
  std::string parse_arg(std::string location);
  std::string parse_method(std::string location);
  std::string parse_type(std::string location);
  void parse_range(std::string location, int &x, int &y);
  
  std::string mira_datetime_2_iso(std::string &timestr);
  
public: 
  static bool isdata(std::string line);
  bool isunit(std::string line);
  static const std::string COLUMN_TAG;
  static const std::string DICT_TAG;
  static const std::string RANGE_OPEN_BRACKET;
  static const std::string RANGE_CLOSE_BRACKET;
  static const std::string RANGE_SEPARATOR;
  static const std::string METHOD_OPEN_BRACKET;
  static const std::string METHOD_CLOSE_BRACKET;
  static const std::string TYPE_OPEN_BRACKET;
  static const std::string TYPE_CLOSE_BRACKET;
  std::map<std::string, std::string> unit_strings; 
  

};
#endif
