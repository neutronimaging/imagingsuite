/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#include "plmutil_config.h"

#include "proj_image.h"
#include "proj_image_filter.h"
#include "ramp_filter.h"

#define FFTW_FOUND 1

void
proj_image_filter (Proj_image *proj)
{
#if (FFTW_FOUND)
    ramp_filter (proj->img, proj->dim[0], proj->dim[1]);
#endif
}
