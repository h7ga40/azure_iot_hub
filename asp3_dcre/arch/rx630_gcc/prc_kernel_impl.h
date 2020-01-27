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

#ifndef TOPPERS_PRC_CONFIG_H
#define TOPPERS_PRC_CONFIG_H

/*
 *  プロセッサの特殊命令のインライン関数定義
 */
#include "prc_insn.h"
#include "sil.h"

#ifndef TOPPERS_MACRO_ONLY

/*
 *  非タスクコンテキスト用のスタック初期値
 */

#define TOPPERS_ISTKPT( istk, istksz ) (( STK_T * )(( char * )( istk ) + ( istksz )))

/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
	void	*sp;		/* スタックポインタ */
	FP		pc;			/* プログラムカウンタ */
} TSKCTXB;


/*
 *  割込み発生回数を保存する変数
 */
extern uint16_t intnest;

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  CPUロック状態での割込み優先度マスク
 *
 *  TIPM_LOCKは，CPUロック状態での割込み優先度マスク，すなわち，カーネ
 *  ル管理外のものを除くすべての割込み要求をマスクする値に定義する．
 *
 *  TMIN_INTPRI変更することで管理外割込みの有無を決定する．
 *  例えばTMIN_INTPRIを-14に設定すると，レベル15の割込みがカーネル管理外と
 *  なる．TMIN_INTPRIを-15に設定すると，NMI以外にカーネル管理外の割込みを
 *  設けないことになる（この場合には-15に設定することを推奨する）．
 */
#ifndef TIPM_LOCK
#define TIPM_LOCK	TMIN_INTPRI
#endif /* TIPM_LOCK */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  TOPPERS標準割込み処理モデルの実現
 */
/*
 *  コンテキストの参照
 *
 *  RXでは，割込みの戻り先がタスクかどうかを判断するために intnest
 *  を使用している．これを用いてコンテキストを判断する．
 */
Inline bool_t
sense_context( void )
{
	/*  ネストカウンタ0より大なら非タスクコンテキスト  */
	return ( intnest > 0U );
}


/*
 *  CPUロックフラグ実現のための変数
 *
 *  これらの変数は，CPUロック状態の時のみ書き換えてよいものとする．
 */
#if TIPM_LOCK != -15
extern bool_t	lock_flag;		/* CPUロックフラグの値を保持する変数 */
extern uint32_t	saved_ipl;		/* 割込み優先度レベルを保存する変数 */
#endif	/* TIPM_LOCK != -15	*/

#endif /* TOPPERS_MACRO_ONLY */


/*
 *  割込み優先度マスクの外部表現と内部表現の変換
 *
 *  RX630では，プロセッサステータスワードレジスタ（PSW）の下から
 *  24～27ビット目の4ビットに割込み優先度レベル（IPL）が置かれている．
 *  カーネル管理外割込みを実装する場合にIPLを使用した制御を行う．
 *  カーネルは割込み優先度マスク（-1から連続した負の値）で管理されて
 *  いるためIPLとの変換が必要となる．
 */
#define IPL_TO_IPM( ipl )	(-(( PRI )(( ipl ) >> 24U )))		/* IPLをIPMに	*/
#define IPM_TO_IPL( ipm )	((( uint32_t )(-( ipm ))) << 24U )	/* IPMをIPLに	*/


/*
 *  CPUロック状態での割込み優先度マスクのIPL
 */
#define IPL_LOCK		( IPM_TO_IPL( TIPM_LOCK ) )

/*
 *  TIPM_ENAALL（割込み優先度マスク全解除）のIPL
 */
#define IPL_ENAALL		( IPM_TO_IPL( TIPM_ENAALL ) )


#ifndef TOPPERS_MACRO_ONLY

/*
 *  割込み要因毎のレベル & 属性定義テーブル
 *  (テンプレートファイルによる出力)
 */
typedef struct cfg_int_info {
	PRI		intpri;
	ATR		intatr;
} CFG_INT_INFO;

extern const CFG_INT_INFO cfg_int_table[];


/*
 *  割込み番号の範囲の判定
 *
 *  割込み番号が有効な値か厳密にチェックするため,
 *  コンフィギュレータ出力テーブルを参照する.
 */
