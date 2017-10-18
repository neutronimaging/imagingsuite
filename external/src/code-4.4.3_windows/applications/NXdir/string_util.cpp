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
#include <iostream>
#include "nxdir.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <stdio.h>

#define ellipse std::string("...,")

// bring some functions into this namespace
using std::string;
using std::stringstream;
using std::cout;
using std::endl;
using std::find;
using std::find_if;

// alias some data types
typedef Tree::const_iterator TreeIter;
typedef StringVector::const_iterator StrVecIter;
typedef Path::const_iterator PathIter;
typedef string::const_iterator StrIter;

// function prototypes
static string path_to_string( PathIter path_begin, PathIter path_end, PathMode print_mode);

// ==================== UTILITY CODE
/*
 * Returns true if the supplied character is a digit [0-9].
 */
static bool my_isdigit(char c){
  static const string digits="0123456789";
  return find(digits.begin(),digits.end(),c)!=digits.end();
}

/*
 * Returns the negation of my_isdigit(char c)
 */
static bool my_isnotdigit(char c){
  return !my_isdigit(c);
}

/*
 * Converts a string into an integer. This checks that the string
 * contains only digits.
 */
extern int str_to_int(const string str){
  // check that it is a number
  StrIter itter=str.begin();
  itter=find_if(itter,str.end(),my_isnotdigit);

  if(itter!=str.end())
    throw "argument not an integer";

  return atoi(str.c_str());
}

/*
 * Returns true if the supplied character is a forward slash "/"
 */
static bool isslash(char c){
  static const string slash="/";
  return find(slash.begin(),slash.end(),c)!=slash.end();
}

/*
 * Returns the negation of isslash(char c)
 */
static bool isnotslash(char c){
  return !isslash(c);
}

/*
 * Converts a slash, "/", delimited string into a Vector of strings.
 */
extern StringVector arrayify(const string path){
  StringVector result;

  StrIter i=path.begin();
  while(i!=path.end()){
    // ignore leading stuff
    i=find_if(i,path.end(),isnotslash);

    // find end of next word
    StrIter j=find_if(i,path.end(),isslash);

    // copy the characters in [i,j)
    if(i!=path.end())
      result.push_back(string(i,j));
    i=j;
  }

  return result;
}

// ==================== PRINTING CODE
/*
 * Just like the name says: prints a path followed by a carriage return.
 */
extern void print_path(const Path path, const PathMode print_mode){
  cout << path_to_str(path,print_mode) << endl;
}

/*
 * Just like the name says: prints the full tree with each path on a
 * separate line.
 */
extern void print_tree(const Tree tree, const PathMode print_mode){
  for( TreeIter iter=tree.begin() ; iter!=tree.end() ; iter++ ){
    print_path(*iter,print_mode);
  }

  return;
}

extern void print_strvec(const StringVector &vec){
  for( StrVecIter it=vec.begin() ; it!=vec.end() ; it++ ){
    std::cout << *it;
    if(it+1!=vec.end()) std::cout << "/";
  }
  std::cout << std::endl;
}

static int num_match(const Path path1, const Path path2){
  int num_match=0;
  int path1_len=path1.size();
  int path2_len=path2.size();
  int len=path1_len;
  if(path2_len<len)
    len=path2_len;

  PathIter path1_it=path1.begin();
  PathIter path2_it=path2.begin();

  for( int i=0 ; i<len ; i++ ){
    if( (path1_it+i)->type!=(path2_it+i)->type )
      return num_match;
    if( (path1_it+i)->name!=(path2_it+i)->name )
      return num_match;
    num_match++;
  }

  return num_match;
}

static StringVector create_tree_str(const Tree tree, const StringVector dims,
                                                     const PrintConfig config){
  StringVector tree_str;

  int length=tree.size();
  if(length<=0)
    return tree_str;

  // variable for adding space a begining of tree print when showing filename
  int tree_tree_space_start=0;
  if(!config.show_filename)
    tree_tree_space_start=1;

  for( int i=0 ; i<length ; i++ ){
    string temp;
    // build string
    if(config.tree_mode==TREE_TREE){
      if(i==0){
        temp+="/"+path_to_string(tree[i].begin(),tree[i].end(),config.path_mode);
      }else{
        int num=num_match(tree[i-1],tree[i]);
        for( int j=tree_tree_space_start ; j<num ; j++ )
          temp+="  ";
        if(num==0)
          temp+="/";
        else
          temp+="|";
        temp+=path_to_string(tree[i].begin()+num,tree[i].end(),config.path_mode);
        temp+=dims[i];
      }
    }else{
      if(config.show_filename && config.tree_mode==TREE_SCRIPT)
        temp+=config.filename+";";
      else if(config.show_filename && config.tree_mode==TREE_MULTI)
        temp+="  ";

      // add in the path and dimensions
      temp+=path_to_str(tree[i],config.path_mode)+dims[i];
    }
    // append it to the result
    tree_str.push_back(temp);
  }

  return tree_str;
}

/*
 * Takes a tree, collects all information needed for printing, and
 * writes it out.
 */
