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

#include "kernel_impl.h"
#include "time_event.h"
#include <sil.h>
#include "target_timer.h"
#ifdef TOPPERS_SUPPORT_OVRHDR
#include "overrun.h"
#endif /* TOPPERS_SUPPORT_OVRHDR */

/*
 * 上位タイマソフトカウンタ
 */
static uint16_t elapse_upper_timer = 0;

/*
 * 上下位タイマ設定カウンタ
 */
static uint16_t timer_upper_set_count = 0, timer_lower_set_count = 0;

#ifdef TOPPERS_SUPPORT_OVRHDR

/* オーバラン上下位タイマ設定カウンタ */
static uint16_t timer_ovr_upper_set_count = 0, timer_ovr_lower_set_count = 0;

/* オーバランタイマ実行中フラグ */
static bool_t timer_ovr_running_flg = false;

/* オーバランタイマ開始時HRTタイマ値 */
static HRTCNT timer_ovr_hrt_backup;

/* オーバランタイマ設定時間 */
static PRCTIM timer_ovr_ovrtim_backup;

/*
 *  オーバランタイマ割込み要求のクリア
 */
Inline void
target_ovrtimer_int_clear()
{
	clear_int(INTNO_TIMER2);
}

#endif /* TOPPERS_SUPPORT_OVRHDR */

/*
 *  タイマの初期化処理
 */
void
target_timer_initialize(intptr_t exinf)
{
	/*
	 * モジュールストップ機能の設定  CMTユニット0 解除
	 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRA_ADDR,
			sil_rew_mem(SYSTEM_MSTPCRA_ADDR) & ~SYSTEM_MSTPCRA_MSTPA15_BIT);
	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xA500);	/* 書込み禁止 */

	/* 
	 * タイマ停止 
	 */
	sil_wrh_mem(CMT_CMSTR0_ADDR,
			sil_reh_mem(CMT_CMSTR0_ADDR) & ~(CMT_CMSTR0_STR0_BIT | CMT_CMSTR0_STR1_BIT));
	
	/*
 	 * カウントアップに用いられるクロック設定
	 * PCLK/8を選択
	 */
	sil_wrh_mem(CMT0_CMCR_ADDR, CMT_PCLK_DIV_8);
	sil_wrh_mem(CMT1_CMCR_ADDR, CMT_PCLK_DIV_8);

	/* 
	 * コンペアマッチタイマカウンタ設定
	 */
	sil_wrh_mem(CMT0_CMCNT_ADDR, 0U);
	sil_wrh_mem(CMT1_CMCNT_ADDR, 0U);

	/* 
	 * コンペアマッチタイマ周期設定
	 */
	sil_wrh_mem(CMT0_CMCOR_ADDR, 0); /* lower */
	sil_wrh_mem(CMT1_CMCOR_ADDR, CMCOR_PERIOD - 1); /* upper */
	elapse_upper_timer = 0;

	/*
	 * コンペアマッチタイマ割り込み要求先設定レジスタ（28,29）
	 */
	sil_wrb_mem(ICU_ISELR028_ADDR, ICU_ISEL_CPU);
	sil_wrb_mem(ICU_ISELR029_ADDR, ICU_ISEL_CPU);

	/*
	 *  タイマ動作開始前の割込み要求をクリア
	 */
	clear_int(INTNO_TIMER0);
	clear_int(INTNO_TIMER1);

	/* 
	 * コンペアマッチタイマ割り込みを許可
	 */
//	sil_wrh_mem(CMT0_CMCR_ADDR,
//			sil_reh_mem(CMT0_CMCR_ADDR) | CMT0_CMCR_CMIE_BIT); /* lower */
	sil_wrh_mem(CMT1_CMCR_ADDR,
			sil_reh_mem(CMT1_CMCR_ADDR) | CMT1_CMCR_CMIE_BIT); /* upper */

	/* 
	 * タイマ動作開始
	 */
//	sil_wrh_mem(CMT_CMSTR0_ADDR,
//			sil_reh_mem(CMT_CMSTR0_ADDR) | CMT_CMSTR0_STR0_BIT); /* lower */
	sil_wrh_mem(CMT_CMSTR0_ADDR,
			sil_reh_mem(CMT_CMSTR0_ADDR) | CMT_CMSTR0_STR1_BIT); /* upper */
}

/*
 *  タイマの停止処理
 */
