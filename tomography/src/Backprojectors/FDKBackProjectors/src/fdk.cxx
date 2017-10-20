/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
// #include "plmreconstruct_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#if (OPENMP_FOUND)
#include <omp.h>
#endif

#include "bowtie_correction.h"
#include "delayload.h"
#include "fdk.h"
// #include "fdk_cuda.h" // add cuda and file util?
#include "fdk_opencl.h"
//#include "file_util.h"
#include "plm_math.h"
#include "plm_timer.h"
#include "proj_image.h"
#include "proj_image_dir.h"
#include "proj_image_filter.h"
#include "proj_matrix.h"
#include "volume.h"

#if CUDA_FOUND
void
CUDA_reconstruct_conebeam (
    Volume *vol, 
    Proj_image_dir *proj_dir,
    Fdk_parms *parms
)
{
    void *dev_state;
    Proj_matrix *pmat;
    Proj_image* cbi;
    int num_imgs;
    int i;
    float scale;
    Plm_timer* timer_total = new Plm_timer;
    double time_total = 0;

#if FDK_CUDA_TIME_KERNEL
    Plm_timer* timer = new Plm_timer;
    double backproject_time = 0.0;
    double filter_time = 0.0;
    double io_time = 0.0;
#endif

    LOAD_LIBRARY_SAFE (libplmreconstructcuda);
    LOAD_SYMBOL (fdk_cuda_state_create, libplmreconstructcuda);
    LOAD_SYMBOL (fdk_cuda_queue_image, libplmreconstructcuda);
    LOAD_SYMBOL (fdk_cuda_fetch_volume, libplmreconstructcuda);
    LOAD_SYMBOL (fdk_cuda_backproject, libplmreconstructcuda);
    LOAD_SYMBOL (fdk_cuda_state_destroy, libplmreconstructcuda);

    // Start timing total execution
    timer_total->start ();

    /* Attempt to set image scale */
    num_imgs = proj_dir->num_proj_images;
    scale = (float) (sqrt(3.0) / (double) num_imgs);
    scale = scale * parms->scale;

    // This is just to retrieve the 2D image dimensions
    cbi = proj_dir->load_image (0);

    dev_state = fdk_cuda_state_create (
                    vol, 
                    cbi->dim[0]*cbi->dim[1], 
                    scale, 
                    parms
                );

    // Free image (we will re-load it in the main loop)
    delete cbi;

    // Project each image into the volume one at a time
    for (i = 0; i < proj_dir->num_proj_images; i++)
    {
#if FDK_CUDA_TIME_KERNEL
        timer->start ();
#endif

        // load the next 2D projection
        cbi = proj_dir->load_image (i);
        pmat = cbi->pmat;

#if FDK_CUDA_TIME_KERNEL
        io_time += timer->report ();
#endif

        if (parms->filter == FDK_FILTER_TYPE_RAMP) {
#if FDK_CUDA_TIME_KERNEL
            timer->start ();
#endif
            proj_image_filter (cbi);
#if FDK_CUDA_TIME_KERNEL
            filter_time += timer->report ();
#endif
        }

	/* Send projection image to device */
	fdk_cuda_queue_image (
	    dev_state, 
	    cbi->dim, 
	    pmat->ic, 
	    pmat->nrm, 
	    pmat->sad, 
	    pmat->sid, 
	    pmat->matrix,
	    cbi->img
	);

        /* Free the current image */
        delete cbi;

#if defined (VERBOSE)
        printf ("Executing kernel... ");
#endif

#if FDK_CUDA_TIME_KERNEL
        timer->start ();
#endif

	/* Execute backprojection kernel */
	fdk_cuda_backproject (dev_state);

#if FDK_CUDA_TIME_KERNEL
        backproject_time += timer->report ();
#endif

    } // next projection

#if defined (VERBOSE)
    printf(" done.\n\n");
#endif

    /* Retrieve reconstructed volume from device */
    fdk_cuda_fetch_volume (dev_state, vol->img, vol->npix * vol->pix_size);


    /* Free memory on device */
    fdk_cuda_state_destroy (dev_state);
    
    UNLOAD_LIBRARY (libplmreconstructcuda);

    /* Report total time */
    time_total = timer_total->report ();
    printf ("========================================\n");
    printf ("[Total Execution Time: %.9fs ]\n", time_total);
#if FDK_CUDA_TIME_KERNEL
    printf ("I/O time = %g\n", io_time);
    printf ("Filter time = %g\n", filter_time);
    printf ("Backprojection time = %g\n", backproject_time);
#endif
    num_imgs = 1 + (parms->last_img - parms->first_img) 
	/ parms->skip_img;
    printf ("[Average Projection Time: %.9fs ]\n", time_total / num_imgs);
#if FDK_CUDA_TIME_KERNEL
    printf ("I/O time = %g\n", io_time / num_images);
    printf ("Filter time = %g\n", filter_time / num_images);
    printf ("Backprojection time = %g\n", backproject_time / num_images);
#endif
    printf ("========================================\n");

#if FDK_CUDA_TIME_KERNEL
    delete timer;
#endif
    delete timer_total;

}
#endif

