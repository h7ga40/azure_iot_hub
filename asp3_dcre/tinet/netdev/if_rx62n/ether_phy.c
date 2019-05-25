/*
 *  TINET (TCP/IP Protocol Stack)
 * 
 *  Copyright (C) 2001-2009 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2014-2017 by Cores Co., Ltd. Japan
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

#include "ether_phy.h"
#include <kernel.h>
#include <sil.h>

#define PHY_CONTROL_REGISTER	0x00

#define PHY_CONTROL_RESET			0x8000
#define PHY_CONTROL_LOOPBACK		0x4000
#define PHY_CONTROL_SPEED_SELECT	0x2000
#define PHY_CONTROL_A_N_ENABLE		0x1000
#define PHY_CONTROL_POWER_DOWN		0x0800
#define PHY_CONTROL_ISOLATE			0x0400
#define PHY_CONTROL_RESTART_A_N		0x0200
#define PHY_CONTROL_DUPLEX_MODE		0x0100
#define PHY_CONTROL_COLLISION_TEST	0x0080

#define PHY_STATUS_REGISTER		0x01

#define PHY_STATUS_100BASE_T4				0x8000
#define PHY_STATUS_100BASE_TX_FULL_DUPLEX	0x4000
#define PHY_STATUS_100BASE_TX_HALF_DUPLEX	0x2000
#define PHY_STATUS_10BASE_T_FULL_DUPLEX		0x1000
#define PHY_STATUS_10BASE_T_HALF_DUPLEX		0x0800
#define PHY_STATUS_A_N_COMPLETE				0x0020
#define PHY_STATUS_REMOTE_FAULT				0x0010
#define PHY_STATUS_A_N_ABILITY				0x0008
#define PHY_STATUS_LINK_STATUS				0x0004
#define PHY_STATUS_JABBER_DETECT			0x0002
#define PHY_STATUS_EXTENDED_CAPABILITY		0x0001

#define PHY_AN_ADVERTISEMENT_REGISTER	0x04

#define PHY_AN_AD_100BASE_T4				0x0200
#define PHY_AN_AD_100BASE_TX_FULL_DUPLEX	0x0100
#define PHY_AN_AD_100BASE_TX_HALF_DUPLEX	0x0080
#define PHY_AN_AD_10BASE_T_FULL_DUPLEX		0x0040
#define PHY_AN_AD_10BASE_T_HALF_DUPLEX		0x0020
#define PHY_AN_AD_SELECTOR_FIELD			0x0001

#define PHY_AN_STATUS_REGISTER	0x05

#define PHY_AN_STATUS_100BASE_T4				0x0200
#define PHY_AN_STATUS_100BASE_TX_FULL_DUPLEX	0x0100
#define PHY_AN_STATUS_100BASE_TX_HALF_DUPLEX	0x0080
#define PHY_AN_STATUS_10BASE_T_FULL_DUPLEX		0x0040
#define PHY_AN_STATUS_10BASE_T_HALF_DUPLEX		0x0020

static void phy_write_bit(bool_t bit);
static void phy_release_bus();
static bool_t phy_read_bit();
static void phy_single_rel_bus();

PHY_STATE_T phy_reset(PHY_STATE_T state, uint8_t phy_addr)
{
	switch(state){
	case PHY_STATE_UNINIT:
		/* PHYのリセット */
		phy_write_reg(phy_addr, PHY_CONTROL_REGISTER, PHY_CONTROL_RESET);

		/* 500ms待つ */
		dly_tsk(500 * 1000);
		/* continue; */
	case PHY_STATE_RESETING:
		/* リセット完了の確認 */
		if((phy_read_reg(phy_addr, PHY_CONTROL_REGISTER) & PHY_CONTROL_RESET) != 0)
			return PHY_STATE_RESETING;

		return PHY_STATE_RESET;
	default:
		return state;
	}
}

