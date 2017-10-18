/*
 * Copyright (c) 2004, P.F.Peterson <petersonpf@ornl.gov>
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
#include "nxdir.h"
#include "nxdir_help.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <cstring>

// -------------------- begin TIMING TEST stuff
#ifdef TIMING_TEST
#include <time.h> // 
#define convert_time(time) (1000.*((float)time)/((float)CLOCKS_PER_SEC))
#define print_time(zero) printf("TIME=%7.3fms\n",convert_time(clock()-zero))
#endif
// -------------------- end TIMING TEST stuff

#define NXDIR_VERSION "0.3.0"

// bring stuff into this name space
using std::cout;
using std::endl;
using std::string;
using std::vector;

// some constants for command line flags
static const string BINDUMP="--dump";
static const string WRITEDATA="--write-data";
static const string TREEMODE="--tree-mode";
static const string TREEMODE_SHORT="-t";
static const string PATHMODE="--path-mode";
static const string DATAMODE="--data-mode";
static const string OUTPUTMODE="--printline";
static const string MAXLENGTH="--max-array";
static const string MAXLENGTH_SHORT="-l";
static const string PRINTDATA="-o";
static const string NOT_PRINTDATA="+o";
static const string ARG_VERSION="--version";
static const string PATH="-p";
static const string HELP="--help";
static const string HELP_SHORT="-h";
static const string VERBOSE="-v";
static const string STAR="*";
#define HELP_PAD "                            "


/*
 * Returns true if str1 starts with str2
 */
static bool starts_with( const string str1, const string str2){
  // empty string can't start with anything
  if(str1.empty())
    return false;

  // can't start with something that is longer
  if(str2.size()>str1.size())
    return false;

  // if they are the same then just return true
  if(str1==str2)
    return true;

  // do the actual comparison
  string cmp_str=str1.substr(0,str2.size());
  return (cmp_str==str2);
}

/*
 * Returns true if str1 ends with str2
 */
static bool ends_with( const string str1, const string str2){
  // empty string can't start with anything
  if(str1.empty())
    return false;

  // can't start with something that is longer
  if(str2.size()>str1.size())
    return false;

  // if they are the same then just return true
  if(str1==str2)
    return true;

  // do the actual comparison
  string cmp_str=str1.substr(str1.size()-str2.size(),str1.size());
  return (cmp_str==str2);
}

static string trim_arg(const string arg, const string flag){
  static const string EQUAL="=";

  // trim off the flag
  string result=arg.substr(flag.size(),arg.size());

  // check that there is something left
  if(result.size()<=0)
    return result;

  // trim off the equal sign (if there)
  if(starts_with(result,EQUAL))
    result=result.substr(1,result.size());

  // return result
  return result;
}

static int get_arg(const string arg1, const string arg2, const string flag, string *result){
  int return_val=0;

  *result=trim_arg(arg1,flag);
  if((*result).size()<=0){
    if(arg2.size()<=0) return -1;
    if(starts_with(arg2,"-")) return -1;
    return_val++;
    *result=arg2;
  }

  return return_val;
}

static int set_treemode(const string arg1, const string arg2, const string flag, PrintConfig *config){
  // get the proper argument to parse
  string result;
  int return_val=get_arg(arg1,arg2,flag,&result);
  if(return_val<0)
    return 0;

  // convert the string to enum
  if(result=="script")
    (*config).tree_mode=TREE_SCRIPT;
  else if(result=="multi")
    (*config).tree_mode=TREE_MULTI;
  else if(result=="tree")
    (*config).tree_mode=TREE_TREE;
  else
    return 0;

  // return the result
  return return_val;
}

