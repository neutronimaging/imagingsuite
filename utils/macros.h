#ifndef MACROS_H
#define MACROS_H 1
#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
    #define UNUSED_VARIABLE(x)\
        (void) (x);
#else
#error "Unknown compiler. Supported compiler (GCC/Clang/MSVC)"
#endif

#endif // MACROS_H