void
target_timer_terminate(intptr_t exinf)
{
	/* 
	 * タイマ停止  lower, upper
	 */
	sil_wrh_mem(CMT_CMSTR0_ADDR,
			sil_reh_mem(CMT_CMSTR0_ADDR) & ~(CMT_CMSTR0_STR0_BIT | CMT_CMSTR0_STR1_BIT));

	/*
	 * タイマ割り込み禁止　lower,upper
	 */
	sil_wrh_mem(CMT0_CMCR_ADDR,
			sil_reh_mem(CMT0_CMCR_ADDR) & ~CMT0_CMCR_CMIE_BIT);
	sil_wrh_mem(CMT1_CMCR_ADDR,
			sil_reh_mem(CMT1_CMCR_ADDR) & ~CMT1_CMCR_CMIE_BIT);

	/*
	 *  タイマ割込み要求をクリア
	 */
	clear_int(INTNO_TIMER0);
	clear_int(INTNO_TIMER1);

	/*
	 * モジュールストップ機能の設定  CMTユニット0 停止
	 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRA_ADDR,
			sil_rew_mem(SYSTEM_MSTPCRA_ADDR) | SYSTEM_MSTPCRA_MSTPA15_BIT);
	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xA500);	/* 書込み禁止 */
}

/*
 *  下位タイマ割込みハンドラ(CMT0)
 */
void target_lower_timer_handler(void)
{
	/*
	 * タイマ停止
	 */
	sil_wrh_mem(CMT_CMSTR0_ADDR,
		sil_reh_mem(CMT_CMSTR0_ADDR) & ~CMT_CMSTR0_STR0_BIT);

	/*
	 * タイマ割り込み禁止
	 */
	sil_wrh_mem(CMT0_CMCR_ADDR,
			sil_reh_mem(CMT0_CMCR_ADDR) & ~CMT0_CMCR_CMIE_BIT);

	/*
	 * 割り込み要因クリア
	 */
	clear_int(INTNO_TIMER0);

	/*
	 * カウント設定クリア
	 */
	timer_lower_set_count = 0;

	/*
	 * タイマ割り込み処理
	 */
	signal_time();
}

/*
 *  上位タイマ割込みハンドラ(CMT1)
 */
void target_upper_timer_handler(void)
{
	/*
	 * 上位タイマソフトカウンタ更新
	 */
	elapse_upper_timer++;

	/*
	 * 上位タイマ設定値更新
	 */
	if(timer_upper_set_count > 0) {
		timer_upper_set_count--;
	}

	/*
	 * 上位タイママッチ処理
	 */
	if(timer_upper_set_count == 0){
		if(timer_lower_set_count > 0){
			/*
			 * 下位タイマ設定、開始
			 */
			sil_wrh_mem(CMT0_CMCOR_ADDR, timer_lower_set_count);
			clear_int(INTNO_TIMER0);
			sil_wrh_mem(CMT0_CMCR_ADDR,
					sil_reh_mem(CMT0_CMCR_ADDR) | CMT0_CMCR_CMIE_BIT);
			sil_wrh_mem(CMT_CMSTR0_ADDR,
					sil_reh_mem(CMT_CMSTR0_ADDR) | CMT_CMSTR0_STR0_BIT);
		}
		else{
			/*
			 * 下位タイマ割り込み強制起動
			 */
			target_hrt_raise_event();
		}
	}

#ifdef TOPPERS_SUPPORT_OVRHDR
	if(timer_ovr_running_flg == true) {
		/*
		 * オーバラン上位タイマ設定値更新
		 */
		if(timer_ovr_upper_set_count > 0) {
			timer_ovr_upper_set_count--;
		}

		/*
		 * オーバラン上位タイママッチ処理
		 */
		if(timer_ovr_upper_set_count == 0) {
			if(timer_ovr_lower_set_count > 0){
				/*
				 * オーバラン下位タイマ設定、開始
				 */
				sil_wrh_mem(CMT2_CMCNT_ADDR, 0U);
				sil_wrh_mem(CMT2_CMCOR_ADDR, timer_ovr_lower_set_count);
				target_ovrtimer_int_clear();
				sil_wrh_mem(CMT2_CMCR_ADDR,
						sil_reh_mem(CMT2_CMCR_ADDR) | CMT2_CMCR_CMIE_BIT);
				sil_wrh_mem(CMT_CMSTR1_ADDR,
						sil_reh_mem(CMT_CMSTR1_ADDR) | CMT_CMSTR1_STR2_BIT);
			}
			else {
				/*
				 * オーバラン下位タイマ割り込み強制起動
				 */
				target_ovrtimer_raise_event();
			}
		}
	}
#endif /* TOPPERS_SUPPORT_OVRHDR */
}

