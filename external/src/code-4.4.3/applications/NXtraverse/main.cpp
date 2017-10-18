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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "nxtraverse.h"


using namespace std;


/*-------------------------------------------------------------------------
 * Function: usage
 *
 * Purpose: print usage
 *
 * Return: void
 *
 *-------------------------------------------------------------------------
 */

void usage(void)
{
    cout << "Usage: nxtraverse -f <filename> [-h] [-r] [-a] [-n N] " << endl;
    cout << endl;
    cout <<  "[-f]    traverse file <filename>" << endl;
    cout <<  "[-h]    print this usage message and exit" << endl;
    cout <<  "[-r]    do not read data" << endl;
    cout <<  "[-a]    do not read attributes" << endl;
    cout <<  "[-n]    print only N number of elements; N defaults to 5, * for all elements" << endl;
  

}

/*-------------------------------------------------------------------------
 * Function: main
 *
 * Purpose: main function
 *
 * Return: Success: 0, Failure: 1
 *
 * Comments:
 *
 *-------------------------------------------------------------------------
 */
int main(int argc, const char **argv)
{
    CNXTraverse  nxtraverse;
    int        i;
    const char *fname = NULL;

  
    if ( argc < 2 )
    {
        usage();
        return 1;

    }

    for ( i = 1; i < argc; i++) 
    {

        //help

        if (strcmp(argv[i], "-h") == 0) 
        {
            usage();
            return 0;
        }

        //file name

        else if (strcmp(argv[i], "-f") == 0) 
        {

            if ( argv[++i] == NULL )
            {
                usage();
                return 1;
            }

            fname = argv[i]; 

        }

        //do not read data

        else if (strcmp(argv[i], "-r") == 0) 
        {

            nxtraverse.options.read_data = 0; 

        }

        //do not read attributes

        else if (strcmp(argv[i], "-a") == 0) 
        {

            nxtraverse.options.read_attr = 0; 

        }

        //number of elements to print

        else if (strcmp(argv[i], "-n") == 0) 
        {

            if ( argv[++i] == NULL )
            {
                usage();
                return 1;
            }
            if ( strcmp(argv[i], "*") == 0 )
            {

                nxtraverse.options.all = 1;

            }
            else
            {
                nxtraverse.options.nelmts = atol( argv[i] ); 
            }

        }

    }

    if ( fname == NULL )
    {
        usage();
        return 1;

    }

    if ( nxtraverse.show_objects ( fname ) == NX_ERROR  )
        return 1;

    return 0;
}



