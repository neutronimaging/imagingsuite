
#ifndef NXDLL_EXPORT_H
#define NXDLL_EXPORT_H

#ifdef NEXUS_CPP_STATIC_DEFINE
#  define NXDLL_EXPORT
#  define NEXUS_CPP_NO_EXPORT
#else
#  ifndef NXDLL_EXPORT
#    ifdef NeXus_CPP_Shared_Library_EXPORTS
        /* We are building this library */
#      define NXDLL_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define NXDLL_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef NEXUS_CPP_NO_EXPORT
#    define NEXUS_CPP_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef NEXUS_CPP_DEPRECATED
#  define NEXUS_CPP_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef NEXUS_CPP_DEPRECATED_EXPORT
#  define NEXUS_CPP_DEPRECATED_EXPORT NXDLL_EXPORT NEXUS_CPP_DEPRECATED
#endif

#ifndef NEXUS_CPP_DEPRECATED_NO_EXPORT
#  define NEXUS_CPP_DEPRECATED_NO_EXPORT NEXUS_CPP_NO_EXPORT NEXUS_CPP_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define NEXUS_CPP_NO_DEPRECATED
#endif

#endif
