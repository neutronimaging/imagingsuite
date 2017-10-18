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
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "nxdir.h"
//#include <utility> // REMOVE

#define NXDATA "NXdata"

using std::string;
using std::cout;
using std::endl;
using std::ostream;

typedef std::vector<Node> NodeVector;
typedef std::vector<Path> PathVector;
typedef std::map<std::string,std::string> PathMap;

// define a "data" structure
typedef struct{
  Path path;
  string name;
  string units;
  int type;
  int rank;
  int dims[NX_MAXRANK];
  int signal;
  void *data;
  PathMap unknown;
}Data;

// define an "axis" structure
typedef struct{
  Path path;
  string name;
  string units;
  int type;
  int dims[NX_MAXRANK];
  int axis;
  int primary;
  void *data;
  PathMap unknown;
}Axis;

static bool path_is_okay(const Path &path){
  if(path.rbegin()->type==NXDATA)
    return true;
  else
    return false;
}

extern Tree remove_nondata(const Tree &tree){
  if(tree.empty())
    return tree;

  Tree good;
  for( Tree::const_iterator path=tree.begin() ; path!=tree.end() ; path++ ){
    if(path_is_okay(*path))
      good.push_back(*path);
  }

  return good;
}

static Path copy_non_attr(const Path &path){
  Path new_path;
  for( Path::const_iterator node=path.begin() ; node!=path.end() ; node++ ){
    if(node->type!=ATTR)
      new_path.push_back(*node);
    else
      break;
  }
  return new_path;
}

static bool contains(const PathVector &vec, const Path &path){
  for( PathVector::const_iterator comp_path=vec.begin() ; comp_path!=vec.end() ;comp_path++ ){
    if(compPath(*comp_path,path)==compPath(path,*comp_path))
      return true;
  }
  return false;
}

static PathVector find_int_attr(NXhandle handle, const Tree &tree,
                                   const string attr_name, const int attr_val){
  PathVector attr_tree;

  // find the named attribute
  for(Tree::const_iterator path=tree.begin() ; path!=tree.end() ; path++ ){
    Node node=*((*path).rbegin());
    if(node.type!=ATTR)
      continue;
    if(node.name==attr_name){
      if(read_int_attr(handle,*path)==attr_val){
        Path short_path=copy_non_attr(*path);
        if(!contains(attr_tree,short_path))
          attr_tree.push_back(short_path);
      }
    }
  }

  if(attr_tree.size()<=0)
    throw "did not find attribute";
  else 
    return attr_tree;
}

static bool starts_with(const Path &path, const Path &begin){
  Path::const_iterator node=path.begin();

  for( Path::const_iterator begin_node=begin.begin() ; begin_node!=begin.end() ; begin_node++ ){
    if(begin_node->type!=node->type)
      return false;
    if(begin_node->name!=node->name)
      return false;
    node++;
  }

  return true;
}

static void fill_axis(NXhandle handle, Axis &axis, PathVector &tree,
                                                    const PrintConfig &config){
  axis.name=(axis.path.rbegin())->name;
  axis.type=-1;
  axis.dims[0]=-1;

  Path units_path;
  Path primary_path;
  Path axis_path;
  PathVector attr_paths;

  // sort out everything
  for( PathVector::const_iterator path=tree.begin() ; path!=tree.end() ; 
                                                                      path++ ){
    Node end_node=*((*path).rbegin());
    if(end_node.type==ATTR){
      if(end_node.name=="units"){
        units_path=*path;
      }else if(end_node.name=="primary"){
        primary_path=*path;
      }else if(end_node.name=="axis"){
        axis_path=*path;
      }else{
        attr_paths.push_back(*path);
      }
    }
  }

  // get the attributes and fill the structure
  if(units_path.size()>0)
    axis.units=read_attr_as_string(handle,units_path,config);
  if(axis_path.size()>0)
    axis.axis=read_int_attr(handle,axis_path);
  if(primary_path.size()>0)
    axis.primary=read_int_attr(handle,primary_path);
  else
    axis.primary=-1;

  // get the unknown attributes
  for( PathVector::const_iterator path=attr_paths.begin() ; path!=attr_paths.end() ; path++ ){
    string value=read_attr_as_string(handle,*path,config);
    string key=(*path).rbegin()->name;

    axis.unknown[key]=value;
  }

  // -- from here down gets the data
  open_path(handle,axis.path);

  // get the rank, dimensions and type
  int rank;
  if(NXgetinfo(handle,&rank,axis.dims,&axis.type)!=NX_OK){
    close_path(handle,axis.path);
    throw "NXgetinfo failed";
  }

  if(rank>1){
    close_path(handle,axis.path);
    throw "RANK>1";
  }

  // allocate space for the data
  if(NXmalloc(&axis.data,rank,axis.dims,axis.type)!=NX_OK){
    close_path(handle,axis.path);
    throw "NXmalloc failed";
  }

  // retrieve the data from the file
  if(NXgetdata(handle,axis.data)!=NX_OK){
    close_path(handle,axis.path);
    throw "NXgetdata failed";
  }

  // close the path
  close_path(handle,axis.path);
}

