/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _fdk_utils_h_
#define _fdk_utils_h_

#include "plmreconstruct_config.h"

class Fdk_parms;
class Proj_image;
class Volume;

PLMRECONSTRUCT_C_API void convert_to_hu (Volume* vol, Fdk_parms* parms);
PLMRECONSTRUCT_C_API Volume* my_create_volume (Fdk_parms* parms);
Proj_image* get_image_pfm (Fdk_parms* parms, int image_num);
Proj_image* get_image_raw (Fdk_parms* parms, int image_num);
void write_coronal_sagittal (Fdk_parms* parms, Volume* vol);

#endif
