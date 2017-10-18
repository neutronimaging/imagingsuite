#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include "napiconfig.h" // needed for HAVE_STDINT_H
#include "data_util.hpp"
#include "nxsummary.hpp"
#include "string_util.hpp"



// use STDINT if possible, otherwise define the types here
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
typedef signed char             int8_t;
typedef short int               int16_t;
typedef int                     int32_t;
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
#ifdef _MSC_VER
typedef signed __int64          int64_t; 
typedef unsigned __int64        uint64_t;
#endif //_MSC_VER
#endif //HAVE_STDINT_H


using std::ostringstream;
using std::string;
using std::runtime_error;

// useful strings for printing
static const string UNITS("units");
static const string SPACE(" ");

// constants for unit conversions
static const string SECOND("second");
static const string MINUTE("minute");
static const string HOUR("hour");
static const string DAY("day");
static const string PICOCOULOMB("picoCoulomb");
static const string MICROAMPHOUR("microAmp*hour");

// list of allowed operations
static const string OP_SUM("SUM");
static const string OP_UNITS("UNITS:");
static const string OP_DIMS("DIMS");
static const string OP_COUNT("COUNT");

/*
 * THIS SHOULD BE ABLE TO DO
 *
 * total counts in entry ???
 * principle investigator ???
 */
namespace nxsum {
  bool operationValid(const std::string &operation) {
    if (operation.compare(OP_SUM) == 0)
      {
        return true;
      }
    if (operation.compare(OP_DIMS) == 0)
      {
        return true;
      }
    if (operation.compare(OP_COUNT) == 0)
      {
        return true;
      }
    if (operation.find(OP_UNITS)!=string::npos)
      {
        return true;
      }
    return false;
  }

  string nxtypeAsString(const int type) {
    if (type == NX_CHAR)
      {
        return "NX_CHAR";
      }
    else if(type == NX_FLOAT32)
      {
        return "NX_FLOAT32";
      }
    else if(type == NX_FLOAT64)
      {
        return "NX_FLOAT64";
      }
    else if(type == NX_UINT8)
      {
        return "NX_UINT8";
      }
    else if(type == NX_UINT16)
      {
        return "NX_UINT16";
      }
    else if(type == NX_UINT32)
      {
        return "NX_UINT32";
      }
    else if(type == NX_INT8)
      {
        return "NX_INT8";
      }
    else if(type == NX_INT16)
      {
        return "NX_INT16";
      }
    else if(type == NX_INT32)
      {
        return "NX_INT32";
      }
    else
      {
        ostringstream s;
        s << type;
        return s.str();
      }
  }

  bool infoOnly(const string &operation) {
    if(operation.size() <= 0)
      {
        return false;
      }
    if (operation.compare(OP_DIMS) == 0)
      {
        return true;
      }
    if (operation.compare(OP_COUNT) == 0)
      {
        return true;
      }
    return false;
  }

  string readAttrAsString(NXhandle handle, const string &label) {
    if (NXinitattrdir(handle)!=NX_OK)
      {
        throw runtime_error("NXinitattrdir failed");
      }
    int num_attr;
    if (NXgetattrinfo(handle, &num_attr)!=NX_OK)
      {
        throw runtime_error("NXgetattrinfo failed");
      }
    char name[GROUP_STRING_LEN];
    int length;
    int type;
    for (int i = 0 ; i < num_attr ; ++i) {
      if (NXgetnextattr(handle, name, &length, &type)!=NX_OK)
        {
          throw runtime_error("NXgetnextattr failed");
        }
      if (label == name)
        {
          void *data;
          if (NXmalloc(&data, 1, &length, type)!=NX_OK)
            {
              throw runtime_error("NXmalloc failed");
            }
          int dims[1]  = {length};
          if (NXgetattr(handle, name, data, dims, &type)!=NX_OK)
            {
            throw runtime_error("NXgetattr failed");
            }
          string result = toString(data, length, type);
          if (NXfree(&data)!=NX_OK)
            {
              throw runtime_error("NXfree failed");
            }
        return result;
        }
    }

    return "";
  }

  template <typename NumT>
  NumT sum(const NumT *data, const int dims[], const int rank) {
    size_t num_ele = 1;
    for (size_t i = 0; i < rank; ++i) {
      num_ele *= dims[i];
    }
    NumT result = static_cast<NumT>(0.);
    for (size_t i = 0; i < num_ele; ++i) {
      result += data[i];
    }
    return result;
  }

  template <typename NumT>
  NumT inverse(const NumT data) {
    return static_cast<NumT>(1.) / data;
  }

