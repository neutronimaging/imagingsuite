#ifndef IMAGINGMODULESGUI_GLOBAL_H
#define IMAGINGMODULESGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef _WINDOWS_
#include <Windows.h>
#include <winnt.h>
#endif


#if defined(IMAGINGMODULESGUI_LIBRARY)
#  define IMAGINGMODULESGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define IMAGINGMODULESGUISHARED_EXPORT Q_DECL_IMPORT
#endif

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#endif // IMAGINGMODULESGUI_GLOBAL_H
