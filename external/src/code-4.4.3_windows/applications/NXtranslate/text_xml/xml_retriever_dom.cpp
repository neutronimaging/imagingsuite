#include <iostream>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdarg.h>
#include "../node.h"
#include "../node_util.h"
#include "../string_util.h"
#include "../xml_util.h"
#include "xml_retriever_dom.h"

using std::cerr;
using std::cout;
using std::endl;
using std::invalid_argument;
using std::string;
using std::runtime_error;
using std::vector;

typedef struct{
  int status; // 0 if everything is fine
  string error_msg; // store message for error reporting
  string char_data; // character data collected for current node
  tree<Node>::iterator node; // current node being worked on
  tree<Node> *tr; // a copy of the actual tree being filled
}UserData;

typedef tree<Node> TreeNode;
typedef tree<Node>::iterator TreeNodeIter;

// must be static to allow for producing meaningful error messages
static xmlParserCtxtPtr context=NULL;

static xmlEntityPtr my_getEntity(void *user_data, const xmlChar *name){
  // let libxml2 do this for us
  return xmlGetPredefinedEntity(name);
}

static void my_startDocument(void *user_data){
  // there is no initialization to be done
}

static void my_endDocument(void *user_data){
  // there is no cleanup to be done
}

/*
 * This function exists only to make the code in startElement more
 * readable. It pushes a node into the tree and moves around all of
 * the appropriate accounting.
 */
static void insertNode(UserData *user_data,Node &node){
  if(user_data->tr->size()<=0) // tree is empty, use a different command
    user_data->node=user_data->tr->insert(user_data->tr->begin(),node);
  else // add as a child of current node
    user_data->node=user_data->tr->append_child(user_data->node,node);
}

static void my_startElement(void *user_data, const xmlChar *name,
                                                       const xmlChar ** attrs){
  static string DEF_TYPE("UNKNOWN_TYPE");
  static string TYPE("type");

  // convert the name to a string
  string str_name=xml_util::xmlChar_to_str(name,-1);
  // create a label for the element when writing out exceptions
  string except_label="<"+str_name+">:";
  // convert the attributes to a vector<string>
  vector<string> str_attrs=xml_util::xmlattr_to_strvec(attrs);

#ifdef DEBUG_XML_RETRIEVER_DOM
  std::cout << "startElement(user_data," << str_name << ")" << std::endl;
#endif

  // create attributes
  string type=DEF_TYPE;
  vector<Attr> attr_list;
  if(str_attrs.size()>=2){
    for( vector<string>::iterator it=str_attrs.begin() ; it!=str_attrs.end() ; it+=2 ){
      if(TYPE==(*it))
        type=(*(it+1));
      else
        attr_list.push_back(make_attr(*it,*(it+1)));
    }
  }

  Node node(str_name,type);
  node.set_attrs(attr_list);
  insertNode((UserData *)user_data,node);

}

static void closeNode(UserData *user_data){
  user_data->node=user_data->tr->parent(user_data->node);
}

static void my_endElement(void *user_data, const xmlChar *name){
#ifdef DEBUG_XML_RETRIEVER_DOM
  std::cout << "endElement(" << name << ")" << std::endl;
#endif
  static string LEFT("[");
  static string RIGHT("]");

  if(((UserData *)user_data)->char_data.size()>0){
    string str_type=((UserData *)user_data)->node->type();

    Node::NXtype type=Node::CHAR;
    vector<int> dims;
    if(string_util::starts_with(str_type,"CHAR")){
      // do nothing, this is the default value
    }else if(str_type.substr(str_type.size()-1,str_type.size())==RIGHT){
      int start=str_type.find(LEFT);
      string str_dim=str_type.substr(start,str_type.size());
      if(str_dim.size()>0){
        dims=string_util::str_to_intVec(str_dim);
      }else{
        dims.push_back(1);
      }
      str_type=str_type.erase(start,str_type.size());
               
      if(!(string_util::starts_with(str_type,"NX_")))
        str_type="NX_"+str_type;
      type=node_type(str_type);
    }

    update_node_from_string(*(((UserData *)user_data)->node), 
                            ((UserData *)user_data)->char_data,
                            dims, type);

  }

  // purge character data
  ((UserData *)user_data)->char_data.clear();

  // close the node by moving pointers up to the parent
  closeNode((UserData *)user_data);
}

static void my_characters(void *user_data, const xmlChar *ch, int len){
  // convert the character array into something useful
  string str=xml_util::xmlChar_to_str(ch,len);

  // if the string is empty there is nothing to do
  if(str.size()<=0) return;

  // add the characters with a space between it and what was there
  ((UserData *)user_data)->char_data+=str;

#ifdef DEBUG_XML_RETRIEVER_DOM
  std::cout << "characters: " << ((UserData *)user_data)->char_data
            << std::endl;
#endif

}

