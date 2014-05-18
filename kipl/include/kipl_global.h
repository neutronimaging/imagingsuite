#ifndef KIPL_GLOBAL_H
#define KIPL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(KIPL_LIBRARY)
#  define KIPLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KIPLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // KIPL_GLOBAL_H
