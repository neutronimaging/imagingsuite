#include <fstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include "nexus_retriever.h"
#include "nexus_util.h"
#include "node.h"
#include "retriever.h"
#include "string_util.h"
#include "tree.hh"

using std::invalid_argument;
using std::runtime_error;
using std::string;
using std::vector;

const static int    GROUP_STRING_LEN = 128;
const static string SDS              = "SDS";

typedef tree<Node> NodeTree;

/**
 * The factory will call the constructor with a string. The string
 * specifies where to locate the data (e.g. a filename), but
 * interpreting the string is left up to the implementing code.
 */
NexusRetriever::NexusRetriever(const string &str): source(str){
  // allocate memory for the handle
  handle=new NXhandle;

  // check if the filename is nonzero
  if(str.size()<=0)
    throw invalid_argument("Cannot initialize from an empty string");

  // check if the file is readable
  { // have the variable quickly go out of scope to enable cleanup
    std::ifstream checkFile(source.c_str());
    bool is_readable(checkFile);
    checkFile.close();
    if(!is_readable)
      throw runtime_error(source+" is not readable");
  }

  // open the file using NAPI
  char filename[50];
  strcpy(filename,str.c_str());
  if(NXopen(filename,NXACC_READ,handle)!=NX_OK){
    delete handle;
    throw runtime_error("NXopen failed");
  }

}

NexusRetriever::~NexusRetriever(){
  if(handle!=NULL){
    if(NXclose(handle)!=NX_OK)
      throw runtime_error("NXclose failed");
    delete handle;
  }
}

/**
 * This can throw a runtime_error if something goes wrong
 */
static void getAttrAsNode(NXhandle *handle, NodeTree &tree, string &name){
  // get ready to find the correct attribute
  if(NXinitattrdir(*handle)!=NX_OK)
    throw runtime_error("NXinitattrdir failed");
  char attr_name[GROUP_STRING_LEN];
  int attr_type;
  int num_attr;
  int attr_len;
  if(NXgetattrinfo(*handle,&num_attr)!=NX_OK)
    throw runtime_error("NXgetattrinfo failed");

  // find the correct attribute
  bool found=false;
  for( int i=0 ; i<num_attr ; i++ ){
    if(NXgetnextattr(*handle,attr_name,&attr_len,&attr_type)!=NX_OK)
      throw runtime_error("NXgetnextattr failed");
    if(name==attr_name){
      found=true;
      break;
    }
  }
  if (!found) {
    throw runtime_error("failed to find attribute");
  }
  // get the value
  int attr_dims[1]={attr_len+1};
  void *attr_value;
  if(NXmalloc(&attr_value,1,attr_dims,attr_type)!=NX_OK)
    throw runtime_error("NXmalloc failed");
  if(NXgetattr(*handle,attr_name,attr_value,attr_dims,&attr_type)!=NX_OK)
    throw runtime_error("NXgetattr failed");
  Node node(name,attr_value,1,attr_dims,attr_type);
  if(NXfree(&attr_value)!=NX_OK)
    throw runtime_error("NXfree failed");

  // put the attribute in the tree
  tree.insert(tree.begin(),node);
}

/**
 * This can throw a runtime_error if something goes wrong
 */
