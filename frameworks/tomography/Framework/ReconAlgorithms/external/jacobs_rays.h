/* jacobs_ray3d
 Calculates intersection lengths of a line with a voxel grid
 Implementation of algorithm described by F. Jacobs et.al.
 
 F. Jacobs, E. Sundermann, B. De Sutter, M. Christiaens and I. Lemahieu, ["A Fast
 Algorithm to Calculate the Exact Radiological Path through a Pixel or Voxel
 Space"
 Journal of Computing and Information Technology - CIT 6 1998, 1, pp 89-94]
 
 Written by David Szotten 2005-2006
 D.Szotten@student.manchester.ac.uk
 
 Copyright (c) The University of Manchester 2006 */

#ifndef _JACOBSRAYS_HEADER_
#define _JACOBSRAYS_HEADER_


/* ************** OPTIONS ************** */


#define PRECISION 0.00000001 /* for calculating rays intersecting voxels*/

/* ************************************* */

#include <mex.h>
#include <math.h>
#include <matrix.h>
#include <stdlib.h>
#include <stdio.h>


#define jacobs_PI 3.14159265358979

struct jacobs_options {
    int im_size_default, b_default, d_default;
    int im_size_x, im_size_y, im_size_z;
    double b_x, b_y, b_z, d_x, d_y, d_z;
};

/*void backproject(int im_size, double *start, double *end, double *ray_data, double *vol_data, struct jacobs_options *options)

*/
#endif
