/*
 * Copyright (c) 2004, P.F.Peterson <petersonpf@ornl.gov>
 *               Spallation Neutron Source at Oak Ridge National Laboratory
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef NXDIR_H
#define NXDIR_H
/* Default values for various command line options */
#define DEFAULT_MAX_LENGTH 10   // number of array elements to print
#define DEFAULT_RECURSE    1    // negative value means all
#define DEFAULT_PRINT_DATA false // can be true or false
#define DEFAULT_PATH_MODE NAME // can be NAME, TYPE, or NAME_TYPE
#define DEFAULT_TREE_MODE TREE_SCRIPT // can be TREE_SCRIPT, TREE_MULTI,
                                      // or TREE_TREE
#define DEFAULT_DATA_MODE DATA_SCRIPT // can be DATA_SCRIPT
#define DEFAULT_OUT_LINE_MODE OUT_SINGLE // can be OUT_SINGLE

/* -------------------- DO NOT EDIT BELOW THIS LINE -------------------- */
#include "napiconfig.h"
#include "napi.h"
#include <string>
#include <vector>
#include <iostream>

#define GROUP_STRING_LEN 128
#define SDS "SDS"
#define ATTR "ATTR"

typedef struct{
  std::string name;
  std::string type;
}Node;

typedef std::vector<Node> Path;
typedef std::vector<Path> Tree;
typedef std::vector<std::string> StringVector;

// enumerations
enum NXdir_status{NXDIR_FAIL,NXDIR_OK};
enum TreeMode{TREE_SCRIPT,TREE_MULTI,TREE_TREE};
enum PathMode{NAME,TYPE,NAME_TYPE};
enum DataMode{DATA_SCRIPT};
enum OutputLineMode{OUT_SINGLE};

// print definition config
typedef struct{
  std::string filename;
  TreeMode tree_mode;
  PathMode path_mode;
  DataMode data_mode;
  OutputLineMode output_line_mode;
  bool print_data; // print array values to cout
  bool show_filename;
  int max_length;
  std::string dump_data_file; // binary dump of data
  std::string data_out_file; // write data as formatted ascii into file
}PrintConfig;

// -------------------- data.cpp
extern StringVector read_all_dims_as_string(NXhandle handle, const Tree tree);
extern StringVector read_all_data_as_string(NXhandle handle, const Tree tree,
                                                     const PrintConfig config);
extern std::string read_attr_as_string(NXhandle handle, const Path &path,
                                                    const PrintConfig &config);
extern long read_int_attr(NXhandle handle, const Path &path);

// -------------------- string_util.cpp
extern StringVector arrayify(const std::string path);
extern int str_to_int(const std::string arg);
extern void print_path(const Path path, const PathMode print_mode);
extern void print_tree(const Tree tree, const PathMode print_mode);
extern void print_strvec(const StringVector &vec);
extern void print_tree_result(NXhandle handle, const Tree tree,
                                               const PrintConfig print_config);
extern std::string path_to_str(const Path path, const PathMode print_mode);
extern std::string oneD_to_string(const void *data, const int length,
                                  const int type, const PrintConfig config);
extern std::string to_string(const void *data, const int dims[],
                     const int rank, const int type, const PrintConfig config);
extern std::string int_to_str(const long value);
template <typename T>
extern std::string value_to_string(const T value);
extern std::string voidptr_to_str(const void *data, int pos,int type);

// -------------------- tree.cpp
extern Tree purge_tree(const Tree tree, const StringVector rel_path, const int rel_recurse);
extern Tree build_tree(NXhandle handle, const StringVector abs_path,
                                                        const int num_recurse);
extern Tree build_rel_tree(NXhandle handle, const Path &path, int num_recurse);
extern bool has_attr(NXhandle handle,const std::string name, int *length,
                                                                    int *type);
extern int open_path(NXhandle handle, const Path path);
extern int close_path(NXhandle handle, const Path path);
extern bool compPath(const Path path1, const Path path2);

// -------------------- data_writer.cpp
extern void write_data(std::ostream &out,NXhandle handle, const Tree &tree,
                                                    const PrintConfig &config);
extern void dump_data(std::string &filename,NXhandle handle, const Tree &tree,
                      const PrintConfig &config);
extern Tree remove_nondata(const Tree &tree);
#endif
