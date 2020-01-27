/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
 *  Copyright (C) 2003-2004 by Platform Development Center
 *                                          RICOH COMPANY,LTD. JAPAN
 *  Copyright (C) 2008-2010 by Witz Corporation, JAPAN
 *  Copyright (C) 2013      by Mitsuhiro Matsuura
 *  Copyright (C) 2017      by Cores Co., Ltd. Japan
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
 *		タイマドライバ（GR-SAKURA用）
 */
 
#ifndef TOPPERS_TARGET_TIMER_H
#define TOPPERS_TARGET_TIMER_H

#include "gr_sakura.h"
#include "rx630_kernel_impl.h"
#include "target_syssvc.h"

/*
 *  タイマ割込みハンドラ登録のための定数
 */
#define INHNO_TIMER0	INT_CMT0_CMI				/* 割込みハンドラ番号 */
#define INTNO_TIMER0	INT_CMT0_CMI				/* 割込み番号 */
#define INTPRI_TIMER0	-5							/* 割込み優先度 */
#define INTATR_TIMER0	(TA_ENAINT | TA_EDGE)		/* 割込み属性 */

#define INHNO_TIMER1	INT_CMT1_CMI				/* 割込みハンドラ番号 */
#define INTNO_TIMER1	INT_CMT1_CMI				/* 割込み番号 */
#define INTPRI_TIMER1	-5							/* 割込み優先度 */
#define INTATR_TIMER1	(TA_ENAINT | TA_EDGE)		/* 割込み属性 */

/*
 *  オーバランタイマ割込みハンドラ登録のための定数
 */
#define INHNO_TIMER2	INT_CMT2_CMI				/* 割込みハンドラ番号 */
#define INTNO_TIMER2	INT_CMT2_CMI				/* 割込み番号 */
#define INTPRI_TIMER2	-5							/* 割込み優先度 */
#define INTATR_TIMER2	(TA_ENAINT | TA_EDGE)		/* 割込み属性 */
#define INTNO_OVR_BASE_TIMER	INTNO_TIMER1		/* 基本タイマに使うタイマ割込み番号 */
#define INTNO_DUMMY_TIMER		255					/* ダミータイマ割込み番号 */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  タイマ値の内部表現の型
 */
typedef uint16_t	CLOCK;

/*
 *  タイマ値の内部表現とミリ秒単位との変換
 *
 *  1msのタイマー割り込みを使用するのでそのクロックを定義する．
 */
#define TIMER_CLOCK				(FREQ_PCLK / 8U)		/* PCLK/最低分周比8 = 6MHz */
#define TO_CLOCK(nume, deno)	((CLOCK)(TIMER_CLOCK * (nume) / (deno)))
#define	TO_USEC(clock)			((SYSUTM)((clock) * 1000U / TIMER_CLOCK))

/*
 *  コンペアマッチタイマコンスタントレジスタ（CMCOR）
 *
 *  コンペアマッチ周期
 *  周辺モジュールクロック（PCLK）48MHzを8分周するので，
 *  6カウント = 1us : max 10922.667us
 */
#define CMCOR_PERIOD	(0x10000U)

/*
 * カウント < - > マイクロ秒 変換値
 * 48MHz / 8 / 1000
 */
#define USEC_CONVERT_VALUE	6U

/*
 *  高分解能タイマのタイマ周期
 *
 *  2^32 / 6
 */
#define TCYC_HRTCNT		715827882U	/* (0x100000000 / 6) = 715827882.667usec */

/*
 *  高分解能タイマのカウント値の進み幅
 */
#define TSTEP_HRTCNT	1U

/*
 *  性能評価用システム時刻を取り出す際の処理時間の見積り値（単位は内部表現）
 */
#define	GET_TOLERANCE	(100U)	/* 処理遅れの見積り値（単位は内部表現）*/
#define	BEFORE_IREQ(clock) \
		((clock) >= (TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE))

/*
 *  コンペアマッチタイマコントロールレジスタの設定値
 */
/*
 *  クロック選択ビット
 *
 *  bit7は予約領域であり, 書込みは1固定
 */
#define CMT_PCLK_DIV_8			(0x0080U)
#define CMT_PCLK_DIV_32			(0x0081U)
#define CMT_PCLK_DIV_128		(0x0082U)
#define CMT_PCLK_DIV_512		(0x0083U)

/*
 * コンペアマッチタイマ割り込み要求先設定レジスタi（ISELRi）（i= 割り込みベクタ番号）
 */
#define ICU_ISEL_CPU			(0x00U)

/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
extern void	target_timer_initialize(intptr_t exinf);

/*
 *  タイマの停止処理
 *
 *  タイマの動作を停止させる．
 */
extern void	target_timer_terminate(intptr_t exinf);

/*
 *  タイマレジスタ値の読み出し
 */
Inline uint16_t rx630_cmt_getcount(void)
{
	return sil_reh_mem((uint16_t *)CMT0_CMCNT_ADDR);
}

/*
 *  タイマレジスタ値の書き込み
 */
Inline void rx630_cmt_setcount(uint16_t count)
{
	sil_wrh_mem(CMT0_CMCNT_ADDR, count);
}

/*
 *  タイマの現在値の読出し
 */
Inline CLOCK
target_timer_get_current(void)
{
	/* アップカウントのためタイマレジスタ値をそのまま返す	*/
	return (rx630_cmt_getcount());
}

/*
 *  タイマ割込み要求のチェック
 */
Inline bool_t
target_timer_probe_int(void)
{
	return(probe_int(INTNO_TIMER0));
}

/*
 *  タイマ割込みハンドラ
 */
extern void	target_lower_timer_handler(void);
extern void	target_upper_timer_handler(void);

/*
 *  割込みタイミングに指定する最大値
 */
#define HRTCNT_BOUND	100000002U

/*
 *  高分解能タイマへの割込みタイミングの設定
 *
 *  高分解能タイマを，hrtcntで指定した値カウントアップしたら割込みを発
 *  生させるように設定する．
 */
extern void target_hrt_set_event(HRTCNT hrtcnt);

/*
 *  高分解能タイマ割込みの要求
 */
extern  void target_hrt_raise_event(void);

/*
 *  高分解能タイマの現在のカウント値の読出し　内部処理
 */
extern HRTCNT target_hrt_get_current_convert(void);

/*
 *  高分解能タイマの現在のカウント値の読出し
 */
Inline HRTCNT
target_hrt_get_current(void)
{
	return (target_hrt_get_current_convert());
}

/*
 * 高分解能タイマの計算のためsignal_timeを呼び出す
 */
#define TOPPERS_CALL_SIGNAL_TIME_FOR_CALC

#ifdef TOPPERS_SUPPORT_OVRHDR

/*
 *  オーバランタイマの初期化処理
 */
extern void target_ovrtimer_initialize(intptr_t exinf);

/*
 *  オーバランタイマの停止処理
 */
extern void target_ovrtimer_terminate(intptr_t exinf);

/*
 *  オーバランタイマの動作開始
 */
extern void target_ovrtimer_start(PRCTIM ovrtim);

/*
 *  オーバランタイマの停止
 */
extern PRCTIM target_ovrtimer_stop(uint_t int_num);

/*
 *  オーバランタイマの現在値の読出し
 */
extern PRCTIM target_ovrtimer_get_current(void);

/*
 *  オーバランタイマの割込みの要求
 */
extern  void target_ovrtimer_raise_event(void);

/*
 *  オーバランタイマ割込みハンドラ
 */
extern void	target_ovrtimer_handler(void);

#endif /* TOPPERS_SUPPORT_OVRHDR */

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_TARGET_TIMER_H */
