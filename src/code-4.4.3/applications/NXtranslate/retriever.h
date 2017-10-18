#ifndef __RETRIEVER_H_GAURD
#define __RETRIEVER_H_GAURD

#include <string>
#include "node.h"
#include "tree.hh"
#include "Ptr.h"

/**
 * The design of this class is that a factory will use a key to choose
 * which concrete Retriever to create and return. The Retriever will
 * then return partial trees each time that getData(string) is
 * called. In practice getData(string) will be called several times
 * with each instance.
 */
class Retriever{
  typedef Ptr<Retriever> RetrieverPtr;

 public:
  /**
   * The factory will call the constructor with a string. The string
   * specifies where to locate the data (e.g. a filename), but
   * interpreting the string is left up to the implementing code.
   */
  //Retriever(const std::string &);

  /**
   * The destructor must be virtual to make sure the right one is
   * called in the end.
   */
  virtual ~Retriever()=0;

  /**
   * This is the method for retrieving data from a file. The whole
   * tree will be written to the new file immediately after being
   * called. Interpreting the string is left up to the implementing
   * code.
   */
  virtual void getData(const std::string &, tree<Node> &)=0;

  /**
   * This method is to be used for debugging purposes only. While the
   * string can be anything, most useful is "[mime_type] source".
   */
  virtual std::string toString() const=0;

  /**
   * Factory method to create new retrievers.
   */
  static RetrieverPtr getInstance(const std::string &, const std::string &);
};
#endif
