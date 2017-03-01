//<LICENCE>

#ifndef KIPL_GLOBAL_H
#define KIPL_GLOBAL_H



#if !defined(NO_QT)
#include <QtCore/qglobal.h>

#if defined(KIPL_LIBRARY)
#  define KIPLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KIPLSHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#define KIPLSHARED_EXPORT
#endif

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

/// \brief The kipl namespace contains the core library for most applications
namespace kipl {}
#endif // KIPL_GLOBAL_H
