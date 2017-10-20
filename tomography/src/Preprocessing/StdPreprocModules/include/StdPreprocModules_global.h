#ifndef STDPREPROCMODULES_GLOBAL_H
#define STDPREPROCMODULES_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STDPREPROCMODULES_LIBRARY)
#  define STDPREPROCMODULESSHARED_EXPORT Q_DECL_EXPORT
#else
#  define STDPREPROCMODULESSHARED_EXPORT Q_DECL_IMPORT
#endif

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#endif // STDPREPROCMODULES_GLOBAL_H
