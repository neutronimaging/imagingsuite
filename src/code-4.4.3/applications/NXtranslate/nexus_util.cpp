#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "nexus_util.h"
#include "string_util.h"
#include "nxtranslate_debug.h"

// ----- start of declaring debug levels
#ifdef DEBUG3_NEXUS_UTIL
#define DEBUG2_NEXUS_UTIL
#endif
#ifdef DEBUG2_NEXUS_UTIL
#define DEBUG1_NEXUS_UTIL
#endif
// ----- end of declaring debug levels

using std::invalid_argument;
using std::runtime_error;
using std::string;
using std::vector;

typedef struct{
  string name;
  string type;
}SimpleNode;

typedef vector<string> StringVec;
typedef vector<SimpleNode> SimpleNodeVec;
typedef tree<Node> TreeNode;

static const int    GROUP_STRING_LEN = 128;
static const string NXROOT           = "NXroot";
static const string SDS              = "SDS";
static const string ATTR             = "ATTR";

static SimpleNodeVec get_groups(NXhandle *handle);

static bool node_exists(NXhandle *handle, const Node &node){
  SimpleNodeVec groups=get_groups(handle);

  for(SimpleNodeVec::const_iterator group=groups.begin() ; group!=groups.end() ; group++ ){
    if(group->name==node.name()){
      if(group->type==node.type())
        return true;
      if(node.is_data() && group->type==SDS)
        return true;
    }
  }

  return false;
}

extern void nexus_util::open(NXhandle *handle, const Node &node){
#ifdef DEBUG2_NEXUS_UTIL
  std::cout << "nexus_util::open(handle," << node.name() << ":" << node.type()
            << ")" << std::endl;
#endif
  // do nothing with a root
  if(node.name()==NXROOT) return;

  // the name is needed by all "make" functions
  char name[GROUP_STRING_LEN];
  strcpy(name,node.name().c_str());

  bool existing_node=node_exists(handle,node);

  if(node.is_data()){
    if(!existing_node){
      int type=(int)node.int_type(); // get the type and cast as an int
      int rank=node.rank();
      if(rank<=0) return;

      int dims[NX_MAXRANK];
      { // copy the dimensions from the node
        vector<int> vec_dim=node.dims();
        for( int i=0 ; i<rank ; i++ )
          dims[i]=vec_dim[i];
      }

      // create the data
      int comp_type=node.compress_type();
      if(comp_type==Node::COMP_NONE){
        //std::cout << "in open: " << node.type() << " " << *(node.dims().begin()) << std::endl; // REMOVE
        if(NXmakedata(*handle,name,type,rank,dims)!=NX_OK){
          std::cout << "NXmakedata failed" << std::endl;
          throw runtime_error("NXmakedata failed");
        }
      }else{
        vector<int> buff_size_vec=node.comp_buffer_dims();
        int *buff_size=new int[rank];
        for( size_t i=0 ; i<rank ; i++ ){
          buff_size[i]=buff_size_vec[i];
        }

        buff_size[rank-1]=dims[rank-1];
        if(NXcompmakedata(*handle,name,type,rank,dims,comp_type,buff_size)!=NX_OK){
          std::cout << "NXcompmakedata failed" << std::endl;
          throw runtime_error("NXcompmakedata failed");
        }
        delete buff_size; // FIXME - is this the correct form?
      }
    }
    // open the data that was just created
    if(NXopendata(*handle,name)!=NX_OK)
      throw runtime_error("NXopendata failed");
  }else{
    char type[GROUP_STRING_LEN];
    strcpy(type,node.type().c_str());
    if(!existing_node) 
      if(NXmakegroup(*handle,name,type)!=NX_OK)
        throw runtime_error("NXmakegroup failed");
    if(NXopengroup(*handle,name,type)!=NX_OK)
      throw runtime_error("NXopengroup failed");
  }
}

static void add_attribute(NXhandle *handle, const Node& node,
                                                           const int attr_num){
  Attr attr=(node.get_attr(attr_num));

  char name[GROUP_STRING_LEN];
  strcpy(name,attr.name().c_str());

  if(NXputattr(*handle,name,attr.value(),attr.length(),attr.type())!=NX_OK)
    throw runtime_error("NXputattr failed");
}

extern void nexus_util::make_data(NXhandle *handle, const Node& node){
  //std::cout << "make(" << node.name() << ")" << std::endl; // REMOVE
  // do nothing with a root
  if(node.name()==NXROOT) return;
  // do nothing unless this is a data
  if(!node.is_data()) return;

  // put in the attributes
  for( int i=0 ; i<node.num_attr() ; i++ )
    add_attribute(handle,node,i);

  // put in the data
  void *data(NULL);
  node.copy_data(data);
  if(data==NULL)
    throw runtime_error("CANNOT ADD NULL DATA");// << std::endl;
  if(NXputdata(*handle,data)!=NX_OK)
    throw runtime_error("NXputdata failed");
  NXfree(&data);
}