static string get_treemode_help(){
  string result="  ";

  result+=TREEMODE_SHORT+"|"+TREEMODE+" <value>";

  result+="    ";

  result+="Sets the formatting of the tree. Allowed values are \n";
  result+=HELP_PAD;

  if(DEFAULT_TREE_MODE==TREE_SCRIPT)
    result+="(script)|multi|tree";
  else if(DEFAULT_TREE_MODE==TREE_MULTI)
    result+="script|(multi)|tree";
  else if(DEFAULT_TREE_MODE==TREE_TREE)
    result+="script|multi|(tree)";

  return result;
}

static int set_pathmode(const string arg1, const string arg2, const string flag, PrintConfig *config){
  // get the proper argument to parse
  string result;
  int return_val=get_arg(arg1,arg2,flag,&result);
  if(return_val<0)
    return 0;

  // convert the string to enum
  if(result=="name")
    (*config).path_mode=NAME;
  else if(result=="class")
    (*config).path_mode=TYPE;
  else if(result=="both")
    (*config).path_mode=NAME_TYPE;

  // return the result
  return return_val;
}

static string get_pathmode_help(){
  string result="  "+PATHMODE+" <value>       ";

  result+="Select whether paths are written with names or \n";
  result+=HELP_PAD;
  result+="classes. Allowed values are ";

  if(DEFAULT_PATH_MODE==NAME)
    result+="(name)|class|both";
  else if(DEFAULT_PATH_MODE==TYPE)
    result+="name|(class)|both";
  else if(DEFAULT_PATH_MODE==NAME_TYPE)
    result+="name|class|(both)";

  return result;
}

static int set_datamode(const string arg1, const string arg2, const string flag, PrintConfig *config){
  // get the proper argument to parse
  string result;
  int return_val=get_arg(arg1,arg2,flag,&result);
  if(return_val<0)
    return 0;

  // convert the string to enum
  (*config).print_data=true;
  if(result=="script")
    (*config).data_mode=DATA_SCRIPT;
  else
    return 0;

  // return the result
  return return_val;
}

static string get_datamode_help(){
  string result="  "+DATAMODE+" <value>       ";

  result+="How data is printed. Allowed values are (script)";

  return result;
}

static int set_outputmode(const string arg1, const string arg2, const string flag, PrintConfig *config){
  // get the proper argument to parse
  string result;
  int return_val=get_arg(arg1,arg2,flag,&result);
  if(return_val<0)
    return 0;

  // convert the string to enum
  if(result=="single")
    (*config).output_line_mode=OUT_SINGLE;
  else
    return 0;

  // return the result
  return return_val;
}

static string get_outputmode_help(){
  string result="  "+OUTPUTMODE+" <value>       ";

  result+="How data is printed with respect to tree. Allowed \n";
  result+=HELP_PAD;
  result+="values are (single).";

  return result;
}

static string get_printdata_help(){
  string result="  "+PRINTDATA+"/"+NOT_PRINTDATA+"                     ";
  result+="Print (or not) the value of selected nodes, if \n";
  result+=HELP_PAD;
  result+="possible. ";
  if(DEFAULT_PRINT_DATA)
    result+="Defaults to true (-o).";
  else
    result+="Defaults to false (+o).";

  return result;
}

// print the version documentation
static void print_version(const string progname, int length){
  cout << progname << " (NXdir) version "<< NXDIR_VERSION << endl;

  if(length<=0) return;

  // print the version information
  cout << endl;
  for( int i=0 ; vers_text[i]!=NULL && i<200 ; i++ )
    cout << vers_text[i] << endl;
}

