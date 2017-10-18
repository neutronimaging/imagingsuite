//
// $Id$
//
// Basic NXtranslate dynamic retriever
//
// Allows a retriever to be implemented as a dynamic library external to 
// NXtranslate
//
// Author: Freddie Akeroyd, CCLRC ISIS <F.A.Akeroyd@rl.ac.uk>
//
// The dynamic library just needs to export the following functions
//
// required:
//
//    void* nxtinit(const char* source) 
//    void* nxtgetdata(void* ref, const char* arg, int* data_type, 
//                     int* dims_array, int* ndims, int* free_data);
//
// optional:
//
//    void nxtfreedata(void* ref, void* arg)
//    void nxtcleanup(void* ref)
//
//  see "test_dynamic.c" for an example of implementing this
//

#ifndef __DYNAMIC_RETRIEVER_GUARD
#define __DYNAMIC_RETRIEVER_GUARD

#include "retriever.h"
//#include <ltdl.h>

typedef void* (*getData_func_t)(void* ref, const char* arg, int* data_type, int* dims_array, int* ndims, int* free_data);
typedef int (*free_func_t)(void* ref, void* arg);
typedef void* (*init_func_t)(const char* source); // returns a "ref"
typedef int (*cleanup_func_t)(void* ref);

class DynamicRetriever: public Retriever {
 public:
  DynamicRetriever(const std::string& source, const std::string& mime_type);
  ~DynamicRetriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private:
  DynamicRetriever(const DynamicRetriever&);
  DynamicRetriever& operator=(const DynamicRetriever&);
  std::string m_module_name; // name of dynamic library
  std::string m_mime_type;   // mime type of library
//  lt_dlhandle m_dlhandle;    // reference to loaded shared library
  void*  m_dlhandle;    // reference to loaded shared library
  getData_func_t m_data_func;
  free_func_t m_free_func;
  init_func_t m_init_func;
  cleanup_func_t m_cleanup_func;
  void* m_ref; // internal reference pointer returned by dynamic module
};

#endif
