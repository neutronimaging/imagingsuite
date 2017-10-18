#ifndef __NODE_UTIL_H_GUARD
#define __NODE_UTIL_H_GUARD

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include "node.h"
//#include "nexus_util.h"
//#include "retriever.h"
//#include "xml_parser.h"
//#include "string_util.h"
//#include "Ptr.h"
//#include "tree.hh"
//#include "nxtranslate_debug.h"

extern void void_ptr_from_string(void *&value, std::string &char_data,
                                 int rank, int *dims, Node::NXtype type);
extern void update_node_from_string(Node &node, std::string &char_data,
                                  std::vector<int> &v_dims, Node::NXtype type);
extern Node::NXtype node_type(const std::string &str);
extern Attr make_attr(const std::string &name, const std::string &value);
#endif

