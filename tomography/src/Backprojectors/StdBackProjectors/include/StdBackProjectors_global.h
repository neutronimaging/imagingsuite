#ifndef STDBACKPROJECTORS_GLOBAL_H
#define STDBACKPROJECTORS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STDBACKPROJECTORS_LIBRARY)
#  define STDBACKPROJECTORS_EXPORT Q_DECL_EXPORT
#else
#  define STDBACKPROJECTORS_EXPORT Q_DECL_IMPORT
#endif

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#endif // STDBACKPROJECTORS_GLOBAL_H
