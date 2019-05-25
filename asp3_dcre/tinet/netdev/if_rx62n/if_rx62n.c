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

/*
 * Copyright (c) 1995, David Greenman
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * $FreeBSD: src/sys/i386/isa/if_ed.c,v 1.148.2.4 1999/09/25 13:08:18 nyan Exp $
 */

#ifdef TARGET_KERNEL_ASP

#define CAST(type, val)		((type)(val))

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "kernel/kernel_impl.h"

#endif	/* of #ifdef TARGET_KERNEL_ASP */

#ifdef TARGET_KERNEL_JSP

#include <s_services.h>
#include <t_services.h>
#include "kernel_id.h"

#endif	/* of #ifdef TARGET_KERNEL_JSP */

#include <tinet_defs.h>
#include <tinet_config.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/net.h>
#include <net/net_timer.h>
#include <net/net_count.h>
#include <net/net_buf.h>

#include "if_rx62nreg.h"
#include <string.h>
#include "ether_phy.h"

#ifdef _MSC_VER
#include <stdlib.h>
#endif

#ifndef TMO_IF_RX62N_GET_NET_BUF
#define TMO_IF_RX62N_GET_NET_BUF		ULONG_C(1000000)	/* [us] */
#endif

void mbed_mac_address(char *mac);

/*
 *  ネットワークインタフェースに依存するソフトウェア情報 
 */

typedef struct t_rx62n_softc {
	PHY_STATE_T state;
	T_RX62N_TX_DESC *tx_write;
	T_RX62N_RX_DESC *rx_read;
	bool_t link_pre;
	bool_t link_now;
	bool_t over_flow;
} T_RX62N_SOFTC;

/*
 *  ネットワークインタフェースのソフトウェア情報
 */

/* ネットワークインタフェースに依存するソフトウェア情報 */

static T_RX62N_SOFTC rx62n_softc;

typedef struct t_rx62n_buf {
	uint8_t rx_buff[NUM_IF_RX62N_RXBUF][32 * (((int)&((T_NET_BUF *)0)->buf + IF_RX62N_BUF_PAGE_SIZE + 31) / 32)];
	uint8_t tx_buff[NUM_IF_RX62N_TXBUF][32 * ((IF_RX62N_BUF_PAGE_SIZE + 31) / 32)];
	T_RX62N_RX_DESC rx_desc[NUM_IF_RX62N_RXBUF];
	T_RX62N_TX_DESC tx_desc[NUM_IF_RX62N_TXBUF];
} T_RX62N_BUF;

#if defined(__RX)
#pragma	section	ETH_MEMORY
#define __attribute__(x)
#endif
T_RX62N_BUF __attribute__((section("ETH_MEMORY"),aligned(16))) rx62n_buf;
#if defined(__RX)
#pragma	section
#endif

/* ネットワークインタフェースに依存しないソフトウェア情報 */

T_IF_SOFTC if_softc = {
	{0,},						/* ネットワークインタフェースのアドレス	*/
	0,							/* 送信タイムアウト			*/
	&rx62n_softc,				/* ディバイス依存のソフトウェア情報	*/
	SEM_IF_RX62N_SBUF_READY,	/* 送信セマフォ			*/
	SEM_IF_RX62N_RBUF_READY,	/* 受信セマフォ			*/

#ifdef SUPPORT_INET6

	IF_MADDR_INIT,				/* マルチキャストアドレスリスト	*/

#endif	/* of #ifdef SUPPORT_INET6 */
};

/*
 *  局所変数
 */

static void rx62n_stop (T_RX62N_SOFTC *sc);
static void rx62n_init_sub (T_IF_SOFTC *ic);
static void rx62n_set_ecmr (T_IF_SOFTC *ic, PHY_MODE_T mode);

#ifdef SUPPORT_INET6

static uint32_t ds_crc (uint8_t *addr);
static void ds_getmcaf (T_IF_SOFTC *ic, uint32_t *mcaf);

/*
 *  ds_crc -- イーサネットアドレスの CRC を計算する。
 */

#define POLYNOMIAL	0x04c11db6

static uint32_t
ds_crc (uint8_t *addr)
{
	uint32_t	crc = ULONG_C(0xffffffff);
	int_t		carry, len, bit;
	uint8_t		byte;

	for (len = ETHER_ADDR_LEN; len -- > 0; ) {
		byte = *addr ++;
		for (bit = 8; bit -- > 0; ) {
			carry   = ((crc & ULONG_C(0x80000000)) ? 1 : 0) ^ (byte & UINT_C(0x01));
			crc   <<= 1;
			byte   >>= 1;
			if (carry)
				crc = (crc ^ POLYNOMIAL) | carry;
		}
	}
	return crc;
}

