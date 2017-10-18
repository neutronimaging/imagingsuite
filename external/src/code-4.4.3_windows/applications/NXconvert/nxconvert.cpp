/*-----------------------------------------------------------------------------
  NeXus - Neutron & X-ray Common Data Format
   
  Utility to convert a NeXus file into HDF4/HDF5/XML/...
 
  Author: Freddie Akeroyd, Ray Osborn
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.
 
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
  For further information, see <http://www.nexusformat.org>
 
 $Id$
-----------------------------------------------------------------------------*/

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#ifdef _MSC_VER
#else
#include <unistd.h>
#endif
#include "napi.h"
#include "nxconvert_common.h"
#include "tclap/CmdLine.h"

using std::runtime_error;
using std::string;
using std::vector;
using namespace TCLAP;

static const string NXCONVERT_VERSION("1.0");
static const string EMPTY("");

int main(int argc, char *argv[])
{
  try {
    // set up the command line  arguments
    CmdLine cmd("Convert a NeXus file between different on disk file formats.",
		' ', NXCONVERT_VERSION, false);
    SwitchArg xmlArg("x", "xml", "Output file in xml format", false);
    vector<int> allowedHdf;
    allowedHdf.push_back(4);
    allowedHdf.push_back(5);
    ValuesConstraint<int> allowedHdfC(allowedHdf);
    ValueArg<int> hdfArg("h", "hdf", "Specify HDF version to write", false, 0, &allowedHdfC);
    SwitchArg defArg("d", "dfn",
		     "Output definition file used for validating NeXus files",
		     false);
    ValueArg<string> defValueArg("D", "definition",
		      "Output definition file used for validating NeXus files. Requires a definition name.",
			    false, EMPTY, "defintion");
    vector<string> allowedXml;
    allowedXml.push_back("keepws");
    allowedXml.push_back("table");
    ValuesConstraint<string> allowedXmlC(allowedXml);
    ValueArg<string> xmlSpecialArg("o", "outputxml",
				   "Special arguments for xml. keepws defines that the whitespace should be preserved. table specifies a format that is more easiliy imported into spreadsheet programs. Either option forces xml output",
				   false, "", &allowedXmlC);
    vector<Arg*> outputformats;
    outputformats.push_back(&hdfArg);
    outputformats.push_back(&xmlArg);
    outputformats.push_back(&defArg);
    outputformats.push_back(&defValueArg);
    outputformats.push_back(&xmlSpecialArg);
    cmd.xorAdd(outputformats);

    UnlabeledMultiArg<string> FileArgs("Files", "Name of input and output files.",
					false, EMPTY);
    cmd.add(FileArgs);
    // parse the arguments and configure converting the file
    cmd.parse(argc, argv);
    int nx_format = -1; // output format
    int nx_write_access = 0; // output write access
    int nx_read_access = NXACC_READ; // input read access
    string definition_name; // NEXUS_SCHEMA_BASE is NULL


    if (xmlArg.isSet()) {
      nx_format = NX_XML;
      nx_write_access |= NXACC_CREATEXML;
    }
    if (defArg.isSet() || defValueArg.isSet()) {
      nx_format = NX_DEFINITION;
      nx_write_access |= NXACC_CREATEXML;
      if (defValueArg.isSet())
	definition_name = defValueArg.getValue();
    }
    if (hdfArg.isSet()) {
      int hdf_type = hdfArg.getValue();
      if (hdf_type == 4) {
          nx_format = NX_HDF4;
          nx_write_access |= NXACC_CREATE4;
      } else if (hdf_type == 5) {
          nx_format = NX_HDF5;
          nx_write_access |= NXACC_CREATE5;
      }
    }
    if (xmlSpecialArg.isSet()) {
      nx_format |= NX_XML;
      nx_write_access |= NXACC_CREATEXML;
      string type = xmlSpecialArg.getValue();
      if (type.compare("keepws")) {
	nx_write_access |= NXACC_NOSTRIP;
	nx_read_access |= NXACC_NOSTRIP;
      }
      else if (type.compare("table")) {
	nx_write_access |= NXACC_TABLE;
      }
    }
    vector<string> file_args = FileArgs.getValue();
    string inFile, outFile;
    if (file_args.size() > 0)
    {
        inFile = file_args[0];
    }
    if (file_args.size() > 1)
    {
        outFile = file_args[1];
    }

    // do the actual conversion
    std::cout << "Converting " << inFile << " to " << nx_formats[nx_format]
	      << " NeXus file " << outFile << std::endl;
    
    if (convert_file(nx_format, inFile.c_str(), nx_read_access, outFile.c_str(), nx_write_access, definition_name.c_str()) != NX_OK) {
      std::cerr << "Conversion failed" << std::endl;
      return 1;
    }

  }
  catch (ArgException &e) {
    std::cerr << "PARSE ERROR:" << e.error() << " for arg " << e.argId()
	      << std::endl;
    return -1;
  }
  catch (runtime_error &e) {
    std::cerr << "RUNTIME ERROR:" << e.what() << std::endl;
    return -1;
  }

  // tell the user that everything went ok
  std::cout << "Convertion successful." << std::endl;
  return 0;
}