/* get_pixel_value_c seems to be no faster than get_pixel_value_b, 
   despite having two fewer compares. */
inline float
get_pixel_value_c (Proj_image* cbi, double r, double c)
{
    int rr, cc;

#if defined (commentout)
    r += 0.5;
    if (r < 0) return 0.0;
    rr = (int) r;
    if (rr >= cbi->dim[1]) return 0.0;

    c += 0.5;
    if (c < 0) return 0.0;
    cc = (int) c;
    if (cc >= cbi->dim[0]) return 0.0;
#endif

    r += 0.5;
    if (r < 0) return 0.0;
    if (r >= (double) cbi->dim[1]) return 0.0;
    rr = (int) r;

    c += 0.5;
    if (c < 0) return 0.0;
    if (c >= (double) cbi->dim[0]) return 0.0;
    cc = (int) c;

    return cbi->img[rr*cbi->dim[0] + cc];
}

/* Nearest neighbor interpolation of intensity value on image */
inline float
get_pixel_value_b (Proj_image* cbi, double r, double c)
{
    int rr, cc;

    rr = ROUND_INT(r);
    if (rr < 0 || rr >= cbi->dim[1]) return 0.0;
    cc = ROUND_INT(c);
    if (cc < 0 || cc >= cbi->dim[0]) return 0.0;
    return cbi->img[rr*cbi->dim[0] + cc];
}


