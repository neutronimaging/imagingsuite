#include "void_copy.h"

// NX_FLOAT32
extern bool void_copy::from_float(const float* source, void *&copy, const int num_elem, const Node::NXtype type){
  if(type==Node::FLOAT64){
    for(int i=0 ; i<num_elem ; i++ )
      ((double *)copy)[i]=static_cast<double>(source[i]);
    return true;
  }else if(type==Node::FLOAT32){
    for(int i=0 ; i<num_elem ; i++ )
      ((float *)copy)[i]=source[i];
    return true;
  }else if(type==Node::INT8){
    // should do something
  }else if(type==Node::INT16){
    // should do something
  }else if(type==Node::INT32){
    // should do something
  }else if(type==Node::UINT8){
    // should do something
  }else if(type==Node::UINT16){
    // should do something
  }else if(type==Node::UINT32){
    // should do something
  }

  return false;
}

// NX_FLOAT64
extern bool void_copy::from_double(const double* source, void *&copy, const int num_elem, const Node::NXtype type){
  if(type==Node::FLOAT64){
    for(int i=0 ; i<num_elem ; i++ )
      ((double *)copy)[i]=source[i];
    return true;
  }else if(type==Node::FLOAT32){
    for(int i=0 ; i<num_elem ; i++ )
      ((float *)copy)[i]=static_cast<float>(source[i]);
    return true;
  }else if(type==Node::INT8){
    // should do something
  }else if(type==Node::INT16){
    // should do something
  }else if(type==Node::INT32){
    // should do something
  }else if(type==Node::UINT8){
    // should do something
  }else if(type==Node::UINT16){
    // should do something
  }else if(type==Node::UINT32){
    // should do something
  }

  return false;
}

// NX_INT16
extern bool void_copy::from_short(const short int* source, void *&copy, const int num_elem, const Node::NXtype type){
  if(type==Node::FLOAT64){
    // should do something
  }else if(type==Node::FLOAT32){
    // should do something
  }else if(type==Node::INT8){
    // should do something
  }else if(type==Node::INT16){
    for(int i=0 ; i<num_elem ; i++ )
      ((short int *)copy)[i]=source[i];
    return true;
  }else if(type==Node::INT32){
    for(int i=0 ; i<num_elem ; i++ )
      ((int *)copy)[i]=static_cast<int>(source[i]);
    return true;
  }else if(type==Node::UINT8){
    // should do something
  }else if(type==Node::UINT16){
    // should do something
  }else if(type==Node::UINT32){
    // should do something
  }

  return false;
}

// NX_INT32
extern bool void_copy::from_int(const int* source, void *&copy, const int num_elem, const Node::NXtype type){
  if(type==Node::FLOAT64){
    // should do something
  }else if(type==Node::FLOAT32){
    // should do something
  }else if(type==Node::INT8){
    // should do something
  }else if(type==Node::INT16){
    for(int i=0 ; i<num_elem ; i++ )
      ((short int *)copy)[i]=static_cast<short int>(source[i]);
    return true;
  }else if(type==Node::INT32){
    for(int i=0 ; i<num_elem ; i++ )
      ((int *)copy)[i]=source[i];
    return true;
  }else if(type==Node::UINT8){
    // should do something
  }else if(type==Node::UINT16){
    // should do something
  }else if(type==Node::UINT32){
    // should do something
  }

  return false;
}

extern bool void_copy::from_long(const long int* source, void *&copy, const int num_elem, const Node::NXtype type){
  return false;
}

// NX_UINT16
extern bool void_copy::from_ushort(const unsigned short int* source, void *&copy, const int num_elem, const Node::NXtype type){
  if(type==Node::FLOAT64){
    // should do something
  }else if(type==Node::FLOAT32){
    // should do something
  }else if(type==Node::INT8){
    // should do something
  }else if(type==Node::INT16){
    // should do something
  }else if(type==Node::INT32){
    // should do something
  }else if(type==Node::UINT8){
    // should do something
  }else if(type==Node::UINT16){
    for(int i=0 ; i<num_elem ; i++ )
      ((unsigned short int *)copy)[i]=source[i];
    return true;
  }else if(type==Node::UINT32){
    for(int i=0 ; i<num_elem ; i++ )
      ((unsigned int *)copy)[i]=static_cast<unsigned int>(source[i]);
    return true;
  }
  return false;
}

// NX_UINT32
extern bool void_copy::from_uint(const unsigned int* source, void *&copy, const int num_elem, const Node::NXtype type){
  if(type==Node::FLOAT64){
    // should do something
  }else if(type==Node::FLOAT32){
    // should do something
  }else if(type==Node::INT8){
    // should do something
  }else if(type==Node::INT16){
    // should do something
  }else if(type==Node::INT32){
    // should do something
  }else if(type==Node::UINT8){
    // should do something
  }else if(type==Node::UINT16){
    for(int i=0 ; i<num_elem ; i++ )
      ((unsigned short int *)copy)[i]=static_cast<unsigned short int>(source[i]);
    return true;
  }else if(type==Node::UINT32){
    for(int i=0 ; i<num_elem ; i++ )
      ((unsigned int *)copy)[i]=source[i];
    return true;
  }

  return false;
}

extern bool void_copy::from_ulong(const unsigned long int* source, void *&copy, const int num_elem, const Node::NXtype type){
  return false;
}
