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
//#ifdef _MSC_VER
//#  define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//// Windows Header Files:
//#  ifndef _WINDOWS_
//#    include <Windows.h>
//#  endif
//#endif

#endif // MODULECONFIG_GLOBAL_H