/* First try at OpenMP FDK... modeled after version c */
void
project_volume_onto_image_d (Volume* vol, Proj_image* cbi, float scale)
{
    long i, j, k, p;
    float* img = (float*) vol->img;
    double *xip, *yip, *zip;
    double acc2[3],acc3[3];
    double dw;
    double sad_sid_2;
    Proj_matrix *pmat = cbi->pmat;

    /* Rescale image (destructive rescaling) */
    sad_sid_2 = (pmat->sad * pmat->sad) / (pmat->sid * pmat->sid);
    for (i = 0; i < cbi->dim[0]*cbi->dim[1]; i++) {
	cbi->img[i] *= sad_sid_2;	// Speedup trick re: Kachelsreiss
	cbi->img[i] *= scale;		// User scaling
    }

    xip = (double*) malloc (3*vol->dim[0]*sizeof(double));
    yip = (double*) malloc (3*vol->dim[1]*sizeof(double));
    zip = (double*) malloc (3*vol->dim[2]*sizeof(double));

    /* Precompute partial projections here */
#pragma omp parallel for
    for (i = 0; i < vol->dim[0]; i++) {
	double x = (double) (vol->origin[0] + i * vol->spacing[0]);
	xip[i*3+0] = x * (pmat->matrix[0] + pmat->ic[0] * pmat->matrix[8]);
	xip[i*3+1] = x * (pmat->matrix[4] + pmat->ic[1] * pmat->matrix[8]);
	xip[i*3+2] = x * pmat->matrix[8];
    }
    
#pragma omp parallel for
    for (j = 0; j < vol->dim[1]; j++) {
	double y = (double) (vol->origin[1] + j * vol->spacing[1]);
	yip[j*3+0] = y * (pmat->matrix[1] + pmat->ic[0] * pmat->matrix[9]);
	yip[j*3+1] = y * (pmat->matrix[5] + pmat->ic[1] * pmat->matrix[9]);
	yip[j*3+2] = y * pmat->matrix[9];
    }

#pragma omp parallel for
    for (k = 0; k < vol->dim[2]; k++) {
	double z = (double) (vol->origin[2] + k * vol->spacing[2]);
	zip[k*3+0] = z * (pmat->matrix[2] + pmat->ic[0] * pmat->matrix[10]) 
		+ pmat->ic[0] * pmat->matrix[11] + pmat->matrix[3];
	zip[k*3+1] = z * (pmat->matrix[6] + pmat->ic[1] * pmat->matrix[10]) 
		+ pmat->ic[1] * pmat->matrix[11] + pmat->matrix[7];
	zip[k*3+2] = z * pmat->matrix[10] + pmat->matrix[11];
    }
    
    /* Main loop */
    
// OpenMP attempt #1 (slower than single core version c)
#pragma omp parallel for
    for (p = 0; p < (vol->dim[2] * vol->dim[1] * vol->dim[0]); p++)
    {
	i = p % vol->dim[0];
	j = ((p - i) / vol->dim[0]) % vol->dim[1];
	k = (((p - i) / vol->dim[0]) / vol->dim[1]) % vol->dim[2];

	vec3_add3 (acc2, &zip[3*k], &yip[3*j]);
	vec3_add3 (acc3, acc2, &xip[3*i]);
	dw = 1 / acc3[2];
	acc3[0] = acc3[0] * dw;
	acc3[1] = acc3[1] * dw;
	img[p] += dw * dw * get_pixel_value_c (cbi, acc3[0], acc3[1]);
    }

/*    
// OpenMP attempt #2 (still slower than single core version c)
#pragma omp parallel for private(i,j)
{
    for (k = 0; k < vol->dim[2]; k++) {
	for (j = 0; j < vol->dim[1]; j++) {
	    vec3_add3 (acc2, &zip[3*k], &yip[3*j]);
	    for (i = 0; i < vol->dim[0]; i++) {
		vec3_add3 (acc3, acc2, &xip[3*i]);
		dw = 1 / acc3[2];
		acc3[0] = acc3[0] * dw;
		acc3[1] = acc3[1] * dw;
		p = i + j*vol->dim[0] + k*vol->dim[0]*vol->dim[1];
		img[p] += dw * dw * get_pixel_value_c (cbi, acc3[0], acc3[1]);
	    }
	}
    }
}
*/


    free (xip);
    free (yip);
    free (zip);
}


/* This version folds ic & wip into zip, as well as using faster 
   nearest neighbor macro. */
void
project_volume_onto_image_c (Volume* vol, Proj_image* cbi, float scale)
{
    plm_long i, j, k;
    float* img = (float*) vol->img;
    double *xip, *yip, *zip;
    double sad_sid_2;
    Proj_matrix *pmat = cbi->pmat;

    /* Rescale image (destructive rescaling) */
    sad_sid_2 = (pmat->sad * pmat->sad) / (pmat->sid * pmat->sid);
    for (i = 0; i < cbi->dim[0]*cbi->dim[1]; i++) {
	cbi->img[i] *= sad_sid_2;	// Speedup trick re: Kachelsreiss
	cbi->img[i] *= scale;		// User scaling
    }

    xip = (double*) malloc (3*vol->dim[0]*sizeof(double));
    yip = (double*) malloc (3*vol->dim[1]*sizeof(double));
    zip = (double*) malloc (3*vol->dim[2]*sizeof(double));

    /* Precompute partial projections here */
    for (i = 0; i < vol->dim[0]; i++) {
	double x = (double) (vol->origin[0] + i * vol->spacing[0]);
	xip[i*3+0] = x * (pmat->matrix[0] + pmat->ic[0] * pmat->matrix[8]);
	xip[i*3+1] = x * (pmat->matrix[4] + pmat->ic[1] * pmat->matrix[8]);
	xip[i*3+2] = x * pmat->matrix[8];
    }
    for (j = 0; j < vol->dim[1]; j++) {
	double y = (double) (vol->origin[1] + j * vol->spacing[1]);
	yip[j*3+0] = y * (pmat->matrix[1] + pmat->ic[0] * pmat->matrix[9]);
	yip[j*3+1] = y * (pmat->matrix[5] + pmat->ic[1] * pmat->matrix[9]);
	yip[j*3+2] = y * pmat->matrix[9];
    }
    for (k = 0; k < vol->dim[2]; k++) {
	double z = (double) (vol->origin[2] + k * vol->spacing[2]);
	zip[k*3+0] = z * (pmat->matrix[2] + pmat->ic[0] * pmat->matrix[10]) 
		+ pmat->ic[0] * pmat->matrix[11] + pmat->matrix[3];
	zip[k*3+1] = z * (pmat->matrix[6] + pmat->ic[1] * pmat->matrix[10]) 
		+ pmat->ic[1] * pmat->matrix[11] + pmat->matrix[7];
	zip[k*3+2] = z * pmat->matrix[10] + pmat->matrix[11];
    }

    /* Main loop */
#pragma omp parallel for
    for (k = 0; k < vol->dim[2]; k++) {
	plm_long p = k * vol->dim[1] * vol->dim[0];
	plm_long j;
	for (j = 0; j < vol->dim[1]; j++) {
	    plm_long i;
	    double acc2[3];
	    vec3_add3 (acc2, &zip[3*k], &yip[3*j]);
	    for (i = 0; i < vol->dim[0]; i++) {
		double dw;
		double acc3[3];
		vec3_add3 (acc3, acc2, &xip[3*i]);
		dw = 1 / acc3[2];
		acc3[0] = acc3[0] * dw;
		acc3[1] = acc3[1] * dw;
		img[p++] += 
		    dw * dw * get_pixel_value_c (cbi, acc3[1], acc3[0]);
	    }
	}
    }
    free (xip);
    free (yip);
    free (zip);
}

