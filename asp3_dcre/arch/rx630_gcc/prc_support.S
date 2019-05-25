; 
;   TOPPERS/ASP Kernel
;       Toyohashi Open Platform for Embedded Real-Time Systems/
;       Advanced Standard Profile Kernel
;  
;   Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
;                               Toyohashi Univ. of Technology, JAPAN
;   Copyright (C) 2007      by Embedded and Real-Time Systems Laboratory
;               Graduate School of Information Science, Nagoya Univ., JAPAN
;   Copyright (C) 2010 by Witz Corporation, JAPAN
;   Copyright (C) 2013      by Mitsuhiro Matsuura
;   Copyright (C) 2017      by Cores Co., Ltd. Japan
;  
;   上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
;   ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
;   変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
;   (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
;       権表示，この利用条件および下記の無保証規定が，そのままの形でソー
;       スコード中に含まれていること．
;   (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
;       用できる形で再配布する場合には，再配布に伴うドキュメント（利用
;       者マニュアルなど）に，上記の著作権表示，この利用条件および下記
;       の無保証規定を掲載すること．
;   (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
;       用できない形で再配布する場合には，次のいずれかの条件を満たすこ
;       と．
;     (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
;         作権表示，この利用条件および下記の無保証規定を掲載すること．
;     (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
;         報告すること．
;   (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
;       害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
;       また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
;       由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
;       免責すること．
;  
;   本ソフトウェアは，無保証で提供されているものである．上記著作権者お
;   よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
;   に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
;   アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
;   の責任を負わない．
;  
;  @(#) $Id$
;

;
;  プロセッサ依存モジュール アセンブリ言語部（RX630用）
;
#define TOPPERS_MACRO_ONLY
#define UINT_C(val)			(val)		/* uint_t型の定数を作るマクロ */
#define ULONG_C(val)		(val)		/* ulong_t型の定数を作るマクロ */
#define CAST(type, val)		(val)		/* 型キャストを行うマクロ */
#include "kernel_impl.h"

;
;  ディスパッチャおよび割込み(CPU例外)出入り口のルール:
;	動作モードを以下のように定義する.
;		ディスパッチャモード:
;			CPUロック状態, 割込み優先度マスク全解除状態,
;			タスクコンテキスト(intnest = 0), タスクスタック
;		割込み(CPU例外)処理モード
;			全割込みロック状態(PSWレジスタIビット = 0),
;			割込み優先度マスク全解除でない状態(IPL != 0)
;			割込みコンテキスト(intnest != 0), 割込みスタック
;
;	カーネル管理外割込みのサポート有無と, CPUロック状態, 割込み優先度
;	マスク全解除状態の関係は以下の通りである.
;		カーネル管理外割込み未サポート時:
;			CPUロック状態(PSWレジスタIビット = 0)
;			割込み優先度マスク全解除状態(IPL = 0)
;		カーネル管理外割込みサポート時:
;			CPUロック状態
;				(PSWレジスタIビット = 0, IPL = IPL_LOCK, lock_flag = true)
;			割込み優先度マスク全解除状態(saved_ipl = 0)
;
;	各構造体ポインタを以下のように各レジスタにアサインする.
;		r15 = p_runtsk	ただしディスパッチャの各出口では無効
;		r14 = *p_runtsk	dispatcher の中では p_runtsk 確定時に再取得する
;	各入り口から最初に変数アクセスするときに上記レジスタに保存する.
;
;	構造体アライメントへの対応
;		構造体アライメントが4Byte(アンパック)の場合:
;			一般的なレジスタ相対アドレッシングが可能
;				例:	mov.l	#__kernel_p_runtsk, r15
;					mov.l  	r0, TCB_sp[r15]
;		構造体アライメントが4Byteではない(パック)の場合:
;			mov.lのようにロングサイズ指定の場合、相対値は4の倍数のみ有効
;			このため, 一度対象アドレスを求めてからアクセスする必要がある
;				例:	mov.l	#__kernel_p_runtsk, r15
;					add		#TCB_sp, r15, r5
;					mov.l  	r0, [r5]
;		各オフセット値を判断し, 条件アセンブルによりコードを切り替える
;

;
;  構造体アクセスのオフセット定義
;
#include "offset.h"

;
;  各種EQU定義(Hファイルの#define定義)
;
#include "target_kernel_impl.h"
#include "prc_kernel_impl.h"

	.global	__kernel_istkpt
	
	.global	__kernel_p_runtsk
	.global	__kernel_p_schedtsk

	.global	__kernel_dispatch
	.global	__exit_and_dispatch
	.global	__kernel_start_r
	.global	__kernel_call_texrtn
	.global	_kernel_interrupt
	.global	_kernel_exception
	.global	__kernel_intnest
	.global	__kernel_call_exit_kernel
	.global	__kernel_start_dispatch

	.global	_ext_tsk
	.global	__kernel_exit_kernel

#if	TIPM_LOCK != -15
	.global	__kernel_lock_flag
	.global	__kernel_saved_ipl
#endif

	.global	__kernel_break_wait
	.global	_sil_dly_nse

#if LOG_INH_ENTER == 1
	.global	_kernel_log_inh_enter
#endif	/* LOG_INH_ENTER == 1 */
#if LOG_INH_LEAVE == 1
	.global	_kernel_log_inh_leave
#endif	/* LOG_INH_LEAVE == 1 */
#if LOG_EXC_ENTER == 1
	.global	_kernel_log_exc_enter
#endif	/* LOG_EXC_ENTER == 1 */
#if LOG_EXC_LEAVE == 1
	.global	_kernel_log_exc_leave
#endif	/* LOG_EXC_LEAVE == 1 */
#if LOG_DSP_ENTER == 1
	.global	_kernel_log_dsp_enter
#endif	/* LOG_DSP_ENTER == 1 */
#if LOG_DSP_LEAVE == 1
	.global	_kernel_log_dsp_leave
#endif	/* LOG_DSP_LEAVE == 1 */

#ifdef TOPPERS_SUPPORT_OVRHDR
	.global	__kernel_ovrtimer_start
	.global	__kernel_ovrtimer_stop
#endif /* TOPPERS_SUPPORT_OVRHDR */

;
;  TCB_sp への書込みマクロ
;
.macro	st_TCB_sp	src, tcb, tmp
#if (TCB_sp % 4) == 0
	mov.l	\src, TCB_sp[\tcb]
#else
	add		#TCB_sp, \tcb, \tmp
	mov.l	\src, [\tmp]
#endif
.endm

;
;  TCB_sp からの読出しマクロ
;
.macro	ld_TCB_sp	dst, tcb, tmp
#if (TCB_sp % 4) == 0
	mov.l	TCB_sp[\tcb], \dst
#else
	add		#TCB_sp, \tcb, \tmp
	mov.l	[tmp], \dst
#endif
.endm

;
;  TCB_pc への書込みマクロ
;
.macro	st_TCB_pc	src, tcb, tmp
#if (TCB_pc % 4) == 0
	mov.l	\src, TCB_pc[\tcb]
#else
	add		#TCB_pc, \tcb, \tmp
	mov.l	\src, [\tmp]
#endif
.endm

;
;  TCB_pc からの読出しマクロ
;
.macro	ld_TCB_pc	dst, tcb, tmp
#if (TCB_pc % 4) == 0
	mov.l	TCB_pc[\tcb], \dst
#else
	add		#TCB_pc, \tcb, \tmp
	mov.l	[\tmp], \dst
#endif
.endm

;
;  TCB_texptn からの読出しマクロ
;
.macro	ld_TCB_texptn	dst, tcb, tmp
#if (TCB_texptn % 4) == 0
	mov.l	TCB_texptn[\tcb], \dst
#else
	add		#TCB_texptn, \tcb, \tmp
	mov.l	[\tmp], \dst
#endif
.endm

;
;  TCB_p_tinib からの読出しマクロ
;
.macro	ld_TCB_p_tinib	dst, tcb, tmp
#if (TCB_p_tinib % 4) == 0
	mov.l	TCB_p_tinib[\tcb], \dst
#else
	add		#TCB_p_tinib, \tcb, \tmp
	mov.l	[\tmp], \dst
#endif
.endm

;
;  TINIB_exinf からの読出しマクロ
;
.macro	ld_TINIB_exinf	dst, tinib, tmp
#if (TINIB_exinf % 4) == 0
	mov.l	TINIB_exinf[\tinib], \dst
#else
	add		#TINIB_exinf, \tinib, \tmp
	mov.l	[\tmp], \dst
#endif
.endm

;
;  TINIB_task からの読出しマクロ
;
.macro	ld_TINIB_task	dst, tinib, tmp
#if (TINIB_task % 4) == 0
	mov.l	TINIB_task[\tinib], \dst
#else
	add		#TINIB_task, \tinib, \tmp
	mov.l	[\tmp], \dst
#endif
.endm


	.section	P, CODE

;
;  APIからのタスクディスパッチャ入口
;
;  呼び出し条件:
;	・ディスパッチャモード(ファイルヘッダ参照)
;
;  ここでは, コンテキストの退避と, 実行再開番地の設定をする.
;
__kernel_dispatch:
#ifdef TOPPERS_SUPPORT_OVRHDR /* (6-5-3-1) */
	mov.l	#255, r1					; r1にダミー割込み番号
	bsr		__kernel_ovrtimer_stop
#endif /* TOPPERS_SUPPORT_OVRHDR */
	pushm	r6-r13						; 非スクラッチレジスタ保存
	mov.l	#__kernel_p_runtsk, r15
	mov.l	[r15], r14
	st_TCB_sp	r0, r14, r5				; スタックポインタをTCBに保存
	st_TCB_pc	#dispatch_r, r14, r5	; 実行再開番地をTCBに保存
	bra		dispatcher

;
;  APIへのタスクディスパッチャ出口
;
;  呼び出し条件:
;	・ディスパッチャモード(ファイルヘッダ参照)
;
;  ここでは, タスク例外ハンドラ呼出しと, コンテキストの復帰をする.
;
dispatch_r:
dispatch_r_rts:
	popm 	r6-r13						; 非スクラッチレジスタ復帰
#ifdef TOPPERS_SUPPORT_OVRHDR /* (6-5-3-2) */
	bsr		__kernel_ovrtimer_start
#endif /* TOPPERS_SUPPORT_OVRHDR */
	rts  	                        	; dispatch 呼び出し元へ戻る.

;
;  タスク起動処理(タスク先頭へのタスクディスパッチャ出口)
;
;  呼び出し条件:
;	・ディスパッチャモード(ファイルヘッダ参照)
;
;  ここでは, CPUロック解除状態にし, タスクを起動する.
;
__kernel_start_r:
#ifdef TOPPERS_SUPPORT_OVRHDR /* (6-5-6-1) */
	push.l	r14
	bsr		__kernel_ovrtimer_start
	pop		r14
#endif /* TOPPERS_SUPPORT_OVRHDR */
	mov.l 	#_ext_tsk, r5
	push.l	r5							; 戻り番地をスタックに積む
	ld_TCB_p_tinib	r5, r14, r4
	ld_TINIB_exinf	r1, r5, r4			; 拡張情報を第一引数に設定
	ld_TINIB_task	r5, r5, r4			; タスクの起動番地を取得
.if	TIPM_LOCK == -15
	setpsw	i							; 割込み許可(CPUロック解除状態)
.else
	mov.l	#__kernel_lock_flag, r4		; CPUロック解除状態へ
	mov.l	#0, [r4]					; ここに来るときは必ず saved_ipl の
	mvtc	#00010000H, psw				; 値は 0 のため, 直値を設定する.
								; 全割込みロック解除状態( I = 1 )
								; 割込み優先度マスク全解除状態( IPL = 0 )
.endif
	jmp		r5

;
;  カーネル起動からのタスクディスパッチャ入口
;
;  このルーチンは，カーネル起動時に，すべての割込みを禁止した状態
; （割込みロック状態と同等）で呼び出される．また，割込みモード（非
;  タスクコンテキストと同等）で呼び出されることを想定している．
;
;  呼び出し条件:
;	・割込み(CPU例外)処理モード(ファイルヘッダ参照)
;
;  ここでは, ディスパッチャモードに変更する.
;
__kernel_start_dispatch:
	mov.l	#__kernel_intnest, r5
	mov.w	#0, [r5]					; タスクコンテキストに切換え
#if	TIPM_LOCK != -15
	mvtc	#(IPL_LOCK | 00010000H), psw	; 全割込みロック解除状態
										; 割込み優先度マスク全解除でない状態
#endif

;
; タスク終了(現在のコンテキストを捨てる)からのタスクディスパッチャ入口
;
;  呼び出し条件:
;	・ディスパッチャモード(ファイルヘッダ参照)
;
;  ここでは, 何もすることはない.
;  なお, p_runtsk のアドレス取得だけは行なう.
;
__exit_and_dispatch:
	mov.l	#__kernel_p_runtsk, r15     
.if	LOG_DSP_ENTER == 1
	mov.l	[r15], r14
.endif
;
; ディスパッチャ本体
;
;  呼び出し条件:
;	・すべてのタスクのコンテキストは保存されている.
;
;  dispatcher 呼出時のスタック:
;	・__kernel_dispatch からきた場合			: タスクスタック
;	・exit_and_dispatch からきた場合: 
;		exit_task からきた場合					: タスクスタック
;		カーネル起動時(__kernel_start_dispatch)	: 割込みスタック
;	・ret_int からきた場合						: タスクスタック
;	・dispatcher_idle_loop からきた場合			: 割込みスタック
;
dispatcher:
.if	LOG_DSP_ENTER == 1
	push.l	r15
	mov.l	r14, r1						; 引数(ディスパッチ元TCB)を設定
	bsr		__kernel_log_dsp_enter
	pop		r15
.endif
	mov.l	#__kernel_p_schedtsk, r5     
	mov.l	[r5], [r15]					; p_schedtsk を p_runtsk に
	mov.l	[r15], r14
	cmp		#0, r14
	bz		dispatcher_pre_idle			; schedtsk がなければアイドルループへ
	ld_TCB_sp	r0, r14, r5				; タスクスタックポインタを復帰
.if	LOG_DSP_LEAVE == 1
	push.l	r14
	mov.l	r14, r1						; 引数(ディスパッチ先TCB)を設定
	bsr		__kernel_log_dsp_leave
	pop		r14
.endif
	ld_TCB_pc	r5, r14, r4
	jmp		r5					; 実行再開番地へジャンプ

;
;  schdedtskがNULLの場合はアイドルループに入る
;  アイドルループは割込み処理モードで動作させる
;
;  ここで割込みモードに切り換えるのは，ここで発生する割込み処理に
;  どのスタックを使うかという問題の解決と，割込みハンドラ内でのタ
;  スクディスパッチの防止という2つの意味がある．
;
dispatcher_pre_idle:
	mov.l	#__kernel_istkpt,r5
	mov.l	[r5], r0					; 割込み用のスタックへ切替え
	mov.l	#__kernel_intnest, r5
	mov.w	#1, [r5]					; 非タスクコンテキストに切換え
#if	TIPM_LOCK != -15
	mov.l	#__kernel_lock_flag, r5		; CPUロック解除状態へ
	mov.l	#0, [r5]
	mvtc	#0, psw						; 優先度0の割込み処理中を偽装
#endif

dispatcher_idle_loop:
	setpsw	i							; 全割込み許可
	wait
	clrpsw	i							; 全割込み禁止

	mov.l	#__kernel_p_schedtsk, r4
	mov.l	[r4], r4					; p_schedtsk -> r4
	mov.l	#__kernel_p_runtsk, r5
	mov.l	[r5], r5					; p_runtsk -> r5
	cmp		r4, r5						; p_schedtsk と p_runtsk が同じでディスパッチ要求無しなら
	beq		dispatcher_idle_loop		; アイドルループを繰り返す
	mov.l	#0, [r5]					; reqflgがtrueならfalseにする
	mov.l	#__kernel_intnest, r5
	mov.w	#0, [r5]					; タスクコンテキストに切換え
#if	TIPM_LOCK != -15
	mov.l	#__kernel_lock_flag, r5		; CPUロック状態へ
	mov.l	#1, [r5]
	mov.l	#__kernel_saved_ipl, r5
	mov.l	#0, [r5]
	mvtc	#(IPL_LOCK | PSW_I_MASK), psw	; 全割込みロック解除状態
										; 割込み優先度マスク全解除でない状態
#endif
	bra		dispatcher					; dispatcher へ戻る


;
; カーネルの終了処理の呼出し
;
; モードとスタックを非タスクコンテキスト用に切り替え．
;
	.global	__kernel_call_exit_kernel
__kernel_call_exit_kernel:
#if	TIPM_LOCK != -15
	clrpsw	i							; 全割込み禁止
	mov.l	#__kernel_lock_flag, r5		; CPUロック解除状態へ
	mov.l	#0, [r5]
#endif
	mov.l	#__kernel_istkpt, r5
	mov.l	[r5], r0					; 割込み用のスタックへ切替え
	mov.l	#__kernel_intnest, r5
	mov.w	#1, [r5]					; 非タスクコンテキストに切換え
	bsr		__kernel_exit_kernel
	bra		__kernel_call_exit_kernel


;
;  割込み(CPU例外)からのタスクディスパッチャ入口
;
;	呼出し条件:
;	・全割込みロック状態(PSWレジスタIビット = 0)
;	・割込み優先度マスク全解除でない状態(IPL != 0)
;	・タスクコンテキスト(intnest=0)
;	・使用スタックはタスクスタック
;	・reqflg = true
;
;  ここでは, ディスパッチャモードに変更し, reqflgをOFFにしてから, 
;  遅延ディスパッチの有無を判断する.
;

;
;  ret_int先頭でスタックに積まれているPSWレジスタへのオフセット
;  ACC + FPSW + R14--R15 + R1--R5 + PC
;
	.equ	RET_INT_GET_PSW_OFFSET,	(8+4+28+4)

ret_int:
.if	TIPM_LOCK == -15
	mov.l	RET_INT_GET_PSW_OFFSET[r0], r5	; 割込み/CPU例外発生前のIPL値取得
	and		#PSW_IPL_MASK, r5
	mvtc	r5, psw						; 全割込みロック(CPUロック)状態
										; 割込み/CPU例外発生前の割込み優先度
.else
	mov.l	#__kernel_lock_flag, r5		; CPUロック状態へ
	mov.l	#1, [r5]
	mov.l	RET_INT_GET_PSW_OFFSET[r0], r5	; 割込み/CPU例外発生前のIPL値取得
	and		#PSW_IPL_MASK, r5
	mov.l	#__kernel_saved_ipl, r4
	mov.l	r5, [r4]
	mvtc	#(IPL_LOCK | PSW_I_MASK), psw	; 全割込みロック解除状態
										; 割込み優先度マスク全解除でない状態
.endif
	mov.l	#__kernel_p_runtsk, r15     
	mov.l	[r15], r14
	mov.l	#__kernel_p_schedtsk, r5
	mov.l	[r5], r5
	cmp		r5, r14						; p_schedtsk と p_runtsk が同じなら
	beq  	ret_int_r_call_tex			; ret_int_r_call_tex へ
	pushm	r6-r13						; 非スクラッチレジスタ保存
	st_TCB_sp	r0, r14, r5				; スタックポインタをTCBに保存
	st_TCB_pc	#ret_int_r, r14, r5		; 実行再開番地をTCBに保存
	bra		dispatcher

;
;  割込み(CPU例外)へのタスクディスパッチャ出口
;
;  呼び出し条件:
;	・ディスパッチャモード(ファイルヘッダ参照)
;
;  ここでは, タスク例外ハンドラ呼出しと, 割込み(CPU例外)処理モードへの
;  変更と, コンテキストの復帰を行い, 割込み(CUP例外)発生元へ戻る.
;
__kernel_break_wait:					;タスクモニタ用ラベル
ret_int_r:
	popm 	r6-r13						; 非スクラッチレジスタ復帰
ret_int_r_call_tex:
#ifdef TOPPERS_SUPPORT_OVRHDR /* 6.6.1 & 6.7.1 */
	push.l	r14
	bsr		__kernel_ovrtimer_start
	pop		r14
#endif /* TOPPERS_SUPPORT_OVRHDR */
ret_int_r_rte:
#if	TIPM_LOCK != -15
	clrpsw	i							; 全割込み禁止
	mov.l	#__kernel_lock_flag, r5		; CPUロック解除状態へ
	mov.l	#0, [r5]
#endif
	popm	r4-r5						; アキュムレータ復帰
	mvtaclo	r5							; ACC最下位16bitは0で復帰
	mvtachi	r4
	popc	fpsw						; FPUステータスレジスタ復帰
	popm	r14-r15						; レジスタ復帰
	popm	r1-r5
	rte									; 割込み前の処理に戻る


;
;  割込みの出入口処理(アセンブリ言語記述部分)
;
;  呼出し条件:
;  ・割込み発生時のH/W処理により, PSWレジスタのIビット=0, IPLは受付け
;    た割込みのIPL.
;  ・スタックは多重割り込みなら割込みスタック, そうでなければ
;    タスクスタック
;  ・割込み発生時のH/W処理により,スタックに割込みからの復帰PCとPSWが
;    保存されている.
;  ・ベクタテーブルに登録された個別の入り口処理により, スタックに
;    スクラッチレジスタ(R1-R5)が保存されている.
;
;  引数:
;  ・r1:割込み要因番号
;  ・r2:割込みハンドラのアドレス
;
;  レジスタがスタック上にどのように保存されているかを以下に示す.
;  この図では上が低位, 下が高位のアドレスで, スタックは下から
;  上方向に向かって積み上げられるものとする.
;
;	-------------------------
;	|       ACC-HI(4byte)   |
;	-------------------------(SP + 0)
;	|       ACC-LO(4byte)   |
;	-------------------------(SP + 4)
;	|       FPSW(4byte)     |
;	-------------------------(SP + 8)
;	|       R14(4byte)      |
;	-------------------------(SP + 12)
;	|       R15(4byte)      |
;	-------------------------(SP + 16)
;	|       R1(4byte)       |
;	-------------------------(SP + 20)
;	|       R2(4byte)       |
;	-------------------------(SP + 24)
;	|       R3(4byte)       |
;	-------------------------(SP + 28)
;	|       R4(4byte)       |
;	-------------------------(SP + 32)
;	|       R5(4byte)       |
;	-------------------------(SP + 36)
;	|       PC(4byte)       |
;	-------------------------(SP + 40)
;	|       PSW(4byte)      |
;	-------------------------(SP + 44)
;
;  ここでは, 割込み処理モードに変更してハンドラを実行する.
;
;  ハンドラからリターンした後は, 多重割込みでなく, かつ reqflg が
;  TRUE になった時に，ret_int へ分岐(遅延ディスパッチ)する．
;
;  多重割込みかどうかは割込みネストカウンタの値で判定する.
;  intnest != 0 ならば多重割込みであると判定する.
;
;  reqflg はCPUロック状態でチェックする. そうでないと，
;  reqflg チェック後に起動された割込みハンドラ内で
;  ディスパッチが要求された場合に，ディスパッチされない.
;
_kernel_interrupt:
	pushm	r14-r15						; スクラッチレジスタを退避
	pushc	fpsw						; FPUステータスレジスタ退避
	mvfacmi	r5
	shll	#16, r5						; ACC最下位16bitは0とする
	mvfachi	r4
	pushm	r4-r5						; アキュムレータ退避
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	add		#1, r4						; ネスト回数をインクリメント
	mov.w	r4, [r5]
	cmp		#1, r4						; 多重割り込みかどうか
	bnz		interrupt_from_int			; 加算前が0でなければ多重割込み
						; 初段の割込み
#ifdef TOPPERS_SUPPORT_OVRHDR /* 6.6.1 */
	pushm	r1-r5
	bsr		__kernel_ovrtimer_stop		; r1に割込み番号
	popm	r1-r5
#endif /* TOPPERS_SUPPORT_OVRHDR */
	mov.l	r0, r3						; スタックポインタを取り出し
	mov.l	#__kernel_istkpt, r5		; 割込み用のスタックへ切替える
	mov.l	[r5], r0
	push.l	r3							; タスクスタックを保持
interrupt_from_int:		; 多重割込み
	setpsw	i							; 割込み許可(CPUロック解除状態)

.if	LOG_INH_LEAVE == 1
	push.l	r1							; ログトレースの引数を保持
.endif

.if	LOG_INH_ENTER == 1
	push.l	r2
	bsr		__kernel_log_inh_enter		; ログトレース関数の呼出し
							; 引数の割込み要因番号は既にr1に入っている
	pop		r2
.endif

	jsr  	r2							; Cルーチン呼び出し

.if	LOG_INH_LEAVE == 1
	pop		r1							; 引数に割込み要因番号を設定
	bsr		__kernel_log_inh_leave		; ログトレース関数の呼出し
.endif

	clrpsw	i							; 割込み禁止(CPUロック状態)
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	sub		#1, r4						; ネスト回数をデクリメント
	mov.w	r4, [r5]
	cmp		#0, r4						; 多重割り込みかどうか
	bnz		interrupt_return			; 多重割り込みならリターン
	; 初段の割込み
	pop		r0							; タスクのスタックに戻す
	mov.l	#__kernel_p_schedtsk, r4
	mov.l	[r4], r4					; p_schedtsk -> r4
	mov.l	#__kernel_p_runtsk, r5
	mov.l	[r5], r5					; p_runtsk -> r5
	cmp		r4, r5						; p_schedtsk と p_runtsk が異なりディスパッチ要求有りなら
	bne		ret_int						; ret_int へジャンプ
#ifdef TOPPERS_SUPPORT_OVRHDR /* 6.6.1 */
	mov.l	#__kernel_p_runtsk, r5
	mov.l	[r5], r4
	cmp		#0, r4
	beq		interrupt_return
	bsr		__kernel_ovrtimer_start
#endif /* TOPPERS_SUPPORT_OVRHDR */
interrupt_return:
	popm	r4-r5						; アキュムレータ復帰
	mvtaclo	r5							; ACC最下位16bitは0で復帰
	mvtachi	r4
	popc	fpsw						; FPUステータスレジスタ復帰
	popm	r14-r15						; レジスタ復帰
	popm	r1-r5
	rte									; 割込み前の処理に戻る


;
;  CPU例外の出入口処理(アセンブリ言語記述部分)
;
;  呼出し条件:
;  ・CPU例外発生時のH/W処理により, PSWレジスタのIビット=0, IPL=0.
;  ・スタックは多重割り込みなら割込みスタック, そうでなければ
;    タスクスタック
;  ・CPU例外発生時のH/W処理により,スタックにCPU例外からの復帰PCと
;    PSWが保存されている.
;  ・ベクタテーブルに登録された個別の入り口処理により, スタックに
;    スクラッチレジスタ(R1-R5)が保存されている.
;
;  引数:
;  ・r1:CPU例外要因番号
;  ・r2:CPU例外ハンドラのアドレス
;
;  レジスタがスタック上にどのように保存されているかを以下に示す.
;  この図では上が低位, 下が高位のアドレスで, スタックは下から
;  上方向に向かって積み上げられるものとする.
;  なお, CPU例外要因番号とR6-R13はCPU例外ハンドラ内で情報を取得
;  する目的で退避しており, 出口処理では保存内容を破棄すればよい.
;
;	-------------------------
;	|    CPU例外要因番号    | <----- p_excinf
;	------------------------- (intptr_t)(p_excinf + 0)
;	|       R6(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 4)
;	|       R7(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 8)
;	|       R8(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 12)
;	|       R9(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 16)
;	|       R10(4byte)      |
;	------------------------- (intptr_t)(p_excinf + 20)
;	|       R11(4byte)      |
;	------------------------- (intptr_t)(p_excinf + 24)
;	|       R12(4byte)      |
;	------------------------- (intptr_t)(p_excinf + 28)
;	|       R13(4byte)      |
;	------------------------- (intptr_t)(p_excinf + 32)
;	|       ACC-HI(4byte)   |
;	------------------------- (intptr_t)(p_excinf + 36)
;	|       ACC-LO(4byte)   |
;	------------------------- (intptr_t)(p_excinf + 40)
;	|       FPSW(4byte)     |
;	------------------------- (intptr_t)(p_excinf + 44)
;	|       R14(4byte)      |
;	------------------------- (intptr_t)(p_excinf + 48)
;	|       R15(4byte)      |
;	------------------------- (intptr_t)(p_excinf + 52)
;	|       R1(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 56)
;	|       R2(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 60)
;	|       R3(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 64)
;	|       R4(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 68)
;	|       R5(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 72)
;	|       PC(4byte)       |
;	------------------------- (intptr_t)(p_excinf + 76)
;	|       PSW(4byte)      |
;	------------------------- (intptr_t)(p_excinf + 80)
;
;  ここでは, 割込み処理モードに変更してハンドラを実行する.
;  CPU例外ハンドラに渡すVP型の変数 p_excinf としてISPの値渡す．
;
;  ハンドラからリターンした後は, 多重割込みでなく, かつ reqflg が
;  TRUE になった時に，ret_int へ分岐(遅延ディスパッチ)する．
;
;  多重割込みかどうかは割込みネストカウンタの値で判定する.
;  intnest != 0 ならば多重割込みであると判定する.
;
;  reqflg はCPUロック状態でチェックする. そうでないと，
;  reqflg チェック後に起動された割込みハンドラ内で
;  ディスパッチが要求された場合に，ディスパッチされない.
;
;
;  CPU例外入口処理
;
;  ここでは, 割込み処理モードに変更してハンドラを実行する.
;

;
;  CPU例外ハンドラ呼出し後に不要となるスタック情報のサイズ
;  EXCNO + R6--R13
;
	.equ	EXCINF_REG_SIZE,	(4+32)

_kernel_exception:
	pushm	r14-r15						; スクラッチレジスタを退避
	pushc	fpsw
	mvfacmi	r5
	shll	#16, r5						; ACC最下位16bitは0とする
	mvfachi	r4
	pushm	r4-r5						; アキュムレータ退避
	pushm	r6-r13						; 非スクラッチレジスタ保存
	push.l	r1							; CPU例外要因番号を保持
	mov.l	r0, r3						; スタックポインタを取り出し
	mov.l	EXC_GET_PSW_OFFSET[r3], r5
	and		#PSW_I_MASK, r5
	bz		exception_nonkernel			; 全割込み禁止(CPUロック)状態なら管理外
#if	TIPM_LOCK != -15
	mov.l	EXC_GET_PSW_OFFSET[r3], r5
	and		#PSW_IPL_MASK, r5
	cmp		#IPL_LOCK, r5
	bgt	exception_nonkernel			; IPLがCPUロックレベル以上なら管理外
#endif
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	add		#1, r4						; ネスト回数をインクリメント
	mov.w	r4, [r5]
	cmp		#1, r4						; 多重割り込みかどうか
	bnz		exception_from_int			; 加算前が0でなければ多重割込み
						; 初段の割込み
#ifdef TOPPERS_SUPPORT_OVRHDR /* 6.7.1 */
	pushm	r1-r5
	mov.l	#255, r1					; r1にダミー割込み番号
	bsr		__kernel_ovrtimer_stop
	popm	r1-r5
#endif /* TOPPERS_SUPPORT_OVRHDR */
	mov.l	#__kernel_istkpt, r5		; 割込み用のスタックへ切替える
	mov.l	[r5], r0
	push.l	r3							; タスクスタックを保持
exception_from_int:		; 多重割込み
	mov.l	EXC_GET_PSW_OFFSET[r3], r5
	mvtc	r5, psw						; CPU例外発生前の状態に戻す
#if	LOG_EXC_LEAVE == 1
	push.l	r1							; ログトレースの引数を保持
#endif

#if	LOG_EXC_ENTER == 1
	pushm	r2-r3
	bsr		__kernel_log_exc_enter		; ログトレース関数の呼出し
							; 引数の割込み要因番号は既にr1に入っている
	popm	r2-r3
#endif

	mov.l	r3, r1						; 引数のp_excinfを設定
	jsr  	r2							; Cルーチン呼び出し

.if	LOG_EXC_LEAVE == 1
	pop		r1							; 引数に割込み要因番号を設定
	bsr		__kernel_log_exc_leave		; ログトレース関数の呼出し
.endif

	clrpsw	i							; ここからは必ず割込み禁止
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	sub		#1, r4						; ネスト回数をデクリメント
	mov.w	r4, [r5]
	cmp		#0, r4						; 多重割り込みかどうか
	bnz		exception_return			; 多重割り込みならリターン
	; 初段の割込み
	pop		r0							; タスクのスタックに戻す
	mov.l	#__kernel_p_schedtsk, r4
	mov.l	[r4], r4					; p_schedtsk -> r4
	mov.l	#__kernel_p_runtsk, r5
	mov.l	[r5], r5					; p_runtsk -> r5
	cmp		r4, r5						; p_schedtsk と p_runtsk が同じでディスパッチ要求無しか
#ifndef TOPPERS_SUPPORT_OVRHDR /* 6.7.1 */
	beq		exception_return			; なければリターン
	add		#EXCINF_REG_SIZE, r0		; CPU例外情報の破棄
	bra		ret_int						; あれば ret_int へジャンプ
#else /* TOPPERS_SUPPORT_OVRHDR */
	beq		exception_return_ovr		; なければリターン
	add		#EXCINF_REG_SIZE, r0		; CPU例外情報の破棄
	bra		ret_int						; あれば ret_int へジャンプ
exception_return_ovr:
	mov.l	#__kernel_p_runtsk, r5
	mov.l	[r5], r4
	cmp		#0, r4
	beq		exception_return
	bsr		__kernel_ovrtimer_start
	bra		exception_return
#endif /* TOPPERS_SUPPORT_OVRHDR */

exception_nonkernel:
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	add		#1, r4						; ネスト回数をインクリメント
	mov.w	r4, [r5]
	cmp		#1, r4						; 多重割り込みかどうか
	bnz		exception_from_nonkernelint	; 加算前が0でなければ多重割込み
						; 初段の割込み
	mov.l	#__kernel_istkpt, r5		; 割込み用のスタックへ切替える
	mov.l	[r5], r0
	push.l	r3							; タスクスタックを保持
exception_from_nonkernelint:	; 多重割込み
	mov.l	EXC_GET_PSW_OFFSET[r3], r5
	mvtc	r5, psw						; CPU例外発生前の状態に戻す

	mov.l	r3, r1						; 引数のp_excinfを設定
	jsr  	r2							; Cルーチン呼び出し

	clrpsw	i							; ここからは必ず割込み禁止
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	sub		#1, r4						; ネスト回数をデクリメント
	mov.w	r4, [r5]
	cmp		#0, r4						; 多重割り込みかどうか
	bnz		exception_return			; 多重割り込みならリターン
	; 初段の割込み
	pop		r0							; タスクのスタックに戻す

exception_return:
	add		#EXCINF_REG_SIZE, r0		; CPU例外情報の破棄
	popm	r4-r5						; アキュムレータ復帰
	mvtaclo	r5							; ACC最下位16bitは0で復帰
	mvtachi	r4
	popc	fpsw						; FPUステータスレジスタ復帰
	popm	r14-r15						; レジスタ復帰
	popm	r1-r5
	rte									; 割込み前の処理に戻る


;
;  微少時間待ち
;
_sil_dly_nse:
	mov.l	#SIL_DLY_TIM1, r5
	sub		r5, r1
	ble		sil_dly_nse_ret
sil_dly_nse_loop:
	mov.l	#SIL_DLY_TIM2, r5
	sub		r5, r1
	bgt		sil_dly_nse_loop
sil_dly_nse_ret:
	rts

	.end

