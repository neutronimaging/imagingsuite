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
#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <algorithm>
#include "nxdir.h"

typedef std::vector<Node> NodeVector;
typedef Tree::const_iterator TreeIter;
typedef Path::const_iterator PathIter;
typedef StringVector::const_iterator StrVecIter;
static const std::string DOT=".";
static const std::string STAR="*";

enum{UNINIT=-1,ISSTAR=-2};

using std::string;
using std::vector;

/*
 * Determine the number of groups in the file
 */
static int num_group(NXhandle handle){
  char group_name[GROUP_STRING_LEN];
  char group_klass[GROUP_STRING_LEN];
  int num_groups;

  if(NXgetgroupinfo(handle,&num_groups,group_name,group_klass)!=NX_OK)
    throw "NXgetgroupinfo failed";

  return num_groups;
}

/*
 * Get the next group in the file at the current level.
 */
static Node get_next_group(NXhandle handle){
  char name[GROUP_STRING_LEN], class_name[GROUP_STRING_LEN];
  int datatype;

  // get the information
  if(NXgetnextentry(handle,name,class_name,&datatype)!=NX_OK)
    throw "NXgetnextentry failed";

  // copy it into the supplied node
  Node node;
  node.name=name;
  node.type=class_name;

  return node;
}

/*
 * Checks the node for certain oddities that should be hidden from the user.
 */
static bool node_is_okay(const Node node){
  if(node.name=="UNKNOWN")
    return false;
  if(node.type=="UNKNOWN")
    return false;
  if(node.type=="CDF0.0")
    return false;
  return true;
}

/*
 * Get the listing of groups at the currently opened level.
 */
static NodeVector get_groups(NXhandle handle){
  if(NXinitgroupdir(handle)!=NX_OK)
    throw "NXinitgroupdir failed";

  // get the listing
  int num_groups=num_group(handle);
  Node node;
  NodeVector result;
  for( int i=0 ; i<num_groups ; i++ ){
    node=get_next_group(handle);
    if(node_is_okay(node))
      result.push_back(node);
  }

  if(NXinitgroupdir(handle)!=NX_OK)
    throw "NXinitgroupdir failed";

  return result;
}

/*
 * Get the listing of attributes of the currently opened SDS.
 */
static NodeVector get_attrs(NXhandle handle){
  // initialize the directory function
  if(NXinitattrdir(handle)!=NX_OK)
    throw "NXinitattrdir failed";

  // set up for getting listing
  char attr_name[GROUP_STRING_LEN];
  int type;
  int num_attr;
  int length;

  // find the number of attributes
  if(NXgetattrinfo(handle,&num_attr)!=NX_OK)
    throw "NXgetattrinfo failed";

  // itterate through the attributes, pushing the names on the stack
  NodeVector result;
  try{
    for( int i=0 ; i<num_attr ; i++ ){
      if(NXgetnextattr(handle,attr_name,&length,&type)!=NX_OK)
        throw "NXgetnextattr failed";

      Node attr;
      attr.name=attr_name;
      attr.type=ATTR;
      result.push_back(attr);
    }
  }catch(const char *str){
    if(NXinitattrdir(handle)!=NX_OK)
      throw "NXinitattrdir failed";
    throw; // propogate the exception up
  }

  // initialize the directory function as cleanup
  if(NXinitattrdir(handle)!=NX_OK)
    throw "NXinitattrdir failed";

  return result;
}

/*
 * Make a copy of the current path.
 */
static Path copy_path(const Path old){
  Path new_path;

  for( PathIter it=old.begin() ; it!=old.end() ; it++ ){
    new_path.push_back(*it);
  }

  return new_path;
}

/*
 * Returns true if the named attribute is at the current level. Also
 * has the side effect of setting the length and type parameters.
 */
