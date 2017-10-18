#ifndef __LOOPY_RETRIEVER_GUARD
#define __LOOPY_RETRIEVER_GUARD

#include "../retriever.h"

/**
 * The loopy retriever creates an array based on information supplied
 * in the location string. There are no resources allocated.
 */
// this is not intended to be inherited from
class LoopyRetriever: public Retriever{
 public:
  LoopyRetriever(const std::string &);
  ~LoopyRetriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const; // done
  static const std::string MIME_TYPE; // done
 private: // do not allow these automatically created functions to be called
  LoopyRetriever(const LoopyRetriever&); //copy constructor
  LoopyRetriever& operator=(const LoopyRetriever&); //operator "=" overloading
};

#endif