static void fill_data(NXhandle handle, Data &data, PathVector &data_tree,
                                                    const PrintConfig &config){
  // local variables to put stuff in
  Path units_path;
  Path signal_path;
  PathVector attr_paths;

  // sort out everything
  for( PathVector::const_iterator path=data_tree.begin() ; path!=data_tree.end() ; path++ ){
    Node end_node=*((*path).rbegin());
    if(end_node.type==ATTR){
      if(end_node.name=="units"){
        units_path=*path;
      }else if(end_node.name=="signal"){
        signal_path=*path;
      }else{
        attr_paths.push_back(*path);
      }
    }else{
      data.path=*path;
      data.name=end_node.name;
      //data.type=end_node.type;
    }
  }

  // get the attributes and fill the structure
  data.units=read_attr_as_string(handle,units_path,config);
  data.signal=read_int_attr(handle,signal_path);

  // get the unknown attributes
  for( PathVector::const_iterator path=attr_paths.begin() ; path!=attr_paths.end() ; path++ ){
    string value=read_attr_as_string(handle,*path,config);
    string key=(*path).rbegin()->name;
    
    data.unknown[key]=value;
  }

  // ----- from here down  gets the data

  // open up to the right place
  open_path(handle,data.path);

  // get the rank, dimensions and type
  if(NXgetinfo(handle,&data.rank,data.dims,&data.type)!=NX_OK){
    close_path(handle,data.path);
    throw "NXgetinfo failed";
  }

  // allocate space for the data
  if(NXmalloc(&data.data,data.rank,data.dims,data.type)!=NX_OK){
    close_path(handle,data.path);
    throw "NXmalloc failed";
  }

  // retrieve the data from the file
  if(NXgetdata(handle,data.data)!=NX_OK){
    close_path(handle,data.path);
    throw "NXgetdata failed";
  }

  // close the path
  close_path(handle,data.path);
}

static void split_vec(PathVector &full_vec, PathVector &new_vec,
                                                       const Path &split_path){
  PathVector old_full;
  old_full.insert(old_full.begin(),full_vec.begin(),full_vec.end());
  full_vec.erase(full_vec.begin(),full_vec.end());
  
  for( PathVector::const_iterator path=old_full.begin() ; path!=old_full.end()
                                                              ; path++ ){
    if(starts_with(*path,split_path))
      new_vec.push_back(*path);
    else
      full_vec.push_back(*path);
  }
}

static string row_to_str(const void *data, const int offset, const int row_len, const int type, const PrintConfig config){
  string result="";
  for( int i=0 ; i<row_len ; i++ ){
    result+=voidptr_to_str(data,offset+i,type);
    if(i+1<row_len)
      result+=" ";
  }
  return result;
}

static void write_overview(ostream &out, const Path &path, const PrintConfig &config){
  out << "# ORIGINAL FILE: " << config.filename << endl;
  out << "# NXdata LOCATION: " << path_to_str(path,NAME) << endl;
}

static void write_data_header(ostream &out, const Data &data,const PrintConfig &config){
  out << "# DATA(signal=" << data.signal << "): " << data.name;
  if(data.rank==1)
    out << "[";
  out << oneD_to_string(data.dims,data.rank,NX_INT16,config);
  if(data.rank==1)
    out << "]";
  out << " (" << data.units << ")" << endl;
  for( PathMap::const_iterator pair=data.unknown.begin() ; pair!=data.unknown.end() ; pair++ ){
    out << "#     " << pair->first <<  "=" << pair->second << endl;
  }
}

static void write_axis_header(ostream &out, const Axis &axis, const PrintConfig &config){
  out << "# AXIS";
  if(axis.primary>0)
    out << "(primary=" << axis.primary << ")";
  out << ": " << axis.name << "[" << oneD_to_string(axis.dims,1,NX_INT16,config) << "] (" << axis.units << ")" << endl;
  for( PathMap::const_iterator pair=axis.unknown.begin() ; pair!=axis.unknown.end() ; pair++ ){
    out << "#     " << pair->first <<  "=" << pair->second << endl;
  }
}

static void write_1D_data(ostream &out,const Path &path, const Data &data,
                                  const Axis &axis, const PrintConfig &config){
  // overview information
  write_overview(out,path,config);

  // information about data
  write_data_header(out,data,config);

  // information about axis
  write_axis_header(out,axis,config);

  // space between header and data
  out << endl;

  // column headings
  out << "#  " << axis.name << "     " << data.name << endl;

  // figure out the range and if there is one more "axis" than "data"
  int range=axis.dims[0];
  bool is_histogram=false;
  if(data.dims[0]<range)
    is_histogram=1;
  else if(data.dims[0]>range)
    range=data.dims[0];

  // print data itself
  for( int i=0 ; i<range ; i++ ){
    out << voidptr_to_str(axis.data,i,axis.type);
    if(!is_histogram || i<range-1)
      out << "  " << voidptr_to_str(data.data,i,data.type);
    out << endl;
  }
}

