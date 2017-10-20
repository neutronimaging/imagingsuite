/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#include "plmbase_config.h"
#include <string>
#include <stdio.h>
#include "direction_cosines.h"
#include "itk_image.h"
#include "plm_math.h"
#include "print_and_exit.h"

#define DIRECTION_COSINES_IDENTITY_THRESH 1e-9

class Direction_cosines_private {
public:
    float direction_cosines[9];
    float inv_direction_cosines[9];
};

Direction_cosines::Direction_cosines ()
{
    this->d_ptr = new Direction_cosines_private;
    this->set_identity ();
}

Direction_cosines::Direction_cosines (const DirectionType& itk_dc)
{
    this->d_ptr = new Direction_cosines_private;
    this->set (itk_dc);
}

Direction_cosines::~Direction_cosines ()
{
    delete this->d_ptr;
}

Direction_cosines::operator const float* () const
{
    return d_ptr->direction_cosines;
}

Direction_cosines::operator float* ()
{
    return d_ptr->direction_cosines;
}

void Direction_cosines::set_identity () {
    d_ptr->direction_cosines[0] = 1.;
    d_ptr->direction_cosines[1] = 0.;
    d_ptr->direction_cosines[2] = 0.;
    d_ptr->direction_cosines[3] = 0.;
    d_ptr->direction_cosines[4] = 1.;
    d_ptr->direction_cosines[5] = 0.;
    d_ptr->direction_cosines[6] = 0.;
    d_ptr->direction_cosines[7] = 0.;
    d_ptr->direction_cosines[8] = 1.;
    solve_inverse ();
}
void Direction_cosines::set_rotated_1 () {
    d_ptr->direction_cosines[0] = 0.894427190999916;
    d_ptr->direction_cosines[1] = 0.447213595499958;
    d_ptr->direction_cosines[2] = 0.;
    d_ptr->direction_cosines[3] = -0.447213595499958;
    d_ptr->direction_cosines[4] = 0.894427190999916;
    d_ptr->direction_cosines[5] = 0.;
    d_ptr->direction_cosines[6] = 0.;
    d_ptr->direction_cosines[7] = 0.;
    d_ptr->direction_cosines[8] = 1.;
    solve_inverse ();
}
void Direction_cosines::set_rotated_2 () {
    d_ptr->direction_cosines[0] = M_SQRT1_2;
    d_ptr->direction_cosines[1] = -M_SQRT1_2;
    d_ptr->direction_cosines[2] = 0.;
    d_ptr->direction_cosines[3] = M_SQRT1_2;
    d_ptr->direction_cosines[4] = M_SQRT1_2;
    d_ptr->direction_cosines[5] = 0.;
    d_ptr->direction_cosines[6] = 0.;
    d_ptr->direction_cosines[7] = 0.;
    d_ptr->direction_cosines[8] = 1.;
    solve_inverse ();
}
void Direction_cosines::set_rotated_3 () {
    d_ptr->direction_cosines[0] = -0.855063803257865;
    d_ptr->direction_cosines[1] = 0.498361271551590;
    d_ptr->direction_cosines[2] = -0.143184969098287;
    d_ptr->direction_cosines[3] = -0.428158353951640;
    d_ptr->direction_cosines[4] = -0.834358655093045;
    d_ptr->direction_cosines[5] = -0.347168631377818;
    d_ptr->direction_cosines[6] = -0.292483018822660;
    d_ptr->direction_cosines[7] = -0.235545489638006;
    d_ptr->direction_cosines[8] = 0.926807426605751;
    solve_inverse ();
}
void Direction_cosines::set_skewed () {
    d_ptr->direction_cosines[0] = 1.;
    d_ptr->direction_cosines[1] = 0.;
    d_ptr->direction_cosines[2] = 0.;
    d_ptr->direction_cosines[3] = M_SQRT1_2;
    d_ptr->direction_cosines[4] = M_SQRT1_2;
    d_ptr->direction_cosines[5] = 0.;
    d_ptr->direction_cosines[6] = 0.;
    d_ptr->direction_cosines[7] = 0.;
    d_ptr->direction_cosines[8] = 1.;
    solve_inverse ();
}