void
project_volume_onto_image_b (Volume* vol, Proj_image* cbi, float scale)
{
    plm_long i, j, k, p;
    float* img = (float*) vol->img;
    double wip[3];
    double *xip, *yip, *zip;
    double acc1[3],acc2[3],acc3[3];
    double *x, *y, *z;
    double dw;
    double sad_sid_2;
    Proj_matrix *pmat = cbi->pmat;

    /* Rescale image (destructive rescaling) */
    sad_sid_2 = (pmat->sad * pmat->sad) / (pmat->sid * pmat->sid);
    for (i = 0; i < cbi->dim[0]*cbi->dim[1]; i++) {
	cbi->img[i] *= sad_sid_2;	// Speedup trick re: Kachelsreiss
	cbi->img[i] *= scale;		// User scaling
    }

    x = (double*) malloc (vol->dim[0]*sizeof(double));
    y = (double*) malloc (vol->dim[1]*sizeof(double));
    z = (double*) malloc (vol->dim[2]*sizeof(double));
    xip = (double*) malloc (3*vol->dim[0]*sizeof(double));
    yip = (double*) malloc (3*vol->dim[1]*sizeof(double));
    zip = (double*) malloc (3*vol->dim[2]*sizeof(double));

    /* Precompute partial projections here */
    for (i = 0; i < vol->dim[0]; i++) {
	x[i] = (double) (vol->origin[0] + i * vol->spacing[0]);
	xip[i*3+0] = x[i] * pmat->matrix[0];
	xip[i*3+1] = x[i] * pmat->matrix[4];
	xip[i*3+2] = x[i] * pmat->matrix[8];
	x[i] *= pmat->nrm[0];
    }
    for (j = 0; j < vol->dim[1]; j++) {
	y[j] = (double) (vol->origin[1] + j * vol->spacing[1]);
	yip[j*3+0] = y[j] * pmat->matrix[1];
	yip[j*3+1] = y[j] * pmat->matrix[5];
	yip[j*3+2] = y[j] * pmat->matrix[9];
	y[j] *= pmat->nrm[1];
    }
    for (k = 0; k < vol->dim[2]; k++) {
	z[k] = (double) (vol->origin[2] + k * vol->spacing[2]);
	zip[k*3+0] = z[k] * pmat->matrix[2];
	zip[k*3+1] = z[k] * pmat->matrix[6];
	zip[k*3+2] = z[k] * pmat->matrix[10];
	z[k] *= pmat->nrm[2];
	z[k] = pmat->sad - z[k];
    }
    wip[0] = pmat->matrix[3];
    wip[1] = pmat->matrix[7];
    wip[2] = pmat->matrix[11];
    
    /* Main loop */
    p = 0;
    for (k = 0; k < vol->dim[2]; k++) {
	vec3_add3 (acc1, wip, &zip[3*k]);
	for (j = 0; j < vol->dim[1]; j++) {
	    vec3_add3 (acc2, acc1, &yip[3*j]);
	    for (i = 0; i < vol->dim[0]; i++) {
		vec3_add3 (acc3, acc2, &xip[3*i]);
		dw = 1 / acc3[2];
		acc3[0] = pmat->ic[0] + acc3[0] * dw;
		acc3[1] = pmat->ic[1] + acc3[1] * dw;
		img[p++] += dw * dw * get_pixel_value_c (cbi, acc3[1], acc3[0]);
	    }
	}
    }
    free (x);
    free (y);
    free (z);
    free (xip);
    free (yip);
    free (zip);
}

