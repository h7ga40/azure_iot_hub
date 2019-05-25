/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2006-2016 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  $Id$
 */

/*
 *		FIFO内蔵シリアルコミュニケーションインタフェース用 簡易SIOドライバ
 */

#include <sil.h>
#include "tSCIF_tecsgen.h"
#include "scif.h"
#include "target_kernel_impl.h"

/*
 *  プリミティブな送信／受信関数
 */

/*
 *  受信バッファに文字があるか？
 */
Inline bool_t
scif_getready(CELLCB *p_cellcb)
{
	/*
	 *  受信フラグがONのときのみ受信バッファから文字を取得する.
	 *  これは, ポーリング受信に対応するためである.
	 *  しかし, RX600シリーズでは受信フラグがないこと, システムサービス
	 *  では受信割込みの中からしかデータを受信しに来ないことから, 常に
	 *  受信バッファから文字を取得する.
	 */
	return true;
}

/*
 *  送信バッファに空きがあるか？
 */
Inline bool_t
scif_putready(CELLCB *p_cellcb)
{
	if((sil_reb_mem(SCI_SSR(ATTR_baseAddress)) & SCI_SSR_TEND_BIT) != 0){
		return(true);
	}
	return(false);
}

/*
 *  送信する文字の書込み
 */
Inline void
scif_putchar(CELLCB *p_cellcb, char c)
{
	sil_wrb_mem(SCI_TDR(ATTR_baseAddress), c);
}

/*
 *  シリアルI/Oポートのオープン
 */
void
eSIOPort_open(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);
	uint_t	cnt, brr, clk;
	volatile uint8_t i;

	if (VAR_initialized) {
		/*
		 *  既に初期化している場合は、二重に初期化しない．
		 */
		return;
	}

	cnt = (FREQ_PCLK * 1000) / (16 * ATTR_baudRate);
	if (cnt < 512)
		clk = 0;
	else if (cnt < 4 * 512)
		clk = 1;
	else if (cnt < 16 * 512)
		clk = 2;
	else if (cnt < 64 * 512)
		clk = 3;
	else
		return;

	brr = cnt / (1 << (2 * clk));
	brr = (brr / 2) + (brr % 2) - 1;

	/*
	 *  SCIドライバの初期化ルーチン
	 */

	/*
	 *  割り込み要求先レジスタの設定(ISELRi)
	 *
	 *  リセット値と同じ値を設定することになるため,
	 *  処理は省略する.
	 */

	/*
	 *  モジュールストップ機能の設定
	 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRB_ADDR,
		sil_rew_mem(SYSTEM_MSTPCRB_ADDR) & ~SCI_MSTPCRB_BIT(ATTR_baseAddress));
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA500);	/* 書込み禁止 */

	/* 送受信禁止, SCKn端子は入出力ポートとして使用 */
	sil_wrb_mem(SCI_SCR(ATTR_baseAddress), 0x00U);

	/* クロック選択ビット(SMR.CKS[1:0]ビットを設定) */
	sil_wrb_mem(SCI_SMR(ATTR_baseAddress),
		(sil_reb_mem(SCI_SMR(ATTR_baseAddress)) & ~0x03) | clk);

	/* SMRに送信／ 受信フォーマットを設定) */
	sil_wrb_mem(SCI_SMR(ATTR_baseAddress),
			sil_reb_mem(SCI_SMR(ATTR_baseAddress)) & (~SCI_SMR_FLG_ENABLE));

	/* ビットレートを設定 */
	sil_wrb_mem(SCI_BRR(ATTR_baseAddress), brr);

	/* ビット期間(基本クロック16サイクルの期間が1ビット期間となる) */
	for(i = 0; i < 16; i++) { }

	/* 送受信許可 */
	sil_wrb_mem(SCI_SCR(ATTR_baseAddress),
			(sil_reb_mem(SCI_SCR(ATTR_baseAddress)) | SCI_SCR_FLG_ENABLE));

	VAR_initialized = true;
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
eSIOPort_close(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	/*
	 *  UART停止
	 */
	sil_wrb_mem(SCI_SCR(ATTR_baseAddress), 0x00U);
}

/*
 *  シリアルI/Oポートへの文字送信
 */
bool_t
eSIOPort_putChar(CELLIDX idx, char c)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (scif_putready(p_cellcb)){
		scif_putchar(p_cellcb, c);
		return(true);
	}
	return(false);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
int_t
eSIOPort_getChar(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);
	uint8_t reg;
	int_t c = -1;

	/* オーバランエラーチェック */ /* ZZZ */
	reg = sil_reb_mem(SCI_SSR(ATTR_baseAddress));
	while((reg & SCI_SSR_ORER_BIT) != 0) {
		sil_wrb_mem(SCI_SSR(ATTR_baseAddress), reg & ~SCI_SSR_ORER_BIT);
		reg = sil_reb_mem(SCI_SSR(ATTR_baseAddress));
	}

	/*
	 *  受信フラグがONのときのみ受信バッファから文字を取得する.
	 *  これは, ポーリング受信に対応するためである.
	 *  しかし, RX600シリーズでは受信フラグがないこと, システムサービス
	 *  では受信割込みの中からしかデータを受信しに来ないことから, 常に
	 *  受信バッファから文字を取得する.
	 */
	c = (int)(sil_reb_mem(SCI_RDR(ATTR_baseAddress)));

	return c;
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
eSIOPort_enableCBR(CELLIDX idx, uint_t cbrtn)
{
	CELLCB		*p_cellcb = GET_CELLCB(idx);

	switch (cbrtn) {
		case SIO_RDY_SND:
			sil_wrb_mem(SCI_SCR(ATTR_baseAddress),
				(sil_reb_mem(SCI_SCR(ATTR_baseAddress)) | SCI_SCR_TEIE_BIT));
			break;
		case SIO_RDY_RCV:
			sil_wrb_mem(SCI_SCR(ATTR_baseAddress),
				(sil_reb_mem(SCI_SCR(ATTR_baseAddress)) | SCI_SCR_RIE_BIT));
			break;
		default:
			assert(1);
			break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
eSIOPort_disableCBR(CELLIDX idx, uint_t cbrtn)
{
	CELLCB		*p_cellcb = GET_CELLCB(idx);

	switch (cbrtn) {
		case SIO_RDY_SND:
			sil_wrb_mem(SCI_SCR(ATTR_baseAddress),
				(sil_reb_mem(SCI_SCR(ATTR_baseAddress)) & (~SCI_SCR_TEIE_BIT)));
			break;
		case SIO_RDY_RCV:
			sil_wrb_mem(SCI_SCR(ATTR_baseAddress),
				(sil_reb_mem(SCI_SCR(ATTR_baseAddress)) & (~SCI_SCR_RIE_BIT)));
			break;
		default:
			assert(1);
			break;
	}
}

/*
 *  シリアルI/Oポートに対する受信割込み処理
 */
void
eiRxISR_main(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (scif_getready(p_cellcb)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		ciSIOCBR_readyReceive();
	}
}

/*
 *  シリアルI/Oポートに対する送信割込み処理
 */
void
eiTxISR_main(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (scif_putready(p_cellcb)) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		ciSIOCBR_readySend();
	}
}
