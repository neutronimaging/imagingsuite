#include <algorithm>
#include <iostream>
#include <fstream>
#include <libxml/xmlmemory.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "retriever.h"
#include "xml_retriever_dom.h"
#include "void_copy.h"
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
using string_util::starts_with;

typedef vector<string>     StringVec;
typedef tree<Node>         NodeTree;
typedef NodeTree::iterator NodeTreeIter;

static string get_type(const string &location){
  static const string CHAR("CHAR");
  if(starts_with(location,CHAR))
    return "NX_CHAR";

  static const string INT8("INT8");
  if(starts_with(location,INT8))
    return "NX_INT8";

  static const string INT16("INT16");
  if(starts_with(location,INT16))
    return "NX_INT16";

  static const string INT32("INT32");
  if(starts_with(location,INT32))
    return "NX_INT32";

  static const string UINT8("UINT8");
  if(starts_with(location,UINT8))
    return "NX_UINT8";

  static const string UINT16("UINT16");
  if(starts_with(location,UINT16))
    return "NX_UINT16";

  static const string UINT32("UINT32");
  if(starts_with(location,UINT32))
    return "NX_UINT32";

  static const string FLOAT32("FLOAT32");
  if(starts_with(location,FLOAT32))
    return "NX_FLOAT32";

  static const string FLOAT64("FLOAT64");
  if(starts_with(location,FLOAT64))
    return "NX_FLOAT64";

  throw invalid_argument("Cannot determine type in location: "+location);
}

static bool is_right_square_bracket(const char c){
  static const string RIGHT="]";
  return find(RIGHT.begin(),RIGHT.end(),c)!=RIGHT.end();
}

static string get_dims(const string &location){
  using std::find;
  static const string LEFT("[");

  if(!starts_with(location,LEFT))
    return "";

  string result="";
  for(string::const_iterator it=location.begin() ; it!=location.end() ; it++ ){
    result+=(*it);
    if(is_right_square_bracket(*it))
      break;
  }

  if(result.size()==location.size())
    return "";
  else
    return result;
}

/**
 * The factory will call the constructor with a string. The string
 * specifies where to locate the data (e.g. a filename), but
 * interpreting the string is left up to the implementing code.
 */
TextXmlRetriever::TextXmlRetriever(const string &str): source(str), __tree(new tree<Node>){

  // fill tree
  string warn=buildTree(str,*__tree);

  // check that the document is not empty
  if(__tree->size()<=0)
    throw runtime_error("Empty document ["+source+"]");
}

TextXmlRetriever::~TextXmlRetriever(){
  //cout << "~TextXmlRetriever()" << endl;
  
  // __tree does not need to be deleted
}

static Node getNode(Ptr<NodeTree> tr, const StringVec &path){
  // set up iterators for dealing with the path
  StringVec::const_iterator path_it=path.begin();
  StringVec::const_iterator path_end=path.end();
  
  for( NodeTreeIter it=tr->begin() ; it!=tr->end() ;  ){
    if(it->name()==(*path_it)){
      path_it++;
      if(path_it==path_end)
        return *it;
      it=tr->child(it,0);
    }else{
      it=tr->next_sibling(it);
    }
  }

  // when the code gets here the path was not found
  string error;
  for( StringVec::const_iterator it=path.begin() ; it!=path_end ; it++ )
    error+="/"+(*it);
  throw runtime_error("PATH["+error+"] NOT FOUND IN FILE");
}

// determine if there is a call for an attribute (denoted by #)
static string strip_attr(StringVec &path){
  static string HASH("#");

  // the attribute call can only be on the last part of the path
  string last=*(path.end()-1);

  // determine if it contains a hash
  int start=last.find(HASH);
  if(start<=0) return string();

  // get the attribute out
  string attr=last.substr(start+1,last.size());
  last=last.erase(start,last.size());

  // repair the path
  path.pop_back();
  path.push_back(last);

  return attr;
}

/**
 * Turns an attribute from a node into a node.
 */
static Node promoteAttribute(const Node &node, const string &name){
  //cout << "promoteAttribute(" << node.name() << "," << name << ")" << endl;

  int num_attr=node.num_attr();
  for( int i=0 ; i<num_attr ; i++ ){
    Attr attr(node.get_attr(i));

    if(attr.name()==name){
      int dims[1]={attr.length()};
      Node result(name,attr.value(),1,dims,attr.type());
      return result;
    }
  }

  throw runtime_error("Attribute ["+name+"] not found");
}

static int getElementCount(const vector<int> &vec){
  if(vec.size()<=0)
    throw invalid_argument("Cannot get number of elements from empty dimension");

  int tot=1;
  for( vector<int>::const_iterator it=vec.begin() ; it!=vec.end() ; it++ ){
    tot*=(*it);
  }

  return tot;
}

static Node convertFromString(const string &name, string &data, vector<int> &dims, Node::NXtype type){
  static string DEF_TYPE="EMPTY";
  Node result(name,DEF_TYPE);
  update_node_from_string(result, data, dims, type);
  return result;
}

