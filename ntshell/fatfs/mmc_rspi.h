/*
 *  TOPPERS PROJECT Home Network Working Group Software
 * 
 *  Copyright (C) 2016-2019 Cores Co., Ltd. Japan
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

#ifndef _MMC_RSPI_H_
#define _MMC_RSPI_H_

#include "t_stddef.h"
#include "diskio.h"

#define INHNO_MMC_RSPI_SPRI		INT_RSPI0_SPRI0	/* 受信割り込み */
#define INTNO_MMC_RSPI_SPRI		INT_RSPI0_SPRI0	/* 受信割り込み */
#define INTATR_MMC_RSPI_SPRI	(TA_NULL)		/* 割込み属性	*/
#define INTPRI_MMC_RSPI_SPRI	(-1)			/* 割込み優先度	*/

#define INHNO_MMC_RSPI_SPTI		INT_RSPI0_SPTI0	/* 送信割り込み */
#define INTNO_MMC_RSPI_SPTI		INT_RSPI0_SPTI0	/* 送信割り込み */
#define INTATR_MMC_RSPI_SPTI	(TA_NULL)		/* 割込み属性	*/
#define INTPRI_MMC_RSPI_SPTI	(-1)			/* 割込み優先度	*/

#define INHNO_MMC_RSPI_SPII		INT_RSPI0_SPII0	/* アイドル割り込み */
#define INTNO_MMC_RSPI_SPII		INT_RSPI0_SPII0	/* アイドル割り込み */
#define INTATR_MMC_RSPI_SPII	(TA_NULL)		/* 割込み属性	*/
#define INTPRI_MMC_RSPI_SPII	(-1)			/* 割込み優先度	*/

DSTATUS mmc_rspi_initialize ();
DSTATUS mmc_rspi_status ();
DRESULT mmc_rspi_read (BYTE* buff, DWORD sector, UINT count);
#if	_USE_WRITE
DRESULT mmc_rspi_write (const BYTE* buff, DWORD sector, UINT count);
#endif
#if	_USE_IOCTL
DRESULT mmc_rspi_ioctl (BYTE cmd, void* buff);
#endif

void mmc_rspi_spri_handler(void);
void mmc_rspi_spti_handler(void);
void mmc_rspi_spii_handler(void);
void mmc_rspi_cychdr(intptr_t exinf);
void mmc_rspi_status_changed(DSTATUS newst);

#endif /* _MMC_RSPI_H_ */