static void my_error(void *user_data, const char* msg, ...){
  static const string SAX_ERROR="SAX_ERROR";

  // get the rest of the arguments
  va_list args;
  va_start(args,msg);

  // get the position of the error
  int line=getLineNumber(context);
  int col =getColumnNumber(context);

  // print out the result
  char str[70];
  int num_out=vsprintf(str,msg,args);
  cerr << SAX_ERROR << " [L" << line << " C" << col << "]: "<< str;

  // clean up args
  va_end(args);

  // set the status to failure
  ((UserData *)user_data)->status=-1;
}

static void my_fatalError(void *user_data, const char* msg, ...){
  static const string FATAL_SAX_ERROR="FATAL_SAX_ERROR";

  // get the rest of the arguments
  va_list args;
  va_start(args,msg);

  // get the position of the error
  int line=getLineNumber(context);
  int col =getColumnNumber(context);

  // print out the result
  char str[70];
  int num_out=vsprintf(str,msg,args);
  cerr << FATAL_SAX_ERROR << " [L" << line << " C" << col << "]: "<< str;

  // clean up args
  va_end(args);

  // set the status to failure
  ((UserData *)user_data)->status=-1;
}

// contains all of the function pointers for dealing with parsing the file
static xmlSAXHandler my_handler = {
  NULL, // internalSubsetSAXFunc internalSubset;
  NULL, // isStandaloneSAXFunc isStandalone;
  NULL, // hasInternalSubsetSAXFunc hasInternalSubset;
  NULL, // hasExternalSubsetSAXFunc hasExternalSubset;
  NULL, // resolveEntitySAXFunc resolveEntity;
  my_getEntity, // getEntitySAXFunc getEntity;
  NULL, // entityDeclSAXFunc entityDecl;
  NULL, // notationDeclSAXFunc notationDecl;
  NULL, // attributeDeclSAXFunc attributeDecl;
  NULL, // elementDeclSAXFunc elementDecl;
  NULL, // unparsedEntityDeclSAXFunc unparsedEntityDecl;
  NULL, // setDocumentLocatorSAXFunc setDocumentLocator;
  my_startDocument, // startDocumentSAXFunc startDocument;
  my_endDocument, // endDocumentSAXFunc endDocument;
  my_startElement, // startElementSAXFunc startElement;
  my_endElement, // endElementSAXFunc endElement;
  NULL, // referenceSAXFunc reference;
  my_characters, // charactersSAXFunc characters;
  NULL, // ignorableWhitespaceSAXFunc ignorableWhitespace;
  NULL, // processingInstructionSAXFunc processingInstruction;
  NULL, // commentSAXFunc comment;
  NULL, // warningSAXFunc warning;
  my_error, // errorSAXFunc error;
  my_fatalError, // fatalErrorSAXFunc fatalError;
};

static string parse_xml_file(const string &filename, TreeNode &tr){
  // if necessary, create user data here
  UserData user_data;
  user_data.status=0;
  user_data.error_msg=string();
  user_data.char_data=string();
  user_data.node=tr.begin();
  user_data.tr=&tr;

  // create context which is static so error messages can have line numbers
  context=xmlCreateFileParserCtxt(filename.c_str());
  context->sax=&my_handler;
  context->userData=&user_data; // bind user data to context
  int result=xmlParseDocument(context);

  // return if the there was an error
  if(result<0)
    throw runtime_error("Error parsing document");
  

  context=NULL;
  return string();
}

#ifdef DEBUG_XML_RETRIEVER_DOM
void print_node(Node &node){
  cout << "[" << node.is_data() << "]" << node.name() << ":" 
       << node.type() << "  [";
  int num=node.num_attr();
  for( int i=0 ; i<num ; i++ ){
    Attr attr=node.get_attr(i);
    cout << attr.name() << ":" << attr.value();
    if(i+1<num) cout << ",";
  }
  cout << "]"  << endl;
}

void print_tree(TreeNode &tr, TreeNodeIter it, TreeNodeIter end){
  if(!tr.is_valid(it)) return;

  int rootdepth=tr.depth(it);
  while(it!=end){
    for( int i=0 ; i<tr.depth(it)-rootdepth ; ++i)
      cout << "  ";
    print_node(*it);
    //cout << it->name() << ":" << it->type() << endl;
    ++it;
  }
}
#endif

extern std::string buildTree(const std::string &filename, tree<Node> &tr){
#ifdef DEBUG_XML_RETRIEVER_DOM
  cout << "buildTree(" << filename << ",tr)" << endl; // REMOVE
#endif

  // check that the filename is okay
  if(filename.size()<=0)
    throw invalid_argument("filename is empty");

  // parse the file to build the tree
  parse_xml_file(filename,tr);

#ifdef DEBUG_XML_RETRIEVER_DOM
  cout << "********** print tree(" << tr.size() << ")" << endl;
  print_tree(tr,tr.begin(),tr.end());
  cout << "********************" << endl;
#endif

  return string();
}
