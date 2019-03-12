///* -----------------------------------------------------------------------
//   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
//   ----------------------------------------------------------------------- */
//#define PLM_CUDA_COMPILE 1
//#include "plmreconstruct_config.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <math.h>
//#if defined (_WIN32)
//#include <windows.h>
//#endif

//#include "fdk.h"
//#include "fdk_cuda.h"


//// JAS 2011.01.20
//// I have changed the type of parameter "scale" from float to double.
//// When typed as float, parameter passing between gpuit and the plmcuda
//// plugin would fail and produce garbage within the function.  This is
//// a temporary kludge/fix.  We *should* be able to use parameters of type float
//// when communicating to the plugin.
//void*
//fdk_cuda_state_create (
//    Volume *vol,
//    unsigned int image_npix,
//    double scale,
//    Fdk_parms *parms
//)
//{
//    return fdk_cuda_state_create_cu (vol, image_npix, scale, parms);
//}

//void
//fdk_cuda_state_destroy (
//    void *void_state
//)
//{
//    fdk_cuda_state_destroy_cu (void_state);
//}

//void
//fdk_cuda_queue_image (
//    void *dev_state,
//    int *dim,
//    double *ic,
//    double *nrm,
//    double sad,
//    double sid,
//    double *matrix,
//    float *img
//)
//{
//    fdk_cuda_queue_image_cu (dev_state, dim, ic, nrm, sad, sid,
//	matrix, img);
//}

//void
//fdk_cuda_backproject (void *dev_state)
//{
//    fdk_cuda_backproject_cu (dev_state);
//}

//void
//fdk_cuda_fetch_volume (
//    void *dev_state,
//    void *host_buf,
//    unsigned int copy_size
//)
//{
//    fdk_cuda_fetch_volume_cu (dev_state, host_buf, copy_size);
//}
