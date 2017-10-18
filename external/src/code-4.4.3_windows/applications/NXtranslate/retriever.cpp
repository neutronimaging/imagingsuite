#include <stdexcept>
#include <map>
#include "retriever.h"
#include "nexus_retriever.h"

#ifdef IPNS_RETRIEVER
#include "IPNS_CPP/ipns_retriever.h"
#endif
#ifdef TEXT_PLAIN_RETRIEVER
#include "text_plain/retriever.h"
#endif
#ifdef SPEC_RETRIEVER
#include "spec/spec_retriever.h"
#endif
#ifdef EDF_RETRIEVER
#include "esrf_edf/edf_retriever.h"
#endif
#ifdef TEXT_XML_RETRIEVER
#include "text_xml/retriever.h"
#endif
#ifdef TEXT_COLLIST_RETRIEVER
#include "text_collist/collist_retriever.h"
#endif
#ifdef BINARY_RETRIEVER
#include "binary/BinaryRetriever.hpp"
#endif
#ifdef DYNAMIC_RETRIEVER
#include "dynamic_retriever.h"
#endif
#ifdef SNS_HISTOGRAM_RETRIEVER
#include "sns_histogram/retriever.h"
#endif
#ifdef FRM2_RETRIEVER
#include "FRM2/frm2_retriever.h"
#endif
#ifdef LOOPY_RETRIEVER
#include "loopy/retriever.h"
#endif

using std::map;
using std::string;
using std::invalid_argument;

typedef Ptr<Retriever> RetrieverPtr;

// hold a map of retrievers that have been instantiated so it can be cached
map<string, RetrieverPtr> retrievers;

RetrieverPtr getNewInstance(const string & type,
                            const string & source) {
  // return appropriate retriever based on type
  if(type==NexusRetriever::MIME_TYPE){
    RetrieverPtr ptr(new NexusRetriever(source));
    return ptr;
#ifdef IPNS_RETRIEVER
  }else if(type==IpnsRetriever::MIME_TYPE){
    RetrieverPtr ptr(new IpnsRetriever(source));
    return ptr;
#endif
#ifdef TEXT_PLAIN_RETRIEVER
  }else if(type==TextPlainRetriever::MIME_TYPE){
    RetrieverPtr ptr(new TextPlainRetriever(source));
    return ptr;
#endif
#ifdef SPEC_RETRIEVER
  }else if(type==SpecRetriever::MIME_TYPE){
    RetrieverPtr ptr(new SpecRetriever(source));
    return ptr;
#endif
#ifdef EDF_RETRIEVER
  }else if(type==EdfRetriever::MIME_TYPE){
    RetrieverPtr ptr(new EdfRetriever(source));
    return ptr;
#endif
#ifdef TEXT_COLLIST_RETRIEVER
  }else if(type==TextCollistRetriever::MIME_TYPE){
    RetrieverPtr ptr(new TextCollistRetriever(source));
    return ptr;
#endif
#ifdef TEXT_XML_RETRIEVER
  }else if(type==TextXmlRetriever::MIME_TYPE){
    RetrieverPtr ptr(new TextXmlRetriever(source));
    return ptr;
#endif
#ifdef BINARY_RETRIEVER
  }else if(type==BinaryRetriever::MIME_TYPE){
    RetrieverPtr ptr(new BinaryRetriever(source));
    return ptr;
#endif
#ifdef DYNAMIC_RETRIEVER
  }else if(type.substr(0,8) == "dynamic/"){
    RetrieverPtr ptr(new DynamicRetriever(source, type));
    return ptr;
#endif
#ifdef SNS_HISTOGRAM_RETRIEVER
  }else if(type==SnsHistogramRetriever::MIME_TYPE){
    RetrieverPtr ptr(new SnsHistogramRetriever(source));
    return ptr;
#endif
#ifdef FRM2_RETRIEVER
  }else if(type==Frm2Retriever::MIME_TYPE){
    RetrieverPtr ptr(new Frm2Retriever(source));
    return ptr;
#endif
#ifdef LOOPY_RETRIEVER
  }else if(type==LoopyRetriever::MIME_TYPE){
    RetrieverPtr ptr(new LoopyRetriever(source));
    return ptr;
#endif
  }

  // if it gets this far the type is not understood
  throw invalid_argument("do not understand mime_type ("+type+") in retriever_factory");
}

// Implementation of a pure virtual destructor. This makes the compiler happy.
Retriever::~Retriever(){}

// factory method
Retriever::RetrieverPtr Retriever::getInstance(const string & type,
                                               const string &source){
  map<string, RetrieverPtr>::iterator existing = retrievers.find(source);
  if (existing != retrievers.end()) {
    return (existing->second);
  }
  else {
    RetrieverPtr retriever = getNewInstance(type, source);
    retrievers.insert(make_pair(source, retriever));
    return retriever;
  }
}
