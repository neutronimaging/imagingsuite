/*
 * Copyright (c) 2010, P. Vicente <pedro.vicente@space-research.org>
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


#ifndef NX_TRAVERSE_H
#define NX_TRAVERSE_H

#include "napi.h"
#include "napiconfig.h"


//command line options
typedef struct 
{  
    int      read_data;    //read data            
    int      read_attr;    //read attributes
    uint64_t nelmts;       //print number of elements
    int      all;          //print all elements

} nxtraverse_opt_t;





/////////////////////////////////////////////////////////////////////////////////////////////////////
//CNXTraverse
/////////////////////////////////////////////////////////////////////////////////////////////////////
class CNXTraverse
{
private:

    //traverse file
    int traverse( NXhandle handle, const char *grp_name  );

    //print dataset
    int show_dataset( NXhandle handle, const char* dset_name, const char* path);

    //read dataset
    int read_dataset( NXhandle handle );

    //read attribute
    int read_attr( NXhandle handle, const char* dset_name, const char* path );

    //print data
    int print_data( int nxdataype, uint64_t nelmts, void* buf );
    
public:
    
    CNXTraverse();
    ~CNXTraverse();

    //print objects
    int show_objects(const char* file_name);

    //command line options
    nxtraverse_opt_t options; 
    
    

};




#endif //NX_TRAVERSE_H
