#ifndef __TEXT_PLAIN_RETRIEVER_GUARD
#define __TEXT_PLAIN_RETRIEVER_GUARD

#include "../retriever.h"
#include <fstream>

// this is not intended to be inherited from
class TextPlainRetriever: public Retriever{
 public:
  TextPlainRetriever(const std::string &);
  ~TextPlainRetriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private:
  TextPlainRetriever(const TextPlainRetriever&);
  TextPlainRetriever& operator=(const TextPlainRetriever&);
  std::string source;
  int current_line;
  std::ifstream infile;
};
#endif