static void recurse_make_data(NXhandle *handle, const TreeNode &tree, const TreeNode::sibling_iterator &begin, const TreeNode::sibling_iterator &end){
  using namespace nexus_util;
  for( TreeNode::sibling_iterator sib=begin ; sib!=end ; sib++ ){
    open(handle,*sib);
    if(sib->is_data())
      make_data(handle,*sib);
    else
      recurse_make_data(handle,tree,tree.begin(sib),tree.end(sib));
    close(handle,*sib);
    sib.skip_children();
  }
}

extern void nexus_util::make_data(NXhandle *handle, const TreeNode &tree){
  //std::cout << "make_data(handle,tree[" << tree.size() << "])" << std::endl; // REMOVE
  // check that the tree is not empty
  if(tree.size()<=0)
    throw invalid_argument("Cannot write empty tree to file");

  // recursively write out the tree
  for( TreeNode::iterator it=tree.begin() ; it!=tree.end() ; it++ ){
    open(handle,*it);
    if(it->is_data())
      make_data(handle,*it);
    else
      recurse_make_data(handle,tree,tree.begin(it),tree.end(it));
    close(handle,*it);
    it.skip_children();
  }
}

extern void nexus_util::close(NXhandle *handle, const Node &node){
#ifdef DEBUG2_NEXUS_UTIL
  std::cout << "nexus_util::close(handle," << node.name() << ":" 
            << node.type() << ")" << std::endl;
#endif

  // do nothing with a root
  if(node.name()==NXROOT) return;

  if(node.is_data()){
    // check that there was something created
    if(node.rank()<=0) return;

    if(NXclosedata(*handle)!=NX_OK)
      throw runtime_error("NXclosedata failed");
  }else{
    if(NXclosegroup(*handle)!=NX_OK)
      throw runtime_error("NXclosegroup failed");
  }
}

static int num_group(NXhandle *handle){
  char group_name[GROUP_STRING_LEN];
  char group_class[GROUP_STRING_LEN];
  int num_groups;

  if(NXgetgroupinfo(*handle,&num_groups,group_name,group_class)!=NX_OK)
    throw runtime_error("NXgetgroupinfo failed");

  return num_groups;
}

static SimpleNode get_next_group(NXhandle *handle){
  //std::cout << "get_next_group(handle)" << std::endl;
  char name[GROUP_STRING_LEN], class_name[GROUP_STRING_LEN];
  int datatype;

  // get the information
  if(NXgetnextentry(*handle,name,class_name,&datatype)!=NX_OK)
    throw "NXgetnextentry failed";

  // copy it into the supplied node
  SimpleNode node;
  node.name=name;
  node.type=class_name;

  return node;
}

static bool node_is_okay(const SimpleNode& node){
  static const string UNKNOWN="UNKNOWN";
  if(node.name==UNKNOWN)
    return false;
  if(node.type==UNKNOWN)
    return false;
  if(node.type=="CDF0.0")
    return false;
  return true;
}

static SimpleNodeVec get_groups(NXhandle *handle){
  //std::cout << "get_groups(handle)" << std::endl; // REMOVE

  // reset the directory
  if(NXinitgroupdir(*handle)!=NX_OK)
    throw runtime_error("NXinitgroupdir failed");

  // determine the number of groups at the current location
  int num_groups = 0;
  try{
    num_groups=num_group(handle);
  }catch(runtime_error &e){
    // let it drop on the floor
  }

  // set up for the listing
  SimpleNodeVec result;
  if(num_groups<=0) return result;

  // get the listing
  SimpleNode node;
  for( int i=0 ; i<num_groups ; i++ ){
    node=get_next_group(handle);
    if(node_is_okay(node))
      result.push_back(node);
  }

  return result;
}

extern vector<string> nexus_util::get_list(NXhandle *handle){
  SimpleNodeVec vec=get_groups(handle);

  vector<string> result;
  for( SimpleNodeVec::const_iterator it=vec.begin() ; it!=vec.end() ; it++ ){
    result.push_back(it->name);
    result.push_back(it->type);
  }

  return result;
}

static string get_type(NXhandle *handle, const string &name){
  SimpleNodeVec groups=get_groups(handle);

  for(SimpleNodeVec::const_iterator group=groups.begin() ; group!=groups.end() ; group++ ){
    if(group->name==name)
      return group->type;
  }
  return "";
}

static void open_node(NXhandle *handle, const string &name, const string &type, int &num_group, int &num_data){
  // do nothing for attributes
  if(type==ATTR)
    return;

  // everything else needs a name
  char ch_name[GROUP_STRING_LEN];
  strcpy(ch_name,name.c_str());

  // open a data if appropriate
  if(type==SDS){
    if(NXopendata(*handle,ch_name)!=NX_OK)
      throw runtime_error("NXopendata failed");
    num_data++;
    return;
  }

  // open the group
  char ch_type[GROUP_STRING_LEN];
  strcpy(ch_type,type.c_str());
  if(NXopengroup(*handle,ch_name,ch_type)!=NX_OK)
    throw runtime_error("NXopengroup failed");
  num_group++;
}

