/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#include "plmbase_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "direction_matrices.h"
#include "logfile.h"
#include "plm_int.h"
#include "plm_math.h"
#include "print_and_exit.h"
#include "volume_header.h"
#include "volume.h"

#define CONVERT_VOLUME(old_type,new_type,new_type_enum)			\
    {									\
	plm_long v;							\
	old_type* old_img;						\
	new_type* new_img;						\
	old_img = (old_type*) ref->img;					\
	new_img = (new_type*) malloc (sizeof(new_type) * ref->npix);	\
	if (!new_img) {							\
	    fprintf (stderr, "Memory allocation failed.\n");		\
	    exit(1);							\
	}								\
	for (v = 0; v < ref->npix; v++) {				\
	    new_img[v] = (new_type) old_img[v];				\
	}								\
	ref->pix_size = sizeof(new_type);				\
	ref->pix_type = new_type_enum;					\
	ref->img = (void*) new_img;					\
	free (old_img);							\
    }

Volume::Volume () {
    init ();
}

Volume::Volume (
    const plm_long dim[3], 
    const float offset[3], 
    const float spacing[3], 
    const float direction_cosines[9], 
    enum Volume_pixel_type vox_type, 
    int vox_planes
) {
    init ();
    create (dim, offset, spacing, direction_cosines, vox_type, 
        vox_planes);
}

Volume::Volume (
    const Volume_header& vh, 
    enum Volume_pixel_type vox_type, 
    int vox_planes
) {
    init ();
    create (vh, vox_type, vox_planes);
}

Volume::~Volume ()
{
    if (this->pix_type == PT_VF_FLOAT_PLANAR) {
	float** planes = (float**) this->img;
	free (planes[0]);
	free (planes[1]);
	free (planes[2]);
    }
    free (this->img);
}

void
Volume::init ()
{
    for (int d = 0; d < 3; d++) {
        dim[d] = 0;
        offset[d] = 0;
        spacing[d] = 0;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            proj[3*i+j] = 0;
            step[3*i+j] = 0;
        }
    }
    npix = 0;
    pix_type = PT_UNDEFINED;
    vox_planes = 0;
    pix_size = 0;
    img = 0;
}

void
Volume::allocate (void)
{
    if (this->pix_type == PT_VF_FLOAT_PLANAR) {
	float** der = (float**) malloc (3*sizeof(float*));
	if (!der) {
	    fprintf (stderr, "Memory allocation failed.\n");
	    exit(1);
	}
	int alloc_size = this->npix;
	for (int i=0; i < 3; i++) {
	    der[i] = (float*) malloc (alloc_size*sizeof(float));
	    if (!der[i]) {
		fprintf (stderr, "Memory allocation failed.\n");
		exit(1);
	    }
	    memset (der[i], 0, alloc_size*sizeof(float));
	}
	this->img = (void*) der;
    } else {
	this->img = (void*) malloc (this->pix_size * this->npix);
	if (!this->img) {
	    fprintf (stderr, "Memory allocation failed (alloc size = %u).\n",
		(int) (this->pix_size * this->npix));
	    exit(1);
	}
	memset (this->img, 0, this->pix_size * this->npix);
    }
}

void 
Volume::create (
    const plm_long dim[3], 
    const float offset[3], 
    const float spacing[3], 
    const float direction_cosines[9], 
    enum Volume_pixel_type vox_type, 
    int vox_planes
)
{
    for (int i = 0; i < 3; i++) {
	this->dim[i] = dim[i];
	this->offset[i] = offset[i];
	this->spacing[i] = spacing[i];
    }
    this->npix = this->dim[0] * this->dim[1] * this->dim[2];
    this->pix_type = vox_type;
    this->vox_planes = vox_planes;

    set_direction_cosines (direction_cosines);

    switch (vox_type) {
    case PT_UCHAR:
	this->pix_size = sizeof(unsigned char);
	break;
    case PT_SHORT:
	this->pix_size = sizeof(short);
	break;
    case PT_UINT16:
	this->pix_size = sizeof(uint16_t);
	break;
    case PT_UINT32:
	this->pix_size = sizeof(uint32_t);
	break;
    case PT_INT32:
	this->pix_size = sizeof(int32_t);
	break;
    case PT_FLOAT:
	this->pix_size = sizeof(float);
	break;
    case PT_VF_FLOAT_INTERLEAVED:
	this->pix_size = 3 * sizeof(float);
	break;
    case PT_VF_FLOAT_PLANAR:
	this->pix_size = sizeof(float);
	break;
    case PT_UCHAR_VEC_INTERLEAVED:
	this->pix_size = this->vox_planes * sizeof(unsigned char);
	break;
    default:
	fprintf (stderr, "Unhandled type in volume_create().\n");
	exit (-1);
    }

    this->allocate ();
}

