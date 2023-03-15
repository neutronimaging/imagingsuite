//<LICENSE>

#ifndef RECONFRAMEWORK_GLOBAL_H
#define RECONFRAMEWORK_GLOBAL_H

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define Q_DECL_EXPORT __declspec(dllexport)
#  define Q_DECL_IMPORT __declspec(dllimport)
#  define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#else
#  define Q_DECL_EXPORT     __attribute__((visibility("default")))
#  define Q_DECL_IMPORT     __attribute__((visibility("default")))
#endif

#if defined(RECONFRAMEWORK_LIBRARY)
#  define RECONFRAMEWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RECONFRAMEWORKSHARED_EXPORT Q_DECL_IMPORT
#endif


#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#endif // RECONFRAMEWORK_GLOBAL_H


