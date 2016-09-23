/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _proj_matrix_h_
#define _proj_matrix_h_

//#include "plmbase_config.h"

class  Proj_matrix {
public:
    Proj_matrix ();

public:
    double ic[2];	  /* Image Center:  ic[0] = x, ic[1] = y */
    double matrix[12];	  /* Projection matrix */
    double sad;		  /* Distance: Source To Axis */
    double sid;		  /* Distance: Source to Image */
    double cam[3];	  /* Location of camera */
    double nrm[3];	  /* Ray from image center to source */

    double extrinsic[16];
    double intrinsic[12];

public:
    void get_nrm (double nrm[3]);
    void get_pdn (double nrm[3]);
    void get_prt (double nrm[3]);
};

Proj_matrix* proj_matrix_clone (Proj_matrix* pmat_in);
void proj_matrix_set (
    Proj_matrix *pmat,
    const double* cam, 
    const double* tgt, 
    const double* vup, 
    double sid, 
    const double* ic, 
    const double* ps, 
    const int* ires
);
void proj_matrix_debug (Proj_matrix *pmat);
void proj_matrix_save (Proj_matrix *pmat, const char *fn);

#endif
