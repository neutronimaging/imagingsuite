#ifndef KIPL_GLOBAL_H
#define KIPL_GLOBAL_H

#ifdef MSCVER
#  define KIPLSHARED_EXPORT __declspec( dllexport )
#else
#  define KIPLSHARED_EXPORT
#endif

#endif // KIPL_GLOBAL_H