extern bool has_attr(NXhandle handle,const string name,int *length, int *type){
  // initialize the directory function
  if(NXinitattrdir(handle)!=NX_OK)
    throw "NXinitattrdir failed";

  char attr_name[GROUP_STRING_LEN];
  const char *my_name=name.c_str();
  int num_attr;

  // find the number of attributes
  if(NXgetattrinfo(handle,&num_attr)!=NX_OK)
    throw "NXgetattrinfo failed";

  // determine if the attribute exists
  bool found=false;
  try{
    for( int i=0 ; i<num_attr ; i++ ){
      if(NXgetnextattr(handle,attr_name,length,type)!=NX_OK)
        throw "NXgetnextattr failed";
      if(strcmp(attr_name,my_name)==0){
        found=true;
        break;
      }
    }
  }catch(const char *str){
    if(NXinitattrdir(handle)!=NX_OK)
      throw "NXinitattrdir failed";
    throw; // propogate the exception up
  }

  // initialize the directory function as cleanup
  if(NXinitattrdir(handle)!=NX_OK)
    throw "NXinitattrdir failed";

  return found;
}

/*
 * Returns true if there is a group with the correct name at the
 * current level.
 */
static bool has_group(NXhandle handle, const string name){
  NodeVector groups=get_groups(handle);

  bool found=false;
  for( NodeVector::const_iterator group=groups.begin() ; group!=groups.end()
         ; group++ ){
    if(group->name==name){
      found=true;
      break;
    }
  }

  return found;
}

/*
 * Close the path a particular number of times. has_data specifies if
 * a SDS needs to be closed first.
 */
static int close_num(NXhandle handle, int length, int has_data){
  if( (has_data<=0) && (length<=0) )
    return 0;

  int num_closes=0;

  if(has_data){
    num_closes++;
    if(NXclosedata(handle)!=NX_OK)
      throw("IOError closing path (NXclosedata)");
  }

  for( int i=0 ; i<length ; i++ ){
    num_closes++;
    if(NXclosegroup(handle)!=NX_OK)
      throw("IOError closing path (NXclosegroup)");
  }

  return num_closes;
}

/*
 * Closes the handle according to the specified path.
 */
extern int close_path(NXhandle handle, const Path path){
  int num_entry=0;
  int has_data=0;

  if(path.empty())
    return 0;

  if(path.rbegin()->type==SDS)
    has_data=1;

  num_entry=(int)path.size()-has_data;

  return close_num(handle,num_entry,has_data);
}

/*
 * Opens the handle according to the specified path.
 */
extern int open_path(NXhandle handle, const Path path){
  int num_open=0;
  int has_data=0;

  char name[GROUP_STRING_LEN];
  char type[GROUP_STRING_LEN];
  for( PathIter iter=path.begin() ; iter!=path.end() ; iter++ ){
    if(iter->type==SDS){
      has_data=1;
      strcpy(name,iter->name.c_str());
      if(NXopendata(handle,name)!=NX_OK){
        close_num(handle,num_open,has_data);
        throw "NXopendata failed";
      }
      break;
    }else{
      strcpy(name,iter->name.c_str());
      strcpy(type,iter->type.c_str());
      if(!has_group(handle,name)){
        close_num(handle,num_open,0);
        throw "Group does not exist";
      }

      if(NXopengroup(handle,name,type)!=NX_OK){
        close_num(handle,num_open,0);
        throw "NXopengroup failed";
      }
      num_open++;
    }
  }

  return num_open+has_data;
}

/*
 * Returns true if two paths point at the same thing
 */
static bool paths_equal(const Path path1, const Path path2){
  if(path1.size()!=path2.size())
    return false;

  int length=path1.size();
  PathIter node1=path1.begin();
  PathIter node2=path2.begin();
  for( int i=0 ; i<length ; i++ ){
    if((node1+i)->type!=(node2+i)->type)
      return false;
    if((node1+i)->name!=(node2+i)->name)
      return false;
  }

  return true;
}

/*
 * Adds a Path to the Tree. This confirms that the Path is not already
 * in the tree.
 */
