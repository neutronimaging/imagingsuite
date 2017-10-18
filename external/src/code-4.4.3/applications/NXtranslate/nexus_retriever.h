#ifndef __NEXUS_RETRIEVER_GUARD
#define __NEXUS_RETRIEVER_GUARD

#include "retriever.h"

// this is not intended to be inherited from
class NexusRetriever: public Retriever{
 public:
  NexusRetriever(const std::string &);
  ~NexusRetriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private:
  NexusRetriever(const NexusRetriever&);
  NexusRetriever& operator=(const NexusRetriever&);
  NXhandle *handle;
  std::string source;
};
#endif
