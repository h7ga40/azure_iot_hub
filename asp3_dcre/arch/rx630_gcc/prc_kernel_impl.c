/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005,2006 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2008-2010 by Witz Corporation, JAPAN
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

/*
 *		プロセッサ依存モジュール（RX630用）
 */

#include "kernel_impl.h"
#include "sil.h"


/*
 *  割込み発生回数を保存する変数
 */
uint16_t	intnest;

/*
 *  CPUロックフラグ実現のための変数
 */
#if TIPM_LOCK != -15
bool_t		lock_flag;		/* CPUロックフラグの値を保持する変数 */
uint32_t	saved_ipl;		/* 割込み優先度レベルを保存する変数 */
#endif	/* TIPM_LOCK != -15	*/


/*
 *  プロセッサ依存の初期化
 */
void
prc_initialize( void )
{
	/*
	 *  割込み発生回数の初期化
	 *
	 *  スタートアップは全割込みロック状態 & 非タスクコンテキスト
	 *  にて動作するため, 非タスクコンテキストに偽装する.
	 */
	intnest = 1U;

#if TIPM_LOCK != -15
	/*
	 *  CPUロックフラグ実現のための変数の初期化
	 */
	lock_flag = true;
	saved_ipl = IPL_ENAALL;
#endif	/* TIPM_LOCK != -15	*/
}


/*
 *  プロセッサ依存の終了処理
 */
void
prc_terminate( void )
{
	/*
	 *  特に必要な処理はない
	 */
}


/*
 *  CPU例外の発生状況のログ出力
 *
 *  CPU例外ハンドラの中から，CPU例外情報ポインタ（p_excinf）を引数とし
 *  て呼び出すことで，CPU例外の発生状況をシステムログに出力する．
 */
#ifdef SUPPORT_XLOG_SYS
void
xlog_sys( void *p_excinf )
{
	syslog(LOG_ERROR , "r1 = %u") , *((uint32_t*)((intptr_t)p_excinf + 14));
	syslog(LOG_ERROR , "r2 = %u") , *((uint32_t*)((intptr_t)p_excinf + 15));
	syslog(LOG_ERROR , "r3 = %u") , *((uint32_t*)((intptr_t)p_excinf + 16));
	syslog(LOG_ERROR , "r4 = %u") , *((uint32_t*)((intptr_t)p_excinf + 17));
	syslog(LOG_ERROR , "r5 = %u") , *((uint32_t*)((intptr_t)p_excinf + 18));
	syslog(LOG_ERROR , "r6 = %u") , *((uint32_t*)((intptr_t)p_excinf + 1));
	syslog(LOG_ERROR , "r7 = %u") , *((uint32_t*)((intptr_t)p_excinf + 2));
	syslog(LOG_ERROR , "r8 = %u") , *((uint32_t*)((intptr_t)p_excinf + 3));
	syslog(LOG_ERROR , "r9 = %u") , *((uint32_t*)((intptr_t)p_excinf + 4));
	syslog(LOG_ERROR , "r10 = %u") , *((uint32_t*)((intptr_t)p_excinf + 5));
	syslog(LOG_ERROR , "r11 = %u") , *((uint32_t*)((intptr_t)p_excinf + 6));
	syslog(LOG_ERROR , "r12 = %u") , *((uint32_t*)((intptr_t)p_excinf + 7));
	syslog(LOG_ERROR , "r13 = %u") , *((uint32_t*)((intptr_t)p_excinf + 8));
	syslog(LOG_ERROR , "r14 = %u") , *((uint32_t*)((intptr_t)p_excinf + 12));
	syslog(LOG_ERROR , "r15 = %u") , *((uint32_t*)((intptr_t)p_excinf + 13));
	syslog(LOG_ERROR , "fpsw = %u") , *((uint32_t*)((intptr_t)p_excinf + 11));
	syslog(LOG_ERROR , "acclo = %u") , *((uint32_t*)((intptr_t)p_excinf + 10));
	syslog(LOG_ERROR , "acchi = %u") , *((uint32_t*)((intptr_t)p_excinf + 9));
	syslog(LOG_ERROR , "pc = %x") , *((uint32_t*)((intptr_t)p_excinf + 19));
	syslog(LOG_ERROR , "psw = %x") , *((uint32_t*)((intptr_t)p_excinf + 20));
}
#endif /* SUPPORT_XLOG_SYS */


/*
 *  割込み要求ラインの属性の設定
 *
 *  ASPカーネルでの利用を想定して，パラメータエラーはアサーションでチェッ
 *  クしている．cfg_intサービスコールを設ける場合には，エラーを返すよう
 *  にすべきであろう．
 */
void
config_int( INTNO intno, ATR intatr, PRI intpri )
{
	uint8_t		irq_offset;
	uint8_t		irq_val;

	assert( VALID_INTNO_CFGINT(intno) );

	assert( ( -15 <= intpri ) && ( intpri <= TMAX_INTPRI ) );
#if TIPM_LOCK != -15
	if( intatr == TA_NONKERNEL ){
		assert( intpri < TMIN_INTPRI );
 	}
	else{
		assert( TMIN_INTPRI <= intpri ); 
	}
#endif

	/*
	 *  割込みのマスク
	 *
	 *  割込みを受け付けたまま，レベルトリガ／エッジトリガの設定や，割
	 *  込み優先度の設定を行うのは危険なため，割込み属性にかかわらず，
	 *  一旦マスクする．
	 */
	( void )disable_int( intno );

	/*
	 *  レベルトリガ/エッジトリガの設定
	 *
	 *  IRQのみサポートする
	 */
	if( INTNO_IRQ( intno ) != 0U ){
		irq_offset = INTNO_IRQ_OFFSET( intno );

		if( ( intatr & TA_POSEDGE ) != 0U ){
			irq_val = IRQ_POSEDGE;
		}
		else if( ( intatr & TA_NEGEDGE ) != 0U ){
			irq_val = IRQ_NEGEDGE;
		}
		else if( ( intatr & TA_BOTHEDGE ) != 0U ){
			irq_val = IRQ_BOTHEDGE;
		}
		else if( ( intatr & TA_LOWLEVEL ) != 0U ){
			irq_val = IRQ_LOWLEVEL;
		}
		else{
			/*
			 *  コンフィギュレータにてチェックしているため,
			 *  ここへくることはありえない.
			 */
			irq_val = 0U;
		}

		sil_wrb_mem(irqcr_reg_addr[irq_offset], irq_val);
	}

	/*
	 *  割込みレベルの設定
	 *
	 *  割込み番号に対応したIPR値を設定する.
	 */
	sil_wrb_mem(ipr_reg_addr[intno], cfg_int_table[intno].intpri);

	/*
	 *  割込みのマスク解除
 	 */
	if( ( intatr & TA_ENAINT ) != 0U ){
		( void )enable_int( intno );
	}
}


#ifndef OMIT_DEFAULT_INT_HANDLER
/*
 *  未登録の割込みが発生した場合に呼び出される
 */
void
default_int_handler( void )
{
	syslog(LOG_EMERG, "Unregistered Interrupt occurs.");
}
#endif /* OMIT_DEFAULT_INT_HANDLER */

#ifndef OMIT_DEFAULT_EXC_HANDLER
/*
 *  未登録の例外が発生した場合に呼び出される
 */
void
default_exc_handler( void )
{
	syslog(LOG_EMERG, "Unregistered Expevt error occurs.");
}
#endif /* OMIT_DEFAULT_EXC_HANDLER */
