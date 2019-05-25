/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2001-2011 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2007-2016 by Embedded and Real-Time Systems Laboratory
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
 *		FIFO内蔵シリアルコミュニケーションインタフェースに関する定義
 */

#ifndef TOPPERS_SCIF_H
#define TOPPERS_SCIF_H

/*
 *  SCIレジスタのベース番地の定義
 */
#define SCI0_BASE	((uint32_t)(SCI0_SMR_ADDR))
#define SCI1_BASE	((uint32_t)(SCI1_SMR_ADDR))
#define SCI2_BASE	((uint32_t)(SCI2_SMR_ADDR))
#define SCI3_BASE	((uint32_t)(SCI3_SMR_ADDR))
#define SCI4_BASE	((uint32_t)(SCI4_SMR_ADDR))
#define SCI5_BASE	((uint32_t)(SCI5_SMR_ADDR))
#define SCI6_BASE	((uint32_t)(SCI6_SMR_ADDR))
#define SCI7_BASE	((uint32_t)(SCI7_SMR_ADDR))
#define SCI8_BASE	((uint32_t)(SCI8_SMR_ADDR))
#define SCI9_BASE	((uint32_t)(SCI9_SMR_ADDR))
#define SCI10_BASE	((uint32_t)(SCI10_SMR_ADDR))
#define SCI11_BASE	((uint32_t)(SCI11_SMR_ADDR))
/* SCI12は下記のSCI_NUMが使用できない */

/* モジュールストップコントロールレジスタB bit */
#define SCI_NUM(base)	(((intptr_t)base - SCI0_BASE) / 0x20)
#define SCI_MSTPCRB_BIT(base)	(1 << (31 - SCI_NUM(base)))

/* 指定可能なクロックソース */
#define CLK_F1	UINT_C(0x00)
#define CLK_F4	UINT_C(0x01)
#define CLK_F16	UINT_C(0x02)
#define CLK_F64	UINT_C(0x03)

/* シリアルモードレジスタ（SMR） */
#define CKS			UINT_C(0x03)
#define MP			UINT_C(0x04)
#define STOP		UINT_C(0x08)
#define PM			UINT_C(0x10)
#define PE			UINT_C(0x20)
#define CHR			UINT_C(0x40)
#define CM			UINT_C(0x80)
#define ASYNC_7BIT	UINT_C(0x00)
#define ASYNC_8BIT	UINT_C(0x40)

/* シリアルコントロールレジスタ（SCR） */
#define CKE			UINT_C(0x03)
#define TEIE		UINT_C(0x04)
#define RE			UINT_C(0x10)
#define TE			UINT_C(0x20)
#define RIE			UINT_C(0x40)
#define TIE			UINT_C(0x80)

/* シリアルステータスレジスタ（SSR） */
#define TEND		UINT_C(0x04)
#define PER			UINT_C(0x08)
#define FER			UINT_C(0x10)
#define ORER		UINT_C(0x20)

/* スマートカードモードレジスタ（SCMR） */
#define SMIF		UINT_C(0x01)
#define SINV		UINT_C(0x04)
#define SDIR		UINT_C(0x08)
#define BCP2		UINT_C(0x80)

/* シリアル拡張モードレジスタ（SEMR） */
#define ACS0		UINT_C(0x01)
#define ABCS		UINT_C(0x10)
#define NFEN		UINT_C(0x20)

/* I2C モードレジスタ1（SIMR1） */
#define IICM		UINT_C(0x01)

/* I2C モードレジスタ2（SIMR2） */
#define IICINTM		UINT_C(0x01)
#define IICCSC		UINT_C(0x02)
#define IICACKT		UINT_C(0x20)

/* I2C モードレジスタ3（SIMR2） */
#define IICSTAREQ	UINT_C(0x01)
#define IICRSTAREQ	UINT_C(0x02)
#define IICSTPREQ	UINT_C(0x04)
#define IICSTIF		UINT_C(0x08)

/* I2C ステータスレジスタ（SISR） */
#define IICACKR		UINT_C(0x01)

/* SPI モードレジスタ（SPMR） */
#define SSE			UINT_C(0x01)
#define CTSE		UINT_C(0x02)
#define MSS			UINT_C(0x04)
#define CKPOL		UINT_C(0x40)
#define CKPH 		UINT_C(0x80)

#define SCI_SCR_FLG_ENABLE	(RE | TE)
#define SCI_SMR_FLG_ENABLE	(STOP | PM | PE | CHR | CM)

/*
 *  SCIレジスタの番地の定義
 */
#define SCI_SMR(base)		((uint8_t *)((intptr_t)base + 0x00U))
#define SCI_BRR(base)		((uint8_t *)((intptr_t)base + 0x01U))
#define SCI_SCR(base)		((uint8_t *)((intptr_t)base + 0x02U))
#define SCI_TDR(base)		((uint8_t *)((intptr_t)base + 0x03U))
#define SCI_SSR(base)		((uint8_t *)((intptr_t)base + 0x04U))
#define SCI_RDR(base)		((uint8_t *)((intptr_t)base + 0x05U))
#define SCI_SCMR(base)		((uint8_t *)((intptr_t)base + 0x06U))
#define SCI_SEMR(base)		((uint8_t *)((intptr_t)base + 0x07U))
#define SCI_SNFR(base)		((uint8_t *)((intptr_t)base + 0x08U))
#define SCI_SIMR1(base)		((uint8_t *)((intptr_t)base + 0x09U))
#define SCI_SIMR2(base)		((uint8_t *)((intptr_t)base + 0x0AU))
#define SCI_SIMR3(base)		((uint8_t *)((intptr_t)base + 0x0BU))
#define SCI_SISR(base)		((uint8_t *)((intptr_t)base + 0x0CU))
#define SCI_SPMR(base)		((uint8_t *)((intptr_t)base + 0x0DU))

#ifndef TOPPERS_MACRO_ONLY

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_RDY_SND    (1U)        /* 送信可能コールバック */
#define SIO_RDY_RCV    (2U)        /* 受信通知コールバック */

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_SCIF_H */
