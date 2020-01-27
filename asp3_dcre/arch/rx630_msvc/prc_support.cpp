/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2014-2019 Cores Co., Ltd. Japan
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
 *  @(#) $Id: prc_support.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
 */

#include "StdAfx.h"
#include "prc_insn.h"
#include "WinKernel.h"
extern "C" {
#include "overrun.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 贋ＩＴＲＯＮカーネル
extern CKernel *g_Kernel;

uint32_t current_psw(void)
{
	return (g_Kernel->CurrentIsLocked() ? 0 : PSW_I_MASK) | ((g_Kernel->GetPrio() << 24) & PSW_IPL_MASK);
}

void set_psw(uint32_t psw)
{
	uint32_t pre = current_psw();

	g_Kernel->SetPrio((psw & PSW_IPL_MASK) >> 24);

	if(((pre ^ psw) & PSW_I_MASK) != 0) {
		if((psw & PSW_I_MASK) == 0)
			disint();
		else
			enaint();
	}
}

/*
 *  NMIを除くすべての割込みの禁止
 */
void disint(void)
{
	if (!g_Kernel->CurrentIsLocked())
		g_Kernel->LockCPU();
}

/*
 *  NMIを除くすべての割込みの許可
 */
void enaint(void)
{
	if (g_Kernel->CurrentIsLocked())
		g_Kernel->UnlockCPU();
}

/*
 *  割込みハンドラの設定
 */
void define_inh(uint_t inhno, FP int_entry)
{
	// 登録破棄の場合
	if ((intptr_t)int_entry == -1) {
		g_Kernel->DefineInterruptHandler(inhno, NULL);
	}
	// 登録の場合
	else {
		g_Kernel->DefineInterruptHandler(inhno, (void (*)())int_entry);
	}
}

bool starting = true;

bool_t sense_lock()
{
	return g_Kernel->CurrentIsLocked();
}

bool_t check_intno_cfg(INTNO intno)
{
	return g_Kernel->CheckIntNoCfg(intno);
}

/*
 *  コンテキストの参照
 */
bool_t sense_context(void)
{
	return !g_Kernel->InTaskContext() && !starting;
}

/*
 *  CPU例外の発生した時のコンテキストの参照
 */
bool_t exc_sense_context( void *p_excinf )
{
	return !g_Kernel->InTaskContext() && !starting;
}

void raise_int(INTNO intno)
{
	g_Kernel->Interrupt(intno);
}

/*
 *  カーネル起動時タスクディスパッチャ
 */
void _start_dispatch(void)
{
	starting = false;
}

/*
 *  タスクディスパッチャ
 */
void dispatch(void)
{
	g_Kernel->Dispatch();
}

/*
 *  現在のコンテキストを捨ててディスパッチ
 */
void _exit_and_dispatch(void)
{
	g_Kernel->ExitAndDispatch();
}

/*
 *  カーネルの終了処理の呼出し
 * 
 *  スタックを非タスクコンテキスト用に切り替え．
 * 
 */
void call_exit_kernel(void)
{
	g_Kernel->Terminate();
}

/*
 *  タスク起動処理
 * 
 */ 
void start_r(void)
{
#if TIPM_LOCK != -15
	lock_flag = false;
#endif	/* TIPM_LOCK != -15	*/
}

extern "C" void Throw()
{
	throw std::exception();
}

extern "C" void trigger_cpu_exception( void )
{
	throw std::exception();
}

extern "C" void kernel_interrupt(INTNO intno, void(*int_entry)())
{
#ifdef TOPPERS_SUPPORT_OVRHDR /* 6.6.1 */
	ovrtimer_stop(intno);
#endif /* TOPPERS_SUPPORT_OVRHDR */
	enaint();
	int_entry();
	disint();
}

extern "C" void kernel_exception(EXCNO excno, void(*exc_entry)())
{
#ifdef TOPPERS_SUPPORT_OVRHDR /* 6.6.1 */
	ovrtimer_stop(255);
#endif /* TOPPERS_SUPPORT_OVRHDR */
	enaint();
	exc_entry();
	disint();
}

extern "C" void PowerON_Reset_PC(void)
{
}
