#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>
#include "attr.h"
#include "nexus_util.h"

using std::runtime_error;
using std::logic_error;
using std::out_of_range;
using std::string;
using std::vector;

// ==================== Node implementation
Attr::Attr(const string name, const void* val,const int len, const int type): __name(name), __length(len), __type(type){
  // create dimensions
  int dims[1]={__length};

  // copy the value
  NXmalloc(&_value,1,dims,__type);
  size_t size=nexus_util::calc_size(1,dims,__type);
  memcpy(_value,val,size);
}

Attr::Attr(const Attr& old): __name(old.__name), __length(old.__length), __type(old.__type){
  // create dimensions
  int dims[1]={__length};

  // copy the value
  NXmalloc(&_value,1,dims,__type);
  size_t size=nexus_util::calc_size(1,dims,__type);
  memcpy(_value,old._value,size);
}

Attr::~Attr(){
  if(NXfree(&_value)!=NX_OK)
    throw runtime_error("NXfree failed");
}

Attr& Attr::operator=(const Attr& old){
  if(this==&old) return *this;

  if(NXfree(&_value)!=NX_OK)
    throw runtime_error("NXfree failed");

  // copy everything other than the value
  __name=old.__name;
  __length=old.__length;
  __type=old.__type;

  // create dimensions
  int dims[1]={__length};

  // copy the value
  NXmalloc(&_value,1,dims,__type);
  size_t size=nexus_util::calc_size(1,dims,__type);
  memcpy(_value,old._value,size);

  return *this;
}

std::string Attr::name(){
  return __name;
}

void* Attr::value(){
  return _value;
}

int Attr::length(){
  return __length;
}

int Attr::type(){
  return __type;
}