#undef POLYNOMIAL

/*
 *  ds_getmcaf -- マルチキャストアドレスのリストからマルチキャストアドレス
 *                フィルタを計算する。
 */

static void
ds_getmcaf (T_IF_SOFTC *ic, uint32_t *mcaf)
{
	uint32_t	count, index;
	uint8_t		*af = (uint8_t*)mcaf;

	mcaf[0] = mcaf[1] = 0;

	for (count = MAX_IF_MADDR_CNT; count -- > 0; ) {
		index = ds_crc(ic->maddrs[count].lladdr) >> 26;
		af[index >> 3] |= 1 << (index & 7);
	}
}

/*
 * rx62n_setrcr -- 受信構成レジスタ (RCR) を設定する。
 */

static void
rx62n_setrcr (T_IF_SOFTC *ic)
{
	T_RX62N_SOFTC	*sc = ic->sc;
}

/*
 * rx62n_addmulti -- マルチキャストアドレスを追加する。
 */

ER
rx62n_addmulti (T_IF_SOFTC *ic)
{
	rx62n_setrcr(ic);
	return E_OK;
}

#endif	/* of #ifdef SUPPORT_INET6 */

/*
 *  rx62n_stop -- ネットワークインタフェースを停止する。
 * 
 *    注意: NIC 割り込み禁止状態で呼び出すこと。
 */

static void
rx62n_stop (T_RX62N_SOFTC *sc)
{
	/* 動作モードクリア */
	sil_wrw_mem(ETHERC_ECMR, 0x00000000);
	/* リンク変化割り込み無効 */
	sil_wrw_mem(ETHERC_ECSIPR, 0x00000000);
}

/*
 *  rx62n_init_sub -- ネットワークインタフェースの初期化
 * 
 *    注意: NIC 割り込み禁止状態で呼び出すこと。
 */

static void
rx62n_init_sub (T_IF_SOFTC *ic)
{
	T_RX62N_SOFTC *sc = ic->sc;
	uint8_t *mac_addr = ic->ifaddr.lladdr;
	PHY_STATE_T state = PHY_STATE_UNINIT;

	/* MAC部ソフトウエア・リセット */
	sil_wrw_mem(EDMAC_EDMR, sil_rew_mem(EDMAC_EDMR) | EDMAC_EDMR_SWR_BIT);

	dly_tsk(1000);

	sil_wrw_mem(ETHERC_MAHR, ((uint32_t)mac_addr[0] << 24)
		| ((uint32_t)mac_addr[1] << 16) | ((uint32_t)mac_addr[2] << 8 )
		|  (uint32_t)mac_addr[3]);
	sil_wrw_mem(ETHERC_MALR, ((uint32_t)mac_addr[4] << 8 )
		| (uint32_t)mac_addr[5]);

	/* PHYリセット */
	while ((state = phy_reset(state, 0)) != PHY_STATE_RESET);
	sc->state = state;

	/* Clear all ETHERC status BFR, PSRTO, LCHNG, MPD, ICD */
	sil_wrw_mem(ETHERC_ECSR, 0x00000037);

	/* リンク変化割り込み有効 */
	sil_wrw_mem(ETHERC_ECSIPR, sil_rew_mem(ETHERC_ECSIPR) | ETHERC_ECSIPR_LCHNGIP);

	/* Clear all ETHERC and EDMAC status bits */
	sil_wrw_mem(EDMAC_EESR, 0x47FF0F9F);

	/* 送受信割り込み有効 */
	sil_wrw_mem(EDMAC_EESIPR, (EDMAC_EESIPR_TCIP | EDMAC_EESIPR_FRIP | EDMAC_EESIPR_RDEIP | EDMAC_EESIPR_FROFIP));

	/* 受信フレーム長上限（バッファサイズ） */
	sil_wrw_mem(ETHERC_RFLR, IF_RX62N_BUF_PAGE_SIZE);

	/* 96ビット時間（初期値） */
	sil_wrw_mem(ETHERC_IPGR, 0x00000014);

	/* Set little endian mode */
	sil_wrw_mem(EDMAC_EDMR, sil_rew_mem(EDMAC_EDMR) | EDMAC_EDMR_DE_BIT);

	/* Initialize Rx descriptor list address */
	sil_wrw_mem(EDMAC_RDLAR, (uint32_t)rx62n_buf.rx_desc);
	/* Initialize Tx descriptor list address */
	sil_wrw_mem(EDMAC_TDLAR, (uint32_t)rx62n_buf.tx_desc);
	/* Copy-back status is RFE & TFE only */
	sil_wrw_mem(EDMAC_TRSCER, 0x00000000);
	/* Threshold of Tx_FIFO */
	sil_wrw_mem(EDMAC_TFTR, 0x00000000);
	/* Transmit fifo & receive fifo is 2048 bytes */
	sil_wrw_mem(EDMAC_FDR, 0x00000707);
	/* RR in EDRRR is under driver control */
	sil_wrw_mem(EDMAC_RMCR, 0x00000001);

	sc->link_pre = false;
	sc->link_now = false;

#if defined(TARGET_KERNEL_ASP)

	/* ターゲット依存部の割込み初期化 */
	rx62n_inter_init();

#endif	/* of #if defined(TARGET_KERNEL_ASP) */

#if defined(TARGET_KERNEL_JSP) && TKERNEL_PRVER >= 0x1042u	/* JSP-1.4.2 以降 */

	/* ターゲット依存部の割込み初期化 */
	rx62n_inter_init();

#endif	/* of #if defined(TARGET_KERNEL_JSP) && TKERNEL_PRVER >= 0x1042u */
}


