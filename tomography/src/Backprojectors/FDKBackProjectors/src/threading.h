/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _threading_h_
#define _threading_h_

//#include "plm_config.h"

/* GCS: You can't use enum types in mixed C / C++ code, because they 
   are not required to have the same size.  Therefore, revert to #defines */
typedef int Threading;
#define THREADING_UNKNOWN         0
#define THREADING_CPU_SINGLE      1
#define THREADING_CPU_OPENMP      2
#define THREADING_BROOK           3
#define THREADING_CUDA            4
#define THREADING_OPENCL          5

#endif
