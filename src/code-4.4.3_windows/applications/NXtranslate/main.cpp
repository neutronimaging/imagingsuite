#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include "xml_parser.h"
#include <napi.h>
#include "string_util.h"
#include <cstdio>

using std::cout;
using std::endl;
using std::find;
using std::string;
using string_util::starts_with;

static const string TRANSLATE_VERSION="0.3.0";

// structure to hold options
typedef struct{
  string infile;
  string outfile;
  NXaccess base;
  bool append;
  bool timing;
  std::map<string,string> map;
}Options;

// constant argument for this file
static const string EQUAL="=";

static bool is_equal(char c){
  return find(EQUAL.begin(),EQUAL.end(),c)!=EQUAL.end();
}

// the return string is the first part, the argument is missing it and an EQUAL
static string split(string& str){
  typedef string::size_type string_size;
  string_size i=0;

  // find the break
  while(i<str.size() && !is_equal(str[i]))
    i++;

  // perform the split
  string result=str.substr(0,i);
  str.erase(0,i+1);

  return result;
}

/*
 * This takes a string and trims off the flag and returns what is left
 */
static void trim_arg(string &arg, const string &flag){

  // trim off the flag
  arg.erase(0,flag.size());

  // check that there is something left
  if(arg.size()<=0)
    return;

  // trim off the equal sign (if there)
  if(starts_with(arg,EQUAL))
    arg.erase(0,1);
}

static void print_version(){
  cout << "nxtranslate (NXtranslate) version " << TRANSLATE_VERSION << endl;
  cout << endl;
  cout << "Copyright (c) 2004-2006, P.F.Peterson <petersonpf@ornl.gov>" << endl;
  cout << "NXtranslate can be copied under the terms of the \"MIT License\", which may be " << endl;
  cout << "found in the source distribution." << endl;
}

static void print_help(const string &progname, int level){
  print_version();
  cout << endl;

  cout << "USAGE: " << progname << " [filename] <options>" << endl;

  if(level<1) return;

  cout << endl;
  cout << "OPTIONS:" << endl;
  cout << " --help        Print out this message and exit" << endl;
  cout << " --version     Print out the version and exit" << endl;
  cout << "  -o <outfile> Specify output file. The default is the translation file with\n"
       << "               \".nxs\" appended." << endl;
  cout << " --append <outfile> Specify appending to the output file." << endl;
#ifdef HDF4
  cout << " --hdf4        Write file using the hdf4 base." << endl;
#endif
#ifdef HDF5
  cout << " --hdf5        Write file using the hdf5 base." << endl;
#endif
#ifdef NXXML
  cout << " --xml         Write file using the xml base." << endl;
#endif
  cout << "  -D <macro>   Specify a macro. The macro should be in the form of\n"
       << "               \"FILE=old_nexus.nxs\". The \"=\" is required." << endl;
#ifdef USE_TIMING
  cout << "  --timing     Print out timing information for the execution of the program." << endl;
#endif
}

int main(int argc, char *argv[]){
#ifdef USE_TIMING
  time_t start_time = time(NULL);
#endif
  Options options;
  options.base=NXACC_CREATE;
  options.append=false;
  options.timing=false;

  // parse the command line options (i=0 is the program name
  for( int i=1 ; i<argc ; i++ ){
    // set up strings for this and the next argument
    string arg1=argv[i];
    string arg2;
    if(i+1<argc)
      arg2=argv[i+1];

    if(starts_with(arg1,"--help")){
      print_help(argv[0],10);
      exit(0);
    }else if(starts_with(arg1,"--version")){
      print_version();
      exit(0);
    }else if(starts_with(arg1,"-o")){
      trim_arg(arg1,"-o");
      if(arg1.size()>0){
        options.outfile=arg1;
      }else if(arg2.size()>0){
        options.outfile=arg2;
        i++;
      }else{
        print_help(argv[0],0);
        exit(-1);
      }
    }else if(starts_with(arg1,"--append")){
      trim_arg(arg1,"--append");
      options.append=true;
      if(arg1.size()>0){
        options.outfile=arg1;
      }else if(arg2.size()>0){
        options.outfile=arg2;
        i++;
      }else{
        print_help(argv[0],0);
        exit(-1);
      }
    }else if(starts_with(arg1,"-D")){
      trim_arg(arg1,"-D");
      if(arg1.size()<=0){
        arg1=arg2;
        i++;
      }
      string key=split(arg1);
      options.map.insert(make_pair(key,arg1));
    }else if(starts_with(arg1, "--timing")) {
      options.timing = true;
#ifdef HDF4
    }else if(starts_with(arg1,"--hdf4")){
      options.base=NXACC_CREATE4;
#endif
#ifdef HDF5
    }else if(starts_with(arg1,"--hdf5")){
      options.base=NXACC_CREATE5;
#endif
#ifdef NXXML
    }else if(starts_with(arg1,"--xml")){
      options.base=NXACC_CREATEXML;
#endif
    }else{
      options.infile=arg1;
    }
  }

  // print usage if there is no input file
  if(options.infile.size()<=0){
    print_help(argv[0],0);
    exit(-1);
  }

  // check what is needed for appending
  if(options.append){
    if(options.outfile.size()<=0){
      cout << "Need to specify a file for \"append\" mode." << endl;
      exit(-1);
    }
    std::ifstream checkFile(options.outfile.c_str());
    bool is_readable(checkFile);
    checkFile.close();
    if(!is_readable)
      options.append=false;
  }

  // if the output file name is blank then create one
  if(options.outfile.size()<=0){
    options.outfile=options.infile;
    string::size_type size=options.outfile.size();
    if(options.outfile.substr(size-4,size)==".xml")
      options.outfile.erase(size-4,size);
    else if(options.outfile.substr(size-4,size)==".nxt")
      options.outfile.erase(size-4,size);
    options.outfile+=".nxs";
  }

  // open the output file
  NXhandle handle;
  char out_file[50];
  strcpy(out_file,options.outfile.c_str());
  NXaccess base=options.base;
  if(options.append)
    base=NXACC_RDWR;
  if(NXopen(out_file,base,&handle)!=NX_OK){
    std::cerr << "Error opening output file \"" << options.outfile << "\"" << endl;
    return -1;
  }

  // parse the file
  bool result=xml_parser::parse_xml_file(options.map,options.infile,&handle,
                                         options.timing);

  // close the output file
  NXclose(&handle);

  // delete the created file if result is non-zero
  if(result){
    if(std::remove(options.outfile.c_str()))
      std::perror("ERROR DELETING FILE");
  }
  
#ifdef USE_TIMING
  if (options.timing) {
    cout << print_time(start_time) << " total time" << endl;
  }
#endif
  return result;
}