/*
 *  rx62n_set_ecmr -- ECMRレジスタの設定
 */

static void
rx62n_set_ecmr (T_IF_SOFTC *ic, PHY_MODE_T mode)
{
	uint32_t ecmr;

	ecmr = ETHERC_ECMR_RE | ETHERC_ECMR_TE/* | ETHERC_ECMR_PRM*/;

	if ((mode & 0x01) != 0)
		ecmr |= ETHERC_ECMR_DM;
	if ((mode & 0x02) != 0)
		ecmr |= ETHERC_ECMR_RTM;

	/* 動作モード設定 */
	sil_wrw_mem(ETHERC_ECMR, ecmr);
}

/*
 * rx62n_reset -- ネットワークインタフェースをリセットする。
 */

void
rx62n_reset (T_IF_SOFTC *ic)
{
#ifdef TARGET_KERNEL_JSP
	IPM	ipm;
#endif

	/* NIC からの割り込みを禁止する。*/
#ifdef TARGET_KERNEL_JSP
	ipm = rx62n_dis_inter();
#endif
#ifdef TARGET_KERNEL_ASP
	syscall(dis_int(INTNO_IF_RX62N_TRX));
#endif
	ether_set_link_down();

	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_RESETS], 1);
	rx62n_stop(ic->sc);
	rx62n_init_sub(ic);

	if (sil_rew_mem(EDMAC_EDRRR) == 0) {
		sil_wrw_mem(EDMAC_EDRRR, EDMAC_EDRRR_RR);
	}

	/* 送受信を有効 */
	rx62n_set_ecmr(ic, 1);

	/* NIC からの割り込みを許可する。*/
#ifdef TARGET_KERNEL_JSP
	rx62n_ena_inter(ipm);
#endif
#ifdef TARGET_KERNEL_ASP
	syscall(ena_int(INTNO_IF_RX62N_TRX));
#endif
}

/*
 *  get_rx62n_softc -- ネットワークインタフェースのソフトウェア情報を返す。
 */

T_IF_SOFTC *
rx62n_get_softc (void)
{
	return &if_softc;
}

/*
 * rx62n_watchdog -- ネットワークインタフェースのワッチドッグタイムアウト
 */

void
rx62n_watchdog (T_IF_SOFTC *ic)
{
	rx62n_reset(ic);
}

/*
 * rx62n_probe -- ネットワークインタフェースの検出
 */

void
rx62n_probe (T_IF_SOFTC *ic)
{
	int i;
#if defined(TARGET_KERNEL_ASP)

	/* ターゲット依存部のバスの初期化 */
	rx62n_bus_init();

#endif	/* of #if defined(TARGET_KERNEL_ASP) */
#if defined(TARGET_KERNEL_JSP) && TKERNEL_PRVER >= 0x1042u	/* JSP-1.4.2 以降 */

	/* ターゲット依存部のバスの初期化 */
	rx62n_bus_init();

#endif	/* of #if defined(TARGET_KERNEL_JSP) && TKERNEL_PRVER >= 0x1042u */

	mbed_mac_address(ic->ifaddr.lladdr);
}

/*
 * rx62n_init -- ネットワークインタフェースの初期化
 */