/*
 *  高分解能タイマへの割込みタイミングの設定
 */
void
target_hrt_set_event(HRTCNT hrtcnt)
{
	uint32_t current_timer_count_work;
	uint32_t total_timer_count;

	/*
	 * 時間 -> タイマカウンタ 変換
	 */
	total_timer_count = hrtcnt * USEC_CONVERT_VALUE;
	timer_upper_set_count = total_timer_count / CMCOR_PERIOD;
	timer_lower_set_count = total_timer_count % CMCOR_PERIOD;

	/*
	 * 上位タイマカウンタ現在値で調整
	 */
	current_timer_count_work = timer_lower_set_count + sil_reh_mem(CMT1_CMCNT_ADDR);
	if(current_timer_count_work >= CMCOR_PERIOD) {
		timer_upper_set_count++;
		timer_lower_set_count = current_timer_count_work - CMCOR_PERIOD;
	}

	/*
	 * 下位タイマ設定
	 */
	if(timer_upper_set_count == 0 && timer_lower_set_count > 0) {
		/*
		 * コンペアマッチタイマカウンタ設定
		 */
		sil_wrh_mem(CMT0_CMCNT_ADDR, 0U);

		/*
		 * 割り込み要因クリア
		 */
		clear_int(INTNO_TIMER0);

		/*
		 * コンペアマッチタイマ周期設定
		 */
		sil_wrh_mem(CMT0_CMCOR_ADDR, timer_lower_set_count);

		/*
		 *  タイマ割り込み許可
		 */
		sil_wrh_mem(CMT0_CMCR_ADDR,
				sil_reh_mem(CMT0_CMCR_ADDR) | CMT0_CMCR_CMIE_BIT);

		/*
		 * タイマ開始
		 */
		sil_wrh_mem(CMT_CMSTR0_ADDR,
				sil_reh_mem(CMT_CMSTR0_ADDR) | CMT_CMSTR0_STR0_BIT);
	}
}

/*
 *  高分解能タイマ割込みの要求
 */
void
target_hrt_raise_event(void)
{
	/*
	 * 下位タイマ強制割り込み起動
	 * 注意：停止するまで割り込みが繰り返し発生する
	 */
	sil_wrh_mem(CMT0_CMCNT_ADDR, 0U);	/* カウンタ初期化	*/
	clear_int(INTNO_TIMER0);			/* 要求クリア		*/
	sil_wrh_mem(CMT0_CMCOR_ADDR, 0U);	/* マッチ周期設定	*/
	sil_wrh_mem(CMT0_CMCR_ADDR,			/* 割り込み許可	*/
			sil_reh_mem(CMT0_CMCR_ADDR) | CMT0_CMCR_CMIE_BIT);
	sil_wrh_mem(CMT_CMSTR0_ADDR,		/* タイマ起動		*/
			sil_reh_mem(CMT_CMSTR0_ADDR) | CMT_CMSTR0_STR0_BIT);
}

/*
 * 高分解能タイマの現在のカウント値の読出し　内部処理
 */
HRTCNT target_hrt_get_current_convert(void)
{
	uint16_t local_cnt, local_eut;
	HRTCNT time;

	local_eut = elapse_upper_timer;
	local_cnt = sil_reh_mem(CMT1_CMCNT_ADDR);

	/*
	 * 上位タイマ コンペアマッチチェック
	 */
	if (probe_int(INTNO_TIMER1)) {
		if (local_cnt < 0x8000)
			local_eut += 1;
	}

	time = (((HRTCNT)local_eut * CMCOR_PERIOD) + (HRTCNT)local_cnt) / USEC_CONVERT_VALUE;
#ifdef _MSC_VER
	// シミュレーションではタイマーが戻らないよう調整
	if (time < current_hrtcnt)
		time = current_hrtcnt + ((HRTCNT)local_cnt / USEC_CONVERT_VALUE);
#endif
	return time;
}

/*
 *  オーバランタイマドライバ
 */
#ifdef TOPPERS_SUPPORT_OVRHDR

/*
 *  オーバランタイマの初期化処理
 */
