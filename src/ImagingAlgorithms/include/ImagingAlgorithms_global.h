#ifndef IMAGINGALGORITHMS_GLOBAL_H
#define IMAGINGALGORITHMS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IMAGINGALGORITHMS_LIBRARY)
#  define IMAGINGALGORITHMSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define IMAGINGALGORITHMSSHARED_EXPORT Q_DECL_IMPORT
#endif

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#endif // IMAGINGALGORITHMS_GLOBAL_H