void
rx62n_init (T_IF_SOFTC *ic)
{
#ifdef TARGET_KERNEL_JSP
	IPM	ipm;
#endif
	T_RX62N_SOFTC *sc = ic->sc;
	T_RX62N_TX_DESC *tdsc;
	T_RX62N_RX_DESC *rdsc;
	int i;

	/* NIC からの割り込みを禁止する。*/
#ifdef TARGET_KERNEL_JSP
	ipm = rx62n_dis_inter();
#endif
#ifdef TARGET_KERNEL_ASP
	syscall(dis_int(INTNO_IF_RX62N_TRX));
#endif

	tdsc = (T_RX62N_TX_DESC *)rx62n_buf.tx_desc;
	sc->tx_write = tdsc;
	for ( i=0 ; i < NUM_IF_RX62N_TXBUF ; i++ ) {
		memset(tdsc, 0, sizeof(*tdsc));
		tdsc->tbl = 0;
		tdsc->tba = (uint32_t)&rx62n_buf.tx_buff[i];
		tdsc++;
	}
	tdsc--;
	tdsc->tdle = 1;

	rdsc = (T_RX62N_RX_DESC *)rx62n_buf.rx_desc;
	sc->rx_read = rdsc;
	for ( i=0 ; i < NUM_IF_RX62N_RXBUF ; i++ ) {
		memset(rdsc, 0, sizeof(*rdsc));
		rdsc->rbl = IF_RX62N_BUF_PAGE_SIZE;
		rdsc->rba = (uint32_t)&rx62n_buf.rx_buff[i] + (uint32_t)&((T_NET_BUF *)0)->buf;
		rdsc->rfl = 0;
		rdsc->ract = 1;
		rdsc++;
	}
	rdsc--;
	rdsc->rdle = 1;

	/* rx62n_init 本体を呼び出す。*/
	rx62n_init_sub(ic);

	if (sil_rew_mem(EDMAC_EDRRR) == 0) {
		sil_wrw_mem(EDMAC_EDRRR, EDMAC_EDRRR_RR);
	}

	/* 送受信を有効 */
	rx62n_set_ecmr(ic, 1);

	/* NIC からの割り込みを許可する。*/
#ifdef TARGET_KERNEL_JSP
	rx62n_ena_inter(ipm);
#endif
#ifdef TARGET_KERNEL_ASP
	syscall(ena_int(INTNO_IF_RX62N_TRX));
#endif

	syscall(sta_cyc(CYC_IF_RX62N_LINK));
}


/* 
 * rx62n_link -- リンク状態の変化に対する処理
 */
bool_t
rx62n_link(T_IF_SOFTC *ic)
{
	T_RX62N_SOFTC *sc = ic->sc;
	PHY_MODE_T mode;
	uint8_t phy_addr = 0;

	if(sc->state == PHY_STATE_NEGOTIATED){
		if(!phy_is_link(phy_addr))
			sc->state = PHY_STATE_RESET;
		return true;
	}

	/* PHYの初期化 */
	sc->state = phy_initialize(sc->state, phy_addr, &mode);
	if(sc->state != PHY_STATE_NEGOTIATED){
		return false;
	}

	/* ECMRレジスタの設定 */
	rx62n_set_ecmr(ic, mode);
	return true;
}

/*
 * rx62n_read -- フレームの読み込み
 */

T_NET_BUF *
rx62n_read (T_IF_SOFTC *ic)
{
	T_RX62N_SOFTC *sc = ic->sc;
	T_RX62N_RX_DESC *desc;
	uint16_t len;
	T_NET_BUF *input = NULL;
	uint16_t align;
	uint8_t *dst;
	ER error;
	PHY_MODE_T mode;

	sc->link_now = rx62n_link(ic);
	if (sc->link_pre != sc->link_now) {
		sc->link_pre = sc->link_now;
		/* リンク状態に変化あり */
		if (sc->link_now) {
			ether_set_link_up();
		}
		else {
			ether_set_link_down();
		}
	}

	if (sc->over_flow) {
		sc->over_flow = false;
	}

	desc = sc->rx_read;

	if (desc->ract != 0) {
		return NULL;
	}

	len = desc->rfl;
	/*
	 *  +-----------+--------+---------+---------+
	 *  | Ehter HDR | IP HDR | TCP HDR | TCP SDU |
	 *  +-----------+--------+---------+---------+
	 *        14        20        20        n
	 *   <----------------- len ---------------->
	 *              ^
	 *              t_net_buf で 4 オクテット境界にアラインされている。
	 *
	 *  tcp_input と udp_input では、擬似ヘッダと SDU でチェックサムを
	 *  計算するが、n が 4 オクテット境界になるように SDU の後ろに 0 を
	 *  パッディングする。その分を考慮して net_buf を獲得しなければならない。
	 */
	align = ((((len - sizeof(T_IF_HDR)) + 3) >> 2) << 2) + sizeof(T_IF_HDR);

	if ((error = tget_net_buf(&input, align, TMO_IF_RX62N_GET_NET_BUF)) == E_OK && input != NULL) {
		dst = input->buf + IF_ETHER_NIC_HDR_ALIGN;
		memcpy(dst, (void *)desc->rba, len);
	}
	else {
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_IN_ERR_PACKETS], 1);
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_NO_BUFS], 1);
	}

	desc->rfp = 0;
	desc->ract = 1;

	desc++;
	if (desc == &rx62n_buf.rx_desc[NUM_IF_RX62N_RXBUF]) {
		desc = rx62n_buf.rx_desc;
	}
	sc->rx_read = desc;

	if (sil_rew_mem(EDMAC_EDRRR) == 0) {
		sil_wrw_mem(EDMAC_EDRRR, EDMAC_EDRRR_RR);
	}

	return input;
}

