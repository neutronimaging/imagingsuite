#include <stdexcept>
#include "napiconfig.h"
#include "retriever.h"
#include "../string_util.h"

using std::invalid_argument;
using std::runtime_error;
using std::string;
using std::cout;
using std::endl;


/**
 * \file NXtranslate/retriever.cpp
 */

/**
 * All nodes returned by LoopyRetriever::getData will have this name.
 */
static const string NAME("loopy");

/**
 * This is the constructor for the object. No resources are allocated.
 *
 * \param str The source string is ignored, but necessary to implement
 * the interface.
 */
LoopyRetriever::LoopyRetriever(const string &str)
{
  // there is nothing to allocate
}

/**
 * Since no resources were allocated, the destructor does nothing
 */
LoopyRetriever::~LoopyRetriever()
{
  // there is nothing to deallocate
}

/**
 * Converts a string describing a type into an enumeration.
 *
 * \param type The string version of the type.
 *
 * \return The integer from the napi.h enumeration.
 */
static int str_to_type(const string &type){
  if(type.find("INT8")==0)
    {
      return NX_INT8;
    }
  else if(type.find("INT16")==0)
    {
      return NX_INT16;
    }
  else if(type.find("INT32")==0)
    {
      return NX_INT32;
    }
  else if(type.find("UINT8")==0)
    {
      return NX_UINT8;
    }
  else if(type.find("UINT16")==0)
    {
      return NX_UINT16;
    }
  else if(type.find("UINT32")==0)
    {
      return NX_UINT32;
    }
  else if(type.find("UINT32")==0)
    {
      return NX_UINT32;
    }
 else if(type.find("FLOAT32")==0)
   {
     return NX_FLOAT32;
   }
 else if(type.find("FLOAT64")==0)
   {
     return NX_FLOAT64;
   }
  throw invalid_argument("Do not understand type:"+type);
}

/**
 * Does the mathematics involved in filling the data array. This is
 * done according to the equation:
 * \f[
 * data[i]=delta i + offset
 * \f]
 * where \f$i\f$ is the looping variable
 *
 * \param data reference to the allocated data array to be filled
 * \param offset is the constant to be added to each value
 * \param delta is the constant spacing between values
 * \param num the number of values to put into the array
 */
template <typename NumT>
void
fill_array(void *&data, const NumT offset, const NumT &delta, const int num){
  // the looping variable is an int because NeXus holds the data
  // length as an int.
  for( int i=0 ; i<num ; i++ )
    {
      (static_cast<NumT *>(data))[i]=(delta*static_cast<NumT>(i)+offset);
    }
}

/**
 * This is the method for retrieving data from a file. The string must
 * be of the form "type:offset,delta,number".
 *
 * \param location is the string that is used by the retriever to
 * create the data.
 * \param tr is the tree to put the result into.
 */
void LoopyRetriever::getData(const string &location, tree<Node> &tr)
{
  // check that the argument is not an empty string
  if(location.size()<=0)
    {
      throw invalid_argument("cannot parse empty string");
    }

  // check that it has the correct number of colons
  if(string_util::count_occur(location,":")!=1)
    {
      throw invalid_argument("location must contain only one colon");
    }

  // check that it has 
  if(string_util::count_occur(location,",")!=2)
    {
      throw invalid_argument("location must contain only two commas");
    }

  string::size_type index;

  // split the string into type and loop information
  index=location.find(":");
  string type=location.substr(0,index);
  string loopinfo=location.substr(index+1);

  // split the loop parameters into separate strings
  index=loopinfo.find(",");
  string offset_str=loopinfo.substr(0,index);
  loopinfo=loopinfo.substr(index+1);
  index=loopinfo.find(",");
  string delta_str=loopinfo.substr(0,index);
  string num_str=loopinfo.substr(index+1);

  // create the dimensions array - this can throw an exception
  int dims[]={static_cast<int>(string_util::str_to_int(num_str))};

  // convert the type to an integer
  int int_type=str_to_type(type);

  // get memory for the data
  void *data;
  if(NXmalloc(&data,1,dims,int_type)!=NX_OK)
    {
      throw runtime_error("NXmalloc failed");
    }

  // fill the data array
  if(int_type==NX_INT8)
    {
      fill_array(data,
                 static_cast<int8_t>(string_util::str_to_int(offset_str)),
                 static_cast<int8_t>(string_util::str_to_int(delta_str)),
                 dims[0]);
    }
  else if(int_type==NX_INT16)
    {
      fill_array(data,
                 static_cast<int16_t>(string_util::str_to_int(offset_str)),
                 static_cast<int16_t>(string_util::str_to_int(delta_str)),
                 dims[0]);
    }
  else if(int_type==NX_INT32)
    {
      fill_array(data,
                 static_cast<int32_t>(string_util::str_to_int(offset_str)),
                 static_cast<int32_t>(string_util::str_to_int(delta_str)),
                 dims[0]);
    }
  else if(int_type==NX_UINT8)
    {
      fill_array(data,
                 static_cast<uint8_t>(string_util::str_to_uint(offset_str)),
                 static_cast<uint8_t>(string_util::str_to_uint(delta_str)),
                 dims[0]);
    }
  else if(int_type==NX_UINT16)
    {
      fill_array(data,
                 static_cast<uint16_t>(string_util::str_to_uint(offset_str)),
                 static_cast<uint16_t>(string_util::str_to_uint(delta_str)),
                 dims[0]);
    }
  else if(int_type==NX_UINT32)
    {
      fill_array(data,
                 static_cast<uint32_t>(string_util::str_to_uint(offset_str)),
                 static_cast<uint32_t>(string_util::str_to_uint(delta_str)),
                 dims[0]);
    }
  else if(int_type==NX_FLOAT32)
    {
      fill_array(data,
                 static_cast<float>(string_util::str_to_float(offset_str)),
                 static_cast<float>(string_util::str_to_float(delta_str)),
                 dims[0]);
    }
  else if(int_type==NX_FLOAT64)
    {
      fill_array(data,
                 static_cast<double>(string_util::str_to_float(offset_str)),
                 static_cast<double>(string_util::str_to_float(delta_str)),
                 dims[0]);
    }

  // create the node - this copies the data
  Node node=Node(NAME,data,1,dims,int_type);
  // insert the data into the tree
  tr.insert(tr.begin(),node);

  // delete the data
  if(NXfree(&data)!=NX_OK)
    {
      throw runtime_error("NXfree failed");
    }

}

/**
 * The MIME_TYPE is necessary so the retriever can be selected by the
 * factory.
 */
const string LoopyRetriever::MIME_TYPE("loopy");

/**
 * This function returns a string representation of the retriever for
 * debugging. Since no resources are allocated the string is always
 * identical.
 *
 * \return The string returned is always "[loopy]".
 */
string LoopyRetriever::toString() const
{
  return "["+MIME_TYPE+"] ";
}
