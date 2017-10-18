#ifndef __XML_RETRIEVER_DOM_GUARD
#define __XML_RETRIEVER_DOM_GUARD

#include <string>
#include "../tree.hh"
#include "../node.h"

/**
 * This function takes the name of a file and converts it into a tree
 * of nodes. The return string is empty unless warnings or recoverable
 * errors were produced.
 */
extern std::string buildTree(const std::string &filename, tree<Node> &tr);

#endif
