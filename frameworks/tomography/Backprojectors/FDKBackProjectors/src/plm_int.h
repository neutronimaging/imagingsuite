/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _plm_int_h_
#define _plm_int_h_

//#include "plmsys_config.h"

/* JAS 2012.05.06
 * If we are using GDCMv1, then gdcmCommon.h must be included **before**
 * the fix below is encountered.  By including it here, we ensure that
 * integer type redefinitions do not occur later if headers are later
 * included "out of order" somewhere in the project. */
//#if GDCM_VERSION_1
//#include "gdcmCommon.h"
//#endif

#if defined (GDCMCOMMON_H)
/* Gdcm 1.x has a broken header file gdcmCommon.h, which defines C99 types 
   (e.g. int32_t) when missing on MSVC.  It conflicts with plm_int.h 
   (which also fixes missing C99 types).  Here is a workaround for 
   this issue. */
#if !defined (CMAKE_HAVE_STDINT_H) && !defined (CMAKE_HAVE_INTTYPES_H) \
    && (defined(_MSC_VER)                                              \
        || defined(__BORLANDC__) && (__BORLANDC__ < 0x0560)            \
        || defined(__MINGW32__))
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif /* GDCMCOMMON_H */
#elif defined (_MSC_VER) && (_MSC_VER < 1600)
#include "msinttypes/stdint.h"
#else
#include <stdint.h>
#endif

/* These are non-standard */
#ifndef UINT32_T_MAX
#define UINT32_T_MAX (0xffffffff)
#endif
#ifndef INT32_T_MAX
#define INT32_T_MAX (0x7fffffff)
#endif
#ifndef INT32_T_MIN
#define INT32_T_MIN (-0x7fffffff - 1)
#endif

/* The data type plm_long is a signed integer with the same size as size_t.
   It is equivalent to the POSIX idea of ssize_t.  It is used for 
   OpenMP 2.0 loop variables which must be signed. */
#if (CMAKE_SIZEOF_SIZE_T == 8)
typedef int64_t plm_long;
#elif (CMAKE_SIZEOF_SIZE_T == 4)
typedef int32_t plm_long;
#else
//#error "Unexpected value for sizeof(size_t)"
typedef int64_t plm_long;

#endif

#endif
