//<LICENSE>

#ifndef MODULECONFIG_GLOBAL_H
#define MODULECONFIG_GLOBAL_H

//#include "targetver.h"
//#ifdef _MSC_VER
//#  define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//// Windows Header Files:
//#  ifndef _WINDOWS_
//#    include <Windows.h>
//#  endif
//#endif

#include <QtCore/qglobal.h>


#if defined(MODULECONFIG_LIBRARY)
#  define MODULECONFIGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MODULECONFIGSHARED_EXPORT Q_DECL_IMPORT
#endif



#endif // MODULECONFIG_GLOBAL_H
