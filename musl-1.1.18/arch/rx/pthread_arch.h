static inline struct pthread *__pthread_self()
{
#ifndef __c2__
	char *self;
	__asm__ __volatile__ ("mvfc fintv,%0" : "=r" (self) );
#else
	extern void *thread_area;
	char *self = (char *)thread_area;
#endif
	return (struct pthread *) (self + 8 - sizeof(struct pthread));
}

#define TLS_ABOVE_TP
#define TP_ADJ(p) ((char *)(p) + sizeof(struct pthread) - 8)

#define MC_PC sc_pc
