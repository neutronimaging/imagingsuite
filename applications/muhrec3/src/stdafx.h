// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _STDAFXMUH3_H
#define _STDAFXMUH3_H

#include "targetver.h"

#ifdef _MSC_VER
//#include <stdio.h>
//#include <tchar.h>
#else

#endif

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#endif
