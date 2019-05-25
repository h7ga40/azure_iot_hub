#include "pthread_impl.h"

int __set_thread_area(void *p)
{
	__asm__ __volatile__ ( "mvtc %0, fintv" : : "r"(p) : "memory" );
	return 0;
}
