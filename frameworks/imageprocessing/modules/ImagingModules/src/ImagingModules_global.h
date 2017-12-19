#ifndef IMAGINGMODULES_GLOBAL_H
#define IMAGINGMODULES_GLOBAL_H
#include <QtCore/qglobal.h>

#ifdef _MSC_VER
    #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
    #include <windows.h>
#endif

#if defined(IMAGINGMODULES_LIBRARY)
#  define IMAGINGMODULESSHARED_EXPORT Q_DECL_EXPORT
#else
#  define IMAGINGMODULESSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // IMAGINGMODULES_GLOBAL_H
