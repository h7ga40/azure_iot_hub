#include <endian.h>

#if __BYTE_ORDER == __BIG_ENDIAN
#define ENDIAN_SUFFIX "eb"
#else
#define ENDIAN_SUFFIX ""
#endif

#if __RX_FPU_ANY__ || __SH4__
#define FP_SUFFIX ""
#else
#define FP_SUFFIX "-nofpu"
#endif

#if __RX_FDPIC__
#define ABI_SUFFIX "-fdpic"
#else
#define ABI_SUFFIX ""
#endif

#define LDSO_ARCH "rx" ENDIAN_SUFFIX FP_SUFFIX ABI_SUFFIX

#define TPOFF_K 8

#define REL_SYMBOLIC    R_RX_DIR32
#define REL_OFFSET      R_RX_REL32
#define REL_GOT         R_RX_GLOB_DAT
#define REL_PLT         R_RX_JMP_SLOT
#define REL_RELATIVE    R_RX_RELATIVE
#define REL_COPY        R_RX_COPY
#define REL_DTPMOD      R_RX_TLS_DTPMOD32
#define REL_DTPOFF      R_RX_TLS_DTPOFF32
#define REL_TPOFF       R_RX_TLS_TPOFF32

#define DL_NOMMU_SUPPORT 1

#if __RX_FDPIC__
#define REL_FUNCDESC    R_RX_FUNCDESC
#define REL_FUNCDESC_VAL R_RX_FUNCDESC_VALUE
#undef  REL_RELATIVE
#define DL_FDPIC 1
#define FDPIC_CONSTDISP_FLAG 0x100
#define CRTJMP(pc,sp) do { \
	register size_t r8 __asm__("r8") = ((size_t *)(sp))[-2]; \
	__asm__ __volatile__( "jsr %0 ; mov %1,r15" \
	: : "r"(pc), "r"(sp), "r"(r8) : "memory" ); } while(0)
#define GETFUNCSYM(fp, sym, got) __asm__ ( \
	"mov.l 1f,%0 ; add %1,%0 ; bra 2f ; nop ; .align 2 \n" \
	"1:	.long " #sym "@GOTOFFFUNCDESC \n2:" \
	: "=&r"(*fp) : "r"(got) : "memory" )
#else
#ifndef __c2__
#define CRTJMP(pc,sp) __asm__ __volatile__( \
	"jsr %0 ; mov %1,r15" : : "r"(pc), "r"(sp) : "memory" )
#else
#define CRTJMP(pc,sp) __asm__ __volatile__( \
	"mov %1,%%esp ; jmp *%0" : : "r"(pc), "r"(sp) : "memory" )
#endif
#endif