#define VALID_INTNO( intno )	\
 				( cfg_int_table[( intno )].intpri > 0 )
#define VALID_INTNO_DISINT( intno )	VALID_INTNO( intno )
#define VALID_INTNO_CFGINT( intno )	VALID_INTNO( intno )

/* cre_intで有効な割込み番号の指定  */
#define VALID_INTNO_CREINT          VALID_INTNO_CFGINT((intno))

/* cre_isrで有効な割込み番号の指定  */
#define	VALID_INTNO_CREISR(intno)	VALID_INTNO_CFGINT((intno))


/*
 *  割込み制御レジスタ関連の定義
 */
#define	IRQ_POSEDGE			( 0x08U )
#define	IRQ_NEGEDGE			( 0x04U )
#define	IRQ_BOTHEDGE		( 0x0CU )
#define	IRQ_LOWLEVEL		( 0x00U )


/*
 *  CPUロック状態への移行
 *
 *  IPM（ハードウェアの割込み優先度マスク）を，saved_iipmに保存し，カー
 *  ネル管理外のものを除くすべての割込み要求をマスクする値（TIPM_LOCK）
 *  に設定する．また，lock_flagをTRUEにする．
 *
 *  IPMが，最初からTIPM_LOCKと同じかそれより高い場合には，それを
 *  saved_iipmに保存するのみで，TIPM_LOCKには設定しない．これは，モデル
 *  上の割込み優先度マスクが，TIPM_LOCKと同じかそれより高いレベルに設定
 *  されている状態にあたる．
 *
 *  この関数は，CPUロック状態（lock_flagがTRUEの状態）で呼ばれることは
 *  ないものと想定している．
 */
Inline void
lock_cpu( void )
{
#if TIPM_LOCK == -15
	disint();
#else	/* TIPM_LOCK == -15	*/
	uint32_t	ipl;

	/*
	 *  current_ipl()の返り値を直接saved_iplに保存せず，一時変数ipl
	 *  を用いているのは，current_ipl()を呼んだ直後に割込みが発生し，
	 *  起動された割込み処理でsaved_iplが変更される可能性があるためで
	 *  ある．
	 */
	ipl = current_ipl();
	if( IPL_LOCK > ipl ){
		set_ipl( IPL_LOCK );
	}

	saved_ipl = ipl;
	lock_flag = true;
#endif	/* TIPM_LOCK == -15	*/
}

#define lock_cpu_dsp()	lock_cpu()

/*
 *  CPUロック状態の解除
 *
 *  lock_flagをFALSEにし，IPM（ハードウェアの割込み優先度マスク）を，
 *  saved_iipmに保存した値に戻す．
 *
 *  この関数は，CPUロック状態（lock_flagがtrueの状態）でのみ呼ばれるも
 *  のと想定している．
 */
Inline void
unlock_cpu( void )
{
#if TIPM_LOCK == -15
	enaint();
#else	/* TIPM_LOCK == -15	*/
	lock_flag = false;
	set_ipl( saved_ipl );
#endif	/* TIPM_LOCK == -15	*/
}

#define unlock_cpu_dsp()	unlock_cpu()

/*
 *  CPUロック状態の参照
 */
Inline bool_t
sense_lock( void )
{
#if TIPM_LOCK == -15
	return (( bool_t )(( current_psw() & PSW_I_MASK) == 0 ));
#else	/* TIPM_LOCK == -15	*/
	return lock_flag;
#endif	/* TIPM_LOCK == -15	*/
}


/*
 *  割込み属性の設定のチェック
 */
Inline bool_t
check_intno_cfg(INTNO intno)
{
	return(cfg_int_table[intno].intpri != 0U);
}

/*
 * （モデル上の）割込み優先度マスクの設定
 *
 *  CPUロックフラグがクリアされている時は，ハードウェアの割込み優先度マ
 *  スクを設定する．CPUロックフラグがセットされている時は，saved_iipm
 *  を設定し，さらに，ハードウェアの割込み優先度マスクを，設定しようと
 *  した（モデル上の）割込み優先度マスクとTIPM_LOCKの高い方に設定する．
 */
