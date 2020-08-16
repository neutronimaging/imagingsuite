#ifndef READERCONFIG_GLOBAL_H
#define READERCONFIG_GLOBAL_H

#if !defined(NO_QT)
#include <QtCore/qglobal.h>

#if defined(READERCONFIG_LIBRARY)
#  define READERCONFIGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define READERCONFIGSHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#  define READERCONFIGSHARED_EXPORT
#endif

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#endif // READERCONFIG_GLOBAL_H
