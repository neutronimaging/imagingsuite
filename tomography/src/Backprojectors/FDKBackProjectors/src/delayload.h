/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _delayload_h_
#define _delayload_h_

//#include "plmsys_config.h"
#ifndef _WIN32
#include <dlfcn.h>
#endif
#include <stdlib.h>

// Needed for delay loading windows DLLs
#if _MSC_VER
    #pragma comment(lib, "delayimp")
    #pragma comment(lib, "user32")
#endif

// JAS 2010.11.23
// ------------------------------------------------------------
// Because these macros contain declarations, their
// usage is restricted under C89.  C89-GNU, C99, and
// C++ are all cool with using these macros pretty
// much anywhere... be careful inside switch statements.
// Still, their usage will determine the portability of
// plastimatch.

// Note: if you attempt to load a library that
//       does not exist or cannot be found, this
//       returns a null pointer.
#if !defined(_WIN32) && defined(PLM_USE_GPU_PLUGINS)
    #define LOAD_LIBRARY(lib)                      \
        void* lib = dlopen_ex (#lib".so");          
#else
    #define LOAD_LIBRARY(lib)                      \
        ;
#endif

// Note: if lib contains a null pointer here
//       (see above note), this will return a
//       null function pointer.  Be careful pls.
#if !defined(_WIN32) && defined(PLM_USE_GPU_PLUGINS)
    #define LOAD_SYMBOL(sym, lib)                  \
        sym##_##t* sym = (sym##_##t*) dlsym (lib, #sym);          
#else
    #define LOAD_SYMBOL(sym, lib)                  \
        ;
#endif

// ------------------------------------------------------------


// JAS 2012.03.29
// ------------------------------------------------------------
// Now that plastimatch is officially C++, we can now safely
// define this macro, which reduces programmer error.  This
// should be used instead of LOAD_LIBRARY
#if !defined(_WIN32) && defined(PLM_USE_GPU_PLUGINS)
    #define LOAD_LIBRARY_SAFE(lib)                 \
        if (!delayload_##lib()) { exit (0); }      \
        void* lib = dlopen_ex (#lib".so");          
#else
    #define LOAD_LIBRARY_SAFE(lib)                 \
        if (!delayload_##lib()) { exit (0); }      \
        ;
#endif

// JAS 2010.12.09
// Despite what the man pages say, dlclose()ing NULL
// was resulting in segfaults!  So, now we check 1st.
#if !defined(_WIN32) && defined(PLM_USE_GPU_PLUGINS)
    #define UNLOAD_LIBRARY(lib)                    \
        if (lib != NULL) {                         \
            dlclose (lib);                         \
        }                                           
#else
    #define UNLOAD_LIBRARY(lib)                    \
        ;
#endif

#define DELAYLOAD_WRAP(f, ...)                          \
    f (__VA_ARGS__); typedef f##_t(__VA_ARGS__);

PLMSYS_C_API int delayload_libplmcuda (void);
PLMSYS_C_API int delayload_libplmreconstructcuda (void);
PLMSYS_C_API int delayload_libplmregistercuda (void);
PLMSYS_C_API int delayload_libplmopencl (void);
PLMSYS_C_API void* dlopen_ex (const char* lib); 

#endif