  template <typename NumT>
  NumT convertUnits(const NumT data, const string &old_units,
                    const string &new_units) {
    if (old_units.compare(new_units) == 0)
      {
        return data;
      }
    else if(old_units.compare(SECOND) == 0)
      {
        NumT my_data = data / static_cast<NumT>(60.);
        if (new_units.compare(MINUTE) == 0)
          {
            return my_data;
          }
        return convertUnits(my_data, MINUTE, new_units);
      }
    else if(old_units.compare(MINUTE) == 0)
      {
        if (new_units.compare(SECOND) == 0)
          {
            return inverse(convertUnits(data, new_units, old_units));
          }
        NumT my_data = data / static_cast<NumT>(60.);
        if (new_units.compare(HOUR) == 0)
          {
            return my_data;
          }
        return convertUnits(my_data, HOUR, new_units);
      }
    else if(old_units.compare(HOUR) == 0)
      {
        if(new_units.compare(MINUTE) == 0)
          {
            return inverse(convertUnits(data, new_units, old_units));
          }
        NumT my_data = data / static_cast<NumT>(24.);
        if (new_units.compare(DAY) == 0)
          {
            return my_data;
          }
        return convertUnits(my_data, DAY, new_units);
      }
    else if(old_units.compare(DAY) == 0)
      {
        if (new_units.compare(HOUR) == 0)
          {
            return inverse(convertUnits(data, new_units, old_units));
          }
      }
    else if(old_units.compare(MICROAMPHOUR) == 0)
      {
        if(new_units.compare(PICOCOULOMB) == 0)
          {
            return data * static_cast<NumT>(36.E8);
          }
      }
    else if(old_units.compare(PICOCOULOMB) == 0)
      {
        if (new_units.compare(MICROAMPHOUR) == 0)
          {
            return inverse(convertUnits(data, new_units, old_units));
          }
      }
    
    // should throw an exception here
    ostringstream s;
    s << "Do not know how to convert \"" << old_units << "\" to \""
      << new_units << "\"";
    throw runtime_error(s.str());
  }

  string operateData(const string &operation, const void *data,
                     const int dims[], const int rank, const int type,
                     const string & units) {
    if (operation.size() <= 0)
      {
        return toString(data, dims, rank, type) + SPACE + units;
      }

    if (operation.compare(OP_SUM) == 0)
      {
        string result;
        if (type == NX_FLOAT32)
          {
            result = toString(sum((float *)data, dims, rank));
          }
        else if(type == NX_FLOAT64)
          {
            result = toString(sum((double *)data, dims, rank));
          }
        else if(type == NX_UINT32)
          {
            result = toString(sum((uint32_t *)data, dims, rank));
          }
        else
          {
            ostringstream s;
            s << "Do not know how to convert units with type="
              << nxtypeAsString(type);
            throw runtime_error(s.str());
          }
        return result + SPACE + units;
      }

    if (operation.compare(OP_DIMS) == 0)
      {
        return toString(dims, rank, NX_INT32);
      }

    if (operation.compare(OP_COUNT) == 0)
      {
        int num_ele = 1;
        for (size_t i = 0; i < rank; ++i) {
          num_ele *= dims[i];
        }
        return toString(num_ele);
      }

    if (operation.find(OP_UNITS)!=string::npos)
      {
        if ((rank != 1) || (dims[0] != 1))
          {
            ostringstream s;
            s << "Cannot convert units on arrays (rank = " << rank << ")";
            throw runtime_error(s.str());
          }

        string new_units = operation.substr(OP_UNITS.size());

        if (type == NX_FLOAT32)
          {
            float result = convertUnits(((float *)data)[0], units, new_units);
            return toString(result) + SPACE + new_units;
          }
        else if (type == NX_FLOAT64)
          {
            double result =convertUnits(((double *)data)[0], units, new_units);
            return toString(result) + SPACE + new_units;
          }

        ostringstream s;
        s << "Do not know how to convert units with type="
          << nxtypeAsString(type);
        throw runtime_error(s.str());
      }

    ostringstream s;
    s << "Could not perform operation \"" << operation << "\"";
    throw runtime_error(s.str());
  }

  string readAsString(NXhandle handle, const string &path,
                      const string &operation) {
    // return empty string if the path is empty
    if (path.empty())
      {
        return string("");
      }

    // convert the path to something c-friendly
    char c_path[GROUP_STRING_LEN];
    strcpy(c_path, path.c_str());

    // open the path
    if(NXopenpath(handle, c_path)!=NX_OK)
      {
        throw runtime_error("COULD NOT OPEN PATH");
        return "";
      }

    // determine rank and dimension
    int rank = 0;
    int type = 0;
    int dims[NX_MAXRANK];
    if (NXgetinfo(handle, &rank, dims, &type)!=NX_OK)
      {
        throw runtime_error("COULD NOT GET NODE INFORMATION");
      }

    // confirm dimension isn't too high
    if (rank > NX_MAXRANK)
      {
        throw runtime_error("DIMENSIONALITY IS TOO HIGH");
      }

    // get the units
    string units = readAttrAsString(handle, UNITS);

    // check if this doesn't need the data to get result
    if (infoOnly(operation))
      {
        return operateData(operation, NULL, dims, rank, type, units);
      }

    // allocate space for data
    void *data;
    if(NXmalloc(&data,rank,dims,type)!=NX_OK)
      {
        throw runtime_error("NXmalloc falied");
      }

    // retrieve data from the file
    if(NXgetdata(handle,data)!=NX_OK)
      {
        throw runtime_error("NXgetdata failed");
      }

    // convert result to string
    string result = operateData(operation, data, dims, rank, type, units);

    //free up the pointer
    if(NXfree(&data)!=NX_OK)
      {
        throw runtime_error("NXfree failed");
      }

    return result;
  }
}