static void 
volume_matrix3x3inverse (float *out, const float *m)
{
    float det;

    det =  
	m[0]*(m[4]*m[8]-m[5]*m[7])
	-m[1]*(m[3]*m[8]-m[5]*m[6])
	+m[2]*(m[3]*m[7]-m[4]*m[6]);

    if (fabs(det)<1e-8) {
	print_and_exit ("Error: singular matrix of direction cosines\n");
    }

    out[0]=  (m[4]*m[8]-m[5]*m[7]) / det;
    out[1]= -(m[1]*m[8]-m[2]*m[7]) / det;
    out[2]=  (m[1]*m[5]-m[2]*m[4]) / det;

    out[3]= -(m[3]*m[8]-m[5]*m[6]) / det;
    out[4]=  (m[0]*m[8]-m[2]*m[6]) / det;
    out[5]= -(m[0]*m[5]-m[2]*m[3]) / det;

    out[6]=  (m[3]*m[7]-m[4]*m[6]) / det;
    out[7]= -(m[0]*m[7]-m[1]*m[6]) / det;
    out[8]=  (m[0]*m[4]-m[1]*m[3]) / det;
}

void 
Direction_cosines::solve_inverse ()
{
    volume_matrix3x3inverse (d_ptr->inv_direction_cosines, 
	d_ptr->direction_cosines);
}

void 
Direction_cosines::set (const float dc[])
{
    for (int i = 0; i < 9; i++) {
        d_ptr->direction_cosines[i] = dc[i];
    }
    solve_inverse ();
}

void 
Direction_cosines::set (const DirectionType& itk_dc)
{
    for (unsigned int d1 = 0; d1 < 3; d1++) {
	for (unsigned int d2 = 0; d2 < 3; d2++) {
#if defined (PLM_CONFIG_ALT_DCOS)
	    d_ptr->direction_cosines[d1*3+d2] = itk_dc[d2][d1];
#else
	    d_ptr->direction_cosines[d1*3+d2] = itk_dc[d1][d2];
#endif
	}
    }
    solve_inverse ();
}

const float *
Direction_cosines::get () const
{
    return d_ptr->direction_cosines;
}

float *
Direction_cosines::get ()
{
    return d_ptr->direction_cosines;
}

const float *
Direction_cosines::get_inverse () const
{
    return d_ptr->inv_direction_cosines;
}

bool Direction_cosines::set_from_string (std::string& str) {
    float dc[9];
    int rc;

    /* First check presets */
    if (str == "identity") {
        this->set_identity ();
        return true;
    }
    else if (str == "rotated-1") {
        this->set_rotated_1 ();
        return true;
    }
    else if (str == "rotated-2") {
        this->set_rotated_2 ();
        return true;
    }
    else if (str == "rotated-3") {
        this->set_rotated_3 ();
        return true;
    }
    else if (str == "skewed") {
        this->set_skewed ();
        return true;
    }

    /* Not a preset, must be 9 digit string */
    rc = sscanf (str.c_str(), "%g %g %g %g %g %g %g %g %g", 
        &dc[0], &dc[1], &dc[2],
        &dc[3], &dc[4], &dc[5],
        &dc[6], &dc[7], &dc[8]);
    if (rc != 9) {
        return false;
    }
    this->set (dc);
    return true;
}

bool Direction_cosines::is_identity () {
    Direction_cosines id;
    float frob = 0.;
	
    for (int i = 0; i < 9; i++) {
        frob += fabs (
            d_ptr->direction_cosines[i] 
            - id.d_ptr->direction_cosines[i]);
    }
    return frob < DIRECTION_COSINES_IDENTITY_THRESH;
}
