#ifndef __NEXUS_UTIL_H_GUARD
#define __NEXUS_UTIL_H_GUARD

#include <napiconfig.h>
#include <napi.h>
#include <string>
#include "node.h"
#include "tree.hh"

namespace nexus_util{
  // open the group or data
  extern void open(NXhandle *handle, const Node &node);
  // open the group or data
  extern std::string open(NXhandle *handle, const std::string &name);
  // fill the opened data with contents of node
  extern void make_data(NXhandle *handle, const Node& node);
  // create a tree, closing the whole thing when done
  extern void make_data(NXhandle *handle, const tree<Node> &tree);
  // close the group or data
  extern void close(NXhandle *handle, const Node &node);
  // open to a specific place
  extern void open_path(NXhandle *handle, const std::vector<std::string> &path, int &num_group, int &num_data);
  // close from a specific place
  extern void close_path(NXhandle *handle, int &num_group, int &num_data);
  // calculate size of a data for copying
  extern size_t calc_size(int rank, int *dims, int type);
  // create a listing of the currently open level. In pairs of (name,type).
  extern std::vector<std::string> get_list(NXhandle *handle);
  // creates a link from link to source
  extern void make_link(NXhandle *handle, const std::vector<std::string> &link, const std::vector<std::string> &source);
}
#endif