// print the help documentation
static void print_help(const string progname, int length){
  if(length>0){
    print_version(progname,1);
    cout << endl;
  }

  cout << "Usage: " << progname << " <filename> [options]" << endl;
  if(length<=0) return;

  // ----- print the standard text
  for( int i=0 ; help_text[i]!=NULL && i<200 ; i++ )
    cout << help_text[i] << endl;
  cout << endl;
  // ----- print the dynamic text
  cout << "About NXdir" << endl;
  cout << "  " << HELP_SHORT << "|" << HELP << "                 Print this help information" << endl;
  cout << "  " << ARG_VERSION << "                 Print the version information" << endl;
  cout << endl;
  cout << "Node Selection" << endl;
  cout << "  " << PATH << "                        Path inside the file to look in. This can be \n" << HELP_PAD << "absolute or relative and can be class or name of a \n" << HELP_PAD << "given field. To anchor the path at the beginning or \n" << HELP_PAD << "end place a \"/\" there.\n" << HELP_PAD << "To specify that a level must exist, but\n" << HELP_PAD << "the name or class can be anything, use a dot \".\"." << endl;
  cout << HELP_PAD << "To specify that any number of levels can exist, use " << endl;
  cout << HELP_PAD << "a star \"*\"." << endl;
  cout << endl;
  cout << "Output Control" << endl;
  cout << get_printdata_help() << endl;
/*
  cout << "  " << PRINTDATA << "/" << NOT_PRINTDATA << "                     Print (or not the value of selected nodes, if " << endl;
  cout << HELP_PAD << "possible. ";
  if(DEFAULT_PRINT_DATA){
    cout << "Defaults to true (-o).";
  }else{
    cout << "Defaults to false (+o).";
  }
  cout << endl;
*/
  cout << "  " << MAXLENGTH_SHORT << "|" << MAXLENGTH << "            Change the number of elements that are printed for \n" << HELP_PAD << "arrays. Forces \"" << PRINTDATA << "\". (" << DEFAULT_MAX_LENGTH << ")" << endl;
  cout << get_treemode_help() << endl;
  cout << get_pathmode_help() << endl;
  cout << get_datamode_help() << endl;
  cout << get_outputmode_help() << endl;
  cout << "  " << WRITEDATA << " <filename>   "
       << "Select a file to write out selected NXdata to." << endl;
  cout << "  " << BINDUMP << " <filename>         " << "Generate a binary dump of the selected node." << endl;
}

// check if the file exists by opening for reading
static bool file_exists(const string file){
  FILE *handle=fopen(file.c_str(),"r");

  bool exists=(handle!=NULL);

  if(exists)
    fclose(handle);

  return exists;
}

static void init_print_config(PrintConfig *print_config){
  print_config->tree_mode=DEFAULT_TREE_MODE;
  print_config->path_mode=DEFAULT_PATH_MODE;
  print_config->data_mode=DEFAULT_DATA_MODE;
  print_config->output_line_mode=DEFAULT_OUT_LINE_MODE;
  print_config->print_data=DEFAULT_PRINT_DATA;
  print_config->max_length=DEFAULT_MAX_LENGTH;
  print_config->show_filename=true;
}

/*
 * This tries to set the value of an int. If it for some reason fails
 * it does nothing to the value.
 */
static int set_int(const string arg1, const string arg2, int *val){
  string number=arg1;
  int arg_num=0;
  if(number.size()<=0){
    number=arg2;
    arg_num++;
  }

  // do nothing with empty strings
  if(number.size()<=0) return 0;

  // do nothing with flag
  if(starts_with(number,"-")) return 0;

  try{
    int result=str_to_int(number);
    *val=result;
    return arg_num;
  }catch(const char *str){
    return 0;
  }

}