static void add_Path(Tree *tree, Path path){

  // check for the path in the tree
  for( TreeIter old_path=(*tree).begin() ; old_path!=(*tree).end() ; old_path++ ){
    if(paths_equal(*old_path,path)){
      return;
    }
  }

  // if it got here the path must not be in the tree already
  (*tree).push_back(path);
}

/*
 * Merges two trees, the second is brought into the first.
 */
static void merge_trees(Tree *dest, const Tree *source){
  for( TreeIter path=(*source).begin() ; path!=(*source).end() ; path++){
    add_Path(dest,*path);
  }
}

static Tree make_tree(Path open_path, NodeVector groups){
  Tree tree;
  for( NodeVector::const_iterator group=groups.begin() ; group!=groups.end() ;
       group++){
    Path path=copy_path(open_path);
    path.push_back(*group);
    tree.push_back(path);
  }

  return tree;
}

/*
 * This builds a tree of attributes from the data at the supplied
 * path.
 */
static Tree build_attr_tree(NXhandle handle, Path path){
  Tree tree;

  // make sure there is somewhere to go
  if(path.empty())
    return tree;

  // make sure that the final spot is an SDS
  if(path.rbegin()->type!=SDS)
    return tree;

  // get a listing of attributes at the location
  open_path(handle,path);
  NodeVector attrs=get_attrs(handle);
  close_path(handle,path);

  // create the tree
  tree=make_tree(path,attrs);

  return tree;
}

/*
 * This checks for everything except "*".
 */
static bool node_okay( const Node &node, const string &restrict){
  if(restrict==DOT)
    return true;
  else if(restrict==node.name)
    return true;
  else if(restrict==node.type)
    return true;

  return false;
}

/*
 * This builds a tree with the restriction that the portion of the
 * path past what is given must match the supplied iterators.
 */
static Tree build_res_rel_tree(NXhandle handle, Path path,StrVecIter res_begin,
                                                           StrVecIter res_end){

  Tree tree;

  // make sure there is something to match against
  if(res_begin==res_end)
    return tree;

  // what to do if the path isn't empty
  if(!path.empty()){
    if(path.rbegin()->type==ATTR){
      return tree;
    }else if(path.rbegin()->type==SDS){
      Tree temp_tree=build_attr_tree(handle,path);
      if(STAR==*res_begin && res_begin+1!=res_end){
        res_begin++;
      }
      for( TreeIter path=temp_tree.begin() ; path!=temp_tree.end() ; path++ ){
        if( node_okay(*((*path).rbegin()),*res_begin) )
          tree.push_back(*path);
      }
      return tree;
    }
  }

  // open the path
  int num_open=open_path(handle,path);
  if(num_open!=path.size()){
    close_num(handle,num_open,0);
    throw "failed to open path";
  }

  // get the list of groups in this path
  NodeVector groups;
  groups=get_groups(handle);

  // close the path
  int num_close=close_path(handle,path);
  if(num_close!=path.size())
    throw "failed to close path";

  // reduce the possible groups
  NodeVector good_groups;
  for( NodeVector::const_iterator group=groups.begin() ; group!=groups.end() ; group++ ){
    if( node_okay(*group,*res_begin) || (STAR==*res_begin) )
      good_groups.push_back(*group);
  }

  // build the new tree at this level
  tree=make_tree(path,good_groups);

  // return now if done parsing
  if(res_begin+1==res_end)
    return tree;

  // recurse down
  Tree full_tree;
  for( TreeIter it=tree.begin() ; it!=tree.end() ; it++ ){
    Tree rel_tree;
    if(STAR==*res_begin){
      if(node_okay(*((*it).rbegin()),*(res_begin+1))){
        if(res_begin+2!=res_end)
          rel_tree=build_res_rel_tree(handle,*it,res_begin+2,res_end);
        else
          return tree;
      }else{
        rel_tree=build_res_rel_tree(handle,*it,res_begin,res_end);
      }
    }else{
      rel_tree=build_res_rel_tree(handle,*it,res_begin+1,res_end);
    }
    merge_trees(&full_tree,&rel_tree);
  }

  return full_tree;
}

