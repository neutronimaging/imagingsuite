#ifndef __NODE_H_GUARD
#define __NODE_H_GUARD

#include <vector>
#include <string>
#include <napiconfig.h>
#include <napi.h>
#include <iostream>
#include "attr.h"

// ==================== Node definition
class Node{
 public:
  // enum for types
  enum NXtype{CHAR=NX_CHAR,   FLOAT32=NX_FLOAT32, FLOAT64=NX_FLOAT64,
              INT8=NX_INT8,   INT16=NX_INT16,     INT32=NX_INT32, INT64=NX_INT64,
              UINT8=NX_UINT8, UINT16=NX_UINT16,   UINT32=NX_UINT32, UINT64=NX_UINT64,
              GROUP};
  enum NXcompress{COMP_NONE=NX_COMP_NONE, COMP_LZW=NX_COMP_LZW,
                  COMP_RLE=NX_COMP_RLE,   COMP_HUF=NX_COMP_HUF};

  // constructors and copy control
  Node(const std::string &name, const std::string &type); // for creating a group
  Node(const std::string &name, void * data, const int rank, const int* dims, const int type);
  Node(const Node &); // copies share reference to the data
  ~Node();
  Node& operator=(const Node &); // asignment operator

  // accesor methods
  const std::string name() const;
  const std::string type() const;
  const bool is_data() const;
  const int rank() const;
  const std::vector<int> dims() const;
  const std::vector<int> comp_buffer_dims() const;
  const NXtype int_type() const;
  const NXcompress compress_type() const;
  void* data() const; // do not mutate value
  void copy_data(void *&)const;
  const int num_attr() const;
  Attr get_attr(const int) const;

  // mutator methods
  const void set_comp(const std::string &comp_type);
  const void set_name(const std::string &name);
  const void set_data(void *&data,const int rank,const int* dims,const int type);
  const void set_attrs(const std::vector<Attr> &attrs);
  const void update_attrs(std::vector<Attr> &attrs);
  const void update_dims(std::vector<int> &dims);

 private:
  const void update_attr(Attr &attr);

  std::string __name;
  std::string __type;
  bool __is_data;
  std::vector<int> __dims;
  std::vector<int> __comp_buffer_dims;
  void *_value;
  std::vector<Attr> __attrs;
  NXcompress __comp_type;
  std::size_t* __ref_count;
};
#endif
