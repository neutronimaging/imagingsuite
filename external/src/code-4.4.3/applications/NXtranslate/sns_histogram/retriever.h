#ifndef __SNS_HISTOGRAM_RETRIEVER_GUARD
#define __SNS_HISTOGRAM_RETRIEVER_GUARD

#include "../retriever.h"
#include <fstream>
#include <iostream>

// this is not intended to be inherited from
class SnsHistogramRetriever: public Retriever{
 public:
  SnsHistogramRetriever(const std::string &);
  ~SnsHistogramRetriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private:
  SnsHistogramRetriever(const SnsHistogramRetriever&); //copy constructor
  SnsHistogramRetriever& operator=(const SnsHistogramRetriever&); //operator "=" overloading
  std::string source;
  std::string location;
  FILE *BinaryFile;
};

#endif
