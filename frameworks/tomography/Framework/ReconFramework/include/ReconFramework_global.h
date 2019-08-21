#ifndef RECONFRAMEWORK_GLOBAL_H
#define RECONFRAMEWORK_GLOBAL_H

#ifndef NO_QT
#include <QtCore/qglobal.h>
#if defined(RECONFRAMEWORK_LIBRARY)
#  define RECONFRAMEWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RECONFRAMEWORKSHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#define RECONFRAMEWORKSHARED_EXPORT
#endif

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif
#endif // MODULECONFIG_GLOBAL_H


