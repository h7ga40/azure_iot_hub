#include <features.h>

#define START "_start"

#include "crt_arch.h"

int main();
#ifdef __c2__
void _init();
void _fini();
#else
void _init() __attribute__((weak));
void _fini() __attribute__((weak));

_Noreturn
#endif
int __libc_start_main(int (*)(), int, char **,
	void (*)(), void(*)(), void(*)());

void _start_c(long *p)
{
	int argc = p[0];
	char **argv = (void *)(p+1);
	__libc_start_main(main, argc, argv, _init, _fini, 0);
}
