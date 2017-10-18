#ifndef NAPICONFIG_H
#define NAPICONFIG_H

#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#ifdef __VMS
#include <nxconfig_vms.h>
#else
#include <nxconfig.h>
#endif /* __VMS */


/*
 * Integer type definitions
 * 
 * int32_t etc will be defined by configure in nxconfig.h 
 * if they exist; otherwise include an appropriate header
 */
#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif /* HAVE_STDINT_H */


#endif /* NAPICONFIG_H */
