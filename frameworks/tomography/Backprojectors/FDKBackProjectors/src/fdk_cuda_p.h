/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _fdk_cuda_p_h_
#define _fdk_cuda_p_h_

#include <cuda.h>
#include "fdk_cuda.h"

typedef struct kernel_args_fdk Fdk_cuda_kernel_args;
struct kernel_args_fdk
{
    size_t img_dim[2];
    float ic[2];
    float nrm[3];
    float sad;
    float sid;
    float scale;
    float vol_origin[3];
    size_t vol_dim[3];
    float vol_spacing[3];
    float matrix[12];
    char padding[4]; //for data alignment
    //padding to 128Bytes
};

typedef struct fdk_cuda_state Fdk_cuda_state;
struct fdk_cuda_state
{
    Fdk_cuda_kernel_args kargs;         // Host kernel args
    Fdk_cuda_kernel_args *dev_kargs;    // Device kernel args
    float *dev_vol;                     // Device volume voxels
    float *dev_img;                     // Device image pixels
    float *dev_matrix;                  // Device projection matrix
    size_t dimGrid[3];                       // CUDA grid size
    size_t dimBlock[3];                      // CUDA block size
    int blocksInY;                      // CUDA grid size
};

 
#endif
