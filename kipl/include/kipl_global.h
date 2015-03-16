#ifndef KIPL_GLOBAL_H
#define KIPL_GLOBAL_H

#ifdef QT_VERSION
#include <QtCore/qglobal.h>

	#if defined(KIPL_LIBRARY)
	#  define KIPLSHARED_EXPORT Q_DECL_EXPORT
	#else
	#  define KIPLSHARED_EXPORT Q_DECL_IMPORT
	#endif
#else
	#define KIPLSHARED_EXPORT
#endif

#endif // KIPL_GLOBAL_H
