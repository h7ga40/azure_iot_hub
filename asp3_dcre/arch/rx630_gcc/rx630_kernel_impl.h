/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2008-2010 by Witz Corporation, JAPAN
 *  Copyright (C) 2013      by Mitsuhiro Matsuura
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
 *
 *  このインクルードファイルは，target_kernel_impl.h（または，そこからインク
 *  ルードされるファイル）のみからインクルードされる．他のファイルから
 *  直接インクルードしてはならない．
 */


#ifndef TOPPERS_RX630_CONFIG_H
#define TOPPERS_RX630_CONFIG_H


/*
 *  割込み要因数
 */
#define	INHNO_MAX	UINT_C( 256 )

#ifndef TOPPERS_MACRO_ONLY

/*
 *  割込み制御用型定義
 */
typedef struct ier_info {
	volatile uint8_t __evenaccess	*addr;
			 uint8_t				offset;
} IER_INFO;

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  割込み制御用定義
 */
#define	INVALID_OFFSET				( 0xFFU )
#define	INTNO_IRQ( intno )			( ( 64U <= ( intno ) ) && ( ( intno ) <= 79U ) )
#define	INTNO_IRQ_OFFSET( intno )	( ( intno ) - 64U )
#define	IRQ_MAX						UINT_C( 16 )

#define IR_TOP						(0x00087010)
#define IR_REG( intno )				((volatile uint8_t __evenaccess * )(IR_TOP + (intno-16)))

#ifndef TOPPERS_MACRO_ONLY

/*
 *  割込み制御用テーブル
 */

/* 割込み要因プライオリティレジスタアドレス */
extern volatile uint8_t __evenaccess * const ipr_reg_addr[];
/* 割込み要求許可レジスタアドレス */
extern const IER_INFO ier_reg_addr[];
/* 割込み要求レジスタアドレス */
extern volatile uint8_t __evenaccess * const ir_reg_addr[];
/* IRQコントロールレジスタアドレス */
extern volatile uint8_t __evenaccess * const irqcr_reg_addr[];

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  プロセッサ依存モジュール（RX630用）
 */
#include "prc_kernel_impl.h"

#endif	/* TOPPERS_RX630_CONFIG_H */

