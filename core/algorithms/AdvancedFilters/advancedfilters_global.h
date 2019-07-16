#ifndef ADVANCEDFILTERS_GLOBAL_H
#define ADVANCEDFILTERS_GLOBAL_H

#ifndef NO_QT
    #include <QtCore/qglobal.h>

    #if defined(ADVANCEDFILTERS_LIBRARY)
    #  define ADVANCEDFILTERSSHARED_EXPORT Q_DECL_EXPORT
    #else
    #  define ADVANCEDFILTERSSHARED_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define ADVANCEDFILTERSSHARED_EXPORT
#endif

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#endif // ADVANCEDFILTERS_GLOBAL_H
