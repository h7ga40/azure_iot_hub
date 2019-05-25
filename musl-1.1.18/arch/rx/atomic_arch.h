#if defined(__SH4A__)

#define a_ll a_ll
static inline int a_ll(volatile int *p)
{
	int v;
	__asm__ __volatile__ ("movli.l @%1, %0" : "=z"(v) : "r"(p), "m"(*p));
	return v;
}

#define a_sc a_sc
static inline int a_sc(volatile int *p, int v)
{
	int r;
	__asm__ __volatile__ (
		"movco.l %2, @%3 ; movt %0"
		: "=r"(r), "=m"(*p) : "z"(v), "r"(p) : "memory", "cc");
	return r;
}

#define a_barrier a_barrier
static inline void a_barrier()
{
	__asm__ __volatile__ ("synco" ::: "memory");
}

#define a_pre_llsc a_barrier
#define a_post_llsc a_barrier

#else

#define a_cas a_cas
static inline int a_cas(volatile int *p, int t, int s)
{
#ifndef __c2__
	__asm__ __volatile__ ("clrpsw i");
	if (*p == t)
		*p = s;
	else
		s = *p;
	__asm__ __volatile__ ("setpsw i");
	return s;
#else
	extern int __attribute__((stdcall)) InterlockedCompareExchange(
		volatile int *Destination, int Exchange, int Comperand);
	/*__asm__ __volatile__(
		"lock ; cmpxchg %3, %1"
		: "=a"(t), "=m"(*p) : "a"(t), "r"(s) : "memory");
	return t;*/
	return InterlockedCompareExchange(p,t,s);
#endif
}

#endif