static void write_2D_data(ostream &out, const Path &path, const Data &data,
              const Axis &axis1, const Axis &axis2, const PrintConfig &config){
  // overview information
  write_overview(out,path,config);
  
  // information about axis1
  write_axis_header(out,axis1,config);
  out << row_to_str(axis1.data,0,axis1.dims[0],axis1.type,config) << endl;
  out << endl;

  // information about axis1
  write_axis_header(out,axis2,config);
  out << row_to_str(axis2.data,0,axis2.dims[0],axis2.type,config) << endl;
  out << endl;

  // information about data
  write_data_header(out,data,config);
  for( int i=0 ; i<data.dims[0] ; i++ )
    out << row_to_str(data.data,i,data.dims[1],data.type,config) << endl;
}

static void write_one_data(ostream &out, NXhandle handle, const Path &path,
                                                    const PrintConfig &config){
  Tree tree=build_rel_tree(handle,path,-1); // build full tree from this point

  Path data_path;
  try{
    PathVector vec=find_int_attr(handle,tree,"signal",1);
    if(vec.size()==1)
      data_path=*(vec.begin());
  }catch( const char *str){
    cout << endl;
    return;
  }

  PathVector data_vec;
  split_vec(tree,data_vec,data_path);

  Data data;
  fill_data(handle,data,data_vec,config);

  // check the rank for filling axes
  Axis axis1;
  try{
    PathVector vec=find_int_attr(handle,tree,"axis",1);
    if(vec.size()==1)
      axis1.path=*(vec.begin());
    else
      throw "more than one axis=1";
  }catch(const char *str){
    cout << "!!!!! random stuff wrong finding axis=1" << endl;
    return;
  }
  PathVector axis1_vec;
  split_vec(tree,axis1_vec,axis1.path);
  fill_axis(handle,axis1,axis1_vec,config);

  // print out the results for 1D data
  if(data.rank==1){
    write_1D_data(out,path,data,axis1,config);
    return;
  }

  Axis axis2;
  try{
    PathVector vec=find_int_attr(handle,tree,"axis",2);
    if(vec.size()==1)
      axis2.path=*(vec.begin());
    else
      throw "more than one axis=2";
  }catch(const char *str){
    cout << "!!!!! random stuff wrong finding axis=1" << endl;
    return;
  }
  PathVector axis2_vec;
  split_vec(tree,axis2_vec,axis2.path);
  fill_axis(handle,axis2,axis2_vec,config);

  // print out the results for 2D data
  if(data.rank==2){
    write_2D_data(out,path,data,axis1,axis2,config);
    return;
  }

  cout << "NO SUPPORT FOR DATA RANK>2" << endl;
}

extern void write_data(ostream &out,NXhandle handle, const Tree &tree,
                                                    const PrintConfig &config){
  for( Tree::const_iterator path=tree.begin() ; path!=tree.end() ; path++ ){
    write_one_data(out,handle,*path,config);
  }
}

extern void dump_data(std::string &filename,NXhandle handle, const Tree &tree,
                      const PrintConfig &config){
  // error check the arguments
  if(tree.size()!=1){
    throw std::runtime_error("Can dump only one node");
  }
  if(filename.empty()){
    throw std::runtime_error("Encountered empty filename");
  }

  // go to the right place in the file
  Path path=*(tree.begin());
  open_path(handle,path);

  // find out about the data
  int rank;
  int dims[NX_MAXRANK];
  int type;
  if(NXgetinfo(handle,&rank,dims,&type)!=NX_OK){
    close_path(handle,path);
    throw std::runtime_error("NXgetinfo failed");
  }

  // determine the "size" of the data
  int size=1;
  for( int i=0 ; i<rank ; i++ ){
    size*=dims[i];
  }
  if(type==NX_CHAR || type==NX_INT8 || type==NX_UINT8){
    // do nothing
  }else if(type==NX_INT16 || type==NX_UINT16){
    size*=2;
  }else if(type==NX_INT32 || type==NX_UINT32 || type==NX_FLOAT32){
    size*=4;
  }else if(type==NX_INT64 || type==NX_UINT64 || type==NX_FLOAT64){
    size*=8;
  }else{
    // NX_BOOLEAN NX_UINT
    // NX_BINARY   21
    throw std::runtime_error("Do not understand supplied type");
  }

  // allocate space for the data
  void *data;
  if(NXmalloc(&data,rank,dims,type)!=NX_OK){
    close_path(handle,path);
    throw "NXmalloc failed";
  }

  // retrieve the data from the file
  if(NXgetdata(handle,data)!=NX_OK){
    close_path(handle,path);
    throw "NXgetdata failed";
  }

  // close the path
  close_path(handle,path);

  // open the output file
  std::ofstream dump_file(filename.c_str(),std::ios::binary);
  if(!dump_file.is_open()){
    throw std::runtime_error("Failed opening dump file");
  }

  // dump the data
  dump_file.write(reinterpret_cast<char *>(data),size);
}