extern void print_tree_result(NXhandle handle, const Tree tree,
                                               const PrintConfig print_config){
  // check the size of the tree
  int length=tree.size();
  if(length<=0) return;

  // get the dimensions of everything
  StringVector dims=read_all_dims_as_string(handle,tree);

  // create the tree for everything
  StringVector tree_str=create_tree_str(tree,dims,print_config);

  // print the filename
  if(print_config.show_filename && print_config.tree_mode!=TREE_SCRIPT)
    cout << print_config.filename << endl;

  if(print_config.print_data){
    // get the data
    StringVector data=read_all_data_as_string(handle,tree,
                                                      print_config);

    for(int i=0 ; i<length ; i++ ){
      if( (tree[i].rbegin()->type!=SDS) && (tree[i].rbegin()->type!=ATTR) )
        continue;
      if(print_config.output_line_mode==OUT_SINGLE)
        cout << tree_str[i] << "=" << data[i] << endl;
    }

  }else{
    for( StrVecIter path=tree_str.begin() ; path!=tree_str.end() ; path++ ){
      cout << *path << endl;
    }
  }
}

// ==================== TOSTRING CODE
static string path_to_string( PathIter path_begin, PathIter path_end, PathMode print_mode){
  string result;
  for( PathIter iter=path_begin ; iter!=path_end ; iter++ ){
    if(iter->type==ATTR)
      result=result+"#";
    if(print_mode==NAME || print_mode==NAME_TYPE)
      result=result+iter->name;
    if(print_mode==NAME_TYPE || print_mode==TYPE)
      result=result+":";
    if(print_mode==TYPE || print_mode==NAME_TYPE)
      result=result+iter->type;
    if(iter->type!=SDS && iter->type!=ATTR)
      result=result+"/";
  }

  return result;
}

/*
 * Converts a path to a string.
 */
extern string path_to_str(const Path path, const PathMode print_mode){
  return "/"+path_to_string(path.begin(),path.end(),print_mode);
}

/*
 * Converts a value to a string using the string stream class.
 */
template <typename T>
extern string value_to_str(const T value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

/*
 * Converts an integer (any precision) to a string.
 */
extern string int_to_str(const long value){
  return value_to_str(value);
}

/*
 * Some character arrays are not null (\0) terminated, but since the
 * length is known this converts them to a proper string.
 */
static string fix_str(char *value,int length){
  string my_string=string((char *)value);
  string result=string("");
  for( int i=0 ; i<length ; i++ ){
    result+=my_string[i];
  }

  return result;
}

/*
 * Convert an individual void* into a string using the types defined
 * in the NeXus API header.
 */
extern string voidptr_to_str(const void *data, int pos,int type){
  if(type==NX_FLOAT32)
    return value_to_str(((float *)data)[pos]);
  else if(type==NX_FLOAT64)
    return value_to_str(((double *)data)[pos]);
  else if(type==NX_INT8)
    return value_to_str(((int8_t *)data)[pos]);
  else if(type==NX_INT16)
    return value_to_str(((int16_t *)data)[pos]);
  else if(type==NX_INT32)
    return value_to_str(((int32_t *)data)[pos]);
  else if(type==NX_INT64)
    return value_to_str(((int64_t *)data)[pos]);
  else if(type==NX_UINT8)
    return value_to_str(((uint8_t *)data)[pos]);
  else if(type==NX_UINT16)
    return value_to_str(((uint16_t *)data)[pos]);
  else if(type==NX_UINT32)
    return value_to_str(((uint32_t *)data)[pos]);
  else if(type==NX_UINT64)
    return value_to_str(((uint64_t *)data)[pos]);
  else
    throw "Do not understand type in voidptr_to_str";
}

/*
 * Converts a one dimesional array (given as a void pointer) into a
 * string.
 */
extern string oneD_to_string(const void *data, const int length,
                                     const int type, const PrintConfig config){
  int max_items=config.max_length;
  // deal with character array
  if(type==NX_CHAR)
    return fix_str((char *)data,length);

  // deal with everything else
  int itter_length=length;
  if( (max_items>0) && (length>max_items) )
    itter_length=max_items;

  // get the first element
  if(length==1)
    return voidptr_to_str(data,0,type);

  // build an array string
  stringstream result;
  result << "[";
  for( int i=0 ; i<itter_length ; i++ ){
    result << voidptr_to_str(data,i,type);
    if(i+1<length)
      result << ",";
  }
  if(length!=itter_length)
    result << ellipse << voidptr_to_str(data,length-1,type);
  result << "]";

  // return the resulting array string
  return result.str();
}

/*
 * Converts a two dimensional array (given as a void pointer) into a string.
 */
static string twoD_to_string(const void *data, const int dims[], int type, PrintConfig config){
  // REMEMBER: int a[10][20] can be accessed using a[20*row+col]

  // set up the result and the length of each dimension
  string result="[";
  int offset=0;
  int col_len=dims[0];
  int row_len=dims[1];
  int itter_length=col_len;
  if( (config.max_length>0) && (col_len>config.max_length) )
    itter_length=config.max_length;

  // create the string for the first itter_length elements
  for( int i=0 ; i<itter_length ; i++ ){
    result+=oneD_to_string((char *)data+offset,row_len,type,config);
    if(i+1<col_len)
      result+=",";
    offset=offset+row_len;
  }

  // add the ellipses and final element if the not all elements are in
  // the string
  if(itter_length!=col_len){
    result+=ellipse;
    offset=row_len*(col_len-1);

    result+=oneD_to_string((char *)data+offset,row_len,type,config);
  }
  result+="]";

  return result;
}

/*
 * Externally available function to allow for converting arrays to strings.
 */
extern string to_string(const void *data, const int dims[], const int rank,
                                     const int type, const PrintConfig config){
  try{
    if(rank==1){
      return oneD_to_string(data,dims[0],type,config);
    }else if(rank==2){
      return twoD_to_string(data,dims,type,config);
    }else{
      return string("");
    }
  }catch(const char *str){
    return string(str);
  }
}