void
project_volume_onto_image_a (Volume* vol, Proj_image* cbi, float scale)
{
    plm_long i, j, k, p;
    float* img = (float*) vol->img;
    double wip[3];
    double *xip, *yip, *zip;
    double acc1[3],acc2[3],acc3[3];
    double *x, *y, *z;
    double s1, s, sad2;
    Proj_matrix *pmat = cbi->pmat;

    /* Rescale image (destructive rescaling) */
    for (i = 0; i < cbi->dim[0]*cbi->dim[1]; i++) {
	cbi->img[i] *= scale;
    }

    x = (double*) malloc (vol->dim[0]*sizeof(double));
    y = (double*) malloc (vol->dim[1]*sizeof(double));
    z = (double*) malloc (vol->dim[2]*sizeof(double));
    xip = (double*) malloc (3*vol->dim[0]*sizeof(double));
    yip = (double*) malloc (3*vol->dim[1]*sizeof(double));
    zip = (double*) malloc (3*vol->dim[2]*sizeof(double));

    /* Precompute partial projections here */
    for (i = 0; i < vol->dim[0]; i++) {
	x[i] = (double) (vol->origin[0] + i * vol->spacing[0]);
	xip[i*3+0] = x[i] * pmat->matrix[0];
	xip[i*3+1] = x[i] * pmat->matrix[4];
	xip[i*3+2] = x[i] * pmat->matrix[8];
	x[i] *= pmat->nrm[0];
    }
    for (j = 0; j < vol->dim[1]; j++) {
	y[j] = (double) (vol->origin[1] + j * vol->spacing[1]);
	yip[j*3+0] = y[j] * pmat->matrix[1];
	yip[j*3+1] = y[j] * pmat->matrix[5];
	yip[j*3+2] = y[j] * pmat->matrix[9];
	y[j] *= pmat->nrm[1];
    }
    for (k = 0; k < vol->dim[2]; k++) {
	z[k] = (double) (vol->origin[2] + k * vol->spacing[2]);
	zip[k*3+0] = z[k] * pmat->matrix[2];
	zip[k*3+1] = z[k] * pmat->matrix[6];
	zip[k*3+2] = z[k] * pmat->matrix[10];
	z[k] *= pmat->nrm[2];
	z[k] = pmat->sad - z[k];
    }
    wip[0] = pmat->matrix[3];
    wip[1] = pmat->matrix[7];
    wip[2] = pmat->matrix[11];
    sad2 = pmat->sad * pmat->sad;
    
    /* Main loop */
    p = 0;
    for (k = 0; k < vol->dim[2]; k++) {
	vec3_add3 (acc1, wip, &zip[3*k]);
	s = z[k];
	for (j = 0; j < vol->dim[1]; j++) {
	    vec3_add3 (acc2, acc1, &yip[3*j]);
	    s1 = z[k] - y[j];
	    for (i = 0; i < vol->dim[0]; i++) {
		s = s1 - x[i];
		//printf ("%10.10g ", s);
		s = sad2 / (s * s);
		vec3_add3 (acc3, acc2, &xip[3*i]);
		//printf ("%10.10g\n", acc3[2]);
		acc3[0] = pmat->ic[0] + acc3[0] / acc3[2];
		acc3[1] = pmat->ic[1] + acc3[1] / acc3[2];
		img[p++] += s * get_pixel_value_b (cbi, acc3[1], acc3[0]);
	    }
	}
    }
    free (x);
    free (y);
    free (z);
    free (xip);
    free (yip);
    free (zip);
}

/* Reference implementation is the most straightforward implementation, 
    also it is the slowest */
