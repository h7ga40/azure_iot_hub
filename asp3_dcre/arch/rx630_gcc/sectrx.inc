;*
;*  TOPPERS/ASP Kernel
;*      Toyohashi Open Platform for Embedded Real-Time Systems/
;*      Advanced Standard Profile Kernel
;* 
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

;
;  ターゲットシステム依存セクション定義ファイル(スターターキット用)
;

;
;  セクションの定義
;
;  セクションのアドレス定義はHEW上の最適化リンカの
;  オプションにて実施する.
;

	;
	;  初期値有りデータ領域のセクション
	;
	;  00000000h - 00020000h
	;
	.section	D, ROMDATA
	.section	D_2, ROMDATA, ALIGN = 2
	.section	D_1, ROMDATA


	;
	;  初期値無しデータ領域のセクション
	;
	;  00000000h - 00020000h
	;
	.section	B, DATA, ALIGN = 4
	.section	B_2, DATA, ALIGN = 2
	.section	B_1, DATA
	.section	W, ROMDATA,ALIGN = 4
	.section	W_1, ROMDATA
	.section	W_2, ROMDATA, ALIGN = 2


	;
	;  カーネルスタック領域のセクション
	;
	;  00000000h - 00020000h
	;
	;  以下.blkb命令で確保しているカーネルスタック領域のサイズを
	;  変更する場合は, target_kernel_impl.hのDEFAULT_ISTKSZの値を, 同じ
	;  値に変更すること.
	;
;	.section	SI, DATA, ALIGN = 4
;	.global _DEFAULT_ISTACK
;_DEFAULT_ISTACK:
;	.blkb		DEFAULT_ISTKSZ
;	.global DEFAULT_ISTACK_TOP
;DEFAULT_ISTACK_TOP:

	;
	;  定数領域のセクション
	;
	;  00100000h - 00107FFFh
	;
	.section	C, ROMDATA, ALIGN = 4
	.section	C_2, ROMDATA, ALIGN = 2
	.section	C_1, ROMDATA

	;
	;  初期値有りデータ領域の初期値
	;
	;  00100000h - 00107FFFh
	;
	.section	R, DATA, ALIGN = 4
	.section	R_2, DATA, ALIGN = 2
	.section	R_1, DATA

	;
	;  可変ベクタテーブル領域のセクション
	;
	;  00100000h - 00107FFFh
	;
	.section	.fvectors, ROMDATA, ALIGN = 4
	.global VVECTOR_TOP
VVECTOR_TOP:

	;
	;  プログラム領域のセクション
	;
	;  FFE80000h - FFFFFF7Fh
	;
	.section	P, CODE

	;
	;  固定ベクタテーブル領域のセクション
	;
	;  FFFFFF80h - FFFFFFFFh
	;
	.section	FVECT, ROMDATA, ALIGN = 4

