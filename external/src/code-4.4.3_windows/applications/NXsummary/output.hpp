/**
 * \file output.hpp
 */
#ifndef __OUTPUT_HPP_GUARD__
#define __OUTPUT_HPP_GUARD__

#include <string>
#include <vector>
#include "nxsummary.hpp"

namespace nxsum {
  void print(const Item &item, const std::string &value, const Config &config);
  void print(const std::string &filename, const std::vector<Item> &preferences,
             const std::vector<std::string> &values,
             const std::vector<bool> &isError, const Config &config);
  void printError(const Item &item, const std::string &msg,
                  const Config &config);
}
#endif
