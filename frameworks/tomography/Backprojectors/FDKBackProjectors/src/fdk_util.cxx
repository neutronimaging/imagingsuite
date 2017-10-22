/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#include "plmreconstruct_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdk.h"
#include "fdk_util.h"
#include "ramp_filter.h"
#include "volume.h"

Volume*
my_create_volume (Fdk_parms* parms)
{
    float offset[3];
    float spacing[3];
    float* vol_size = parms->vol_size;
    plm_long* dim = parms->dim;

    spacing[0] = vol_size[0] / dim[0];
    spacing[1] = vol_size[1] / dim[1];
    spacing[2] = vol_size[2] / dim[2];

    offset[0] = -vol_size[0] / 2.0f + spacing[0] / 2.0f;
    offset[1] = -vol_size[1] / 2.0f + spacing[1] / 2.0f;
    offset[2] = -vol_size[2] / 2.0f + spacing[2] / 2.0f;

    return new Volume (dim, offset, spacing, 0, PT_FLOAT, 1);
}

float
convert_to_hu_pixel (float in_value)
{
    float hu;
    float diameter = 40.0;  /* reconstruction diameter in cm */
    hu = 1000 * ((in_value / diameter) - .167) / .167;
    return hu;
}

void
convert_to_hu (Volume* vol, Fdk_parms* parms)
{
    plm_long i, j, k, p;
    float* img = (float*) vol->img;
    
    p = 0;
    for (k = 0; k < vol->dim[2]; k++) {
	for (j = 0; j < vol->dim[1]; j++) {
	    for (i = 0; i < vol->dim[0]; i++) {
		img[p] = convert_to_hu_pixel (img[p]);
		p++;
	    }
	}
    }
}
