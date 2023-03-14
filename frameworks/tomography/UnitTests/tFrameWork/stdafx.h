//<LICENSE>

#pragma once

#include "targetver.h"

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#ifndef _WINDOWS_
#include <Windows.h>
#endif

#define DLL_EXPORT __declspec(dllexport)

#else
#define DLL_EXPORT
#endif