int main(int argc, char *argv[]){
  // define the default options
  StringVector files;
  StringVector abs_path;
  StringVector rel_path;
  int recurse=DEFAULT_RECURSE;
  int build_recurse=DEFAULT_RECURSE;
  PrintConfig print_config;
  init_print_config(&print_config);
  bool verbose=false;

  // get the name of the compiled file
  string progname=argv[0];
  progname=*(arrayify(progname).rbegin());

  // process the command line
  if(argc<=1){
    cout << "NXdir: no input files" << endl;
    print_help(progname,0);
    exit(0);
  }
  for( int i=1 ; i<argc ; i++ ){

    // get a string from the argument
    string arg1=argv[i];
    string arg2;
    if(i+1<argc)
      arg2=argv[i+1];
    if(arg1==HELP || arg1==HELP_SHORT){
      print_help(progname,1);
      exit(0);
    }else if(arg1==ARG_VERSION){
      print_version(progname,1);
      exit(0);
    }else if(starts_with(arg1,TREEMODE)){
      int ret=set_treemode(arg1,arg2,TREEMODE,&print_config);
      if(ret<0){
        std::cerr << "ERROR: option \"" << TREEMODE
                  << "\" needs a value specified" << endl;
        exit(-1);
      }
      i+=ret;
    }else if(starts_with(arg1,TREEMODE_SHORT)){
      int ret=set_treemode(arg1,arg2,TREEMODE_SHORT,&print_config);
      if(ret<0){
        std::cerr << "ERROR: option \"" << TREEMODE_SHORT
                  << "\" needs a value specified" << endl;
        exit(-1);
      }
      i+=ret;
    }else if(starts_with(arg1,PATHMODE)){
      int ret=set_pathmode(arg1,arg2,PATHMODE,&print_config);
      if(ret<0){
        std::cerr << "ERROR: option \"" << PATHMODE
                  << "\" needs a value specified" << endl;
        exit(-1);
      }
      i+=ret;
    }else if(starts_with(arg1,DATAMODE)){
      int ret=set_datamode(arg1,arg2,DATAMODE,&print_config);
      if(ret<0){
        std::cerr << "ERROR: option \"" << DATAMODE
                  << "\" needs a value specified" << endl;
        exit(-1);
      }
      i+=ret;
    }else if(starts_with(arg1,OUTPUTMODE)){
      int ret=set_outputmode(arg1,arg2,OUTPUTMODE,&print_config);
      if(ret<0){
        std::cerr << "ERROR: option \"" << OUTPUTMODE
                  << "\" needs a value specified" << endl;
        exit(-1);
      }
      i+=ret;
    }else if(starts_with(arg1,BINDUMP)){
      i+=get_arg(arg1,arg2,BINDUMP,&print_config.dump_data_file);
    }else if(starts_with(arg1,WRITEDATA)){
      i+=get_arg(arg1,arg2,WRITEDATA,&print_config.data_out_file);
    }else if(arg1==VERBOSE){
      verbose=true;
    }else if(arg1==PRINTDATA){
      print_config.print_data=true;
    }else if(arg1==NOT_PRINTDATA){
      print_config.print_data=false;
    }else if(starts_with(arg1,MAXLENGTH)){
      print_config.max_length=-1;
      arg1=trim_arg(arg1,MAXLENGTH);
      i+=set_int(arg1,arg2,&print_config.max_length);
      print_config.print_data=true;
    }else if(starts_with(arg1,MAXLENGTH_SHORT)){
      print_config.max_length=-1;
      arg1=trim_arg(arg1,MAXLENGTH_SHORT);
      i+=set_int(arg1,arg2,&print_config.max_length);
      print_config.print_data=true;
    }else if(starts_with(arg1,PATH)){
      string result;
      int ret=get_arg(arg1,arg2,PATH,&result);
      if(ret<0){
        std::cerr << "ERROR: option \"" << PATH
                  << "\" needs a value specified" << endl;
        exit(-1);
      }
      i+=ret;
      rel_path=arrayify(result);
      if(starts_with(result,"/"))
        abs_path=rel_path;
      if(ends_with(result,"/"))
        recurse=0;
    }else if(file_exists(arg1)){
      files.push_back(arg1);
    }else if(starts_with(arg1,"-")){
      std::cerr << "ERROR: Do not understand option " << i << ": \"" << arg1 << "\"" << endl;
      exit(-1);
    }else{
      std::cerr << "ERROR: File \"" << arg1 << "\" does not exist (skipping)" << endl;
    }
  }

  // fix the relative path (if needed)
  if( !rel_path.empty() ){
    bool non_empty_abs=(!abs_path.empty());

    // trim off stars at the beginning
    if( !rel_path.empty() && *(rel_path.begin())==STAR ){
      build_recurse=-1;
      if(non_empty_abs){
        abs_path.erase(abs_path.begin(),abs_path.end());
        non_empty_abs=false;
      }
      while( !rel_path.empty() && *(rel_path.begin())==STAR )
        rel_path.erase(rel_path.begin(),rel_path.begin()+1);
    }

    // trim off stars at the end
    if( !rel_path.empty() && *(rel_path.rbegin())==STAR ){
      recurse=-1;
      if(non_empty_abs) build_recurse=-1;
      while( !rel_path.empty() && *(rel_path.rbegin())==STAR )
        rel_path.pop_back();
    }

    // look for double stars in the middle
    bool keep_going=!rel_path.empty();
    while(keep_going){
      for( StringVector::iterator it=rel_path.begin() ; it!=rel_path.end() ; it++ ){
        if(*it==STAR){
          if(it+1!=rel_path.end()){
            if(*(it+1)==STAR){
              rel_path.erase(it,it+1);
              break;
            }
          }
        }
        if(it+1==rel_path.end())
          keep_going=false;
      }
    }

    // update the absolute path if anything has been changed
    if( non_empty_abs && (abs_path.size()!=rel_path.size()) ){
      abs_path.erase(abs_path.begin(),abs_path.end());
      abs_path.insert(abs_path.end(),rel_path.begin(),rel_path.end());
    }
  }

  // set full recursion if no absolute path is set
  if(abs_path.empty() && !rel_path.empty())
    build_recurse=-1;

  /* QUICK DEBUG THING
  std::cout << "ABS:/"; print_strvec(abs_path); // REMOVE
  std::cout << "REL:"; print_strvec(rel_path); // REMOVE
  */

  // check that there are files to work with
  if(files.empty()){
    cout << "NXdir: no input files" << endl;
    exit(0);
  }

  if(files.size()==1)
    print_config.show_filename=false;

#ifdef TIMING_TEST
  clock_t time_zero=clock(); // TIMING TEST
#endif

  if(files.size()>1 && print_config.data_out_file.length()>0){
    std::cerr << "CANNOT OUTPUT DATA FROM MORE THAN ONE SOURCE FILE" << endl;
    exit(-1);
  }

  // loop over files
  for( StringVector::const_iterator file=files.begin() ; file!=files.end()
                                                                    ; file++ ){
    // set the filename
    print_config.filename=*file;

    // open the file for reading
    NXhandle handle;
    char filename[GROUP_STRING_LEN];
    strcpy(filename,(*file).c_str());
    if(NXopen(filename,NXACC_READ,&handle)!=NX_OK)
      continue;

    // create the tree
    Tree tree;
    try{
      tree=build_tree(handle,abs_path,build_recurse);
    }catch(const char *str){
      NXclose(&handle);
      cout << "While reading " << *file << ":" << endl;
      cout << str << endl;
      continue;
    }

    // purge the tree
    if(!rel_path.empty())
      tree=purge_tree(tree,rel_path,recurse);

    // remove any non-data from the tree
    if(print_config.data_out_file.length()>0)
      tree=remove_nondata(tree);

    if(verbose && tree.empty()){
      std::cout << "NOTHING MEETS CRITERIA IN FILE: " << filename << std::endl;
    }

    // print the results
    try{
      if(print_config.data_out_file.length()>0){
        std::ofstream out_file(print_config.data_out_file.c_str());
        write_data(out_file,handle,tree,print_config);
      }else if(!print_config.dump_data_file.empty()){
        dump_data(print_config.dump_data_file,handle,tree,print_config);
      }else{
        print_tree_result(handle,tree,print_config);
      }
    }catch(const char *str){
      NXclose(&handle);
      continue;
    }

    // close the file
    if(NXclose(&handle)!=NX_OK){
      continue;
    }
  }

#ifdef TIMING_TEST
  print_time(time_zero); // TIMING TEST
#endif

  return 0;
}
