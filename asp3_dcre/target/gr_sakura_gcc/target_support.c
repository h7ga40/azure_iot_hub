/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2014 Cores Co., Ltd. Japan
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
 *	ターゲットシステム依存モジュール（GR-SAKURA用）
 */

#include "kernel_impl.h"
#include <sil.h>

void hardware_init_hook()
{
	/*
	 * 動作モード設定
	 *
	 * 動作モードはボード上の端子によって決定されるため,
	 * ここでは特別設定しない.
	 * ただし, リトルエンディアン/シングルチップモード
	 * であることを前提とする.
	 *
	 * またリセット後, 内蔵ROM：有効, 内蔵RAM：有効, 外部バス：無効
	 * である.
	 */

	/*
	 * クロック設定
	 *
	 * 入力：12MHz
	 * PLL回路：12 x 8 = 96MHz
	 * システムクロック：96MHz
	 * 周辺モジュールクロック：48MHz
	 * 外部バスクロック：96MHz
	 * とする.
	 */
	sil_wrb_mem(PORT3_PDR_ADDR,
		sil_reb_mem(PORT3_PDR_ADDR) & ~PORT_PDR_B6_BIT);
	sil_wrb_mem(PORT3_PDR_ADDR,
		sil_reb_mem(PORT3_PDR_ADDR) & ~PORT_PDR_B7_BIT);
	sil_wrb_mem(PORT3_PMR_ADDR,
		sil_reb_mem(PORT3_PMR_ADDR) & ~PORT_PDR_B6_BIT);
	sil_wrb_mem(PORT3_PMR_ADDR,
		sil_reb_mem(PORT3_PMR_ADDR) & ~PORT_PDR_B7_BIT);

	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xa503);

	/* メインクロック発振器強制発振 */
	while((sil_reb_mem(SYSTEM_MOFCR_ADDR) & SYSTEM_MOFCR_MOFXIN) == 0)
		sil_wrb_mem(SYSTEM_MOFCR_ADDR,
			sil_reb_mem(SYSTEM_MOFCR_ADDR) | SYSTEM_MOFCR_MOFXIN);

	/* メインクロック発振器動作 */
	while((sil_reb_mem(SYSTEM_MOSCCR_ADDR) & SYSTEM_MOSCCR_MOSTP) != 0)
		sil_wrb_mem(SYSTEM_MOSCCR_ADDR,
			sil_reb_mem(SYSTEM_MOSCCR_ADDR) & ~SYSTEM_MOSCCR_MOSTP);

	sil_wrh_mem(SYSTEM_PLLCR_ADDR,
		(sil_reh_mem(SYSTEM_PLLCR_ADDR) & ~SYSTEM_PLLCR_STC_MASK)
		| (0x0f << SYSTEM_PLLCR_STC_OFFSET));

	sil_wrb_mem(SYSTEM_PLLCR2_ADDR,
		sil_reb_mem(SYSTEM_PLLCR2_ADDR) & ~SYSTEM_PLLCR2_PLLEN);

	sil_wrw_mem(SYSTEM_SCKCR_ADDR, 0x21021211);

	sil_wrh_mem(SYSTEM_SCKCR2_ADDR, 0x0033);

	/* PLL回路選択 */
	sil_wrh_mem(SYSTEM_SCKCR3_ADDR,
		(sil_reh_mem(SYSTEM_SCKCR3_ADDR) & ~SYSTEM_SCKCR3_CKSEL_MASK)
		| (4 << SYSTEM_SCKCR3_CKSEL_OFFSET));

	/*
	 * モジュールストップ
	 *
	 * リセット後, 動作している内蔵周辺モジュールはDMAC, DTC,
	 * 内蔵RAMのみである.
	 * それ以外の内蔵周辺モジュールに関しては使用する側で
	 * 設定すること.
	 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, 0xa500);
}

void software_init_hook()
{
	/*
	 * 本コンパイラではweak definitionの機能が無いため,
	 * 特に必要な処理はないが, 必ず呼び出すこととする.
	 */
}
