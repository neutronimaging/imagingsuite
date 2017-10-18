/**
 * \file output.hpp
 */
#ifndef __XML_UTIL_HPP_GUARD__
#define __XML_UTIL_HPP_GUARD__

#include <libxml/tree.h>

namespace nxsum {
  static const xmlChar *root_name = xmlCharStrdup("nxsummary");
  static const xmlChar *item_name = xmlCharStrdup("item");
  static const xmlChar *path_name = xmlCharStrdup("path");
  static const xmlChar *label_name = xmlCharStrdup("label");
  static const xmlChar *operation_name = xmlCharStrdup("operation");
}
#endif
