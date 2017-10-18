//+**********************************************************************
//
// File:	edf_retriever.h
//
// Project:	 Fable data to NeXus translation
//
// Description:	definition of a edf_retriever class. The retriever allows to
//              read data from EDF file and store in Nexus file format.
//		It is a plugin of NXtranslate program. 
//
// Author(s):	Jaroslaw Butanowicz
//
// Original:	April 2006
//
//+**********************************************************************


#ifndef __EDF_RETRIEVER_GUARD
#define __EDF_RETRIEVER_GUARD

#include "../retriever.h"
#include <fstream>
//
// this is not intended to be inherited from
//
class EdfRetriever: public Retriever{
 public:
  EdfRetriever(const std::string &);
  ~EdfRetriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private:
  std::string source;
  std::ifstream infile;
};
#endif
