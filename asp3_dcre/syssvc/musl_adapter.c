/*
 *  TOPPERS PROJECT Home Network Working Group Software
 * 
 *  Copyright (C) 2019 Cores Co., Ltd. Japan
 * 
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  @(#) $Id$
 */

#include <kernel.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <target_kernel_impl.h>
#include "syssvc/musl_adapter.h"
#include "kernel_cfg.h"

#ifndef _MSC_VER
void _start_c(const char * const*args);

void
_sta_ker()
{
	static const char *const args[] = {
		(char *)1,
		"asp3",
		0,
		// envp
		"TZ=JST-9",
		0,
		// auxv
		0
	};
	_start_c(args);
}

void *tid_address;

__attribute__((weak))
long SYS_set_tid_address(long a) {
	tid_address = (void *)a;
	return 0;
}

__attribute__((weak))
int SYS_gettid()
{
	ID tskid;
	ER ret;

	ret = get_tid(&tskid);
	if (ret != E_OK)
		return -ENOSYS;

	return tskid;
}

__attribute__((weak))
int SYS_getpid()
{
	return 1;
}

__attribute__((weak))
int SYS_tkill(int tid, int sig)
{
	ER ret;

	ret = ter_tsk(tid);
	if (ret != E_OK)
		return -ENOSYS;

	return 0;
}

sigset_t g_sigmask;

__attribute__((weak))
long SYS_rt_sigprocmask(int how, const sigset_t *restrict set, sigset_t *restrict old)
{
	if (old != NULL)
		memcpy(old, &g_sigmask, sizeof(sigset_t));

	switch (how) {
	case SIG_BLOCK:
		for (int i = 0; i < sizeof(g_sigmask.__bits) / sizeof(g_sigmask.__bits[0]); i++) {
			g_sigmask.__bits[i] |= set->__bits[i];
		}
		break;
	case SIG_UNBLOCK:
		for (int i = 0; i < sizeof(g_sigmask.__bits) / sizeof(g_sigmask.__bits[0]); i++) {
			g_sigmask.__bits[i] &= ~set->__bits[i];
		}
		break;
	case SIG_SETMASK:
		memcpy(&g_sigmask, set, sizeof(sigset_t));
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

__attribute__((weak))
void SYS_exit_group()
{
	ext_ker();
}

__attribute__((weak))
void SYS_exit()
{
	ext_ker();
}

__attribute__((weak))
int SYS_open(const char *path, int flags, void *arg)
{
	return -ENOSYS;
}

__attribute__((weak))
long SYS_poll()
{
	return -ENOSYS;
}

__attribute__((weak))
long SYS_brk(long a) {
	return (long)shell_brk((void *)a);
}

__attribute__((weak))
long SYS_mmap2(long a, long b, long c, long d, long e, long f, long g) {
	return (int)shell_mmap2((void *)a, (size_t)b, (int)c, (int)d, (int)e, ((off_t)f << 32) | (off_t)g);
}

__attribute__((weak))
long SYS_mprotect(long a, long b, long c) {
	return shell_mprotect((void *)a, (size_t)b, (int)c);
}

uint32_t __sync_add_and_fetch_4(uint32_t *count, uint32_t value)
{
	uint32_t result;
	bool_t lock = sense_lock();

	if (!lock)
		loc_cpu();

	result = *count;
	result += value;
	*count = result;

	if (!lock)
		unl_cpu();

	return result;
}

uint32_t __sync_sub_and_fetch_4(uint32_t *count, uint32_t value)
{
	uint32_t result;
	bool_t lock = sense_lock();

	if (!lock)
		loc_cpu();

	result = *count;
	result -= value;
	*count = result;

	if (!lock)
		unl_cpu();

	return result;
}

#endif

#ifndef _MSC_VER
extern uint32_t _HeapBase;
extern uint32_t _HeapLimit;
#else
uint8_t _HeapBase[16 * 4096];
#define _HeapLimit _HeapBase[sizeof(_HeapBase)]
#endif

void *shell_brk(void *addr)
{
	if (addr == 0) {
		return (void *)(&_HeapBase);
	}
	if ((addr >= (void *)&_HeapBase) && (addr < (void *)&_HeapLimit)) {
		return addr;
	}
	return (void *)-1;
}

void *shell_mmap2(void *start, size_t length, int prot, int flags, int fd, off_t pgoffset)
{
	if (fd != -1)
		return (void *)-EINVAL;

	if ((length >= 0) && (length <= sizeof(&_HeapBase))) {
		return &_HeapBase;
	}
	return (void *)-1;
}

int shell_mprotect(void *addr, size_t len, int prot)
{
	//if ((addr >= (void *)&_HeapBase) && (addr + len < (void *)&_HeapLimit)) {
	return 0;
//}
//return -1;
}

#include "syssvc/tlsf/tlsf.h"

static tlsf_t sys_tlsf;
static pool_t sys_pool;

void sys_malloc_init(intptr_t exinf)
{
	sys_tlsf = tlsf_create(&_HeapBase);
	if (sys_tlsf == NULL)
		return;

	sys_pool = tlsf_add_pool(sys_tlsf, ((uint8_t *)&_HeapBase) + tlsf_size(), ((intptr_t)&_HeapLimit - (intptr_t)&_HeapBase) - tlsf_size());
}

void sys_malloc_fini(intptr_t exinf)
{
	tlsf_destroy(sys_tlsf);
}

void *malloc(size_t size)
{
	void *result;
	wai_sem(SEM_MALLOC);
	result = tlsf_malloc(sys_tlsf, size);
	sig_sem(SEM_MALLOC);
	if (result == NULL)
		tlsf_check_pool(sys_pool);
	return result;
}

void *calloc(size_t size, size_t count)
{
	void *result;
	wai_sem(SEM_MALLOC);
	result = tlsf_malloc(sys_tlsf, count * size);
	sig_sem(SEM_MALLOC);
	if (result != NULL)
		memset(result, 0, count * size);
	else
		tlsf_check_pool(sys_pool);
	return result;
}

void *realloc(void *ptr, size_t size)
{
	void *result;
	wai_sem(SEM_MALLOC);
	result = tlsf_realloc(sys_tlsf, ptr, size);
	sig_sem(SEM_MALLOC);
	if (result == NULL)
		tlsf_check_pool(sys_pool);
	return result;
}

void free(void *ptr)
{
	wai_sem(SEM_MALLOC);
	tlsf_free(sys_tlsf, ptr);
	sig_sem(SEM_MALLOC);
}