void 
Volume::create (
    const Volume_header& vh, 
    enum Volume_pixel_type vox_type, 
    int vox_planes
)
{
    this->create (vh.get_dim(), vh.get_origin(), vh.get_spacing(), 
	vh.get_direction_cosines(), vox_type, vox_planes);
}

const float*
Volume::get_origin ()
{
    return this->offset;
}

void
Volume::set_origin (const float origin[3])
{
    for (int d = 0; d < 3; d++) {
        this->offset[d] = origin[d];
    }
}

float*
Volume::get_direction_cosines ()
{
    return this->direction_cosines;
}

void 
Volume::set_direction_cosines (
    const float direction_cosines[9]
)
{
    const float identity[9] = {1., 0., 0., 0., 1., 0., 0., 0., 1.};
    const float* dc;
    if (direction_cosines) {
	dc = direction_cosines;
    } else {
	dc = identity;
    }

    this->direction_cosines.set (dc);

    compute_direction_matrices (step, proj, 
        this->direction_cosines, this->spacing);

#if defined (commentout)
    const float* inv_dc = this->direction_cosines.get_inverse ();
    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 3; j++) {
	    this->step[3*i+j] = this->direction_cosines[3*i+j] 
		* this->spacing[i];
	    this->proj[3*i+j] = inv_dc[3*i+j] 
		/ this->spacing[j];
	}
    }
#endif
}

const float* 
Volume::get_step (void) const
{
    return this->step;
}

const float* 
Volume::get_proj (void) const
{
    return this->proj;
}

Volume*
volume_clone_empty (Volume* ref)
{
    Volume* vout;
    vout = new Volume (ref->dim, ref->offset, ref->spacing, 
	ref->direction_cosines, ref->pix_type, ref->vox_planes);
    return vout;
}

Volume*
volume_clone (const Volume* ref)
{
    Volume* vout;
    vout = new Volume (ref->dim, ref->offset, ref->spacing, 
	ref->direction_cosines, ref->pix_type, ref->vox_planes);
    switch (ref->pix_type) {
    case PT_UCHAR:
    case PT_SHORT:
    case PT_UINT16:
    case PT_UINT32:
    case PT_INT32:
    case PT_FLOAT:
    case PT_VF_FLOAT_INTERLEAVED:
    case PT_UCHAR_VEC_INTERLEAVED:
	memcpy (vout->img, ref->img, ref->npix * ref->pix_size);
	break;
    case PT_VF_FLOAT_PLANAR:
    default:
	fprintf (stderr, "Unsupported clone\n");
	exit (-1);
	break;
    }
    return vout;
}

Volume*
Volume::clone_raw () {
    return volume_clone (this);
}

Volume::Pointer
Volume::clone ()
{
    return Volume::New (this->clone_raw ());
}

void
volume_convert_to_float (Volume* ref)
{
    switch (ref->pix_type) {
    case PT_UCHAR:
	CONVERT_VOLUME (unsigned char, float, PT_FLOAT);
	break;
    case PT_SHORT:
	CONVERT_VOLUME (short, float, PT_FLOAT);
	break;
    case PT_UINT16:
	CONVERT_VOLUME (uint16_t, float, PT_FLOAT);
	break;
    case PT_UINT32:
	CONVERT_VOLUME (uint32_t, float, PT_FLOAT);
	break;
    case PT_INT32:
	CONVERT_VOLUME (int32_t, float, PT_FLOAT);
	break;
    case PT_FLOAT:
	/* Nothing to do */
	break;
    case PT_VF_FLOAT_INTERLEAVED:
    case PT_VF_FLOAT_PLANAR:
    case PT_UCHAR_VEC_INTERLEAVED:
    default:
	/* Can't convert this */
	fprintf (stderr, "Sorry, unsupported conversion to FLOAT\n");
	exit (-1);
	break;
    }
}

