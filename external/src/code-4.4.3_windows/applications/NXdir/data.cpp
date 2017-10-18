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
#include <cstring>
#include <string>
#include <map>
#include "nxdir.h"

#define ellipse std::string("...,")
#define MAX_DIMS 2

using std::string;

/*
 * This creates a string describing the dimensions of an n-dimensional
 * field at a specified path. This will return an empty string if the
 * Path does not point to an SDS.
 */
static string read_dims_as_string(NXhandle handle, Path path){
  if(path.rbegin()->type!=SDS)
    return string("");

  open_path(handle,path);
  
  int rank=0;
  int type=0;
  int dims[NX_MAXRANK];
  if(NXgetinfo(handle,&rank,dims,&type)!=NX_OK){
    close_path(handle,path);
    throw "NXgetinfo failed";
  }

  close_path(handle,path);

  string result("[");
  for( int i=0 ; i<rank ; i++ ){
    result=result+int_to_str(dims[i]);
    if(i+1<rank)
      result=result+",";
  }
  result=result+"]";

  return result;
}

/*
 * This collects the dimension strings for the entire tree together.
 */
extern StringVector read_all_dims_as_string(NXhandle handle, const Tree tree){
  StringVector result;
  int length=tree.size();

  std::string dims;
  for( int i=0 ; i<length ; i++ ){
    dims=read_dims_as_string(handle,tree[i]);
    result.push_back(dims);
  }

  return result;
}


/*
 * This reads the value of an attribute at the given Path and returns
 * it as a string.
 */
extern string read_attr_as_string(NXhandle handle, const Path &path, const PrintConfig &config){
  if(path.rbegin()->type!=ATTR)
    return string("");

  open_path(handle,path);

  char attr_name[GROUP_STRING_LEN];
  strcpy(attr_name,path.rbegin()->name.c_str());
  int attr_length,attr_type;

  if(!has_attr(handle,attr_name,&attr_length,&attr_type)){
    close_path(handle,path);
    return string("");
  }

  // allocate array to store data
  int dims[1]={attr_length+1};
  void *data;
  if(NXmalloc(&data,1,dims,attr_type)!=NX_OK){
    close_path(handle,path);
    throw "NXmalloc failed";
  }

  // get the value
  if(NXgetattr(handle,attr_name,data,dims,&attr_type)!=NX_OK){
    close_path(handle,path);
    throw "NXgetattr failed";
  }

  // close the path
  close_path(handle,path);

  // convert the value to a string
  string result=oneD_to_string(data,attr_length,attr_type,config);

  // release memory
  if(NXfree(&data)!=NX_OK)
    throw "NXfree failed";

  return result;
}

/*
 * This read the value of an integer attrubte. If it isn't an integer
 * an exception is thrown.
 */
extern long read_int_attr(NXhandle handle, const Path &path){
  if(path.rbegin()->type!=ATTR)
    throw "Path is to non-attribute";

  open_path(handle,path);
  char attr_name[GROUP_STRING_LEN];
  strcpy(attr_name,path.rbegin()->name.c_str());
  int attr_length,attr_type;

  if(!has_attr(handle,attr_name,&attr_length,&attr_type)){
    close_path(handle,path);
    throw "Specified attribute does not exist";
  }

  // check that the length is okay
  if(attr_length!=1)
    throw "read_int_attr only supports scalar attributes";

  // allocate array to store data
  int dims[1]={attr_length+1};
  void *data;
  if(NXmalloc(&data,1,dims,attr_type)!=NX_OK){
    close_path(handle,path);
    throw "NXmalloc failed";
  }

  // check that the type is okay
  if(attr_type==NX_INT8){
  }else if(attr_type==NX_INT16){
  }else if(attr_type==NX_INT32){
  }else if(attr_type==NX_INT64){
  }else if(attr_type==NX_UINT8){
  }else if(attr_type==NX_UINT16){
  }else if(attr_type==NX_UINT32){
  }else if(attr_type==NX_UINT64){
  }else{
    throw "Unsupported type in read_int_attr";
  }

  // get the value
  if(NXgetattr(handle,attr_name,data,dims,&attr_type)!=NX_OK){
    close_path(handle,path);
    throw "NXgetattr failed";
  }

  close_path(handle,path);

  // convert the value to an integer
  long result;
  if(attr_type==NX_INT8)
    result=static_cast<long>(((int8_t *)data)[0]);
  else if(attr_type==NX_INT16)
    result=static_cast<long>(((int16_t *)data)[0]);
  else if(attr_type==NX_INT32)
    result=static_cast<long>(((int32_t *)data)[0]);
  else if(attr_type==NX_INT64)
    result=static_cast<long>(((int64_t *)data)[0]);
  else if(attr_type==NX_UINT8)
    result=static_cast<long>(((uint8_t *)data)[0]);
  else if(attr_type==NX_UINT16)
    result=static_cast<long>(((uint16_t *)data)[0]);
  else if(attr_type==NX_UINT32)
    result=static_cast<long>(((uint32_t *)data)[0]);
  else if(attr_type==NX_UINT64)
    result=static_cast<long>(((uint64_t *)data)[0]);
  else
    throw "Unsupported type cast in read_int_attr";

  // release memory
  if(NXfree(&data)!=NX_OK)
    throw "NXfree failed";

  return result;
}

/*
 * This reads the value of a data item (or attribute) and returns its
 * value as a string. It will return an empty string if the type of
 * the ending Node in the Path is not understood.
 */
static string read_data_as_string(NXhandle handle, Path path,
                                                           PrintConfig config){
  // call the code to read the attribute
  if(path.rbegin()->type==ATTR)
    return read_attr_as_string(handle,path,config);

  // if this is not an SDS return an empty string
  if(path.rbegin()->type!=SDS)
    return string("");

  open_path(handle,path);
  
  // determine the rank and dimension
  int rank=0;
  int type=0;
  int dims[NX_MAXRANK];
  if(NXgetinfo(handle,&rank,dims,&type)!=NX_OK){
    close_path(handle,path);
    throw "NXgetinfo failed";
  }

  // if the rank is too big close the path and return an empty string
  if(rank>MAX_DIMS){
    close_path(handle,path);
    return "";
  }

  // allocate space for data
  void *data;
  if(NXmalloc(&data,rank,dims,type)!=NX_OK){
    close_path(handle,path);
    throw "NXmalloc falied";
  }

  // retrieve data from the file
  if(NXgetdata(handle,data)!=NX_OK){
    close_path(handle,path);
    throw "NXgetdata failed";
  }

  // close the path
  close_path(handle,path);

  // convert to string
  string result=to_string(data,dims,rank,type,config);

  //free up the pointer
  if(NXfree(&data)!=NX_OK)
    throw "NXfree failed";

  return result;
}

/**
 * Read the value of all elements of the tree.
 */
extern StringVector read_all_data_as_string(NXhandle handle, const Tree tree,
                                                     const PrintConfig config){
  StringVector result;
  int length=tree.size();

  std::string data;
  for( int i=0 ; i<length ; i++ ){
    try{
      data=read_data_as_string(handle,tree[i],config);
    }catch(const char *str){
      data=str;
    }
    result.push_back(data);
  }

  return result;
}