void
project_volume_onto_image_reference (
    Volume* vol, 
    Proj_image* cbi, 
    float scale
)
{
    plm_long i, j, k, p;
    double vp[4];   /* vp = voxel position */
    float* img = (float*) vol->img;
    Proj_matrix *pmat = cbi->pmat; /* projection matrix 3D -> 2D 
				      in homogenous coordinates */
    p = 0;
    vp[3] = 1.0;
    /* Loop k (slices), j (rows), i (cols) */
    for (k = 0; k < vol->dim[2]; k++) {
	vp[2] = (double) (vol->origin[2] + k * vol->spacing[2]);
	for (j = 0; j < vol->dim[1]; j++) {
	    vp[1] = (double) (vol->origin[1] + j * vol->spacing[1]);
	    for (i = 0; i < vol->dim[0]; i++) {
		double ip[3];        /* ip = image position */
		double s;            /* s = projection of vp onto s axis */
		vp[0] = (double) (vol->origin[0] + i * vol->spacing[0]);
		mat43_mult_vec3 (ip, pmat->matrix, vp);  /* ip = matrix * vp */
		ip[0] = pmat->ic[0] + ip[0] / ip[2];
		ip[1] = pmat->ic[1] + ip[1] / ip[2];
		/* Distance on cenral axis from voxel center to source */
		/* pmat->nrm is normal of panel */
		s = vec3_dot (pmat->nrm, vp);
		/* Conebeam weighting factor */
		s = pmat->sad - s;
		s = pmat->sad * pmat->sad / (s * s);
		img[p++] += scale * s * get_pixel_value_b (cbi, ip[1], ip[0]);
	    }
	}
    }
}

/* Main reconstruction loop - Loop over images, and backproject each 
    image into the volume. */
void
reconstruct_conebeam (
    Volume* vol, 
    Proj_image_dir *proj_dir, 
    Fdk_parms* parms
)
{
    int i;
    int num_imgs = proj_dir->num_proj_images;
    float scale;
    double filter_time = 0.0;
    double backproject_time = 0.0;
    double io_time = 0.0;
    Proj_image* cbi;    /* cbi == cone beam image */
    Plm_timer* timer = new Plm_timer;

    /* Arbitrary scale applied to each image */
    scale = (float) (sqrt(3.f) / (double) num_imgs);
    scale = scale * parms->scale;

    for (i = 0; i < num_imgs; i++) {
        printf ("Processing image %d\n", i);

        timer->start ();
        cbi = proj_dir->load_image (i);
        io_time += timer->report ();

        /* Apply ramp filter */
        if (parms->filter == FDK_FILTER_TYPE_RAMP) {
            timer->start ();
            proj_image_filter (cbi);
            filter_time += timer->report ();
        }
    
        // printf ("Projecting Image %d\n", i);
        timer->start ();

	switch (parms->flavor) {
	case '0':
	    project_volume_onto_image_reference (vol, cbi, scale);
	    break;
	case 'a':
	    project_volume_onto_image_a (vol, cbi, scale);
	    break;
	case 'b':
	    project_volume_onto_image_b (vol, cbi, scale);
	    break;
	default:
	case 'c':
	    project_volume_onto_image_c (vol, cbi, scale);
	    break;
	case 'd':
	    project_volume_onto_image_d (vol, cbi, scale);
	    break;
	}

        backproject_time += timer->report ();

        delete cbi;
    }

    printf ("I/O time (total) = %g\n", io_time);
    printf ("I/O time (per image) = %g\n", io_time / num_imgs);
    printf ("Filter time = %g\n", filter_time);
    printf ("Filter time (per image) = %g\n", filter_time / num_imgs);
    printf ("Backprojection time = %g\n", backproject_time);
    printf ("Backprojection time (per image) = %g\n", 
	backproject_time / num_imgs);

    delete timer;
}

void
fdk_do_bowtie (Volume* vol, Fdk_parms* parms)
{
    int norm_exists;
    if (parms->full_fan)
	norm_exists = file_exists (parms->Full_normCBCT_name);
    else
	norm_exists = file_exists (parms->Half_normCBCT_name);

    if (norm_exists) {
	bowtie_correction (vol, parms);
    } else {
	printf("%s\n%s\n", 
	    parms->Full_normCBCT_name,
	    parms->Half_normCBCT_name);
	printf("Skip bowtie correction because norm files do not exits\n");
    }
}