void
volume_convert_to_short (Volume* ref)
{
    switch (ref->pix_type) {
    case PT_UCHAR:
	fprintf (stderr, "Sorry, UCHAR to SHORT is not implemented\n");
	exit (-1);
	break;
    case PT_SHORT:
	/* Nothing to do */
	break;
    case PT_UINT16:
    case PT_UINT32:
    case PT_INT32:
	fprintf (stderr, "Sorry, UINT16/UINT32/INT32 to SHORT is not implemented\n");
	exit (-1);
	break;
    case PT_FLOAT:
	CONVERT_VOLUME (float, short, PT_SHORT);
	break;
    case PT_VF_FLOAT_INTERLEAVED:
    case PT_VF_FLOAT_PLANAR:
    case PT_UCHAR_VEC_INTERLEAVED:
    default:
	/* Can't convert this */
	fprintf (stderr, "Sorry, unsupported conversion to SHORT\n");
	exit (-1);
	break;
    }
}

void
volume_convert_to_uchar (Volume* ref)
{
    switch (ref->pix_type) {
    case PT_UCHAR:
	/* Nothing to do */
	break;
    case PT_SHORT:
	CONVERT_VOLUME (short, unsigned char, PT_UCHAR);
	break;
    case PT_UINT16:
	CONVERT_VOLUME (uint16_t, unsigned char, PT_UCHAR);
	break;
    case PT_UINT32:
	CONVERT_VOLUME (uint32_t, unsigned char, PT_UCHAR);
	break;
    case PT_INT32:
	CONVERT_VOLUME (int32_t, unsigned char, PT_UCHAR);
	break;
    case PT_FLOAT:
	CONVERT_VOLUME (float, unsigned char, PT_UCHAR);
	break;
    case PT_VF_FLOAT_INTERLEAVED:
    case PT_VF_FLOAT_PLANAR:
    case PT_UCHAR_VEC_INTERLEAVED:
    default:
	/* Can't convert this */
	fprintf (stderr, "Sorry, unsupported conversion to UCHAR\n");
	exit (-1);
	break;
    }
}

void
volume_convert_to_uint16 (Volume* ref)
{
    switch (ref->pix_type) {
    case PT_UCHAR:
    case PT_SHORT:
	fprintf (stderr, "Sorry, UCHAR/SHORT to UINT16 is not implemented\n");
	exit (-1);
	break;
    case PT_UINT16:
	/* Nothing to do */
	break;
    case PT_UINT32:
	fprintf (stderr, "Sorry, UINT32 to UINT16 is not implemented\n");
	break;
    case PT_INT32:
	fprintf (stderr, "Sorry, UINT32 to INT32 is not implemented\n");
	break;
    case PT_FLOAT:
	CONVERT_VOLUME (float, uint16_t, PT_UINT32);
	break;
    case PT_VF_FLOAT_INTERLEAVED:
    case PT_VF_FLOAT_PLANAR:
    case PT_UCHAR_VEC_INTERLEAVED:
    default:
	/* Can't convert this */
	fprintf (stderr, "Sorry, unsupported conversion to UINT32\n");
	exit (-1);
	break;
    }
}

void
volume_convert_to_uint32 (Volume* ref)
{
    switch (ref->pix_type) {
    case PT_UCHAR:
    case PT_SHORT:
	fprintf (stderr, "Sorry, UCHAR/SHORT to UINT32 is not implemented\n");
	exit (-1);
	break;
    case PT_UINT16:
	fprintf (stderr, "Sorry, UINT16 to UINT32 is not implemented\n");
	exit (-1);
	break;
    case PT_UINT32:
	/* Nothing to do */
	break;
    case PT_INT32:
	fprintf (stderr, "Sorry, INT32 to UINT32 is not implemented\n");
	exit (-1);
	break;
    case PT_FLOAT:
	CONVERT_VOLUME (float, uint32_t, PT_UINT32);
	break;
    case PT_VF_FLOAT_INTERLEAVED:
    case PT_VF_FLOAT_PLANAR:
    case PT_UCHAR_VEC_INTERLEAVED:
    default:
	/* Can't convert this */
	fprintf (stderr, "Sorry, unsupported conversion to UINT32\n");
	exit (-1);
	break;
    }
}

