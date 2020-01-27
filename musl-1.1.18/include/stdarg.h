#ifndef _STDARG_H
#define _STDARG_H

#ifdef __cplusplus
extern "C" {
#endif

#define __NEED_va_list

#include <bits/alltypes.h>

#if !defined(_MSC_VER) && !defined(__c2__)

#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

#else

#define _INTSIZEOF(n)          ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#define __crt_va_start(ap, v)   ((void)(ap = (va_list)&(v) + _INTSIZEOF(v)))
#define __crt_va_arg(ap, t)     (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define __crt_va_end(ap)        ((void)(ap = (va_list)0))

#define va_start __crt_va_start
#define va_arg   __crt_va_arg
#define va_end   __crt_va_end
#define va_copy(destination, source) ((destination) = (source))

#endif

#ifdef __cplusplus
}
#endif

#endif
