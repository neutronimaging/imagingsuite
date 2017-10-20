/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _plm_math_h_
#define _plm_math_h_

//#include "plmsys_config.h"
#include <float.h>
#include <math.h>
#include <string.h>
//#include "compiler_warnings.h"

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif
#ifndef M_SQRT2
#define M_SQRT2         1.41421356237309504880
#endif
#ifndef M_SQRTPI
#define M_SQRTPI        1.77245385090551602792981
#endif
#ifndef M_TWOPI
#define M_TWOPI         (M_PI * 2.0)
#endif
#ifndef DBL_MAX
#define DBL_MAX         (1E+37)
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2       0.70710678118654752440  /* 1/sqrt(2) */
#endif
#ifndef M_SQRT2PI
#define M_SQRT2PI       2.50662827463100        /* sqrt(2*pi) - nonstandard */
#endif

/* Returns integer data type */
#define ROUND_INT(x) (((x) >= 0) ? ((long)((x)+0.5)) : (long)(-(-(x)+0.5)))

/* Returns plm_long data type */
#define FLOOR_PLM_LONG(x) ((plm_long) floor (x))
#define ROUND_PLM_LONG(x) \
    (((x) >= 0) ? ((plm_long)((x)+0.5)) : (plm_long)(-(-(x)+0.5)))

/* Returns unsigned integer data type */
#define FLOOR_SIZE_T(x) (((x) >= 0) ? ((size_t)(x)) : 0)
#define ROUND_SIZE_T(x) (((x) >= 0) ? ((size_t)((x)+0.5)) : 0)

/* Returns double data type -- note MSVC does not have C99 round(). */
#define ROUND(x) ((double) (ROUND_INT(x)))

/* Returns +1 or -1, depeding on sign.  Zero yeilds +1. */
#define SIGN(x) (((x) >= 0) ? (+1) : (-1))

// Fix for logf() under MSVC 2005 32-bit (math.h has an erronous semicolon)
// http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=98751
#if _MSC_VER == 1400
#if !defined (_M_IA64) && !defined (_M_AMD64) && defined (_WIN32)
#ifdef logf
#undef logf
#define logf(x)     ((float)log((double)(x)))
#endif
#endif
#endif

/* exp10() is not in C/C++ standard */
static inline double exp10_ (double m) {
    return exp (2.3025850929940456840179914546844 * m);
}

/* Primatives */
static inline void vec2_add2 (double* v1, const double* v2) {
    v1[0] += v2[0]; v1[1] += v2[1];
}

static inline void vec3_add2 (double* v1, const double* v2) {
    v1[0] += v2[0]; v1[1] += v2[1]; v1[2] += v2[2];
}

static inline void vec3_add3 (double* v1, const double* v2, const double* v3) {
    v1[0] = v2[0] + v3[0]; v1[1] = v2[1] + v3[1]; v1[2] = v2[2] + v3[2];
}

static inline void vec3_copy (double* v1, const double* v2) {
    v1[0] = v2[0]; v1[1] = v2[1]; v1[2] = v2[2];
}

static inline void vec4_copy (double* v1, const double* v2) {
    v1[0] = v2[0]; v1[1] = v2[1]; v1[2] = v2[2]; v1[3] = v2[3];
}

static inline double vec3_dot (const double* v1, const double* v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

static inline double vec4_dot (const double* v1, const double* v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
}

static inline void vec3_scale2 (double* v1, double a) {
    v1[0] *= a; v1[1] *= a; v1[2] *= a;
}

static inline void vec3_scale3 (double* v1, const double* v2, double a) {
    v1[0] = a * v2[0]; v1[1] = a * v2[1]; v1[2] = a * v2[2];
}

static inline void vec3_sub2 (double* v1, const double* v2) {
    v1[0] -= v2[0]; v1[1] -= v2[1]; v1[2] -= v2[2];
}

static inline void vec3_sub3 (double* v1, const double* v2, const double* v3) {
    v1[0] = v2[0] - v3[0]; v1[1] = v2[1] - v3[1]; v1[2] = v2[2] - v3[2];
}

static inline void vec3_invert (double* v1) {
    vec3_scale2 (v1, -1.0);
}

static inline void vec_zero (double* v1, int n) {
    memset (v1, 0, n*sizeof(double));
}

/* Length & distance */
static inline double vec3_len (const double* v1) {
    return sqrt(vec3_dot(v1,v1));
}

static inline void vec3_normalize1 (double* v1) {
    vec3_scale2 (v1, 1 / vec3_len(v1));
}

static inline double vec3_dist (const double* v1, const double* v2) {
    double tmp[3];
    vec3_sub3 (tmp, v1, v2);
    return vec3_len(tmp);
}

/* Cross product */
static inline void vec3_cross (double* v1, const double* v2, const double* v3)
{
    v1[0] = v2[1] * v3[2] - v2[2] * v3[1];
    v1[1] = v2[2] * v3[0] - v2[0] * v3[2];
    v1[2] = v2[0] * v3[1] - v2[1] * v3[0];
}

/* Outer product */
static inline void vec_outer (double* v1, const double* v2, const double* v3, const int n)
{
    int i,j;
    for (j=0; j<n; j++) {
        for (i=0; i<n; i++) {
            v1[n*j + i] = v2[j] * v3[i];
        }
    }
}

/* Matrix ops */

/* Matrix element m[i,j] for matrix with c columns */
#define m_idx(m1,c,i,j) m1[i*c+j]

/* v1 = m2 * v3 */
static inline void mat43_mult_vec3 (double* v1, const double* m2, const double* v3) {
    v1[0] = vec4_dot(&m2[0], v3);
    v1[1] = vec4_dot(&m2[4], v3);
    v1[2] = vec4_dot(&m2[8], v3);
}

/* m1 = m2 * m3 */
static inline void mat_mult_mat (
    double* m1, 
    const double* m2, int m2_rows, int m2_cols, 
    const double* m3, int m3_rows, int m3_cols)
{
    UNUSED_VARIABLE (m3_rows);
    int i,j,k;
    for (i = 0; i < m2_rows; i++) {
	for (j = 0; j < m3_cols; j++) {
	    double acc = 0.0;
	    for (k = 0; k < m2_cols; k++) {
		acc += m_idx(m2,m2_cols,i,k) * m_idx(m3,m3_cols,k,j);
	    }
	    m_idx(m1,m3_cols,i,j) = acc;
	}
    }
}

/* 0 when value is NaN or infinity */
static inline int is_number (const double x)
{
    if (!(x == x)) return 0;

    if (x > DBL_MAX || x < -DBL_MAX) return 0;

    return 1;
}


#endif
