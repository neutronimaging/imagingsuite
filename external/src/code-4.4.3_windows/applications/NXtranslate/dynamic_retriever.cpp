//
// $Id$
//
// NXtranslate Dynamic retriever
//
// Freddie Akeroyd, CCLRC ISIS Facility <F.A.Akeroyd@rl.ac.uk>
//
// see dynamic_retriever.h for details of interface
//
#include <stdexcept>
#include <string>
#include <vector>
#include <napiconfig.h>
#include "node.h"
#include "node_util.h"
#include "string_util.h"
#include "tree.hh"
#include "dynamic_retriever.h"
#if HAVE_DLFCN_H
#include <dlfcn.h>
#else
#define dlopen(a,b)	NULL
#define dlsym(a,b)	NULL
#define dlclose(a)	NULL
#define dlerror()	"Dynamic loading not supported - no <dlfcn.h>"
#define RTLD_NOW	2
#endif /* HAVE_DLFCN_H */

using std::ifstream;
using std::invalid_argument;
using std::runtime_error;
using std::string;
using std::vector;

DynamicRetriever::DynamicRetriever(const string &source, const string& mime_type) : m_mime_type(mime_type), m_dlhandle(NULL), m_ref(NULL)
{
    int i = mime_type.find("/");
    m_module_name = mime_type.substr(i+1);
    m_dlhandle = dlopen(m_module_name.c_str(), RTLD_NOW);
    if (m_dlhandle == NULL)
    {
	throw invalid_argument("Cannot load " + m_module_name + ": " + dlerror());
    }
    // initialize and getdata are required
    m_init_func = (init_func_t)dlsym(m_dlhandle, "nxtinit");
    if (m_init_func == NULL)
    {
	throw invalid_argument("Cannot find initialize() in " + m_module_name + ": " + dlerror());
    }
    m_data_func = (getData_func_t)dlsym(m_dlhandle, "nxtgetdata");
    if (m_data_func == NULL)
    {
	throw invalid_argument("Cannot find getdata() in " + m_module_name + ": " + dlerror());
    }
    // freedata and cleanup are optional
    m_free_func = (free_func_t)dlsym(m_dlhandle, "nxtfreedata");
    m_cleanup_func = (cleanup_func_t)dlsym(m_dlhandle, "nxtcleanup");
    // get our unique reference ID to pass to other functions
    m_ref = (*m_init_func)(source.c_str());
}

DynamicRetriever::~DynamicRetriever()
{
  if (m_ref != NULL && m_cleanup_func != NULL)
  {
        (*m_cleanup_func)(m_ref);
        m_ref = NULL;
  }
  if (m_dlhandle != NULL)
  {
	dlclose(m_dlhandle);
	m_dlhandle = NULL;
  }
}

void DynamicRetriever::getData(const string &location, tree<Node> &tr)
{
  int free_data = 0, rank = 1, dims[NX_MAXRANK], type = NX_CHAR;
  void* data = NULL;
  dims[0] = 1;

  data = (*m_data_func)(m_ref, location.c_str(), &type, dims, &rank, &free_data);

  // add an attribute to say where the data came from
  vector<Attr> attrs;
  Attr my_attr("translate_source", location.c_str(), 
               location.length(), NX_CHAR);
  attrs.push_back(my_attr);

  Node node(location, data, rank, dims, type);
  node.set_attrs(attrs);
  tr.insert(tr.begin(),node);
  if (free_data)
  {
	(*m_free_func)(m_ref, data);
  }
}

string DynamicRetriever::toString() const
{
  return "["+m_mime_type+"]";
}
