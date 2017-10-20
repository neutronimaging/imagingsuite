/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#include "plmreconstruct_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "autotune_opencl.h"
#include "fdk.h"
#include "fdk_opencl_p.h"
#include "opencl_util.h"
#include "plm_math.h"
#include "proj_image.h"
#include "proj_image_dir.h"
#include "proj_image_filter.h"
#include "proj_matrix.h"
#include "volume.h"

void 
opencl_reconstruct_conebeam (
    Volume *vol, 
    Proj_image_dir *proj_dir, 
    Fdk_parms *parms
)
{
    Opencl_device ocl_dev;
    Opencl_buf *ocl_buf_vol;
    Opencl_buf *ocl_buf_img;
    Opencl_buf *ocl_buf_matrix;
    cl_int4 ocl_vol_dim;
    Opencl_buf *ocl_buf_vol_offset;
    Opencl_buf *ocl_buf_vol_spacing;
    cl_int2 ocl_proj_dim;
    Opencl_buf *ocl_buf_nrm;
    Opencl_buf *ocl_buf_ic;
    Proj_image *proj;
    int image_num;
    float scale;

    LOAD_LIBRARY_SAFE (libplmopencl);
    LOAD_SYMBOL (opencl_open_device, libplmopencl);
    LOAD_SYMBOL (opencl_load_programs, libplmopencl);
    LOAD_SYMBOL (opencl_kernel_create, libplmopencl);
    LOAD_SYMBOL (opencl_buf_create, libplmopencl);
    LOAD_SYMBOL (opencl_buf_write, libplmopencl);
    LOAD_SYMBOL (opencl_set_kernel_args, libplmopencl);
    LOAD_SYMBOL (opencl_kernel_enqueue, libplmopencl);
    LOAD_SYMBOL (opencl_buf_read, libplmopencl);

    /* Set up devices and kernels */
    opencl_open_device (&ocl_dev);
    opencl_load_programs (&ocl_dev, "fdk_opencl.cl");
    opencl_kernel_create (&ocl_dev, "fdk_kernel_nn");

    /* Retrieve 2D image to get dimensions */
    proj = proj_dir->load_image (0);

    /* Set up device memory */
    ocl_buf_vol = opencl_buf_create (
        &ocl_dev, 
        CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 
        vol->pix_size * vol->npix,
        vol->img
    );

    ocl_buf_img = opencl_buf_create (
        &ocl_dev, 
        CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, 
        proj->dim[1] * proj->dim[0] * sizeof(float),
        0
    );

    ocl_buf_matrix = opencl_buf_create (
        &ocl_dev, 
        CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, 
        12 * sizeof(float),
        0
    );

    /* Copy volume dim (convert from size_t to int) */
    opencl_idx(ocl_vol_dim,0) = vol->dim[0];
    opencl_idx(ocl_vol_dim,1) = vol->dim[1];
    opencl_idx(ocl_vol_dim,2) = vol->dim[2];

    ocl_buf_vol_offset = opencl_buf_create (
        &ocl_dev, 
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 
        3 * sizeof(float),
        vol->offset
    );

    ocl_buf_vol_spacing = opencl_buf_create (
        &ocl_dev, 
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 
        3 * sizeof(float),
        vol->spacing
    );

    /* Copy projection image dim (convert from size_t to int) */
    opencl_idx(ocl_proj_dim,0) = proj->dim[0];
    opencl_idx(ocl_proj_dim,1) = proj->dim[1];

    ocl_buf_nrm = opencl_buf_create (
        &ocl_dev, 
        CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, 
        3 * sizeof(float),
        0
    );

    ocl_buf_ic = opencl_buf_create (
        &ocl_dev, 
        CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, 
        2 * sizeof(float),
        0
    );

    /* Calculate the scale */
    image_num = 1 + (parms->last_img - parms->first_img) 
        / parms->skip_img;
    scale = (float)(sqrt(3.0)/(double)image_num);
    scale = scale * parms->scale;

    /* Free proj image */
    delete proj;

    /* Project each image into the volume one at a time */
    for (image_num = parms->first_img; 
         image_num < proj_dir->num_proj_images; 
         image_num++)
    {
        int i;
        float matrix[12], nrm[3], ic[2], sad;

        /* Load the current image and properties */
        proj = proj_dir->load_image (image_num);

        /* Apply ramp filter */
        if (parms->filter == FDK_FILTER_TYPE_RAMP) {
            proj_image_filter (proj);
        }

        /* Copy image bytes to device */
        opencl_buf_write (&ocl_dev, ocl_buf_img, 
            proj->dim[1] * proj->dim[0] * sizeof(float), proj->img);

        /* Copy matrix to device (convert from double to float) */
        for (i = 0; i < 12; i++) {
            matrix[i] = proj->pmat->matrix[i];
        }
        opencl_buf_write (&ocl_dev, ocl_buf_matrix, 
            12 * sizeof(float), matrix);

        /* Copy ic to device (convert from double to float) */
        ic[0] = proj->pmat->ic[0];
        ic[1] = proj->pmat->ic[1];
        opencl_buf_write (&ocl_dev, ocl_buf_ic, 2 * sizeof(float), ic);

        /* Copy nrm to device (convert from double to float) */
        nrm[0] = proj->pmat->nrm[0];
        nrm[1] = proj->pmat->nrm[1];
        nrm[2] = proj->pmat->nrm[2];
        opencl_buf_write (&ocl_dev, ocl_buf_nrm, 3 * sizeof(float), nrm);

        /* Convert sad from double to float */
        sad = proj->pmat->sad;

        /* Set fdk kernel arguments */
        opencl_set_kernel_args (
            &ocl_dev, 
            sizeof (cl_mem), &ocl_buf_vol[0], 
            sizeof (cl_mem), &ocl_buf_img[0], 
            sizeof (cl_mem), &ocl_buf_matrix[0], 
            sizeof (cl_int4), &ocl_vol_dim, 
            sizeof (cl_mem), &ocl_buf_vol_offset[0], 
            sizeof (cl_mem), &ocl_buf_vol_spacing[0], 
            sizeof (cl_int2), &ocl_proj_dim, 
            sizeof (cl_mem), &ocl_buf_nrm[0], 
            sizeof (cl_mem), &ocl_buf_ic[0], 
            sizeof (cl_float), &sad, 
            sizeof (cl_float), &scale,
            (size_t) 0
        );

        /* Compute workgroup size */
        /* (Max local_work_size for my ATI RV710 is 128) */
        size_t local_work_size = 128;
        size_t global_work_size = (float) vol->npix;

#if defined (commentout)
        /* For debugging */
        local_work_size = 4;
        global_work_size = 8;
#endif

        /* Invoke kernel */
        opencl_kernel_enqueue (&ocl_dev, global_work_size, local_work_size);
    }

    /* Read back results */
    opencl_buf_read (&ocl_dev, ocl_buf_vol, 
        vol->pix_size * vol->npix, vol->img);

#if defined (commentout)
    /* For debugging */
    int num_nonzero = 0;
    for (cl_uint i = 0; i < vol->npix; i++) {
        if (img[i] != 0.0f) {
            printf ("[%d] %f\n", i, img[i]);
            if (++num_nonzero == 10) {
                break;
            }
        }
    }
#endif

    UNLOAD_LIBRARY (libplmopencl);
}