Inline void
t_set_ipm( PRI intpri )
{
	uint32_t	ipl = IPM_TO_IPL( intpri );
#if TIPM_LOCK == -15
	set_ipl(ipl);
#else	/* TIPM_LOCK == -15	*/
	if( !lock_flag ){
		set_ipl(ipl);
	}
	else {
		saved_ipl = ipl;
		set_ipl(ipl > IPL_LOCK ? ipl : IPL_LOCK);
    }
#endif	/* TIPM_LOCK == -15	*/
}



/*
 * （モデル上の）割込み優先度マスクの参照
 *
 *  CPUロックフラグがクリアされている時はハードウェアの割込み優先度マ
 *  スクを，セットされている時はsaved_iipmを参照する．
 */
Inline PRI
t_get_ipm( void )
{
	uint32_t	ipl;
#if TIPM_LOCK == -15
	ipl = current_ipl();
#else	/* TIPM_LOCK == -15	*/
	if (!lock_flag) {
		ipl = current_ipl();
	}
	else {
		ipl = saved_ipl;
	}
#endif	/* TIPM_LOCK == -15	*/
	return IPL_TO_IPM( ipl );
}



/*
 *  割込み要求禁止フラグのセット
 *
 *  割込み属性が設定されていない割込み要求ラインに対して割込み要求禁止
 *  フラグをセットしようとした場合には，FALSEを返す．
 */
Inline bool_t
disable_int( INTNO intno )
{
	/*
	 *  レベル定義が0である場合はCFG_INTされていない
	 */
	if( cfg_int_table[intno].intpri == 0 ){
		return ( false );
	}

	sil_wrb_mem(ier_reg_addr[intno].addr,
		sil_reb_mem(ier_reg_addr[intno].addr) & ~ier_reg_addr[intno].offset);

	return ( true );
}


/*
 *  割込み要求禁止フラグのクリア
 *
 *  割込み属性が設定されていない割込み要求ラインに対して割込み要求禁止
 *  フラグをクリアしようとした場合には，FALSEを返す．
 */
Inline bool_t
enable_int( INTNO intno )
{
	/*
	 *  レベル定義が0である場合はCFG_INTされていない
	 */
	if( cfg_int_table[intno].intpri == 0 ){
		return ( false );
	}

	sil_wrb_mem(ier_reg_addr[intno].addr,
		sil_reb_mem(ier_reg_addr[intno].addr) | ier_reg_addr[intno].offset);

	return ( true );
}


/*
 *  割込み要求がクリアできる状態か？
 */
Inline bool_t
check_intno_clear(INTNO intno)
{
	return(true);
}

/*
 *  割込み要求のクリア
 */
Inline void
clear_int( INTNO intno )
{
	sil_wrb_mem(IR_REG(intno), 0U);
}


/*
 *  割込みが要求できる状態か？
 */
Inline bool_t
check_intno_raise(INTNO intno)
{
	return(true);
}

/*
 * 割込みの要求
 * 注意：チップでサポートされていない操作
 */
Inline void
raise_int( INTNO intno )
{
	*IR_REG(intno) = 1U;
}

/*
 *  割込み要求のチェック
 */
Inline bool_t
probe_int( INTNO intno )
{
	/*
	 *  割込み要求レジスタは0 or 1でしかないため,
	 *  そのままの値を返す.
	 */
	return sil_reb_mem(IR_REG(intno));
}

/*
 *  割込み要求ラインの属性の設定
 */
extern void	config_int( INTNO intno, ATR intatr, PRI intpri );

/*
 *  割込みを受け付けるための遅延処理
 */
Inline void
delay_for_interrupt(void)
{
}

/*
 *  最高優先順位タスクへのディスパッチ（prc_support.a30）
 *
 *  dispatchは，タスクコンテキストから呼び出されたサービスコール処理か
 *  ら呼び出すべきもので，タスクコンテキスト・CPUロック状態・ディスパッ
 *  チ許可状態・（モデル上の）割込み優先度マスク全解除状態で呼び出さな
 *  ければならない．
 */
extern void	dispatch( void );

/*
 *  非タスクコンテキストからのディスパッチ要求
 */
#define request_dispatch_retint()

