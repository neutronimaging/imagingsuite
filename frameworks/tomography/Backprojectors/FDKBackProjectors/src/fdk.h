/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _fdk_h_
#define _fdk_h_

//#include "plmreconstruct_config.h"
// #include "plm_int.h"
#include "threading.h"

class Proj_image_dir;
class Volume;

enum Fdk_filter_type {
    FDK_FILTER_TYPE_NONE,
    FDK_FILTER_TYPE_RAMP
};

class Fdk_parms {
public:
    Threading threading;

    int image_range_requested;
    int first_img;
    int skip_img;
    int last_img;

    plm_long dim[3];
    float vol_size[3];
    float xy_offset[2];

    float scale;
    enum Fdk_filter_type filter;

    char flavor;

    char* input_dir;
    char* output_file;

    int full_fan;            //Full_fan=1, Half_fan=0;
    char* Full_normCBCT_name;
    int Full_radius;
    char* Half_normCBCT_name;
    int Half_radius;
};

void reconstruct_conebeam (
    Volume* vol, 
    Proj_image_dir *proj_dir, 
    Fdk_parms* parms
);
void CUDA_reconstruct_conebeam (
    Volume *vol, 
    Proj_image_dir *proj_dir,
    Fdk_parms *parms
);

void fdk_do_bowtie (Volume* vol, Fdk_parms* parms);


#endif
