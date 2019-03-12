/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _fdk_cuda_h_
#define _fdk_cuda_h_

//#include "plmreconstruct_config.h"
#include "fdkreconbase.h"
#include "delayload.h"

class Fdk_parms;
class Volume;


#define FDK_CUDA_TIME_KERNEL 0


#if defined __cplusplus
extern "C" {
#endif

//// CUDA Plugin (plmcuda) interfaces ////////////

//PLMRECONSTRUCTCUDA_API
//DELAYLOAD_WRAP (
//void* fdk_cuda_state_create,
//    Volume *vol,
//    unsigned int image_npix,
//    double scale,
//    Fdk_parms *parms
//);

//PLMRECONSTRUCTCUDA_API
//DELAYLOAD_WRAP (
//void fdk_cuda_state_destroy,
//    void *void_state
//);

//PLMRECONSTRUCTCUDA_API
//DELAYLOAD_WRAP (
//void fdk_cuda_queue_image,
//    void *dev_state,
//    int *dim,
//    double *ic,
//    double *nrm,
//    double sad,
//    double sid,
//    double *matrix,
//    float *img
//);

//PLMRECONSTRUCTCUDA_API
//DELAYLOAD_WRAP (
//void fdk_cuda_backproject,
//    void *dev_state
//);

//PLMRECONSTRUCTCUDA_API
//DELAYLOAD_WRAP (
//void fdk_cuda_fetch_volume,
//    void *dev_state,
//    void *host_buf,
//    unsigned int copy_size
//);

//////////////////////////////////////////////////


void*
fdk_cuda_state_create_cu (
    Volume *vol, 
    unsigned int image_npix, 
    float scale, 
    Fdk_parms *parms
);

void
fdk_cuda_state_destroy_cu (
    void *void_state
);

void
fdk_cuda_queue_image_cu (
    void *dev_state, 
    int *dim, 
    double *ic, 
    double *nrm, 
    double sad, 
    double sid, 
    double *matrix, 
    float *img
);

void
fdk_cuda_backproject_cu (void *dev_state);

void
fdk_cuda_fetch_volume_cu (
    void *dev_state, 
    void *host_buf, 
    unsigned int copy_size
);

#if defined __cplusplus
}
#endif

#endif
