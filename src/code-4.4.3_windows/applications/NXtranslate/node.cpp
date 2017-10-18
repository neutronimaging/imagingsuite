#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>
#include "nexus_util.h"
#include "node.h"
#include "string_util.h"

using std::invalid_argument;
using std::runtime_error;
using std::logic_error;
using std::out_of_range;
using std::string;
using std::vector;

static const string LEFT  = "[";
static const string RIGHT = "]";

static string convert_type(int type){
  if(type==Node::CHAR)
    return "NX_CHAR";
  else if(type==Node::FLOAT32)
    return "NX_FLOAT32";
  else if(type==Node::FLOAT64)
    return "NX_FLOAT64";
  else if(type==Node::INT8)
    return "NX_INT8";
  else if(type==Node::INT16)
    return "NX_INT16";
  else if(type==Node::INT32)
    return "NX_INT32";
  else if(type==Node::INT64)
    return "NX_INT64";
  else if(type==Node::UINT8)
    return "NX_UINT8";
  else if(type==Node::UINT16)
    return "NX_UINT16";
  else if(type==Node::UINT32)
    return "NX_UINT32";
  else if(type==Node::UINT64)
    return "NX_UINT64";
  else
    throw runtime_error("Did not understand type in set_data");
}

// ==================== Node implementation
Node::Node(const std::string &name, const std::string &type):__name(name),__type(type),_value(NULL),__comp_type(COMP_NONE),__ref_count(new size_t(1)){
  // determine if this is data
  try{
    Node::NXtype int_type=Node::int_type(); // throws an exception if not data
    __is_data=true;
  }catch(runtime_error &err){
    __is_data=false;
  }
}

Node::Node(const Node &old): __name(old.__name), __type(old.__type), __is_data(old.__is_data), __dims(old.__dims), _value(old._value), __attrs(old.__attrs), __comp_type(old.__comp_type), __ref_count(old.__ref_count){
  (*__ref_count)++;
  //std::cout << "Node(" << __name << ")" << std::endl; // REMOVE
}

Node::Node(const string &name, void * data, const int rank, const int* dims, const int type): __name(name), __is_data(true), __comp_type(COMP_NONE), __ref_count(new size_t(1)){
  this->set_data(data,rank,dims,type);
}

Node& Node::operator=(const Node &old){
  if(this==&old) return *this;

  // delete the old value (if necessary)
  (*__ref_count)--;
  if( (__is_data) && (*__ref_count==0) && (_value!=NULL) )
    NXfree(&_value);

  // copy everything except the value
  __name=old.__name;
  __type=old.__type;
  __is_data=old.__is_data;
  __dims.clear();
  __dims.insert(__dims.begin(),old.__dims.begin(),old.__dims.end());
  __attrs.clear();
  __attrs.insert(__attrs.begin(),old.__attrs.begin(),old.__attrs.end());
  __comp_type=old.__comp_type;
  __ref_count=new size_t(1);
  _value=NULL;


  // copy the value
  if(__is_data){
    int rank=__dims.size();
    int dims[NX_MAXRANK];
    for( int i=0 ; i<rank ; i++ )
      dims[i]=__dims[i];
    int type=this->int_type();

    // allocate space for the data
    NXmalloc(&_value,rank,dims,type);

    // determine how much to copy
    size_t size=nexus_util::calc_size(rank,dims,type);

    // copy the array
    memcpy(_value,old._value,size);
  }

  return *this;
}

Node::~Node(){
  //std::cout << "~Node(" << __name << "," << *__ref_count << ")" << std::endl; // REMOVE
  (*__ref_count)--;
  if(*__ref_count==0){
    if(_value!=NULL){
      if(NXfree(&_value)!=NX_OK)
        throw runtime_error("NXfree failed in destructor");
      _value=NULL;
    }
    delete __ref_count;
  }
}

const string Node::name() const{
  return __name;
}

const string Node::type() const{
  return __type;
}

const bool Node::is_data() const{
  return __is_data;
}

const int Node::rank() const{
  if(!__is_data)
    throw logic_error("node is not data");

  return __dims.size();
}

const vector<int> Node::dims() const{
  vector<int> vec(__dims.begin(),__dims.end());

  return vec;
}