/*
 * Build a tree relative to the supplied path. This is a recursive
 * function which continues down to any available SDSs. The NXhandle
 * is back in its initial state after the function completes.
 */
extern Tree build_rel_tree(NXhandle handle, const Path &path, int num_recurse){
  // allocate the static StringVector for use in build_res_rel_tree
  static StringVector restrict;
  if(restrict.empty()) restrict.push_back(DOT);

  // prepare the variable to store the tree
  Tree tree;

  // check for a sensible path to recurse into
  if(num_recurse==0)
    return tree;

  // decrease the number of levels left
  num_recurse--;

  // build this level
  tree=build_res_rel_tree(handle,path,restrict.begin(),restrict.end());

  // recurse down
  Tree full_tree;
  full_tree.insert(full_tree.end(),tree.begin(),tree.end());
  for( TreeIter it=tree.begin() ; it!=tree.end() ; it++ ){
    Tree rel_tree=build_rel_tree(handle,*it,num_recurse);
    merge_trees(&full_tree,&rel_tree);
  }

  return full_tree;
}

/*
 * Open the tree to the absolute path specified suplied and build a
 * one-level tree there, which is returned. The NXhandle is back in
 * its initial state after the function completes.
 */
static Tree build_tree_start(NXhandle handle, StringVector abs_path){
  Path path;
  Tree tree;
  if(abs_path.empty()){
    NodeVector groups=get_groups(handle);
    tree=make_tree(path,groups);
  }else{  
    tree=build_res_rel_tree(handle,path,abs_path.begin(),abs_path.end());
  }

  return tree;
}

/*
 * Effectively a writting of operator< for Path.
 */
bool compPath(const Path path1, const Path path2){

  // set up for how many elements to itterate over
  int path1_len=path1.size();
  int path2_len=path2.size();
  int len=path1_len;
  if(path2_len<len)
    len=path2_len;

  // compare the names alphabetically
  for( int i=0 ; i<len ; i++ ){
    if(path1[i].name==path2[i].name)
      continue;
    else
      return (path1[i].name<path2[i].name);
  }

  // compare the lengths
  return (path1_len<path2_len);
}

static int num_star( const StringVector strvec){
  int num=0;
  for( StrVecIter str=strvec.begin() ; str!=strvec.end() ; str++ )
    if(STAR==*str)
      num++;

  return num;
}

/*
 * This will build a Tree of the file as specified by options. The
 * NXhandle is returned to it original state and the Tree is sorted.
 */
extern Tree build_tree(NXhandle handle, const StringVector abs_path,
                                                        const int num_recurse){
  // build the start of the tree
  Tree tree=build_tree_start(handle,abs_path);
  if(tree.empty())
    return tree;

  // purge the existing tree if using stars to build the start
  if(num_star(abs_path)>0)
    tree=purge_tree(tree,abs_path,-1);

  // adjust the recurse level as necessary
  int rec_lev=num_recurse;
  if(abs_path.size()<=0) rec_lev--;

  // if should recurse build from the existing tree
  if(rec_lev!=0){
    Tree full_tree;
    full_tree.insert(full_tree.end(),tree.begin(),tree.end());
    for( TreeIter it=tree.begin() ; it!=tree.end() ; it++ ){
      Tree rel_tree=build_rel_tree(handle,*it,rec_lev);
      merge_trees(&full_tree,&rel_tree);
    }
    tree=full_tree;
  }

  // sort the results
  std::sort(tree.begin(),tree.end(),compPath);

  return tree;
}

static bool has_all_res(const Path &path, const StringVector &restrict){
  int num=0;

  for( StrVecIter res=restrict.begin() ; res!=restrict.end() ; res++ ){
    if(*res==STAR){
      num++;
      continue;
    }
    for( PathIter node=path.begin() ; node!=path.end() ; node++ ){
      if(node_okay(*node,*res)){
        num++;
        break;
      }
    }
  }

  return (restrict.size()==num);
}