void
target_ovrtimer_initialize(intptr_t exinf)
{
	/*
	 * モジュールストップ機能の設定  CMTユニット1 解除
	 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRA_ADDR,
			sil_rew_mem(SYSTEM_MSTPCRA_ADDR) & ~SYSTEM_MSTPCRA_MSTPA14_BIT);
	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xA500);	/* 書込み禁止 */

	/*
	 * タイマ停止 CMT2
	 */
	sil_wrh_mem(CMT_CMSTR1_ADDR,
			sil_reh_mem(CMT_CMSTR1_ADDR) & ~CMT_CMSTR1_STR2_BIT);

	/*
 	 * カウントアップに用いられるクロック設定 CMT2
	 * PCLK/8を選択
	 */
	sil_wrh_mem(CMT2_CMCR_ADDR, CMT_PCLK_DIV_8);

	/*
	 * コンペアマッチタイマカウンタ設定 CMT2
	 */
	sil_wrh_mem(CMT2_CMCNT_ADDR, 0U);

	/*
	 * コンペアマッチタイマ周期設定 CMT2
	 */
	sil_wrh_mem(CMT2_CMCOR_ADDR, 0U);

	/*
	 * コンペアマッチタイマ割り込み要求先設定レジスタ（30）
	 */
	sil_wrb_mem(ICU_ISELR030_ADDR, ICU_ISEL_CPU);

	/*
	 *  タイマ動作開始前の割込み要求をクリア
	 */
	target_ovrtimer_int_clear();
}

void
target_ovrtimer_start(PRCTIM ovrtim)
{
	uint32_t current_timer_count_work;
	uint32_t total_timer_count;

	/*
	 * 時間 -> タイマカウンタ 変換
	 */
	total_timer_count = ovrtim * USEC_CONVERT_VALUE;
	timer_ovr_upper_set_count = total_timer_count / CMCOR_PERIOD;
	timer_ovr_lower_set_count = total_timer_count % CMCOR_PERIOD;

	/*
	 * 高分解能上位タイマカウンタ現在値で調整
	 */
	timer_ovr_ovrtim_backup = ovrtim;
	timer_ovr_hrt_backup = target_hrt_get_current();
	current_timer_count_work = timer_ovr_lower_set_count + sil_reh_mem(CMT1_CMCNT_ADDR);
	if(current_timer_count_work >= CMCOR_PERIOD) {
		timer_ovr_upper_set_count++;
		timer_ovr_lower_set_count = current_timer_count_work - CMCOR_PERIOD;
	}

	/*
	 * オーバランタイマ動作中フラグ
	 */
	timer_ovr_running_flg = true;

	/*
	 * 下位タイマ設定
	 */
	if(timer_ovr_upper_set_count == 0) {
		if(timer_ovr_lower_set_count == 0) {
			target_ovrtimer_raise_event();
		}
		else {
			/*
			 * コンペアマッチタイマカウンタクリア CMT2
			 */
			sil_wrh_mem(CMT2_CMCNT_ADDR, 0U);

			/*
			 * コンペアマッチタイマ周期設定 CMT2
			 */
			sil_wrh_mem(CMT2_CMCOR_ADDR, timer_ovr_lower_set_count);

			/*
			 *  タイマ動作開始前の割込み要求をクリア
			 */
			target_ovrtimer_int_clear();

			/*
			 * コンペアマッチタイマ割り込みを許可 CMT2
			 */
			sil_wrh_mem(CMT2_CMCR_ADDR,
					sil_reh_mem(CMT2_CMCR_ADDR) | CMT2_CMCR_CMIE_BIT);

			/*
			 * タイマ動作開始 CMT2
			 */
			sil_wrh_mem(CMT_CMSTR1_ADDR,
					sil_reh_mem(CMT_CMSTR1_ADDR) | CMT_CMSTR1_STR2_BIT);
		}
	}
}

/*
 *  オーバランタイマの停止処理
 */
void
target_ovrtimer_terminate(intptr_t exinf)
{
	/*
	 * タイマ停止
	 */
	sil_wrh_mem(CMT_CMSTR1_ADDR,
			sil_reh_mem(CMT_CMSTR1_ADDR) & ~CMT_CMSTR1_STR2_BIT);

	/*
	 * タイマ割り込み禁止
	 */
	sil_wrh_mem(CMT2_CMCR_ADDR,
			sil_reh_mem(CMT2_CMCR_ADDR) & ~CMT2_CMCR_CMIE_BIT);

	/*
	 *  タイマ割込み要求をクリア
	 */
	target_ovrtimer_int_clear();

	/*
	 * オーバランタイマ動作中フラグ
	 */
	timer_ovr_running_flg = false;

	/*
	 * モジュールストップ機能の設定  CMTユニット0 停止
	 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRA_ADDR,
			sil_rew_mem(SYSTEM_MSTPCRA_ADDR) | SYSTEM_MSTPCRA_MSTPA14_BIT);
	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xA500);	/* 書込み禁止 */
}