const Node::NXtype Node::int_type() const{
  if(__type=="NX_CHAR")
    return CHAR;
  else if(__type=="NX_FLOAT32")
    return FLOAT32;
  else if(__type=="NX_FLOAT64")
    return FLOAT64;
  else if(__type=="NX_INT8")
    return INT8;
  else if(__type=="NX_INT16")
    return INT16;
  else if(__type=="NX_INT32")
    return INT32;
  else if(__type=="NX_INT64")
    return INT64;
  else if(__type=="NX_UINT8")
    return UINT8;
  else if(__type=="NX_UINT16")
    return UINT16;
  else if(__type=="NX_UINT32")
    return UINT32;
  else if(__type=="NX_UINT64")
    return UINT64;
  else
    throw runtime_error("do not understand type");
}

const Node::NXcompress Node::compress_type() const{
  return __comp_type;
}

const std::vector<int> Node::comp_buffer_dims() const{
  return __comp_buffer_dims;
}

void* Node::data() const{
  return _value;
}

void Node::copy_data(void *&data)const{
  // determine the type
  int type=int_type();

  // determine the rank
  int int_rank=rank();

  // int array version of dimensions
  int my_dims[NX_MAXRANK];
  for( int i=0 ; i<int_rank ; i++ )
    my_dims[i]=__dims[i];


  // allocate space for the data
  NXmalloc(&data,int_rank,my_dims,type);

  // determine how much to copy
  size_t size=nexus_util::calc_size(int_rank,my_dims,type);

  // copy the array
  memcpy(data,_value,size);
}

const int Node::num_attr() const{
  return (__attrs.size());
}

Attr Node::get_attr(const int attr_num) const{
  if(attr_num>this->num_attr())
    throw out_of_range("asked for attribute with higher index than size");
  return (*(__attrs.begin()+attr_num));
}

const void Node::set_comp(const string &comp_type){
  if(string_util::starts_with(comp_type,"NONE")){
    __comp_type=COMP_NONE;
  }else if(string_util::starts_with(comp_type,"LZW")){
    __comp_type=COMP_LZW;
  }else if(string_util::starts_with(comp_type,"RLE")){
    __comp_type=COMP_RLE;
  }else if(string_util::starts_with(comp_type,"HUF")){
    __comp_type=COMP_HUF;
  }else{
    throw invalid_argument("Do not understand compression type: "+comp_type);
  }

  // work with user specified dimension information
  vector<string> temp=string_util::split(comp_type,":");
  if(temp.size()==2){
    __comp_buffer_dims=string_util::str_to_intVec(*(temp.rbegin()));
  }

  // use default if anything is wrong
  if(__comp_buffer_dims.size()!=__dims.size()){
    __comp_buffer_dims=__dims;
    for( size_t i=0 ; i<__comp_buffer_dims.size()-1 ; ++i ){
      __comp_buffer_dims[i]=1;
    }
  }
}

const void Node::set_name(const string &name){
  __name=name;
}

const void Node::set_data(void *&data,const int irank, const int* dims,const int type){
  // copy the dimensions
  __dims.clear();
  int my_dims[NX_MAXRANK];
  for( int i=0 ; i<irank ; i++ ){
    __dims.push_back(dims[i]);
    my_dims[i]=dims[i];
  }

  // copy the type
  /* __type=str_type; */
  __type=convert_type(type);

  // allocate space for the data
  NXmalloc(&_value,irank,my_dims,type);

  // determine how much to copy
  size_t size=nexus_util::calc_size(irank,my_dims,type);

  // copy the array
  memcpy(_value,data,size);

  // set that this is a data
  __is_data=true;
}

const void Node::set_attrs(const vector<Attr> &attrs){
  __attrs.clear();
  __attrs.insert(__attrs.begin(),attrs.begin(),attrs.end());
}

const void Node::update_attr(Attr &attr){
  for( vector<Attr>::iterator it=__attrs.begin() ; it!=__attrs.end() ; it++ ){
    if(it->name()==attr.name()){
      __attrs.erase(it);
      it--;
    }
  }
  if(attr.length()>0)
    __attrs.push_back(attr);
}

const void Node::update_attrs(vector<Attr> &attrs){
  for( vector<Attr>::iterator it=attrs.begin() ; it!=attrs.end() ; it++ )
    update_attr(*it);
}

const void Node::update_dims(std::vector<int> &dims){
  // make sure that the caller is trying to do something
  if(dims.size()<=0) return;

  // compare sizes
  int old_total_size=1;
  for( unsigned int i=0 ; i<__dims.size() ; i++ )
    old_total_size*=__dims[i];
  int new_total_size=1;
  for( unsigned int i=0 ; i<dims.size() ; i++ )
    new_total_size*=dims[i];

  // throw an exception if the total number of elements is not conserved
  if(old_total_size!=new_total_size)
    throw invalid_argument("old and new dimensions must describe same number of elements");

  // set the new dimensions
  __dims=dims;
}
