/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _FDK_OPENCL_H_
#define _FDK_OPENCL_H_

//#include "plmreconstruct_config.h"
#include "delayload.h"

class Fdk_parms;
class Proj_image_dir;
class Volume;

void opencl_reconstruct_conebeam (
    Volume *vol, 
    Proj_image_dir *proj_dir, 
    Fdk_parms *parms
);

#endif