static Node convertFromNumeric(const Node &node, vector<int> &dims, const string &type){
  // get the name of the resulting node
  string name=node.name();

  // determine the numeric types
  Node::NXtype source_type=node.int_type();
  Node::NXtype copy_type=node_type(type);

  // if the types match, just return the node given
  if(source_type==copy_type)
    return node;

  // create the dimension and rank information
  const int rank=dims.size();
  std::vector<int> int_dims(rank);
  int tot_num=1;
  for( int i=0 ; i<rank ; i++ ){
    int_dims[i]=dims[i];
    tot_num*=dims[i];
  }

  // allocate space for the data array
  void *value(NULL);
  NXmalloc(&value,rank,&(int_dims[0]),copy_type);

  // temporary variable stating whether or not the cast worked
  bool worked=false;

  // make the cast
  if(source_type==Node::FLOAT32)
    worked=void_copy::from_float(((float*)(node.data())),value,tot_num,copy_type);
  else if(source_type==Node::FLOAT64)
    worked=void_copy::from_double(((double*)(node.data())),value,tot_num,copy_type);
  else if(source_type==Node::INT16)
    worked=void_copy::from_short(((short int*)(node.data())),value,tot_num,copy_type);
  else if(source_type==Node::INT32)
    worked=void_copy::from_int(((int*)(node.data())),value,tot_num,copy_type);
  else if(source_type==Node::UINT16)
    worked=void_copy::from_ushort(((unsigned short int*)(node.data())),value,tot_num,copy_type);
  else if(source_type==Node::UINT32)
    worked=void_copy::from_uint(((unsigned int*)(node.data())),value,tot_num,copy_type);
  // ---------- add code here to work with other types

  // error out if the cast did not work
  if(!worked){
    NXfree(&value);
    value=NULL;
    throw runtime_error("Cannot convert "+node.type()+" to "+type);
  }

  // package up the result
  Node result(name,"EMPTY");
  result.set_data(value,rank,&(int_dims[0]),copy_type);

  // free up the temporary memory
  NXfree(&value);
  value=NULL;

  // return the result of the cast
  return result;
}

static Node convertType(const Node &node, const string &type,string &str_dims){
  // can not convert non-data
  if(!node.is_data())
    throw invalid_argument("Cannot convert non-data to data");

  // convert type to integer
  Node::NXtype int_type=node_type(type);
  Node::NXtype node_int_type=node.int_type();

  // move dimensions over to vector<int> that the functions expect
  vector<int> dims=string_util::str_to_intVec(str_dims);

  // convert from string
  if(node_int_type==Node::CHAR){
    // convert void pointer to string
    string value((char *)node.data());

    // do the actual conversion
    return convertFromString(node.name(),value,dims,int_type);
  }

  // if they are already the same type, update the dimensions and return
  if(int_type==node_int_type){
    // change the dimensionality for numerics
    Node result(node);
    result.update_dims(dims);
    return result;
  }

  // check that the number of elements is unchanged
  int node_tot=getElementCount(node.dims());
  int res_tot =getElementCount(dims);
  if(node_tot!=res_tot)
    throw runtime_error("Cannot change number of elements in a node");

  // do the actual conversion
  return convertFromNumeric(node,dims,type);
}

/**
 * This is the method for retrieving data from a file. The whole
 * tree will be written to the new file immediately after being
 * called. Interpreting the string is left up to the implementing
 * code.
 */
void TextXmlRetriever::getData(const string &location, tree<Node> &tr){
  //  cout << "TextXmlRetriever::getData(" << location << ",tree)" << endl; // REMOVE
  // check that the argument is not an empty string
  if(location.size()<=0)
    throw invalid_argument("cannot parse empty string");

  // variables for the divided string version of the location
  string str_path;
  string type;
  string str_dims;

  // convert the location to a type and (string) path
  if(starts_with(location,"/")){
    str_path=location;
    type="NX_CHAR";
    str_dims="";
  }else{
    // get the type and remove it from the location
    type=get_type(location);
    str_path=location.substr(type.size()-3,location.size());
    // get the dimensions and remove it from the location
    str_dims=get_dims(str_path);
    str_path=str_path.substr(str_dims.size(),str_path.size());
    if(str_dims.empty())
      str_dims="[1]";
    // remove the separating colon
    str_path=str_path.substr(1,str_path.size());
  }
  //std::cout << "TYPE=" << type << " DIMS=" << str_dims << " PATH=" << str_path << std::endl; // REMOVE

  // split path up
  StringVec path=string_util::string_to_path(str_path);

  // get out the attribute
  string attr=strip_attr(path);

  // get the requested node
  Node node=getNode(__tree,path);

  // upgrade attributes to be a node, if requested
  if(attr.size()>0){
    node=promoteAttribute(node,attr);
  }

  // change the type of a node
  if((node.type()!=type) && (str_dims.size()>0))
    node=convertType(node,type,str_dims);

  // put the node in the supplied tree to pass it back
  tr.insert(tr.begin(),node);
}

const string TextXmlRetriever::MIME_TYPE("text/xml");

string TextXmlRetriever::toString() const{
  return "["+MIME_TYPE+"] "+source;
}