void
volume_convert_to_int32 (Volume* ref)
{
    switch (ref->pix_type) {
    case PT_UCHAR:
    case PT_SHORT:
	fprintf (stderr, "Sorry, UCHAR/SHORT to INT32 is not implemented\n");
	exit (-1);
	break;
    case PT_UINT16:
	fprintf (stderr, "Sorry, UINT16 to INT32 is not implemented\n");
	exit (-1);
	break;
    case PT_INT32:
	/* Nothing to do */
	break;
    case PT_UINT32:
	fprintf (stderr, "Sorry, UINT32 to INT32 is not implemented\n");
	exit (-1);
	break;
    case PT_FLOAT:
	CONVERT_VOLUME (float, int32_t, PT_INT32);
	break;
    case PT_VF_FLOAT_INTERLEAVED:
    case PT_VF_FLOAT_PLANAR:
    case PT_UCHAR_VEC_INTERLEAVED:
    default:
	/* Can't convert this */
	fprintf (stderr, "Sorry, unsupported conversion to INT32\n");
	exit (-1);
	break;
    }
}

void
vf_convert_to_interleaved (Volume* vf)
{
    switch (vf->pix_type) {
    case PT_VF_FLOAT_INTERLEAVED:
	/* Nothing to do */
	break;
    case PT_VF_FLOAT_PLANAR:
	{
	    plm_long v;
	    float** planar = (float**) vf->img;
	    float* inter = (float*) malloc (3*sizeof(float*)*vf->npix);
	    if (!inter) {
		fprintf (stderr, "Memory allocation failed.\n");
		exit(1);
	    }
	    for (v = 0; v < vf->npix; v++) {
		inter[3*v + 0] = planar[0][v];
		inter[3*v + 1] = planar[1][v];
		inter[3*v + 2] = planar[2][v];
	    }
	    free (planar[0]);
	    free (planar[1]);
	    free (planar[2]);
	    free (planar);
	    vf->img = (void*) inter;
	    vf->pix_type = PT_VF_FLOAT_INTERLEAVED;
	    vf->pix_size = 3*sizeof(float);
	}
	break;
    case PT_UCHAR:
    case PT_SHORT:
    case PT_UINT16:
    case PT_UINT32:
    case PT_INT32:
    case PT_FLOAT:
    case PT_UCHAR_VEC_INTERLEAVED:
    default:
	/* Can't convert this */
	fprintf (stderr, "Sorry, unsupported conversion to VF\n");
	exit (-1);
	break;
    }
}

void
vf_convert_to_planar (Volume* ref)
{
    switch (ref->pix_type) {
    case PT_VF_FLOAT_INTERLEAVED:
	{
	    float* img = (float*) ref->img;
	    float** der = (float**) malloc (3*sizeof(float*));
	    if (!der) {
		printf ("Memory allocation failed.\n");
		exit(1);
	    }
	    int alloc_size = ref->npix;
	    for (int i=0; i < 3; i++) {
		der[i] = (float*) malloc (alloc_size*sizeof(float));
		if (!der[i]) {
		    print_and_exit ("Memory allocation failed.\n");
		}
	    }
	    for (plm_long i = 0; i < ref->npix; i++) {
		der[0][i] = img[3*i + 0];
		der[1][i] = img[3*i + 1];
		der[2][i] = img[3*i + 2];
	    }
	    free (ref->img);
	    ref->img = (void*) der;
	    ref->pix_type = PT_VF_FLOAT_PLANAR;
	    ref->pix_size = sizeof(float);
	}
        break;
    case PT_VF_FLOAT_PLANAR:
	/* Nothing to do */
	break;
    case PT_UCHAR:
    case PT_SHORT:
    case PT_UINT32:
    case PT_INT32:
    case PT_FLOAT:
    case PT_UCHAR_VEC_INTERLEAVED:
    default:
	/* Can't convert this */
	fprintf (stderr, "Sorry, unsupportd conversion to VF\n");
	exit (-1);
	break;
    }
}

void
Volume::convert (Volume_pixel_type new_type)
{
    switch (new_type) {
    case PT_UCHAR:
        volume_convert_to_uchar (this);
        break;
    case PT_SHORT:
        volume_convert_to_short (this);
        break;
    case PT_UINT16:
        volume_convert_to_uint16 (this);
        break;
    case PT_UINT32:
        volume_convert_to_uint32 (this);
        break;
    case PT_INT32:
        volume_convert_to_int32 (this);
        break;
    case PT_FLOAT:
        volume_convert_to_float (this);
        break;
    case PT_VF_FLOAT_INTERLEAVED:
        vf_convert_to_interleaved (this);
        break;
    case PT_VF_FLOAT_PLANAR:
        vf_convert_to_planar (this);
        break;
    case PT_UCHAR_VEC_INTERLEAVED:
    default:
	/* Can't convert this */
	print_and_exit (
            "Sorry, unsupported conversion type to %d in Volume::convert()\n",
            new_type);
	break;
    }
}

