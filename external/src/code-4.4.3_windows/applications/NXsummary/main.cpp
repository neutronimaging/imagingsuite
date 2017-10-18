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

#include "napiconfig.h" // needed for HAVE_STDINT_H
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include "data_util.hpp"
#include "nxsummary.hpp"
#include "output.hpp"
#include "preferences.hpp"
#include "string_util.hpp"
#include "tclap/CmdLine.h"
#include "napi_internal.h" // needed for NXMDisableErrorReporting

using std::cerr;
using std::cout;
using std::endl;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;
using namespace TCLAP;
using namespace nxsum;

static const string NXSUM_VERSION("0.1.1");
static const string EMPTY("");

static void openFile(const string &file, NXhandle &handle) {
  char filename[GROUP_STRING_LEN];
  strcpy(filename, file.c_str());
  if(NXopen(filename,NXACC_READ,&handle)!=NX_OK)
    {
      ostringstream s;
      s << "Could not open file \"" << file << "\"";
      throw runtime_error(s.str());
    }
}

static void closeFile(const string &file, NXhandle &handle) {
  if (handle == NULL)
    {
      return;
    }
  if(NXclose(&handle)!=NX_OK)
    {
      ostringstream s;
      s << "Could not close file \"" << file << "\"";
      throw runtime_error(s.str());
    }
}

static void printInfo(NXhandle handle, const Item &item, const Config &config) {
  try {
    string value = readAsString(handle, item.path, item.operation);
    print(item, value, config);
  } catch(runtime_error &e) {
    printError(item, e.what(), config);
  }
}

static void printSummary(const string &file, const Config &config) {
  NXhandle handle = NULL;
  openFile(file, handle);

  vector<string> values;
  vector<bool> isError;

  size_t length = config.preferences.size();
  for (size_t i = 0 ; i < length ; ++i ) {
    try {
      string value = readAsString(handle, config.preferences[i].path,
                                  config.preferences[i].operation);
      values.push_back(value);
      isError.push_back(false);
    } catch(runtime_error &e) {
      values.push_back(e.what());
      isError.push_back(true);
    }
  }

  print(file, config.preferences, values, isError, config);

  closeFile(file, handle);
}

static void printValue(const string &file, const Item &item,
                       const Config &config) {
  NXhandle handle = NULL;
  openFile(file, handle);

  if (config.multifile)
    {
      cout << file << ";";
    }
  printInfo(handle, item, config);

  closeFile(file, handle);
}

int main(int argc, char *argv[]) {
  try
    {
      // set up the long documentation
      ostringstream descr;
      descr << "Generate summary of a NeXus file";
      descr << "\n";
      descr << "This program relies heavily on the configuration file that is located in \"${HOME}/.nxsummary.conf\" or \"/etc/nxsummary.conf\". A sample configuration file can be obtained using the \"--writeconfig\" flag.";
      descr << "Each \"item\" tag in the file describes a node to print from the NeXus file. The \"path\" attribute describes where in the NeXus file to get information from. The \"label\" attributes is what will be printed when showing the value of the specified field. The optional \"operation\" attribute provides for certain operations to be performed on the data before printing out the result. Valid operations are:";
      descr << "\n";
      descr << "\"COUNT\" - The number of elements in the requested field.";
      descr << "\n";
      descr << "\"DIMS\" - The dimensions of the requested field.";
      descr << "\n";
      descr << "\"SUM\" - Add together all of the array elements and print the result.";
      descr << "\n";
      descr << "\"UNITS:<new units>\" - Specify the units to print the result in.";

      // set up the parser
      CmdLine cmd(descr.str(), ' ', NXSUM_VERSION);

      // configure the arguments
      SwitchArg verboseArg("", "verbose", "Turn on verbose printing", 
                           cmd, false);
      UnlabeledMultiArg<string> filenameArg("filename",
                                            "Name of a file to be viewed",
                                            false, "filename",cmd);
      ValueArg<string> configArg("", "config", "Specify configuration file",
                                 false, "", "config", cmd);
      ValueArg<string> writeConfigArg("", "writeconfig",
                              "Write the default configuration out to a file",
                                      false, "", "config", cmd);
      ValueArg<string> valueArg("", "value",
                               "Get value of the item pointed to by the label",
                                false, "", "label", cmd);
      SwitchArg printXmlArg("", "xml", "Print results as xml",
                            cmd, false);

      // parse the arguments
      cmd.parse(argc, argv);

      // fill in the config object
      struct Config config;
      config.verbose = verboseArg.getValue();
      config.show_label = true;
      config.print_xml = printXmlArg.getValue();

      // load in the preferences
      loadPreferences(configArg.getValue(), config.preferences);

      // write out the preferences and exit
      string configOut = writeConfigArg.getValue();
      if (configOut != "")
        {
          writePreferences(configOut, config.preferences);
          return 0;
        }

      // turn of NeXus debug printing
      NXMDisableErrorReporting();

      // get the list of filenames
      vector<string> files=filenameArg.getValue();
      if(files.empty())
        {
          std::cerr << "ERROR: failed to supply <filename>" << endl;
          cmd.getOutput()->usage(cmd);
          return -1;
        }
      config.multifile = (files.size()>1);

      // are we looking for a particular value
      Item item;
      bool getValue = (valueArg.getValue().size()>0);
      if (getValue)
        {
          item = getPreference(valueArg.getValue(), config.preferences);
          config.show_label = config.verbose;
        }

      // go through the list of files
      for (vector<string>::const_iterator file = files.begin() ;
           file != files.end() ; file++ )
        {
          if (!canRead(*file))
            {
              if (config.verbose)
                {
                  cout << "Cannot open \"" << *file << "\"" << endl;
                }
              continue;
            }
          try
            {
              if (getValue)
                {
                  printValue(*file, item, config);
                }
              else
                {
                  printSummary(*file, config);
                }
            }
          catch(runtime_error &e)
            {
              if ((!config.multifile) || (config.verbose))
                {
                  std::cerr << "RUNTIME ERROR:" << e.what() <<endl;
                }
            }
        }
    }
  catch(ArgException &e)
    {
      std::cerr << "PARSE ERROR:" << e.error() << " for arg " << e.argId()
                << endl;
      return -1;
    }
  catch(runtime_error &e)
    {
      std::cerr << "RUNTIME ERROR:" << e.what() <<endl;
      return -1;
    }

  // all is well
  return 0;
}
