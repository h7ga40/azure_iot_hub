;*
;*  TOPPERS/ASP Kernel
;*      Toyohashi Open Platform for Embedded Real-Time Systems/
;*      Advanced Standard Profile Kernel
;* 
;*  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
;*                              Toyohashi Univ. of Technology, JAPAN
;*  Copyright (C) 2005,2006 by Embedded and Real-Time Systems Laboratory
;*              Graduate School of Information Science, Nagoya Univ., JAPAN
;*  Copyright (C) 2008-2010 by Witz Corporation, JAPAN
;* 
;*  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
;*  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
;*  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
;*  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
;*      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
;*      スコード中に含まれていること．
;*  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
;*      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
;*      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
;*      の無保証規定を掲載すること．
;*  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
;*      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
;*      と．
;*    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
;*        作権表示，この利用条件および下記の無保証規定を掲載すること．
;*    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
;*        報告すること．
;*  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
;*      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
;*      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
;*      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
;*      免責すること．
;* 
;*  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
;*  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
;*  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
;*  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
;*  の責任を負わない．
;* 
;*  @(#) $Id$
;* 

#define TOPPERS_MACRO_ONLY
#define UINT_C(val)			(val)		/* uint_t型の定数を作るマクロ */
#define ULONG_C(val)		(val)		/* ulong_t型の定数を作るマクロ */
#define CAST(type, val)		(val)		/* 型キャストを行うマクロ */
#include "kernel_impl.h"
#include "sectrx.inc"

	;
	;  メモリマップの定義(ターゲット依存部)
	;

.equ	DFLRE,				007FC440H		; DFLREレジスタ

.equ	DB_BLOCK_ENABLE,	02D0FH

;
;  初期化ルーチン
;
	.section	P, CODE

	.global	_start
	.global	_sta_ker
	.global	_software_init_hook
	.global	_hardware_init_hook
	.global	__kernel_istkpt
	.global	_vvector_table

_start:
	;
	;  電源投入後はレベル0, 全割込み禁止状態である.
	;  カーネルでは高速割込みを使用しないため、BPC, BPSW及びFINTV
	;  の初期化は行わない.
	;

	;
	;  内蔵ROM（データフラッシュ）領域の設定
	;
	;  リセット後, データフラッシュ領域は読み出し禁止となっている
	;  ため, 読み出し許可に設定する.
	;
	mov.l	#DFLRE, r5
	mov.w	#DB_BLOCK_ENABLE, [R5]
	
	;
	;  スタックポインタの設定
	;
	;  ユーザにて非タスクコンテキスト用スタック領域を用意した場合,
	;  ユーザ領域を設定する.
	;  kernel_cfg.cにて"_kernel_istkpt"の値を決定する.
	;
	mov.l	#__kernel_istkpt, r5
	mov.l	[r5], r0			; 非タスクコンテキスト用スタック領域を設定

	;
	;  可変ベクタテーブルの設定
	;
	; mvtc	#VVECTOR_TOP, intb	; ベクタテーブルの設定
	mvtc	#_vvector_table, intb	; ベクタテーブルの設定

	;
	;  ハードウェア固有の初期化ルーチンの呼出し
	;
	mov.l	#_hardware_init_hook, r5
	cmp		#0, r5
	beq.b	init_D
	jsr		r5

	;
	;  初期値あり変数領域の初期化
	;
init_D:
	mov.l	#(_edata - _data), r3
	cmp		#0, r3
	beq.b	clear_B						; サイズ0なら初期化必要なし
	mov.l	#(_data), r1
	mov.l	#(_mdata), r2
	smovf

	;
	;  初期値なし変数領域の初期化
	;
clear_B:
	mov.l	#0, r2
	mov.l	#(_bss), r1
	mov.l	#(_ebss - _bss) , r3
	sstr.b

	;
	;  software_init_hook を呼出し（0 でない場合）
	;
	;  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処理
	;  がある場合は，software_init_hook という関数を用意すればよい.
	;
	mov.l	#_software_init_hook, r5
	cmp		#0, r5
	beq.b	kernel_link
	jsr		r5

	;
	;  カーネルスタート
	;
kernel_link:
	bra.a	__sta_ker					; --> __sta_ker()

	.end