extern string nexus_util::open(NXhandle *handle, const string &name){
  string type=get_type(handle,name);
  if(type.size()<=0)
    throw runtime_error(string("could not find type for node[")+name+"]");
  int num_group=0;
  int num_data=0;
  open_node(handle,name,type,num_group,num_data);
  return type;
}

static string path_to_str(const StringVec &path){
  string str="";
  for( StringVec::const_iterator it=path.begin() ; it!=path.end() ; it++ )
    str+=("/"+*it);
  return str;
}

extern void nexus_util::open_path(NXhandle *handle, const StringVec &path, int &num_group, int &num_data){
#ifdef DEBUG1_NEXUS_UTIL
  std::cout << "open_path(" << handle << ",";
  for( StringVec::const_iterator it=path.begin() ; it!=path.end() ; it++ )
    std::cout << "/" << *it;
  std::cout << "," << num_group << "," << num_data << ")" << std::endl;
#endif
  for( StringVec::const_iterator it=path.begin() ; it!=path.end() ; it++ ){
    string type=get_type(handle,*it);
    if(type.size()<=0){
      close_path(handle,num_group,num_data);
      throw runtime_error("Path ["+path_to_str(path)+"] did not exist");
    }
    try{
      open_node(handle,*it,type,num_group,num_data);
    }catch(runtime_error &e){
      throw  runtime_error("Could not open path ["+path_to_str(path)+"]");
    }
  }
}

extern void nexus_util::close_path(NXhandle *handle, int &num_group, int &num_data){
#ifdef DEBUG1_NEXUS_UTIL
  std::cout << "close_path(" << handle << "," << num_group << ","
            << num_data << ")" << std::endl;
#endif
  if( (num_group<=0) && (num_data<=0) ){
    num_group=0;
    num_data=0;
    return;
  }

  if(num_data){
    if(NXclosedata(*handle)!=NX_OK)
      throw runtime_error("NXclosedata failed");
    num_data--;
  }

  for( ; num_group>0 ; num_group-- ){
    if(NXclosegroup(*handle)!=NX_OK)
      throw runtime_error("NXclosegroup failed");
  }
}

extern size_t nexus_util::calc_size(int rank, int *dims, int type){
  // check that the rank is reasonable
  if(rank<=0) throw invalid_argument("Do not understand rank<=0");

  // determine how much to copy
  size_t size=1;
  for( int i=0 ; i<rank ; i++ )
    size*=dims[i];
  if(type==NX_CHAR || type==NX_INT8 || type==NX_UINT8){
    // size is already correct
  }else if(type==NX_INT16 || type==NX_UINT16){
    size*=2;
  }else if(type==NX_INT32 || type==NX_UINT32 || type==NX_FLOAT32){
    size*=4;
  }else if(type==NX_FLOAT64 || type==NX_UINT64 || type==NX_INT64){
    size*=8;
  }else{
    throw invalid_argument("Did not understand type in nexus_util::calc_size");
  }

  return size;
}

extern void nexus_util::make_link(NXhandle *handle, const vector<string> &link, const vector<string> &source){
/*
  // ********** begin debug print
  std::cout << "LINKING ";
  for( vector<string>::const_iterator it=link.begin() ; it!=link.end() ; it++ )
    std::cout << "/" << *it;
  std::cout << " -> ";
  for( vector<string>::const_iterator it=source.begin() ; it!=source.end() ; it++ )
    std::cout << "/" << *it;
  std::cout << std::endl;
  // ********** end debug print
*/
  // keep track of location in file
  NXlink *link_id=(NXlink *)malloc(sizeof(NXlink));
  int num_group=0;
  int num_data=0;

  // open the path to the source
  nexus_util::open_path(handle,source,num_group,num_data);

  // get the link_id
  if(num_data>0){
    if(NXgetdataID(*handle,link_id)!=NX_OK)
      throw new runtime_error("could not get data ID for linking");
  }else{
    if(NXgetgroupID(*handle,link_id)!=NX_OK)
      throw new runtime_error("could not get group ID for linking");
  }

  // return to the root node in the file
  nexus_util::close_path(handle,num_group,num_data);

  // open to link's parent location
  nexus_util::open_path(handle,link,num_group,num_data);

  // create the link
  if(NXmakelink(*handle,link_id)!=NX_OK)
    throw new runtime_error("could not create link");

  // return to the root node in the file
  nexus_util::close_path(handle,num_group,num_data);

  // free up the NXlink variable
  free(link_id);
}