/*
 *  ディスパッチャの動作開始（prc_support.a30）
 *
 *  start_dispatchは，カーネル起動時に呼び出すべきもので，すべての割込
 *  みを禁止した状態（全割込みロック状態と同等の状態）で呼び出さなければ
 *  ならない．
 */
extern void	start_dispatch( void ) NoReturn;

/*
 *  現在のコンテキストを捨ててディスパッチ（prc_support.a30）
 *
 *  exit_and_dispatchは，ext_tskから呼び出すべきもので，タスクコンテキ
 *  スト・CPUロック状態・ディスパッチ許可状態・（モデル上の）割込み優先
 *  度マスク全解除状態で呼び出さなければならない．
 */
#define _kernel_exit_and_dispatch();	\
	_exit_and_dispatch();		\
	ercd = E_SYS;

extern void	_exit_and_dispatch( void ) NoReturn;

/*
 *  カーネルの終了処理の呼出し（prc_support.a30）
 *
 *  call_exit_kernelは，カーネルの終了時に呼び出すべきもので，非タスク
 *  コンテキストに切り換えて，カーネルの終了処理（exit_kernel）を呼び出
 *  す．
 */
extern void call_exit_kernel( void ) NoReturn;

/*
 *  タスクコンテキストの初期化
 *
 *  タスクが休止状態から実行できる状態に移行する時に呼ばれる．この時点
 *  でスタック領域を使ってはならない．
 *
 *  activate_contextを，インライン関数ではなくマクロ定義としているのは，
 *  この時点ではTCBが定義されていないためである．
 */
extern void	start_r( void );

#define activate_context( p_tcb )														\
{																						\
	{																					\
																						\
		/*  スタックポインタ初期値の設定  */											\
		( p_tcb )->tskctxb.sp = ( void * )((( uint32_t ) ( p_tcb )->p_tinib->stk ) + 	\
								( p_tcb )->p_tinib->stksz );							\
		/* 起動番地の設定 */															\
		( p_tcb )->tskctxb.pc = ( FP ) start_r;											\
	}																					\
}


/*
 *  calltexは使用しない
 */
#define OMIT_CALLTEX


/*
 *  割込みハンドラの設定
 *
 *  RX630はROMに割込みベクタを配置するため，本関数は空関数で実装する．
 */
Inline void
define_inh( INHNO inhno, FP inthdr )
{
}

/*
 *  CPU例外ハンドラの設定
 *
 *  RX630はROMに例外ベクタを配置するため，本関数は空関数で実装する．
 */
Inline void
define_exc( EXCNO excno, FP exchdr )
{
}

/*
 *  割込み/例外ハンドラの入口処理の生成マクロ
 *
 *  テンプレートファイルにより生成するため空マクロとする．
 */
#define HDR_ENTRY(hdr, intexc_num , tobejmp)

/*
 *  割込みハンドラの入口処理の生成マクロ
 */
#define _INT_ENTRY(inhno, inthdr)				_kernel_##inthdr##_##inhno##_entry
#define INT_ENTRY(inhno, inthdr)				_INT_ENTRY(inhno , inthdr)

#define _INTHDR_ENTRY(inhno, inhno_num ,inthdr)	\
		extern void _kernel_##inthdr##_##inhno##_entry(void);
#define INTHDR_ENTRY(inhno, inhno_num ,inthdr)	_INTHDR_ENTRY(inhno, inhno_num ,inthdr)


/*
 *  CPU例外ハンドラの入口処理の生成マクロ
 */
#define _EXC_ENTRY(excno , exchdr)					_kernel_##exchdr##_##excno##_entry
#define EXC_ENTRY(excno , exchdr)					_EXC_ENTRY(excno , exchdr)

#define _EXCHDR_ENTRY(excno , excno_num , exchdr)	\
		extern void _kernel_##exchdr##_##excno##_entry(void);
#define EXCHDR_ENTRY(excno , excno_num , exchdr)	_EXCHDR_ENTRY(excno , excno_num , exchdr)


/*
 *  CPU例外の発生した時のコンテキストの参照
 *
 *  CPU例外の発生した時のコンテキストが，タスクコンテキストの時にFALSE，
 *  そうでない時にtrueを返す．
 */