/*
 *  オーバランタイマの停止
 */
PRCTIM
target_ovrtimer_stop(uint_t int_num)
{
	uint32_t	cnt;

	/*
	 * タイマ停止
	 */
	sil_wrh_mem(CMT_CMSTR1_ADDR,
			sil_reh_mem(CMT_CMSTR1_ADDR) & ~CMT_CMSTR1_STR2_BIT);

	/*
	 * オーバランタイマ動作中フラグ
	 */
	timer_ovr_running_flg = false;

	if(int_num == INTNO_TIMER2) {
		/*
		 *  オーバラン割込みの場合
		 */
		target_ovrtimer_int_clear();
		return(0U);
	}
	else {
		HRTCNT timer_ovr_hrt_current = target_hrt_get_current();
		if((timer_ovr_hrt_current < timer_ovr_hrt_backup) && (int_num == INTNO_OVR_BASE_TIMER)) {
			timer_ovr_hrt_current += (CMCOR_PERIOD / USEC_CONVERT_VALUE);
		}

		cnt = timer_ovr_ovrtim_backup - (timer_ovr_hrt_current - timer_ovr_hrt_backup);
		if(timer_ovr_hrt_current < timer_ovr_hrt_backup) {
			cnt += TCYC_HRTCNT;
		}

		if(cnt > timer_ovr_ovrtim_backup) { /* 設定時間を過ぎた */
			cnt = 0;
		}
		return (PRCTIM)cnt;
	}
}

/*
 *  オーバランタイマの現在値の読出し
 */
PRCTIM
target_ovrtimer_get_current(void)
{
	uint32_t	cnt;

	if (probe_int(INTNO_TIMER2)) {
		/*
		 *  割込み要求が発生している場合
		 */
		return(0U);
	}
	else {
		HRTCNT timer_ovr_hrt_current = target_hrt_get_current();
		cnt = timer_ovr_ovrtim_backup - (timer_ovr_hrt_current - timer_ovr_hrt_backup);
		if(timer_ovr_hrt_current < timer_ovr_hrt_backup) {
			cnt += TCYC_HRTCNT;
		}

		if(cnt > timer_ovr_ovrtim_backup) { /* 設定時間を過ぎた */
			cnt = 0;
		}
		return (PRCTIM)cnt;
	}
}

/*
 *  高分解能タイマ割込みの要求
 */
void
target_ovrtimer_raise_event(void)
{
	/*
	 * オーバラン下位タイマ強制割り込み起動
	 * 注意：停止するまで割り込みが繰り返し発生する
	 */
	sil_wrh_mem(CMT2_CMCNT_ADDR, 0U);	/* カウンタ初期化	*/
	clear_int(INTNO_TIMER2);			/* 要求クリア		*/
	sil_wrh_mem(CMT2_CMCOR_ADDR, 0U);	/* マッチ周期設定	*/
	sil_wrh_mem(CMT2_CMCR_ADDR,			/* 割り込み許可	*/
			sil_reh_mem(CMT2_CMCR_ADDR) | CMT2_CMCR_CMIE_BIT);
	sil_wrh_mem(CMT_CMSTR1_ADDR,		/* タイマ起動		*/
			sil_reh_mem(CMT_CMSTR1_ADDR) | CMT_CMSTR1_STR2_BIT);
}

/*
 *  オーバランタイマ割込みハンドラ
 *
 *  このルーチンに来るまでに，target_ovrtimer_stopが呼ばれているため，
 *  OSタイマを停止する必要はない．
 */
void
target_ovrtimer_handler(void)
{
	/*
	 * タイマ割り込み禁止
	 */
	sil_wrh_mem(CMT2_CMCR_ADDR,
			sil_reh_mem(CMT2_CMCR_ADDR) & ~CMT2_CMCR_CMIE_BIT);

	/*
	 * 上下位タイマカウンタクリア
	 */
	timer_ovr_upper_set_count = 0;
	timer_ovr_lower_set_count = 0;

	call_ovrhdr();	/* オーバランハンドラの起動処理 */
}

#endif /* TOPPERS_SUPPORT_OVRHDR */