void
volume_scale (Volume* vol, float scale)
{
    float *img;

    if (vol->pix_type != PT_FLOAT) {
	print_and_exit ("volume_scale required PT_FLOAT type.\n");
    }

    img = (float*) vol->img;
    for (plm_long i = 0; i < vol->npix; i++) {
	img[i] = img[i] * scale;
    }
}

void
Volume::debug ()
{
    lprintf ("dim:%d %d %d\n",
	(int) dim[0],
	(int) dim[1],
	(int) dim[2]
    );
    lprintf ("org:%f %f %f\n",
	offset[0],
	offset[1],
	offset[2]
    );
    lprintf ("spac:%f %f %f\n",
	spacing[0],
	spacing[1],
	spacing[2]
    );
    lprintf ("dc:%8f %8f %8f\n%8f %8f %8f\n%8f %8f %8f\n",
	direction_cosines[0],
	direction_cosines[1],
	direction_cosines[2],
	direction_cosines[3],
	direction_cosines[4],
	direction_cosines[5],
	direction_cosines[6],
	direction_cosines[7],
	direction_cosines[8]
    );
}

void
Volume::direction_cosines_debug ()
{
    lprintf ("org:%f %f %f\n",
	offset[0],
	offset[1],
	offset[2]
    );
    lprintf ("spac:%f %f %f\n",
	spacing[0],
	spacing[1],
	spacing[2]
    );
    lprintf ("dc:\n%8f %8f %8f\n%8f %8f %8f\n%8f %8f %8f\n",
	direction_cosines[0],
	direction_cosines[1],
	direction_cosines[2],
	direction_cosines[3],
	direction_cosines[4],
	direction_cosines[5],
	direction_cosines[6],
	direction_cosines[7],
	direction_cosines[8]
    );
    lprintf ("step:\n%8f %8f %8f\n%8f %8f %8f\n%8f %8f %8f\n",
	step[3*0+0],
	step[3*0+1],
	step[3*0+2],
	step[3*1+0],
	step[3*1+1],
	step[3*1+2],
	step[3*2+0],
	step[3*2+1],
	step[3*2+2]
    );
    lprintf ("proj:\n%8f %8f %8f\n%8f %8f %8f\n%8f %8f %8f\n",
	proj[3*0+0],
	proj[3*0+1],
	proj[3*0+2],
	proj[3*1+0],
	proj[3*1+1],
	proj[3*1+2],
	proj[3*2+0],
	proj[3*2+1],
	proj[3*2+2]
    );
}

