#ifndef _BINARY_RETRIEVER_HPP
#define _BINARY_RETRIEVER_HPP 1

#include "retriever.h"

/**
 */
// this is not intended to be inherited from
class BinaryRetriever: public Retriever{
 public:
  BinaryRetriever(const std::string &source_file);
  ~BinaryRetriever();
  void getData(const std::string &location, tree<Node> &tr);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private: // do not allow these automatically created functions to be called
  BinaryRetriever(const BinaryRetriever &other); //copy constructor
  BinaryRetriever& operator=(const BinaryRetriever &other);//operator "=" overloading
  std::string filename;
};

#endif
