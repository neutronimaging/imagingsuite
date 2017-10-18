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

#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nxtraverse.h"



using namespace std;




/////////////////////////////////////////////////////////////////////////////////////////////////////
//CNXTraverse class
//constructor
/////////////////////////////////////////////////////////////////////////////////////////////////////
CNXTraverse::CNXTraverse ()
{

    options.read_data = 1; //read data by default
    options.read_attr = 1; //read attributes by default
    options.nelmts    = 5; //default printed number of elements
    options.all       = 0; //default: do not print all elements


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//CNXTraverse
//destructor
/////////////////////////////////////////////////////////////////////////////////////////////////////
CNXTraverse::~CNXTraverse ()
{

}



/*-------------------------------------------------------------------------
* Function: show_objects
*
* Purpose: traverse a NeXus file and print all objects
*
*-------------------------------------------------------------------------
*/

int CNXTraverse::show_objects(const char* file_name)
{
    NXhandle handle;
  
   // open file to traverse
   if  (NXopen ( file_name, NXACC_READ, &handle) != NX_OK) 
   {
      printf ("NX_ERROR: Can't open %s\n", file_name);
      return NX_ERROR;
   }


    // traverse
    if ( traverse( handle,  "/"  ) == NX_ERROR )
        goto out;

    //close file
    if ( NXclose (&handle) != NX_OK) 
        return NX_ERROR;

    return NX_OK;

    //out

out:
   
    NXclose (&handle);
 
    return NX_ERROR;;
}



/*-------------------------------------------------------------------------
* Function: traverse
*
* Purpose: recursive function that traverses a NeXus file and prints all objects
*
*
*-------------------------------------------------------------------------
*/

int CNXTraverse::traverse( NXhandle handle, const char *grp_name  )
{

    NXname name, nxclass;
    int nxdataype;
    int num_groups;
    std::string path;

    if ( NXinitgroupdir (handle) != NX_OK) 
        return NX_ERROR;

    if ( NXgetgroupinfo( handle, &num_groups, name, nxclass) != NX_OK )
        return NX_ERROR;

    for ( int i = 0; i < num_groups; i++ )
    {

        if ( NXgetnextentry ( handle, name, nxclass, &nxdataype) == NX_ERROR ) 
            return NX_ERROR;

        if (strcmp(nxclass,"SDS") == 0)
        {

            
            path = grp_name;
            path += "/";
            path += name;

            if ( options.read_data )
            {
                //do not print end line, data will be appended to this line
                cout << path << "=";

                if ( show_dataset( handle, name, path.c_str() ) == NX_ERROR ) 
                    return NX_ERROR;
            }
            else
            {

                //print name and end line
                cout << path << endl;

            }
           

        }
        else
        {


            path = grp_name;
            if ( path !=  "/" )
            path += "/";
            path += name;


            cout << path << endl;

            if ( NXopengroup( handle, name, nxclass ) != NX_OK )
                return NX_ERROR;

            if ( traverse( handle, path.c_str() ) != NX_OK )
                return NX_ERROR;

            if ( NXclosegroup( handle ) != NX_OK )
                return NX_ERROR;

      
        }



    }
    

    return NX_OK;
}





/*-------------------------------------------------------------------------
* Function: show_dataset
*
* Purpose: read, display NeXus dataset
*
*
*-------------------------------------------------------------------------
*/

int CNXTraverse::show_dataset( NXhandle handle, const char* dset_name, const char* path)
{
    int      rank;
    int      dims[NX_MAXRANK];
    int      nxdataype;
    void     *buf=NULL;
    uint64_t nelmts;

    if ( NXopendata ( handle, dset_name) != NX_OK) 
        goto out;

    if ( NXgetinfo ( handle, &rank, dims, &nxdataype) != NX_OK) 
        goto out;

    if ( NXmalloc( &buf, rank, dims, nxdataype) != NX_OK)
        goto out;

    if ( NXgetdata( handle, buf) != NX_OK)
        goto out;

    //get number of elements
    nelmts = 1;
    for ( int i = 0; i < rank; i++)
    {
        nelmts *= dims[i];
    }

    //print
    if ( print_data( nxdataype, nelmts, buf ) != NX_OK )
        goto out;

    if ( options.read_attr )
    {

        if ( read_attr( handle, dset_name, path ) == NX_ERROR ) 
            return NX_ERROR;

    }


    if ( NXclosedata( handle) != NX_OK) 
        goto out;

    if ( NXfree (&buf) != NX_OK) 
        goto out;




    return NX_OK;

    //out

out:

    if ( buf != NULL )
    {
        NXfree (&buf);
    }

    cout << "cannot open/read dataset" << dset_name << endl;
    return NX_ERROR;
}






/*-------------------------------------------------------------------------
 * Function: read_attr
 *
 * Purpose: read, print, a NeXus attribute 
 *
 *-------------------------------------------------------------------------
 */

int CNXTraverse::read_attr( NXhandle handle, const char* dset_name, const char* path )
{
    char attr_name[128];
    int  nxdataype;
    int  num_attr;
    int  length;
    void *buf=NULL;

    if ( NXinitattrdir( handle ) != NX_OK)
        goto out;

    // find the number of attributes

    if ( NXgetattrinfo( handle, &num_attr )!=NX_OK)
        goto out;

    for ( int i = 0 ; i < num_attr ; i++ )
    {
        if ( NXgetnextattr( handle, attr_name, &length, &nxdataype ) != NX_OK)
            goto out;

        //print the path in front of attribute name

        cout << path << "#" << attr_name << "=";

        //read attribute

        if ( NXmalloc( &buf, 1, &length, nxdataype) != NX_OK)
            goto out;

        if ( NXgetattr( handle, attr_name, buf, &length, &nxdataype) != NX_OK)
            goto out;

        //print

        if ( print_data( nxdataype, length, buf ) != NX_OK )
            goto out;

        if ( NXfree (&buf) != NX_OK) 
            goto out;

    }



    return NX_OK;

    //out

out:

    if ( buf != NULL )
    {
        NXfree (&buf);
    }

    cout << "cannot open/read attribute" << dset_name << endl;
    return NX_ERROR;
}




/*-------------------------------------------------------------------------
 * Function: print_data
 *
 * Purpose: print void* BUF into stdout
 *
 *-------------------------------------------------------------------------
 */

int CNXTraverse::print_data( int nxdataype, uint64_t nelmts, void* buf )
{
    unsigned char      *ucbuf = (unsigned char *)buf;
    char               *cbuf = (char *)buf;
    char               *s;
    int                more;
    uint64_t           n;
    uint64_t           i; 
    float              tempfloat;
    double             tempdouble;
    unsigned char      tempuchar;
    signed char        tempschar;
    int                tempint;
    unsigned int       tempuint;
    short              tempshort;
    unsigned short     tempushort;
    long long          templlong;
    unsigned long long tempullong;
  

     //how many elements to print
    if ( options.all )
    {
        n = nelmts;
        more = 0;
    }
    else
    {
        n = ( nelmts < options.nelmts) ? nelmts : options.nelmts;
        more = ( nelmts < options.nelmts) ? 0 : 1;
    }



    switch ( nxdataype )
    {
    default:
        assert(0);
        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_CHAR
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_CHAR:

        s = cbuf;
        if (s == NULL) 
        {


        }
        else 
        {
            for ( unsigned int j = 0; j < nelmts; j++) 
            {

                printf( "%c", s[j] ); 

            }

        }

        break;


        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_INT8
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_INT8:

        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(signed char);
            memcpy(&tempschar, mem, sizeof(signed char));
            printf( "%d ", tempschar ); 
         

        }
        if ( more )
            printf( "..."); 


        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_UINT8
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_UINT8:

        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(unsigned char);
            memcpy(&tempuchar, mem, sizeof(unsigned char));
            printf( "%u ", tempuchar ); 
           

        }
        if ( more )
            printf( "..."); 

        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_INT16
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_INT16:

        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(short);
            memcpy(&tempshort, mem, sizeof(short));
            printf( "%d ", tempshort ); 
            

        }
        if ( more )
            printf( "..."); 

        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_UINT16
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_UINT16:

        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(unsigned short);
            memcpy(&tempushort, mem, sizeof(unsigned short));
            printf( "%u ", tempushort ); 
           

        }
        if ( more )
            printf( "..."); 


        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_INT32
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_INT32:


        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(int);
            memcpy(&tempint, mem, sizeof(int));
            printf( "%d ", tempint ); 
           
        }
        if ( more )
            printf( "..."); 

        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_UINT32
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_UINT32:

        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(unsigned int);
            memcpy(&tempuint, mem, sizeof(unsigned int));
            printf( "%u ", tempuint ); 
           

        }
        if ( more )
            printf( "..."); 

        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_INT64
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_INT64:

        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(long long) ;
            memcpy(&templlong, mem, sizeof(long long));
            printf( "%lld ", templlong ); 
            

        }
        if ( more )
            printf( "..."); 

        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_UINT64
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_UINT64:


        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(unsigned long long);
            memcpy(&tempullong, mem, sizeof(unsigned long long));
            printf( "%llu ", tempullong ); 
          

        }
        if ( more )
            printf( "..."); 

        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_FLOAT32
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_FLOAT32:

        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(float);
            memcpy(&tempfloat, mem, sizeof(float));
            printf( "%g ", tempfloat ); 
         

        }
        if ( more )
            printf( "..."); 


        break;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //NX_FLOAT64
        /////////////////////////////////////////////////////////////////////////////////////////////////////


    case NX_FLOAT64:

        for (i = 0; i < n; i++) 
        {
            void* mem = ucbuf + i * sizeof(double);
            memcpy(&tempdouble, mem, sizeof(double));
            printf( "%g ", tempdouble ); 

        }
        if ( more )
            printf( "..."); 



        break;

    }



     //print end line for data
    printf( "\n"); 

    return NX_OK;


}