PHY_STATE_T phy_initialize(PHY_STATE_T state, uint8_t phy_addr, PHY_MODE_T *mode)
{
#if defined(_MSC_VER) || defined(__c2__)
	static int reset = 0;
#endif
	uint16_t status;
	*mode = (PHY_MODE_T)0;

	switch(state){
	case PHY_STATE_RESET:
		/* オートネゴシエーションのお知らせを有効に設定 */
		phy_write_reg(phy_addr, PHY_AN_ADVERTISEMENT_REGISTER, 
			PHY_AN_AD_100BASE_T4 | PHY_AN_AD_100BASE_TX_FULL_DUPLEX | PHY_AN_AD_100BASE_TX_HALF_DUPLEX
			| PHY_AN_AD_10BASE_T_FULL_DUPLEX | PHY_AN_AD_10BASE_T_HALF_DUPLEX | PHY_AN_AD_SELECTOR_FIELD);

		/* オートネゴシエーションを有効に設定 */
		phy_write_reg(phy_addr, PHY_CONTROL_REGISTER, PHY_CONTROL_SPEED_SELECT | PHY_CONTROL_A_N_ENABLE);
		/* continue; */
	case PHY_STATE_LINKE_WAIT:
#if defined(_MSC_VER) || defined(__c2__)
		reset++;
		if(reset < 2){
#endif
		/* リンク完了の確認 */
		if(!phy_is_link(phy_addr))
			return PHY_STATE_LINKE_WAIT;
#if defined(_MSC_VER) || defined(__c2__)
		}
		reset = 0;
#endif
		/* continue; */
	case PHY_STATE_LINKED:
#if defined(_MSC_VER) || defined(__c2__)
		reset++;
		if(reset < 2){
#endif
		/* オートネゴシエーション完了の確認 */
		if((phy_read_reg(phy_addr, PHY_STATUS_REGISTER) & PHY_STATUS_A_N_COMPLETE) == 0)
			return PHY_STATE_LINKED;
#if defined(_MSC_VER) || defined(__c2__)
		}
#endif
		status = phy_read_reg(phy_addr, PHY_AN_STATUS_REGISTER);

		if ((status & (PHY_AN_STATUS_100BASE_T4 | PHY_AN_STATUS_10BASE_T_FULL_DUPLEX | PHY_AN_STATUS_100BASE_TX_FULL_DUPLEX)) != 0)
			*mode = (PHY_MODE_T)(((int)*mode) | 0x01);

		if ((status & (PHY_AN_STATUS_100BASE_T4 | PHY_AN_STATUS_100BASE_TX_FULL_DUPLEX | PHY_AN_STATUS_100BASE_TX_HALF_DUPLEX)) != 0)
			*mode = (PHY_MODE_T)(((int)*mode) | 0x02);

		return PHY_STATE_NEGOTIATED;
	}

	return state;
}

bool_t phy_is_link(uint8_t phy_addr)
{
#if !defined(_MSC_VER) && !defined(__c2__)
	/* リンクの確認 */
	return (phy_read_reg(phy_addr, PHY_STATUS_REGISTER) & PHY_STATUS_LINK_STATUS) != 0;
#else
	return true;
#endif
}

uint16_t phy_read_reg(uint8_t phy_addr, uint8_t reg_addr)
{
	uint16_t result = 0, bit;
	int i;

	/* PRE ：32個の連続した1b */
	for (i = 0; i < 32; i++)
		phy_write_bit(true);

	/* ST ：フレームの先頭を示す01bのライト */
	phy_write_bit(false);
	phy_write_bit(true);

	/* OP ：アクセス種別を示すコードのライト */
	phy_write_bit(true);
	phy_write_bit(false);

	/* PHYAD ：PHY-LSIのアドレスが1番の場合、00001bをライト（MSB から順次ライト）。 */
	phy_write_bit((phy_addr & 0x10) != 0);
	phy_write_bit((phy_addr & 0x08) != 0);
	phy_write_bit((phy_addr & 0x04) != 0);
	phy_write_bit((phy_addr & 0x02) != 0);
	phy_write_bit((phy_addr & 0x01) != 0);

	/* REGAD ：レジスタアドレスが1番の場合、00001bをライト（MSBから順次ライト）。 */
	phy_write_bit((reg_addr & 0x10) != 0);
	phy_write_bit((reg_addr & 0x08) != 0);
	phy_write_bit((reg_addr & 0x04) != 0);
	phy_write_bit((reg_addr & 0x02) != 0);
	phy_write_bit((reg_addr & 0x01) != 0);

	/* TA ：MII/RMIIインタフェース上でデータの送信元を切り替える時間 */
	/* 「バス解放」（Z0と表記）を行う */
	phy_release_bus();

	/* DATA ：16ビットのデータ。MSBから順次リード */
	for (bit = 0x8000; bit != 0; bit >>= 1) {
		if(phy_read_bit())
			result |= bit;
	}

	/* IDLE ：次のMII管理フォーマット入力までの待機時間 */
	/* すでにTA時にバス解放済みであり制御不要 */
	phy_release_bus();

	return result;
}

