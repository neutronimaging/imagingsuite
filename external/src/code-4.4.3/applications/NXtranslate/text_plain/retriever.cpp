#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "retriever.h"
#include "../node.h"
#include "../node_util.h"
#include "../string_util.h"
#include "../tree.hh"

using std::ifstream;
using std::invalid_argument;
using std::runtime_error;
using std::string;
using std::cout;
using std::endl;
using std::vector;

static const int BUFFER_SIZE=256;

static string read_line(ifstream &file){
  static char buffer[BUFFER_SIZE];
  file.get(buffer,BUFFER_SIZE);
  file.get();
  return string(buffer);
}

static void skip_to_line(ifstream &file,int &cur_line, int new_line){
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

/**
 * The factory will call the constructor with a string. The string
 * specifies where to locate the data (e.g. a filename), but
 * interpreting the string is left up to the implementing code.
 */
TextPlainRetriever::TextPlainRetriever(const string &str): source(str),current_line(0){
  //cout << "TextPlainRetriever(" << source << ")" << endl; // REMOVE

  // open the file
  infile.open(source.c_str());

  // check that open was successful
  if(!infile.is_open())
    throw invalid_argument("Could not open file: "+source);
}

TextPlainRetriever::~TextPlainRetriever(){
  //cout << "~TextPlainRetriever()" << endl;

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
void TextPlainRetriever::getData(const string &location, tree<Node> &tr){
  //cout << "TextPlainRetriever::getData(" << location << ",tree)" << endl; // REMOVE
  // check that the argument is not an empty string
  if(location.size()<=0)
    throw invalid_argument("cannot parse empty string");

  // check that the argument is an integer
  int line_num=string_util::str_to_int(location);

  // set stream to the line before
  skip_to_line(infile,current_line,line_num);
  // read the line and print it to the console
  string text=read_line(infile);

  // create an empty node
  Node node("empty","empty");

  // put the data in the node
  vector<int> dims;
  dims.push_back(text.size());
  update_node_from_string(node,text,dims,Node::CHAR);
  tr.insert(tr.begin(),node);
}

const string TextPlainRetriever::MIME_TYPE("text/plain");

string TextPlainRetriever::toString() const{
  return "["+MIME_TYPE+"] "+source;
}
