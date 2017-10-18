//+**********************************************************************
//
// File:	spec_retriever.h
//
// Project:	 Fable data to NeXus translation
//
// Description:	definition of a spec_retriever class. The retriever allows to
//              read data from SPEC file and store in Nexus file format.
//		It uses SPEClib for reading SPEC files. It is a plugin of NXtranslate
//              program. 
//
// Author(s):	Jaroslaw Butanowicz
//
// Original:	March 2006
//
//+**********************************************************************

#ifndef __SPEC_RETRIEVER_GUARD
#define __SPEC_RETRIEVER_GUARD

#include "../retriever.h"
#include <fstream>
//
// this is not intended to be inherited from
//
class SpecRetriever: public Retriever{
 public:
  SpecRetriever(const std::string &);
  ~SpecRetriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private:
  void auto_translation(tree<Node> &);
 // SpecRetriever(const SpecRetriever&);
 // SpecRetriever& operator=(const SpecRetriever&);
  std::string source;
  std::ifstream infile;
};
#endif