static int calc_offset(const int* array, int index){
  if(index<=0)
    return 0;

  int val=array[index-1];
  if(val>=0)
    return val+1;
  if(val==ISSTAR)
    return(calc_offset(array,index-1));

  return 0;
}

static int * index_path(const Path &path, const StringVector &restrict){
  int res_len=restrict.size();
  int* indices=new int[res_len];

  for( int i=0 ; i<res_len ; i++ )
    indices[i]=UNINIT;

  StrVecIter res=restrict.begin();
  PathIter node=path.begin();
  int path_len=path.size();
  for( int i=0 ; i<res_len ; i++ ){
    if( *(res+i)==STAR){
      indices[i]=ISSTAR;
      continue;
    }
    for( int j=calc_offset(indices,i) ; j<path_len ; j++ ){
      if(node_okay(*(node+j),*(res+i))){
        indices[i]=j;
        break;
      }
    }
  }

  return indices;
}

/*
 * Returns true or false if the path is okay.
 */
static bool path_is_okay(const Path path, const StringVector rel_path){
  // determine if the path is long enough to hold the relative path
  if(path.size()<rel_path.size()-num_star(rel_path))
    return false;

  // determine if all of the required path elements are in the path
  if(!has_all_res(path,rel_path))
    return false;

  // index the path
  int* indices=index_path(path,rel_path);

  /* REMOVE
  std::cout << "PATH[";
  for( int i=0 ; i<rel_path.size() ; i++ ){
    std::cout << indices[i];
    if(i+1<rel_path.size()) std::cout << ",";
  }
  std::cout << "]"; print_path(path,NAME);
  */

  // do checks against indices
  int last_index=UNINIT;
  for( int i=0 ; i<rel_path.size() ; i++ ){
    int this_index=indices[i];
    if(this_index==UNINIT)
      return false;
    else if(this_index==ISSTAR)
      continue;
    if(last_index==UNINIT)
      last_index=this_index;
    else if(this_index<=last_index)
      return false;
    else
      last_index=this_index;
  }

  // if we got this far it must be okay
  return true;
}

/*
 * Determine the location of the restriction from the end.
 */
static int find_index(const Path &path, string restrict){
  int index=0;

  for( Path::const_reverse_iterator node=path.rbegin() ; node!=path.rend()
                                                                    ; node++ ){
    if(node_okay(*node,restrict))  break;
    index++;
  }

  return index;
}

/*
 * Remove unwanted Paths from the tree according to rules in path_is_okay.
 */
extern Tree purge_tree(const Tree tree, const StringVector rel_path,
                                                        const int rel_recurse){
  if(tree.empty() || rel_path.empty())
    return tree;

  Tree good;

  for( TreeIter path=tree.begin() ; path!=tree.end() ; path++ ){
    if(path_is_okay(*path,rel_path))
      good.push_back(*path);
  }

  // if the relative recurse level is -1 then just return the full tree
  if(rel_recurse<0 || rel_path.empty())
    return good;

  // trim out the tree according to where the final element occurs
  Tree trimmed;
  string res_end=*(rel_path.rbegin());
  for( TreeIter path=good.begin() ; path!=good.end() ; path++ ){
    if(find_index(*path,res_end)<=rel_recurse)
      trimmed.push_back(*path);
  }

  return trimmed;
}

/*
 * Remove parts of the tree that do not end with the relative path
 */
/*
extern Tree trim_tree_end(const Tree tree, const StringVector rel_path){
  if(tree.empty() || rel_path.empty())
    return tree;

  Tree good;
  string path_end=*(rel_path.rbegin());
//  std::cout << "PATH_END=" << path_end << std::endl;

  for( TreeIter path=tree.begin() ; path!=tree.end() ; path++ ){
    Node node=*((*path).rbegin());

    if( node_okay(node,path_end) )
      good.push_back(*path);
  }

  return good;
}
*/
