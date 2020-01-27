#include "pthread_impl.h"

int __set_thread_area(void *p)
{
#ifndef __c2__
	__asm__ __volatile__ ( "mvtc %0, fintv" : : "r"(p) : "memory" );
	return 0;
#else
#ifdef SYS_set_thread_area
	return __syscall(SYS_set_thread_area, p);
#else
	return -ENOSYS;
#endif
#endif
}