void phy_write_reg(uint8_t phy_addr, uint8_t reg_addr, uint16_t reg_data)
{
	int i;
	uint16_t bit;

	/* PRE ：32個の連続した1b */
	for (i = 0; i < 32; i++)
		phy_write_bit(true);

	/* ST ：フレームの先頭を示す01bのライト */
	phy_write_bit(false);
	phy_write_bit(true);

	/* OP ：アクセス種別を示すコードのライト */
	phy_write_bit(false);
	phy_write_bit(true);

	/* PHYAD ：PHY-LSIのアドレスが1番の場合、00001bをライト（MSB から順次ライト）。 */
	phy_write_bit((phy_addr & 0x10) != 0);
	phy_write_bit((phy_addr & 0x08) != 0);
	phy_write_bit((phy_addr & 0x04) != 0);
	phy_write_bit((phy_addr & 0x02) != 0);
	phy_write_bit((phy_addr & 0x01) != 0);

	/* REGAD ：レジスタアドレスが1番の場合、00001bをライト（MSBから順次ライト）。 */
	phy_write_bit((reg_addr & 0x10) != 0);
	phy_write_bit((reg_addr & 0x08) != 0);
	phy_write_bit((reg_addr & 0x04) != 0);
	phy_write_bit((reg_addr & 0x02) != 0);
	phy_write_bit((reg_addr & 0x01) != 0);

	/* TA ：MII/RMIIインタフェース上でデータの送信元を切り替える時間 */
	/* 10bをライト */
	phy_write_bit(true);
	phy_write_bit(false);

	/* DATA ：16ビットのデータ。MSBから順次ライト */
	for (bit = 0x8000; bit != 0; bit >>= 1) {
		phy_write_bit((reg_data & bit) != 0);
	}

	/* IDLE ：次のMII管理フォーマット入力までの待機時間 */
	/* 「単独バス解放」（Xと表記）を行う */
	phy_single_rel_bus();
}

#define ETHERC_PIR_WAIT 4

/*
 * 1 ビットデータのライト
 */
static void phy_write_bit(bool_t bit)
{
	int i;
	uint32_t data = bit ? ETHERC_PIR_MDO : 0;

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, data | ETHERC_PIR_MMD | 0/*ETHERC_PIR_MDC*/);

	for(i = ETHERC_PIR_WAIT; i > 0; i--);

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, data | ETHERC_PIR_MMD | ETHERC_PIR_MDC);

	for(i = 2 * ETHERC_PIR_WAIT; i > 0; i--);

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, data | ETHERC_PIR_MMD | 0/*ETHERC_PIR_MDC*/);

	for(i = ETHERC_PIR_WAIT; i > 0; i--);
}

/*
 * バス解放
 */
static void phy_release_bus()
{
	int i;

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, 0/*ETHERC_PIR_MMD*/ | 0/*ETHERC_PIR_MDC*/);

	for(i = ETHERC_PIR_WAIT; i > 0; i--);

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, 0/*ETHERC_PIR_MMD*/ | ETHERC_PIR_MDC);

	for(i = 2 * ETHERC_PIR_WAIT; i > 0; i--);

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, 0/*ETHERC_PIR_MMD*/ | 0/*ETHERC_PIR_MDC*/);

	for(i = ETHERC_PIR_WAIT; i > 0; i--);
}

/*
 * 1 ビットデータのリード
 */
static bool_t phy_read_bit()
{
	bool_t bit;
	int i;

	for(i = ETHERC_PIR_WAIT; i > 0; i--);

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, 0/*ETHERC_PIR_MMD*/ | ETHERC_PIR_MDC);

	for(i = ETHERC_PIR_WAIT; i > 0; i--);

	/* PHY部インタフェースレジスタへのライト */
	bit = (sil_rew_mem(ETHERC_PIR) & ETHERC_PIR_MDI) != 0;

	for(i = ETHERC_PIR_WAIT; i > 0; i--);

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, 0/*ETHERC_PIR_MMD*/ | 0/*ETHERC_PIR_MDC*/);

	for(i = ETHERC_PIR_WAIT; i > 0; i--);

	return bit;
}

/*
 * 単独バス解放
 */
static void phy_single_rel_bus()
{
	int i;

	for(i = ETHERC_PIR_WAIT; i > 0; i--);

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, ETHERC_PIR_MMD | ETHERC_PIR_MDC);

	for(i = 2 * ETHERC_PIR_WAIT; i > 0; i--);

	/* PHY部インタフェースレジスタへのライト */
	sil_wrw_mem(ETHERC_PIR, 0/*ETHERC_PIR_MMD*/ | 0/*ETHERC_PIR_MDC*/);

	for(i = ETHERC_PIR_WAIT; i > 0; i--);
}