static void getDataNode(NXhandle *handle, string &name, Node &node){
  // get the node and all of its attributes
  int rank=0;
  int type=0;
  int dims[NX_MAXRANK];
  if(NXgetinfo(*handle,&rank,dims,&type)!=NX_OK)
    throw runtime_error("NXgetinfo failed");

  // allocate space for the data
  void *data;
  if(NXmalloc(&data,rank,dims,type)!=NX_OK)
    throw runtime_error("NXmalloc failed");

  // retrieve data from the file
  if(NXgetdata(*handle,data)!=NX_OK)
    throw runtime_error("NXgetdata failed");

  // retrieve attributes from the file
  if(NXinitattrdir(*handle)!=NX_OK)
    throw runtime_error("NXinitattrdir failed");
  char attr_name[GROUP_STRING_LEN];
  int attr_type;
  int num_attr;
  int attr_len;
  if(NXgetattrinfo(*handle,&num_attr)!=NX_OK)
    throw runtime_error("NXgetattrinfo failed");
  vector<Attr> attrs;
  for( int i=0 ; i<num_attr ; i++ ){
    if(NXgetnextattr(*handle,attr_name,&attr_len,&attr_type)!=NX_OK)
      throw runtime_error("NXgetnextattr failed");
    int attr_dims[1]={attr_len+1};
    void *attr_value;
    if(NXmalloc(&attr_value,1,attr_dims,attr_type)!=NX_OK)
      throw runtime_error("NXmalloc failed");
    if(NXgetattr(*handle,attr_name,attr_value,attr_dims,&attr_type)!=NX_OK)
      throw runtime_error("NXgetattr failed");
    Attr my_attr(attr_name,attr_value,attr_len,attr_type);
    if(NXfree(&attr_value)!=NX_OK)
      throw runtime_error("NXfree failed");
    attrs.push_back(my_attr);
  }

  // create the data
  node=Node(name,data,rank,dims,type);
  //node.set_name(name);
  //node.set_data(data,rank,dims,type);
  node.set_attrs(attrs);

  // free the temporary data
  if(NXfree(&data)!=NX_OK)
    throw runtime_error("NXfree failed");
}

void get_subtree(NXhandle *handle, NodeTree &tree, NodeTree::pre_order_iterator parent, string &name){
  //std::cout << "get_subtree(handle,tree,parent," << name << ")" << std::endl; // REMOVE
  static const string SDS="SDS";
  // open the correct level and deal with things correctly
  string type=nexus_util::open(handle,name);
  Node node(name,type);
  bool is_data=(type==SDS);

  // if we are at a data get it and return
  if(is_data){
    // get the data
    try{
      getDataNode(handle,name,node);
    }catch(runtime_error &e){ // cleanup and rethrow
      nexus_util::close(handle,node);
      throw;
    }
  }

  // add the node to the tree
  NodeTree::pre_order_iterator new_parent;
  if(parent==tree.end())
    new_parent=tree.insert(tree.begin(),node);
  else
    new_parent=tree.append_child(parent,node);

  if(!is_data){
    // get the listing and check result
    typedef vector<string> StringVec;
    StringVec listing=nexus_util::get_list(handle);
    if(listing.size()%2 || !listing.size()){
      nexus_util::close(handle,node);
      throw runtime_error("listing of NeXus file returned odd result");
    }
  
    // itterate through the listing
    for( StringVec::iterator it=listing.begin() ; it!=listing.end() ; it+=2 )
      get_subtree(handle,tree,new_parent,*it);
  }
  // close the path to the node
  nexus_util::close(handle,node);
}

/**
 * This is the method for retrieving data from a file. The whole
 * tree will be written to the new file immediately after being
 * called. Interpreting the string is left up to the implementing
 * code.
 */
void NexusRetriever::getData(const string &location, NodeTree &tree){
  //std::cout << "\"" << source << "\"." << "getData(" << location << ",tree)" << std::endl; // REMOVE
  if(location.size()<=0)
    throw invalid_argument("cannot parse empty string");

  // get the name
  vector<string> path=string_util::string_to_path(location);
  string name=*(path.rbegin());
  // remove the last bit from the path
  path.pop_back();

  // open the path to the node
  int num_group=0;
  int num_data=0;
  nexus_util::open_path(handle,path,num_group,num_data);
  // get the subtree
  if(num_data>0)
    getAttrAsNode(handle,tree,name);
  else
    get_subtree(handle,tree,tree.end(),name);

  // close the path to the node
  nexus_util::close_path(handle,num_group,num_data);
}

const string NexusRetriever::MIME_TYPE("application/x-NeXus");

string NexusRetriever::toString() const{
  return "["+MIME_TYPE+"] "+source;
}
