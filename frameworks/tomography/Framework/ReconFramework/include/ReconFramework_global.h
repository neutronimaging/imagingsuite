#ifndef RECONFRAMEWORK_GLOBAL_H
#define RECONFRAMEWORK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RECONFRAMEWORK_LIBRARY)
#  define RECONFRAMEWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RECONFRAMEWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

//#ifdef _MSC_VER
//#  define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//// Windows Header Files:
//#  ifndef _WINDOWS_
//#    include <Windows.h>
//#  endif
//#endif

#endif // MODULECONFIG_GLOBAL_H