/*
 * rx62n_start -- 送信フレームをバッファリングする。
 */

void
rx62n_start (T_IF_SOFTC *ic, T_NET_BUF *output)
{
	T_RX62N_SOFTC *sc = ic->sc;
	T_RX62N_TX_DESC *desc, *next;
	uint8_t *buf = NULL;
	int32_t len, res, pos;
	uint32_t tfp;

	for ( res = output->len, pos = 0; res > 0; res -= len, pos += len ) {
		desc = sc->tx_write;

		while (desc->tact != 0) {
			tslp_tsk(1000);
		}

		buf = (uint8_t *)desc->tba + IF_ETHER_NIC_HDR_ALIGN;

		next = desc + 1;
		if (next == &rx62n_buf.tx_desc[NUM_IF_RX62N_TXBUF]) {
			next = rx62n_buf.tx_desc;
		}
		sc->tx_write = next;

		len = res;
		if ( len > IF_RX62N_BUF_PAGE_SIZE ) {
			len = IF_RX62N_BUF_PAGE_SIZE;
			tfp = 0x0;
		}
		else
			tfp = 0x1;

		if (pos == 0)
			tfp |= 0x2;

		memcpy(buf, (uint8_t *)output->buf + pos, len);

		desc->tbl = len;
		desc->tfp = tfp;
		desc->tact = 1;
	}

	if (sil_rew_mem(EDMAC_EDTRR) == 0) {
		sil_wrw_mem(EDMAC_EDTRR, EDMAC_EDTRR_TR);
	}
}

/*
 *  RX62N Ethernet Controler 送受信割り込みハンドラ
 */

void
if_rx62n_trx_handler (void)
{
	T_IF_SOFTC *ic;
	T_RX62N_SOFTC *sc;
	uint32_t ecsr, eesr;

	ic = &if_softc;
	sc = ic->sc;

	ecsr = sil_rew_mem(ETHERC_ECSR);

	if (ecsr & ETHERC_ECSR_LCHNG) {
		/* ETHERC部割り込み要因クリア */
		sil_wrw_mem(ETHERC_ECSR, ETHERC_ECSR_LCHNG);

		/* 受信割り込み処理 */
		isig_sem(ic->semid_rxb_ready);
	}

	eesr = sil_rew_mem(EDMAC_EESR);

	if (eesr & EDMAC_EESR_FR) {
		/* DMA部割り込み要因クリア */
		sil_wrw_mem(EDMAC_EESR, EDMAC_EESR_FR);

		/* 受信割り込み処理 */
		isig_sem(ic->semid_rxb_ready);
	}
	if (eesr & EDMAC_EESR_TC) {
		/* DMA部割り込み要因クリア */
		sil_wrw_mem(EDMAC_EESR, EDMAC_EESR_TC);

		/* 送信割り込み処理 */
		isig_sem(ic->semid_txb_ready);
	}
	if (eesr & (EDMAC_EESR_FROF | EDMAC_EESR_RDE)) {
		/* DMA部割り込み要因クリア */
		sil_wrw_mem(EDMAC_EESR, EDMAC_EESR_FROF | EDMAC_EESR_RDE);

		sc->over_flow = true;

		/* 受信割り込み処理 */
		isig_sem(ic->semid_rxb_ready);
	}
}

void if_rx62n_cyclic_handler(void)
{
	T_IF_SOFTC *ic;
	T_RX62N_SOFTC *sc;

	ic = &if_softc;
	sc = ic->sc;

	/* 受信割り込み処理 */
	isig_sem(ic->semid_rxb_ready);
}