/* In mm coordinates */
void
volume_calc_grad_dcos (Volume* vout, const Volume* vref)
{
    plm_long i, j, k;
    plm_long i_n, j_n, k_n;       /* n is next */
    int i_p, j_p, k_p;          /* p is prev */
    plm_long gi, gj, gk;
    plm_long idx_p, idx_n;
    float *out_img, *ref_img;

    out_img = (float*) vout->img;
    ref_img = (float*) vref->img;

    const float *inv_dc = vref->direction_cosines.get_inverse();

    plm_long v = 0;
    for (k = 0; k < vref->dim[2]; k++) {
	k_p = k - 1;
	k_n = k + 1;
	if (k == 0) k_p = 0;
	if (k == vref->dim[2]-1) k_n = vref->dim[2]-1;
	for (j = 0; j < vref->dim[1]; j++) {
	    j_p = j - 1;
	    j_n = j + 1;
	    if (j == 0) j_p = 0;
	    if (j == vref->dim[1]-1) j_n = vref->dim[1]-1;
	    for (i = 0; i < vref->dim[0]; i++, v++) {
		float diff;
		i_p = i - 1;
		i_n = i + 1;
		if (i == 0) i_p = 0;
		if (i == vref->dim[0]-1) i_n = vref->dim[0]-1;
		
		gi = 3 * v + 0;
		gj = 3 * v + 1;
		gk = 3 * v + 2;
		out_img[gi] = 0.f;
		out_img[gj] = 0.f;
		out_img[gk] = 0.f;

#if defined (commentout)		
		idx_p = volume_index (vref->dim, i_p, j, k);
		idx_n = volume_index (vref->dim, i_n, j, k);
		diff = (float) (ref_img[idx_n] - ref_img[idx_p]) 
		    / 2.0 / vref->spacing[0];
		out_img[gi] += diff * vref->direction_cosines[0*3+0];
		out_img[gj] += diff * vref->direction_cosines[0*3+1];
		out_img[gk] += diff * vref->direction_cosines[0*3+2];

		idx_p = volume_index (vref->dim, i, j_p, k);
		idx_n = volume_index (vref->dim, i, j_n, k);
		diff = (float) (ref_img[idx_n] - ref_img[idx_p]) 
		    / 2.0 / vref->spacing[1];
		out_img[gi] += diff * vref->direction_cosines[1*3+0];
		out_img[gj] += diff * vref->direction_cosines[1*3+1];
		out_img[gk] += diff * vref->direction_cosines[1*3+2];

		idx_p = volume_index (vref->dim, i, j, k_p);
		idx_n = volume_index (vref->dim, i, j, k_n);
		diff = (float) (ref_img[idx_n] - ref_img[idx_p]) 
		    / 2.0 / vref->spacing[2];
		out_img[gi] += diff * vref->direction_cosines[2*3+0];
		out_img[gj] += diff * vref->direction_cosines[2*3+1];
		out_img[gk] += diff * vref->direction_cosines[2*3+2];
#endif

		idx_p = volume_index (vref->dim, i_p, j, k);
		idx_n = volume_index (vref->dim, i_n, j, k);
		diff = (float) (ref_img[idx_n] - ref_img[idx_p]) 
		    / 2.0 / vref->spacing[0];
		out_img[gi] += diff * inv_dc[0*3+0];
		out_img[gj] += diff * inv_dc[1*3+0];
		out_img[gk] += diff * inv_dc[2*3+0];

		idx_p = volume_index (vref->dim, i, j_p, k);
		idx_n = volume_index (vref->dim, i, j_n, k);
		diff = (float) (ref_img[idx_n] - ref_img[idx_p]) 
		    / 2.0 / vref->spacing[1];
		out_img[gi] += diff * inv_dc[0*3+1];
		out_img[gj] += diff * inv_dc[1*3+1];
		out_img[gk] += diff * inv_dc[2*3+1];

		idx_p = volume_index (vref->dim, i, j, k_p);
		idx_n = volume_index (vref->dim, i, j, k_n);
		diff = (float) (ref_img[idx_n] - ref_img[idx_p]) 
		    / 2.0 / vref->spacing[2];
		out_img[gi] += diff * inv_dc[0*3+2];
		out_img[gj] += diff * inv_dc[1*3+2];
		out_img[gk] += diff * inv_dc[2*3+2];
	    }
	}
    }
    lprintf ("volume_calc_grad_dcos complete.\n");
}

void
volume_calc_grad (Volume* vout, const Volume* vref)
{
    volume_calc_grad_dcos (vout, vref);
}

Volume* 
volume_make_gradient (Volume* ref)
{
    Volume *grad;
    grad = new Volume (ref->dim, ref->offset, ref->spacing, 
	ref->direction_cosines, PT_VF_FLOAT_INTERLEAVED, 3);
    volume_calc_grad (grad, ref);

    return grad;
}

// Computes the intensity differences between two images
Volume*
volume_difference (Volume* vol, Volume* warped)
{
    plm_long i, j, k;
    int p = 0; // Voxel index
    short* temp2;
    short* temp1;
    short* temp3;
    Volume* temp;

    temp = (Volume*) malloc (sizeof(Volume));
    if (!temp) {
	fprintf (stderr, "Memory allocation failed.\n");
	exit(1);
    }

    for(i=0;i<3; i++){
	temp->dim[i] = vol->dim[i];
	temp->offset[i] = vol->offset[i];
	temp->spacing[i] = vol->spacing[i];
    }

    temp->npix = vol->npix;
    temp->pix_type = vol->pix_type;

    temp->img = (void*) malloc (sizeof(short)*temp->npix);
    if (!temp->img) {
	fprintf (stderr, "Memory allocation failed.\n");
	exit(1);
    }
    memset (temp->img, -1200, sizeof(short)*temp->npix);

    p = 0; // Voxel index
    temp2 = (short*)vol->img;
    temp1 = (short*)warped->img;
    temp3 = (short*)temp->img;

    for (i=0; i < vol->dim[2]; i++) {
	for (j=0; j < vol->dim[1]; j++) {
	    for (k=0; k < vol->dim[0]; k++) {
		temp3[p] = (temp2[p] - temp1[p]) - 1200;
		p++;
	    }
	}
    }
    return temp;
}
