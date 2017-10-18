/*
 * Copyright (c) 2007, P.F.Peterson <petersonpf@ornl.gov>
 *               Spallation Neutron Source at Oak Ridge National Laboratory
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * \file nxsummary.hpp
 */
#ifndef __NXSUMMARY_H_GUARD__
#define __NXSUMMARY_H_GUARD__

#include <string>
#include <vector>

// useful constants
/**
 * Size of buffer for character arrays used in node names.
 */
static const size_t GROUP_STRING_LEN = 512;

/**
 * String to return when path is not found in the file.
 */
static const std::string NOT_FOUND = "NOT FOUND:";

/**
 * Object used to define a thing to print from the file.
 */
struct Item{
  std::string path;
  std::string label;
  std::string operation;
};

/**
 * Run time configuration of the process.
 */
struct Config{
  bool verbose;
  bool multifile;
  bool show_label;
  bool print_xml;
  std::vector<Item> preferences;
};
#endif
