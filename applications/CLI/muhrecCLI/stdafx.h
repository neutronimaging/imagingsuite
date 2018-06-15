//<LICENSE>

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __STDAFX_H
#define __STDAFX_H

#include "../include/targetver.h"

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#ifndef _WINDOWS_
#include <windows.h>
#endif

#define DLL_EXPORT __declspec(dllexport)

#else
#define DLL_EXPORT
#endif


#endif