Inline bool_t
exc_sense_context( void *p_excinf )
{
	/* 
	 *  ネストカウンタが1より大なら非タスクコンテキスト
	 */
	return( intnest > 1U );
}

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  CPU例外情報 p_excinf から PSW の値を取得するためのオフセット値
 *  EXCNO + ACC + FPSW + R1～R15 + PC
 */
#define EXC_GET_PSW_OFFSET	(4+8+4+60+4)

#ifndef TOPPERS_MACRO_ONLY

/*
 *  CPU例外の発生した時のIPLの参照
 */
Inline uint32_t
exc_get_ipl(void *p_excinf)
{
	return((*((uint32_t *)((uintptr_t)p_excinf+EXC_GET_PSW_OFFSET)))
			& PSW_IPL_MASK);
}

Inline bool_t
exc_sense_i(void *p_excinf)
{
	return((*(((uint32_t *)((uintptr_t)p_excinf+EXC_GET_PSW_OFFSET)))
			 & PSW_I_MASK) != 0u);
}


/*
 *  CPU例外の発生した時のコンテキストと割込みのマスク状態の参照
 *
 *  CPU例外の発生した時のシステム状態が，カーネル実行中でなく，タスクコ
 *  ンテキストであり，全割込みロック状態でなく，CPUロック状態でなく，（モ
 *  デル上の）割込み優先度マスク全解除状態である時にtrue，そうでない時
 *  にFALSEを返す（CPU例外がカーネル管理外の割込み処理中で発生した場合
 *  にもFALSEを返す）．
 *  
 *  カーネル実行中でない→ (TIPM_LOCK== -15) Iフラグ == 1
 *                         (else)           IPL < IPL_LOCK
 *  タスクコンテキストである→intnest == 0
 *  全割込みロックでない→ Iフラグ == 1
 *  割込み優先度マスクが全解除→IPL == 0
 *  
 *  
 */
Inline bool_t
exc_sense_intmask(void *p_excinf)
{
	return((!exc_sense_context(p_excinf))
			&& exc_sense_i(p_excinf)
			&& (exc_get_ipl(p_excinf) == IPL_ENAALL));
}

/*
 *  CPU例外の発生した時のコンテキストと割込み／CPUロック状態の参照
 *
 *  CPU例外の発生した時のシステム状態が，カーネル実行中でなく，タスクコ
 *  ンテキストであり，全割込みロック状態でなく，CPUロック状態でない時に
 *  true，そうでない時にfalseを返す（CPU例外がカーネル管理外の割込み処
 *  理中で発生した場合にもfalseを返す）．
 *  
 *  カーネル実行中でない→ (TIPM_LOCK== -15) Iフラグ == 1
 *                         (else)           IPL < TIPM_LOCK
 *  タスクコンテキストである→intnest < 1 
 *  全割込みロックでない→ Iフラグ == 1
 *  CPUロック状態でない→(TIPM_LOCK== -15) Iフラグ == 1
 *                         (else)           IPL < TIPM_LOCK
 */
Inline bool_t
exc_sense_unlock(void *p_excinf)
{
#if TIPM_LOCK == -15
	return((!exc_sense_context(p_excinf))
			&& exc_sense_i(p_excinf));
#else
	return((!exc_sense_context(p_excinf))
			&& exc_sense_i(p_excinf)
			&& exc_get_ipl(p_excinf) < IPL_LOCK);
#endif
}


/*
 *  プロセッサ依存の初期化
 */
extern void	prc_initialize( void );

/*
 *  プロセッサ依存の終了時処理
 */
extern void	prc_terminate( void );


#ifndef OMIT_DEFAULT_INT_HANDLER
/*
 *  未登録の割込みが発生した場合に呼び出される
 */
void default_int_handler( void );
#endif /* OMIT_DEFAULT_INT_HANDLER */

#ifndef OMIT_DEFAULT_EXC_HANDLER
/*
 *  未登録の例外が発生した場合に呼び出される
 */
void default_exc_handler( void );
#endif /* OMIT_DEFAULT_EXC_HANDLER */


#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_PRC_CONFIG_H */

