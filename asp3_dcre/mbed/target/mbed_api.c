/*
 *  TOPPERS PROJECT Home Network Working Group Software
 *
 *  Copyright (C) 2014-2018 Cores Co., Ltd. Japan
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

#include <stdint.h>
#include <time.h>
#include <gpio_api.h>
#include <ticker_api.h>
#include <serial_api.h>
#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "rx630.h"
#include "target_stddef.h"
#include "target_kernel_impl.h"
#include "scif.h"

 /* PWM出力(490Hz) */
#define TPU_BASE_COUNTER	(48000000 / 4 / 490)

void mbed_api_init()
{
}

void error(const char *text)
{
	syslog(LOG_ERROR, text);
}

void core_util_critical_section_enter(void)
{

}

void core_util_critical_section_exit(void)
{

}

void wait_ms(int ms)
{
	dly_tsk(ms * 1000);
}

void us_ticker_init(void)
{
}

void us_ticker_set_interrupt(timestamp_t timestamp)
{
}

void us_ticker_disable_interrupt(void)
{
}

void us_ticker_clear_interrupt(void)
{
}

uint32_t us_ticker_read(void)
{
	SYSTIM result;
	get_tim(&result);
	return result;
}

void gpio_init(gpio_t *obj, PinName pin)
{
	obj->pin = pin;
	obj->dir = PIN_INPUT;
	obj->mode = PullDefault;
}


static bool_t pin_function_table[8] = {
	true, true, true, true, true, true, true, true
};

static ER change_pin_function(int pin, bool_t gpio)
{
	ER result = E_PAR;

	if ((pin < 0) || (pin >= 8))
		return E_PAR;

	if (pin_function_table[pin] == gpio)
		return E_OK;

	/* 汎用入出力ポートに設定 */
	switch (pin) {
	case D0:
		/* P21 */
		sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) & ~PORT_PMR_B1_BIT);
		break;
	case D1:
		/* P20 */
		sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) & ~PORT_PMR_B0_BIT);
		break;
	case D2:
		/* P22 */
		sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) & ~PORT_PMR_B2_BIT);
		break;
	case D3:
		/* P23 */
		sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) & ~PORT_PMR_B3_BIT);
		break;
	case D4:
		/* P24 */
		sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) & ~PORT_PMR_B4_BIT);
		break;
	case D5:
		/* P25 */
		sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) & ~PORT_PMR_B5_BIT);
		break;
	case D6:
		/* P32 */
		sil_wrb_mem(PORT3_PMR_ADDR, sil_reb_mem(PORT3_PMR_ADDR) & ~PORT_PMR_B2_BIT);
		break;
	case D7:
		/* P33 */
		sil_wrb_mem(PORT3_PMR_ADDR, sil_reb_mem(PORT3_PMR_ADDR) & ~PORT_PMR_B3_BIT);
		break;
	default:
		break;
	}

	/* 書き込みプロテクトレジスタの設定 PFSWEビットへの書き込みを許可 */
	sil_wrb_mem(MPC_PWPR_ADDR, 0x00);
	/* 書き込みプロテクトレジスタの設定 PxxFSレジスタへの書き込みを許可 */
	sil_wrb_mem(MPC_PWPR_ADDR, 0x40);

	switch (pin) {
		/* P21/TIOCA3 */
	case D0:
		if (gpio) {
			/* P21端子機能制御レジスタ P21とする */
			sil_wrb_mem(MPC_P21PFS_ADDR, 0x00);
		}
		else {
			/* P21端子機能制御レジスタ TIOCA3とする */
			sil_wrb_mem(MPC_P21PFS_ADDR, 0x03);
		}
		result = E_OK;
		break;
		/* P20/TIOCB3 */
	case D1:
		if (gpio) {
			/* P20端子機能制御レジスタ P20とする */
			sil_wrb_mem(MPC_P20PFS_ADDR, 0x00);
		}
		else {
			/* P20端子機能制御レジスタ TIOCB3とする */
			sil_wrb_mem(MPC_P20PFS_ADDR, 0x03);
		}
		result = E_OK;
		break;
		/* P22/TIOCC3 */
	case D2:
		if (gpio) {
			/* P22端子機能制御レジスタ P22とする */
			sil_wrb_mem(MPC_P22PFS_ADDR, 0x00);
		}
		else {
			/* P22端子機能制御レジスタ TIOCC3とする */
			sil_wrb_mem(MPC_P22PFS_ADDR, 0x03);
		}
		result = E_OK;
		break;
		/* P23/TIOCD3 */
	case D3:
		if (gpio) {
			/* P23端子機能制御レジスタ P23とする */
			sil_wrb_mem(MPC_P23PFS_ADDR, 0x00);
		}
		else {
			/* P23端子機能制御レジスタ TIOCD3とする */
			sil_wrb_mem(MPC_P23PFS_ADDR, 0x03);
		}
		result = E_OK;
		break;
		/* P24/TIOCB4 */
	case D4:
		if (gpio) {
			/* P24端子機能制御レジスタ P24とする */
			sil_wrb_mem(MPC_P24PFS_ADDR, 0x00);
		}
		else {
			/* P24端子機能制御レジスタ TIOCB4とする */
			sil_wrb_mem(MPC_P24PFS_ADDR, 0x03);
		}
		result = E_OK;
		break;
		/* P25/TIOCA4 */
	case D5:
		if (gpio) {
			/* P25端子機能制御レジスタ P25とする */
			sil_wrb_mem(MPC_P25PFS_ADDR, 0x00);
		}
		else {
			/* P25端子機能制御レジスタ TIOCA4とする */
			sil_wrb_mem(MPC_P25PFS_ADDR, 0x03);
		}
		result = E_OK;
		break;
		/* P32/TIOCC0 */
	case D6:
		if (gpio) {
			/* P32端子機能制御レジスタ P32とする */
			sil_wrb_mem(MPC_P32PFS_ADDR, 0x00);
		}
		else {
			/* P32端子機能制御レジスタ TIOCC0とする */
			sil_wrb_mem(MPC_P32PFS_ADDR, 0x03);
		}
		result = E_OK;
		break;
		/* P33/TIOCD0 */
	case D7:
		if (gpio) {
			/* P33端子機能制御レジスタ P33とする */
			sil_wrb_mem(MPC_P33PFS_ADDR, 0x00);
		}
		else {
			/* P33端子機能制御レジスタ TIOCD0とする */
			sil_wrb_mem(MPC_P33PFS_ADDR, 0x03);
		}
		result = E_OK;
		break;
	default:
		break;
	}

	/* 書き込みプロテクトレジスタの設定 書き込みを禁止 */
	sil_wrb_mem(MPC_PWPR_ADDR, 0x80);

	/* 機能ポートに設定 */
	if (!gpio) {
		switch (pin) {
			/* P21/TIOCA3 */
		case D0:
			sil_wrb_mem(PORT2_PDR_ADDR, sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B1_BIT);
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B1_BIT);
			break;
			/* P20/TIOCB3 */
		case D1:
			sil_wrb_mem(PORT2_PDR_ADDR, sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B0_BIT);
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B0_BIT);
			break;
			/* P22/TIOCC3 */
		case D2:
			sil_wrb_mem(PORT2_PDR_ADDR, sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B2_BIT);
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B2_BIT);
			break;
			/* P23/TIOCD3 */
		case D3:
			sil_wrb_mem(PORT2_PDR_ADDR, sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B3_BIT);
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B3_BIT);
			break;
			/* P24/TIOCB4 */
		case D4:
			sil_wrb_mem(PORT2_PDR_ADDR, sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B4_BIT);
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B4_BIT);
			break;
			/* P25/TIOCA4 */
		case D5:
			sil_wrb_mem(PORT2_PDR_ADDR, sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B5_BIT);
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B5_BIT);
			break;
			/* P32/TIOCC0 */
		case D6:
			sil_wrb_mem(PORT3_PDR_ADDR, sil_reb_mem(PORT3_PDR_ADDR) | PORT_PDR_B2_BIT);
			sil_wrb_mem(PORT3_PMR_ADDR, sil_reb_mem(PORT3_PMR_ADDR) | PORT_PMR_B2_BIT);
			break;
			/* P33/TIOCD0 */
		case D7:
			sil_wrb_mem(PORT3_PDR_ADDR, sil_reb_mem(PORT3_PDR_ADDR) | PORT_PDR_B3_BIT);
			sil_wrb_mem(PORT3_PMR_ADDR, sil_reb_mem(PORT3_PMR_ADDR) | PORT_PMR_B3_BIT);
			break;
		default:
			break;
		}
	}

	pin_function_table[pin] = gpio;

	return result;
}

static void change_pin_mode(gpio_t *obj)
{
	volatile __evenaccess uint8_t *pdr; int bit;

	switch (obj->pin) {
	case D0:
		/* P21 */
		pdr = PORT2_PDR_ADDR; bit = PORT_PDR_B1_BIT;
		break;
	case D1:
		/* P20 */
		pdr = PORT2_PDR_ADDR; bit = PORT_PDR_B0_BIT;
		break;
	case D2:
		/* P22 */
		pdr = PORT2_PDR_ADDR; bit = PORT_PDR_B2_BIT;
		break;
	case D3:
		/* P23 */
		pdr = PORT2_PDR_ADDR; bit = PORT_PDR_B3_BIT;
		break;
	case D4:
		/* P24 */
		pdr = PORT2_PDR_ADDR; bit = PORT_PDR_B4_BIT;
		break;
	case D5:
		/* P25 */
		pdr = PORT2_PDR_ADDR; bit = PORT_PDR_B5_BIT;
		break;
	case D6:
		/* P32 */
		pdr = PORT3_PDR_ADDR; bit = PORT_PDR_B2_BIT;
		break;
	case D7:
		/* P33 */
		pdr = PORT3_PDR_ADDR; bit = PORT_PDR_B3_BIT;
		break;
	case D8:
		/* PC2 */
		pdr = PORTC_PDR_ADDR; bit = PORT_PDR_B2_BIT;
		break;
	case D9:
		/* PC3 */
		pdr = PORTC_PDR_ADDR; bit = PORT_PDR_B3_BIT;
		break;
	case D10:
		/* PC4 */
		pdr = PORTC_PDR_ADDR; bit = PORT_PDR_B4_BIT;
		break;
	case D11:
		/* PC6 */
		pdr = PORTC_PDR_ADDR; bit = PORT_PDR_B6_BIT;
		break;
	case D12:
		/* PC7 */
		pdr = PORTC_PDR_ADDR; bit = PORT_PDR_B7_BIT;
		break;
	case D13:
		/* PC5 */
		pdr = PORTC_PDR_ADDR; bit = PORT_PDR_B5_BIT;
		break;
	default:
		return;
	}

	volatile __evenaccess uint8_t *pcr = pdr + (PORT0_PCR_ADDR - PORT0_PDR_ADDR);

	switch (obj->dir) {
	case PIN_INPUT:
		sil_wrb_mem(pdr, sil_reb_mem(pdr) & ~bit);
		break;
	case PIN_OUTPUT:
		sil_wrb_mem(pdr, sil_reb_mem(pdr) | bit);
		break;
	default:
		break;
	}

	switch (obj->mode) {
	case PullUp:
		sil_wrb_mem(pcr, sil_reb_mem(pcr) | bit);
		break;
	default:
		sil_wrb_mem(pcr, sil_reb_mem(pcr) & ~bit);
		break;
	}
}

void gpio_mode(gpio_t *obj, PinMode mode)
{
	/* 汎用入出力ポートに設定 */
	change_pin_function(obj->pin, true);

	obj->mode = mode;

	change_pin_mode(obj);
}

void gpio_dir(gpio_t *obj, PinDirection direction)
{
	/* 汎用入出力ポートに設定 */
	change_pin_function(obj->pin, true);

	obj->dir = direction;

	change_pin_mode(obj);
}

void gpio_write(gpio_t *obj, int value)
{
	volatile __evenaccess uint8_t *podr;
	int bit;

	switch (obj->pin) {
	case D0:
		/* P21 */
		podr = PORT2_PODR_ADDR; bit = PORT_PODR_B1_BIT;
		break;
	case D1:
		/* P20 */
		podr = PORT2_PODR_ADDR; bit = PORT_PODR_B0_BIT;
		break;
	case D2:
		/* P22 */
		podr = PORT2_PODR_ADDR; bit = PORT_PODR_B2_BIT;
		break;
	case D3:
		/* P23 */
		podr = PORT2_PODR_ADDR; bit = PORT_PODR_B3_BIT;
		break;
	case D4:
		/* P24 */
		podr = PORT2_PODR_ADDR; bit = PORT_PODR_B4_BIT;
		break;
	case D5:
		/* P25 */
		podr = PORT2_PODR_ADDR; bit = PORT_PODR_B5_BIT;
		break;
	case D6:
		/* P32 */
		podr = PORT3_PODR_ADDR; bit = PORT_PODR_B2_BIT;
		break;
	case D7:
		/* P33 */
		podr = PORT3_PODR_ADDR; bit = PORT_PODR_B3_BIT;
		break;
	case D8:
		/* PC2 */
		podr = PORTC_PODR_ADDR; bit = PORT_PODR_B2_BIT;
		break;
	case D9:
		/* PC3 */
		podr = PORTC_PODR_ADDR; bit = PORT_PODR_B3_BIT;
		break;
	case D10:
		/* PC4 */
		podr = PORTC_PODR_ADDR; bit = PORT_PODR_B4_BIT;
		break;
	case D11:
		/* PC6 */
		podr = PORTC_PODR_ADDR; bit = PORT_PODR_B6_BIT;
		break;
	case D12:
		/* PC7 */
		podr = PORTC_PODR_ADDR; bit = PORT_PODR_B7_BIT;
		break;
	case D13:
		/* PC5 */
		podr = PORTC_PODR_ADDR; bit = PORT_PODR_B5_BIT;
		break;
	default:
		return;
	}

	switch (value) {
	case 0:
		sil_wrb_mem(podr, sil_reb_mem(podr) & ~bit);
		break;
	case 1:
		sil_wrb_mem(podr, sil_reb_mem(podr) | bit);
		break;
	}
}

int gpio_read(gpio_t *obj)
{
	volatile __evenaccess uint8_t *pidr;
	int bit;

	switch (obj->pin) {
	case D0:
		/* P21 */
		pidr = PORT2_PIDR_ADDR; bit = PORT_PIDR_B1_BIT;
		break;
	case D1:
		/* P20 */
		pidr = PORT2_PIDR_ADDR; bit = PORT_PIDR_B0_BIT;
		break;
	case D2:
		/* P22 */
		pidr = PORT2_PIDR_ADDR; bit = PORT_PIDR_B2_BIT;
		break;
	case D3:
		/* P23 */
		pidr = PORT2_PIDR_ADDR; bit = PORT_PIDR_B3_BIT;
		break;
	case D4:
		/* P24 */
		pidr = PORT2_PIDR_ADDR; bit = PORT_PIDR_B4_BIT;
		break;
	case D5:
		/* P25 */
		pidr = PORT2_PIDR_ADDR; bit = PORT_PIDR_B5_BIT;
		break;
	case D6:
		/* P32 */
		pidr = PORT3_PIDR_ADDR; bit = PORT_PIDR_B2_BIT;
		break;
	case D7:
		/* P33 */
		pidr = PORT3_PIDR_ADDR; bit = PORT_PIDR_B3_BIT;
		break;
	case D8:
		/* PC2 */
		pidr = PORTC_PIDR_ADDR; bit = PORT_PIDR_B2_BIT;
		break;
	case D9:
		/* PC3 */
		pidr = PORTC_PIDR_ADDR; bit = PORT_PIDR_B3_BIT;
		break;
	case D10:
		/* PC4 */
		pidr = PORTC_PIDR_ADDR; bit = PORT_PIDR_B4_BIT;
		break;
	case D11:
		/* PC6 */
		pidr = PORTC_PIDR_ADDR; bit = PORT_PIDR_B6_BIT;
		break;
	case D12:
		/* PC7 */
		pidr = PORTC_PIDR_ADDR; bit = PORT_PIDR_B7_BIT;
		break;
	case D13:
		/* PC5 */
		pidr = PORTC_PIDR_ADDR; bit = PORT_PIDR_B5_BIT;
		break;
	default:
		return 0;
	}

	if ((sil_reb_mem(pidr) & bit) != 0) {
		return 1;
	}
	else {
		return 0;
	}
}

void pin_function(PinName pin, int function)
{
}

void pin_mode(PinName pin, PinMode mode)
{

}

#define arduino_ad_table_count 64
static uint16_t arduino_ad_table[8][arduino_ad_table_count];
static int arduino_ad_pos = 0;
static uint32_t arduino_ad_avelage[8];

void analogin_init(analogin_t *obj, PinName pin)
{
	/*
	* モジュールストップ機能の設定(S12AD)
	*/
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRA_ADDR,
		sil_rew_mem(SYSTEM_MSTPCRA_ADDR) & ~SYSTEM_MSTPCRA_MSTPA17_BIT);
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA500);	/* 書込み禁止 */

	/* 12bitADC初期化 */
	sil_wrh_mem(S12AD_ADEXICR_ADDR, 0x0000);	/* 温度センサ出力、内部基準電圧非選択 */
	sil_wrh_mem(S12AD_ADANS0_ADDR, 0x00FF);		/* 変換ポートAN000～AN007選択、AN008～AN015非選択 */
	sil_wrh_mem(S12AD_ADANS1_ADDR, 0x0000);		/* 変換ポートAN016～AN020非選択 */
}

uint16_t analogin_read_u16(analogin_t *obj)
{
	uint16_t result = 0;

	switch (obj->pin) {
	case A0:
		/* *value = sil_reh_mem(S12AD_ADDR0_ADDR); */
		result = (arduino_ad_avelage[0] / (arduino_ad_table_count));
		break;
	case A1:
		/* *value = sil_reh_mem(S12AD_ADDR1_ADDR); */
		result = (arduino_ad_avelage[1] / (arduino_ad_table_count));
		break;
	case A2:
		/* *value = sil_reh_mem(S12AD_ADDR2_ADDR); */
		result = (arduino_ad_avelage[2] / (arduino_ad_table_count));
		break;
	case A3:
		/* *value = sil_reh_mem(S12AD_ADDR3_ADDR); */
		result = (arduino_ad_avelage[3] / (arduino_ad_table_count));
		break;
	case A4:
		/* *value = sil_reh_mem(S12AD_ADDR4_ADDR); */
		result = (arduino_ad_avelage[4] / (arduino_ad_table_count));
		break;
	case A5:
		/* *value = sil_reh_mem(S12AD_ADDR5_ADDR); */
		result = (arduino_ad_avelage[5] / (arduino_ad_table_count));
		break;
	case A6:
		/* *value = sil_reh_mem(S12AD_ADDR6_ADDR); */
		result = (arduino_ad_avelage[6] / (arduino_ad_table_count));
		break;
	case A7:
		/* *value = sil_reh_mem(S12AD_ADDR7_ADDR); */
		result = (arduino_ad_avelage[7] / (arduino_ad_table_count));
		break;
	default:
		return 0;
	}

	return 65535 - ((result << 4) | (result >> 8));
}

void arduino_tick()
{
	static volatile __evenaccess uint16_t *const regs[8] = {
		S12AD_ADDR0_ADDR,
		S12AD_ADDR1_ADDR,
		S12AD_ADDR2_ADDR,
		S12AD_ADDR3_ADDR,
		S12AD_ADDR4_ADDR,
		S12AD_ADDR5_ADDR,
		S12AD_ADDR6_ADDR,
		S12AD_ADDR7_ADDR
	};
	uint16_t ad_value;
	int i;

	/* ADCの変換結果取得 */
	if ((sil_reb_mem(S12AD_ADCSR_ADDR) & S12AD_ADCSR_ADST_BIT) == 0) {
		for (i = 0; i < 8; i++) {
			arduino_ad_avelage[i] -= arduino_ad_table[i][arduino_ad_pos];
			ad_value = sil_reh_mem(regs[i]);
			arduino_ad_table[i][arduino_ad_pos] = ad_value;
			arduino_ad_avelage[i] += ad_value;
		}

		arduino_ad_pos++;
		if (arduino_ad_pos >= arduino_ad_table_count) {
			arduino_ad_pos = 0;
		}

		/* 変換開始（シングルスキャンモード） */
		sil_wrb_mem(S12AD_ADCSR_ADDR, S12AD_ADCSR_ADST_BIT);
	}
}

void analogout_init(dac_t *obj, PinName pin)
{
}

void analogout_write_u16(dac_t *obj, uint16_t value)
{
}

void pwmout_init(pwmout_t* obj, PinName pin)
{
	/* PWM出力(490Hz) */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRA_ADDR,
		sil_rew_mem(SYSTEM_MSTPCRA_ADDR) & ~SYSTEM_MSTPCRA_MSTPA13_BIT); /* TPU0～TPU5 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA500);	/* 書込み禁止 */

	/* カウンタクロックの選択、カウンタクリア要因の選択 */
	sil_wrb_mem(TPU0_TCR_ADDR, (1 << TPU_TCR_TPSC_OFFSET) | (1 << TPU_TCR_CKEG_OFFSET)
		| (1 << TPU_TCR_CCLR_OFFSET));
	sil_wrb_mem(TPU3_TCR_ADDR, (1 << TPU_TCR_TPSC_OFFSET) | (1 << TPU_TCR_CKEG_OFFSET)
		| (3 << TPU_TCR_CCLR_OFFSET));
	sil_wrb_mem(TPU4_TCR_ADDR, (1 << TPU_TCR_TPSC_OFFSET) | (1 << TPU_TCR_CKEG_OFFSET)
		| (3 << TPU_TCR_CCLR_OFFSET));
	/* 波形出力レベルの選択 */
	sil_wrb_mem(TPU0_TIORL_ADDR, (5 << TPU_TIORL_IOC_OFFSET) | (5 << TPU_TIORL_IOD_OFFSET));
	sil_wrb_mem(TPU3_TIORH_ADDR, (5 << TPU_TIORH_IOA_OFFSET) | (5 << TPU_TIORH_IOB_OFFSET));
	sil_wrb_mem(TPU3_TIORL_ADDR, (5 << TPU_TIORL_IOC_OFFSET) | (5 << TPU_TIORL_IOD_OFFSET));
	sil_wrb_mem(TPU4_TIOR_ADDR, (5 << TPU_TIOR_IOA_OFFSET) | (5 << TPU_TIOR_IOB_OFFSET));
	/* TGRyの設定 */
	sil_wrh_mem(TPU0_TGRA_ADDR, TPU_BASE_COUNTER);
	sil_wrh_mem(TPU0_TGRC_ADDR, 0);
	sil_wrh_mem(TPU0_TGRD_ADDR, 0);
	sil_wrh_mem(TPU3_TGRA_ADDR, 0);
	sil_wrh_mem(TPU3_TGRB_ADDR, 0);
	sil_wrh_mem(TPU3_TGRC_ADDR, 0);
	sil_wrh_mem(TPU3_TGRD_ADDR, 0);
	sil_wrh_mem(TPU4_TGRA_ADDR, 0);
	sil_wrh_mem(TPU4_TGRB_ADDR, 0);
	/* PWMモード2の設定 */
	sil_wrb_mem(TPU0_TMDR_ADDR, 3);
	sil_wrb_mem(TPU3_TMDR_ADDR, 3);
	sil_wrb_mem(TPU4_TMDR_ADDR, 3);
	/* 同期動作設定 */
	sil_wrb_mem(TPUA_TSYR_ADDR, TPU_TSYR_SYNC0_BIT | TPU_TSYR_SYNC3_BIT | TPU_TSYR_SYNC4_BIT);
	/* カウント動作開始 */
	sil_wrb_mem(TPUA_TSTR_ADDR, TPU_TSTR_CST0_BIT | TPU_TSTR_CST3_BIT | TPU_TSTR_CST4_BIT);
}

void pwmout_free(pwmout_t* obj)
{
	/* カウント動作終了 */
	sil_wrb_mem(TPUA_TSTR_ADDR, 0);
}

void pwmout_period_us(pwmout_t* obj, int us)
{
}

void pwmout_pulsewidth_us(pwmout_t* obj, int us)
{
}

typedef enum {
	sciSerial,
	sciI2C,
	sciSPIM,
	sciSPIS,
} sci_mode_t;

void sci_enable(void *base_address, sci_mode_t mode)
{
	/*
	 *  モジュールストップ機能の設定
	 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRB_ADDR,
		sil_rew_mem(SYSTEM_MSTPCRB_ADDR) & ~SCI_MSTPCRB_BIT(base_address));
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA500);	/* 書込み禁止 */

	/* 送受信禁止, SCKn端子は入出力ポートとして使用 */
	sil_wrb_mem(SCI_SCR(base_address), 0x00U);

	switch ((uint32_t)base_address) {
	/* TXD/SDA/MOSI:P20, RXD/SCL/MISO:P21, SCK:P22 */
	case SCI0_BASE:
		switch (mode) {
		case sciSerial:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT2_PODR_ADDR,
				sil_reb_mem(PORT2_PODR_ADDR) | PORT_PODR_B0_BIT | PORT_PODR_B1_BIT);
			/* TX出力/RX入力ポート設定 */
			sil_wrb_mem(PORT2_PDR_ADDR, (sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B0_BIT) & ~PORT_PDR_B1_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORT2_PCR_ADDR, sil_reb_mem(PORT2_PCR_ADDR) & ~(PORT_PCR_B0_BIT | PORT_PCR_B1_BIT));
			/* RX CMOS */
			sil_wrb_mem(PORT2_ODR0_ADDR,
				(sil_reb_mem(PORT2_ODR0_ADDR) & ~(PORT_ODR_Pm0_MASK | PORT_ODR_Pm1_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm0_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm1_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B0_BIT | PORT_PMR_B1_BIT);
			break;
		case sciI2C:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT2_PODR_ADDR,
				sil_reb_mem(PORT2_PODR_ADDR) | PORT_PODR_B0_BIT | PORT_PODR_B1_BIT);
			/* SCL,SDA 出力ポート設定 */
			sil_wrb_mem(PORT2_PDR_ADDR, sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B0_BIT | PORT_PDR_B1_BIT);
			/* プルアップ有効 */
			sil_wrb_mem(PORT2_PCR_ADDR, sil_reb_mem(PORT2_PCR_ADDR) | PORT_PCR_B0_BIT | PORT_PCR_B1_BIT);
			/* Nチャネルオープンドレイン */
			sil_wrb_mem(PORT2_ODR0_ADDR,
				(sil_reb_mem(PORT2_ODR0_ADDR) & ~(PORT_ODR_Pm0_MASK | PORT_ODR_Pm1_MASK))
				| (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm0_OFFSET) | (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm1_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B0_BIT | PORT_PMR_B1_BIT);
			break;
		case sciSPIM:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT2_PODR_ADDR,
				sil_reb_mem(PORT2_PODR_ADDR) | PORT_PODR_B0_BIT | PORT_PODR_B1_BIT | PORT_PODR_B2_BIT);
			/* MOSI出力/MISO入力/SCK入出力ポート設定 */
			sil_wrb_mem(PORT2_PDR_ADDR, (sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B0_BIT | PORT_PDR_B2_BIT) & ~PORT_PDR_B1_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORT2_PCR_ADDR, sil_reb_mem(PORT2_PCR_ADDR) & ~(PORT_PCR_B0_BIT | PORT_PCR_B1_BIT | PORT_PCR_B2_BIT));
			/* MISO CMOS */
			sil_wrb_mem(PORT2_ODR0_ADDR,
				(sil_reb_mem(PORT2_ODR0_ADDR) & ~(PORT_ODR_Pm0_MASK | PORT_ODR_Pm1_MASK | PORT_ODR_Pm2_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm0_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm1_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm2_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B0_BIT | PORT_PMR_B1_BIT | PORT_PMR_B2_BIT);
			break;
		}
		/* 書き込みプロテクトレジスタの設定 PFSWEビットへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x00);
		/* 書き込みプロテクトレジスタの設定 PxxFSレジスタへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x40);

		/* P20端子機能制御レジスタ TXD0/SMOSI0/SSDA0とする */
		sil_wrb_mem(MPC_P20PFS_ADDR, 0x0A);
		/* P21端子機能制御レジスタ RXD0/SMISO0/SSCL0とする */
		sil_wrb_mem(MPC_P21PFS_ADDR, 0x0A);
		if (mode == sciSPIM) {
			/* P22端子機能制御レジスタ SCK0とする */
			sil_wrb_mem(MPC_P22PFS_ADDR, 0x0A);
		}

		/* 書き込みプロテクトレジスタの設定 書き込みを禁止 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x80);
		break;
	/* TXD/SDA/MOSI:P50, RXD/SCL/MISO:P52, SCK:P51 */
	case SCI2_BASE:
		switch (mode) {
		case sciSerial:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT5_PODR_ADDR,
				sil_reb_mem(PORT5_PODR_ADDR) | PORT_PODR_B0_BIT | PORT_PODR_B2_BIT);
			/* TX出力/RX入力ポート設定 */
			sil_wrb_mem(PORT5_PDR_ADDR, (sil_reb_mem(PORT5_PDR_ADDR) | PORT_PDR_B0_BIT) & ~PORT_PDR_B2_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORT5_PCR_ADDR, sil_reb_mem(PORT5_PCR_ADDR) & ~(PORT_PCR_B0_BIT | PORT_PCR_B2_BIT));
			/* RX CMOS */
			sil_wrb_mem(PORT5_ODR0_ADDR,
				(sil_reb_mem(PORT5_ODR0_ADDR) & ~(PORT_ODR_Pm0_MASK | PORT_ODR_Pm2_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm0_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm2_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT5_PMR_ADDR, sil_reb_mem(PORT5_PMR_ADDR) | PORT_PMR_B0_BIT | PORT_PMR_B2_BIT);
			break;
		case sciI2C:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT5_PODR_ADDR,
				sil_reb_mem(PORT5_PODR_ADDR) | PORT_PODR_B0_BIT | PORT_PODR_B2_BIT);
			/* SCL,SDA 出力ポート設定 */
			sil_wrb_mem(PORT5_PDR_ADDR, sil_reb_mem(PORT5_PDR_ADDR) | PORT_PDR_B0_BIT | PORT_PDR_B2_BIT);
			/* プルアップ有効 */
			sil_wrb_mem(PORT5_PCR_ADDR, sil_reb_mem(PORT5_PCR_ADDR) | PORT_PCR_B0_BIT | PORT_PCR_B2_BIT);
			/* Nチャネルオープンドレイン */
			sil_wrb_mem(PORT5_ODR0_ADDR,
				(sil_reb_mem(PORT5_ODR0_ADDR) & ~(PORT_ODR_Pm0_MASK | PORT_ODR_Pm2_MASK))
				| (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm0_OFFSET) | (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm2_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT5_PMR_ADDR, sil_reb_mem(PORT5_PMR_ADDR) | PORT_PMR_B0_BIT | PORT_PMR_B2_BIT);
			break;
		case sciSPIM:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT5_PODR_ADDR,
				sil_reb_mem(PORT5_PODR_ADDR) | PORT_PODR_B0_BIT | PORT_PODR_B1_BIT | PORT_PODR_B2_BIT);
			/* MOSI出力/MISO入力/SCK入出力ポート設定 */
			sil_wrb_mem(PORT5_PDR_ADDR, (sil_reb_mem(PORT5_PDR_ADDR) | PORT_PDR_B0_BIT | PORT_PDR_B1_BIT) & ~PORT_PDR_B2_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORT5_PCR_ADDR, sil_reb_mem(PORT5_PCR_ADDR) & ~(PORT_PCR_B0_BIT | PORT_PCR_B1_BIT | PORT_PCR_B2_BIT));
			/* MISO CMOS */
			sil_wrb_mem(PORT5_ODR0_ADDR,
				(sil_reb_mem(PORT5_ODR0_ADDR) & ~(PORT_ODR_Pm0_MASK | PORT_ODR_Pm1_MASK | PORT_ODR_Pm2_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm0_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm1_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm2_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT5_PMR_ADDR, sil_reb_mem(PORT5_PMR_ADDR) | PORT_PMR_B0_BIT | PORT_PMR_B1_BIT | PORT_PMR_B1_BIT | PORT_PMR_B2_BIT);
			break;
		}
		/* 書き込みプロテクトレジスタの設定 PFSWEビットへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x00);
		/* 書き込みプロテクトレジスタの設定 PxxFSレジスタへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x40);

		/* P50端子機能制御レジスタ SSDA2とする */
		sil_wrb_mem(MPC_P50PFS_ADDR, 0x0A);
		/* P52端子機能制御レジスタ SSCL2とする */
		sil_wrb_mem(MPC_P52PFS_ADDR, 0x0A);
		if (mode == sciSPIM) {
			/* P51端子機能制御レジスタ SCK2とする */
			sil_wrb_mem(MPC_P51PFS_ADDR, 0x0A);
		}

		/* 書き込みプロテクトレジスタの設定 書き込みを禁止 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x80);
		break;
	/* TXD/SDA/MOSI:P23, RXD/SCL/MISO:P25, SCK:P24 */
	case SCI3_BASE:
		switch (mode) {
		case sciSerial:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT2_PODR_ADDR,
				sil_reb_mem(PORT2_PODR_ADDR) | PORT_PODR_B3_BIT | PORT_PODR_B5_BIT);
			/* TX出力/RX入力ポート設定 */
			sil_wrb_mem(PORT2_PDR_ADDR, (sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B3_BIT) & ~PORT_PDR_B5_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORT2_PCR_ADDR, sil_reb_mem(PORT2_PCR_ADDR) & ~(PORT_PCR_B3_BIT | PORT_PCR_B5_BIT));
			/* RX CMOS */
			sil_wrb_mem(PORT2_ODR0_ADDR,
				(sil_reb_mem(PORT2_ODR0_ADDR) & ~(PORT_ODR_Pm3_MASK | PORT_ODR_Pm5_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm3_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm5_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B3_BIT | PORT_PMR_B5_BIT);
			break;
		case sciI2C:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT2_PODR_ADDR,
				sil_reb_mem(PORT2_PODR_ADDR) | PORT_PODR_B3_BIT | PORT_PODR_B5_BIT);
			/* SCL,SDA 出力ポート設定 */
			sil_wrb_mem(PORT2_PDR_ADDR, sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B3_BIT | PORT_PDR_B5_BIT);
			/* プルアップ有効 */
			sil_wrb_mem(PORT2_PCR_ADDR, sil_reb_mem(PORT2_PCR_ADDR) | PORT_PCR_B3_BIT | PORT_PCR_B5_BIT);
			/* Nチャネルオープンドレイン */
			sil_wrb_mem(PORT2_ODR0_ADDR,
				(sil_reb_mem(PORT2_ODR0_ADDR) & ~(PORT_ODR_Pm3_MASK | PORT_ODR_Pm5_MASK))
				| (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm3_OFFSET) | (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm5_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B3_BIT | PORT_PMR_B5_BIT);
			break;
		case sciSPIM:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT2_PODR_ADDR,
				sil_reb_mem(PORT2_PODR_ADDR) | PORT_PODR_B3_BIT | PORT_PODR_B4_BIT | PORT_PODR_B5_BIT);
			/* MOSI出力/MISO入力/SCK入出力ポート設定 */
			sil_wrb_mem(PORT2_PDR_ADDR, (sil_reb_mem(PORT2_PDR_ADDR) | PORT_PDR_B3_BIT | PORT_PDR_B4_BIT) & ~PORT_PDR_B5_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORT2_PCR_ADDR, sil_reb_mem(PORT2_PCR_ADDR) & ~(PORT_PCR_B3_BIT | PORT_PCR_B4_BIT | PORT_PCR_B5_BIT));
			/* MISO CMOS */
			sil_wrb_mem(PORT2_ODR0_ADDR,
				(sil_reb_mem(PORT2_ODR0_ADDR) & ~(PORT_ODR_Pm3_MASK | PORT_ODR_Pm4_MASK | PORT_ODR_Pm5_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm3_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm4_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm5_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT2_PMR_ADDR, sil_reb_mem(PORT2_PMR_ADDR) | PORT_PMR_B3_BIT | PORT_PMR_B4_BIT | PORT_PMR_B4_BIT | PORT_PMR_B5_BIT);
			break;
		}
		/* 書き込みプロテクトレジスタの設定 PFSWEビットへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x00);
		/* 書き込みプロテクトレジスタの設定 PxxFSレジスタへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x40);

		/* P23端子機能制御レジスタ SSDA2とする */
		sil_wrb_mem(MPC_P23PFS_ADDR, 0x0A);
		/* P25端子機能制御レジスタ SSCL2とする */
		sil_wrb_mem(MPC_P25PFS_ADDR, 0x0A);
		if (mode == sciSPIM) {
			/* P24端子機能制御レジスタ SCK2とする */
			sil_wrb_mem(MPC_P24PFS_ADDR, 0x0A);
		}

		/* 書き込みプロテクトレジスタの設定 書き込みを禁止 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x80);
		break;
	/* TXD/SDA/MOSI:P32, RXD/SCL/MISO:P33, SCK:P34 */
	case SCI6_BASE:
		switch (mode) {
		case sciSerial:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT3_PODR_ADDR,
				sil_reb_mem(PORT3_PODR_ADDR) | PORT_PODR_B2_BIT | PORT_PODR_B3_BIT);
			/* TX出力/RX入力ポート設定 */
			sil_wrb_mem(PORT3_PDR_ADDR, (sil_reb_mem(PORT3_PDR_ADDR) | PORT_PDR_B2_BIT) & ~PORT_PDR_B3_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORT3_PCR_ADDR, sil_reb_mem(PORT3_PCR_ADDR) & ~(PORT_PCR_B2_BIT | PORT_PCR_B3_BIT));
			/* RX CMOS */
			sil_wrb_mem(PORT3_ODR0_ADDR,
				(sil_reb_mem(PORT3_ODR0_ADDR) & ~(PORT_ODR_Pm2_MASK | PORT_ODR_Pm3_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm2_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm3_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT3_PMR_ADDR, sil_reb_mem(PORT3_PMR_ADDR) | PORT_PMR_B2_BIT | PORT_PMR_B3_BIT);
			break;
		case sciI2C:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT3_PODR_ADDR,
				sil_reb_mem(PORT3_PODR_ADDR) | PORT_PODR_B2_BIT | PORT_PODR_B3_BIT);
			/* SCL,SDA 出力ポート設定 */
			sil_wrb_mem(PORT3_PDR_ADDR, sil_reb_mem(PORT3_PDR_ADDR) | PORT_PDR_B2_BIT | PORT_PDR_B3_BIT);
			/* プルアップ有効 */
			sil_wrb_mem(PORT3_PCR_ADDR, sil_reb_mem(PORT3_PCR_ADDR) | PORT_PCR_B2_BIT | PORT_PCR_B3_BIT);
			/* Nチャネルオープンドレイン */
			sil_wrb_mem(PORT3_ODR0_ADDR,
				(sil_reb_mem(PORT3_ODR0_ADDR) & ~(PORT_ODR_Pm2_MASK | PORT_ODR_Pm3_MASK))
				| (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm2_OFFSET) | (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm3_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT3_PMR_ADDR, sil_reb_mem(PORT3_PMR_ADDR) | PORT_PMR_B2_BIT | PORT_PMR_B3_BIT);
			break;
		case sciSPIM:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORT3_PODR_ADDR,
				sil_reb_mem(PORT3_PODR_ADDR) | PORT_PODR_B2_BIT | PORT_PODR_B3_BIT | PORT_PODR_B4_BIT);
			/* MOSI出力/MISO入力/SCK入出力ポート設定 */
			sil_wrb_mem(PORT3_PDR_ADDR, (sil_reb_mem(PORT3_PDR_ADDR) | PORT_PDR_B2_BIT | PORT_PDR_B4_BIT) & ~PORT_PDR_B3_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORT3_PCR_ADDR, sil_reb_mem(PORT3_PCR_ADDR) & ~(PORT_PCR_B2_BIT | PORT_PCR_B3_BIT | PORT_PCR_B4_BIT));
			/* MISO CMOS */
			sil_wrb_mem(PORT3_ODR0_ADDR,
				(sil_reb_mem(PORT3_ODR0_ADDR) & ~(PORT_ODR_Pm2_MASK | PORT_ODR_Pm3_MASK | PORT_ODR_Pm4_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm2_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm3_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm4_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORT3_PMR_ADDR, sil_reb_mem(PORT3_PMR_ADDR) | PORT_PMR_B2_BIT | PORT_PMR_B3_BIT | PORT_PMR_B4_BIT);
			break;
		}

		/* 書き込みプロテクトレジスタの設定 PFSWEビットへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x00);
		/* 書き込みプロテクトレジスタの設定 PxxFSレジスタへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x40);

		/* P32端子機能制御レジスタ TXD6/SMOSI6/SSDA6とする */
		sil_wrb_mem(MPC_P32PFS_ADDR, 0x0A);
		/* P33端子機能制御レジスタ RXD6/SMISO6/SSCL6とする */
		sil_wrb_mem(MPC_P33PFS_ADDR, 0x0A);
		if (mode == sciSPIM) {
			/* P34端子機能制御レジスタ SCK6とする */
			sil_wrb_mem(MPC_P34PFS_ADDR, 0x0A);
		}

		/* 書き込みプロテクトレジスタの設定 書き込みを禁止 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x80);
		break;
	/* TXD/SDA/MOSI:PC7, RXD/SCL/MISO:PC6, SCK:PC5 */
	case SCI8_BASE:
		switch (mode) {
		case sciSerial:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORTC_PODR_ADDR,
				sil_reb_mem(PORTC_PODR_ADDR) | PORT_PODR_B6_BIT | PORT_PODR_B7_BIT);
			/* TX出力/RX入力ポート設定 */
			sil_wrb_mem(PORTC_PDR_ADDR, (sil_reb_mem(PORTC_PDR_ADDR) | PORT_PDR_B6_BIT) & ~PORT_PDR_B7_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORTC_PCR_ADDR, sil_reb_mem(PORTC_PCR_ADDR) & ~(PORT_PCR_B6_BIT | PORT_PCR_B7_BIT));
			/* RX CMOS */
			sil_wrb_mem(PORTC_ODR0_ADDR,
				(sil_reb_mem(PORTC_ODR0_ADDR) & ~(PORT_ODR_Pm6_MASK | PORT_ODR_Pm7_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm6_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm7_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORTC_PMR_ADDR, sil_reb_mem(PORTC_PMR_ADDR) | PORT_PMR_B6_BIT | PORT_PMR_B7_BIT);
			break;
		case sciI2C:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORTC_PODR_ADDR,
				sil_reb_mem(PORTC_PODR_ADDR) | PORT_PODR_B6_BIT | PORT_PODR_B7_BIT);
			/* SCL,SDA 出力ポート設定 */
			sil_wrb_mem(PORTC_PDR_ADDR, sil_reb_mem(PORTC_PDR_ADDR) | PORT_PDR_B6_BIT | PORT_PDR_B7_BIT);
			/* プルアップ有効 */
			sil_wrb_mem(PORTC_PCR_ADDR, sil_reb_mem(PORTC_PCR_ADDR) | PORT_PCR_B6_BIT | PORT_PCR_B7_BIT);
			/* Nチャネルオープンドレイン */
			sil_wrb_mem(PORTC_ODR0_ADDR,
				(sil_reb_mem(PORTC_ODR0_ADDR) & ~(PORT_ODR_Pm6_MASK | PORT_ODR_Pm7_MASK))
				| (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm6_OFFSET) | (PORT_ODR_NCH_OPEN_DRAIN << PORT_ODR_Pm7_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORTC_PMR_ADDR, sil_reb_mem(PORTC_PMR_ADDR) | PORT_PMR_B6_BIT | PORT_PMR_B7_BIT);
			break;
		case sciSPIM:
			/* HIGH状態に設定 */
			sil_wrb_mem(PORTC_PODR_ADDR,
				sil_reb_mem(PORTC_PODR_ADDR) | PORT_PODR_B7_BIT | PORT_PODR_B6_BIT | PORT_PODR_B5_BIT);
			/* MOSI出力/MISO入力/SCK入出力ポート設定 */
			sil_wrb_mem(PORTC_PDR_ADDR, (sil_reb_mem(PORTC_PDR_ADDR) | PORT_PDR_B5_BIT | PORT_PDR_B7_BIT) & ~PORT_PDR_B6_BIT);
			/* プルアップ無効 */
			sil_wrb_mem(PORTC_PCR_ADDR, sil_reb_mem(PORTC_PCR_ADDR) & ~(PORT_PCR_B5_BIT | PORT_PCR_B7_BIT | PORT_PCR_B6_BIT));
			/* MISO CMOS */
			sil_wrb_mem(PORTC_ODR0_ADDR,
				(sil_reb_mem(PORTC_ODR0_ADDR) & ~(PORT_ODR_Pm5_MASK | PORT_ODR_Pm6_MASK | PORT_ODR_Pm7_MASK))
				| (PORT_ODR_CMOS << PORT_ODR_Pm5_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm6_OFFSET) | (PORT_ODR_CMOS << PORT_ODR_Pm7_OFFSET));
			/* 周辺機能として使用 */
			sil_wrb_mem(PORTC_PMR_ADDR, sil_reb_mem(PORTC_PMR_ADDR) | PORT_PMR_B5_BIT | PORT_PMR_B6_BIT | PORT_PMR_B7_BIT);
			break;
		}

		/* 書き込みプロテクトレジスタの設定 PFSWEビットへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x00);
		/* 書き込みプロテクトレジスタの設定 PxxFSレジスタへの書き込みを許可 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x40);

		/* PC7端子機能制御レジスタ TXD8/SMOSI8/SSDA8とする */
		sil_wrb_mem(MPC_PC7PFS_ADDR, 0x0A);
		/* PC6端子機能制御レジスタ RXD8/SMISO8/SSCL8とする */
		sil_wrb_mem(MPC_PC6PFS_ADDR, 0x0A);
		if (mode == sciSPIM) {
			/* PC5端子機能制御レジスタ SCK8とする */
			sil_wrb_mem(MPC_PC5PFS_ADDR, 0x0A);
		}

		/* 書き込みプロテクトレジスタの設定 書き込みを禁止 */
		sil_wrb_mem(MPC_PWPR_ADDR, 0x80);
		break;
	}
}

void sci_disable(void *base_address)
{
	/* 送受信禁止, SCKn端子は入出力ポートとして使用 */
	sil_wrb_mem(SCI_SCR(base_address), 0x00U);

	/*
	 *  モジュールストップ機能の設定
	 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRB_ADDR,
		sil_rew_mem(SYSTEM_MSTPCRB_ADDR) | SCI_MSTPCRB_BIT(base_address));
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA500);	/* 書込み禁止 */
}

void sci_set_frequency(void *base_address, int hz)
{
	uint_t	cnt, brr, clk;

	cnt = (FREQ_PCLK * 1000) / (16 * hz);
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

	/* SMRに送信／ 受信フォーマットを設定 */
	/* クロック選択ビット(SMR.CKS[1:0]ビットを設定)、それ以外のビットを“0”に設定 */
	sil_wrb_mem(SCI_SMR(base_address), clk);

	/* ビットレートを設定 */
	sil_wrb_mem(SCI_BRR(base_address), brr);
}

void serial_init(serial_t *obj, PinName tx, PinName rx)
{
#ifdef TOPPERS_GRCITRUS
	if ((rx == D1) && (tx == D0)) {
		obj->base_address = (void *)SCI0_BASE;
		obj->rxi_intno = INT_SCI0_RXI;
		obj->tei_intno = INT_SCI0_TEI;
	}
	else if ((rx == D6) && (tx == D5)) {
		obj->base_address = (void *)SCI2_BASE;
		obj->rxi_intno = INT_SCI2_RXI;
		obj->tei_intno = INT_SCI2_TEI;
	}
	else if ((rx == D8) && (tx == D7)) {
		obj->base_address = (void *)SCI6_BASE;
		obj->rxi_intno = INT_SCI6_RXI;
		obj->tei_intno = INT_SCI6_TEI;
	}
	else if ((rx == D11) && (tx == D12)) {
		obj->base_address = (void *)SCI8_BASE;
		obj->rxi_intno = INT_SCI8_RXI;
		obj->tei_intno = INT_SCI8_TEI;
	}
	else {
		obj->base_address = NULL;
		return;
	}
#else
	if ((rx == D0) && (tx == D1)) {
		obj->base_address = (void *)SCI0_BASE;
		obj->rxi_intno = INT_SCI0_RXI;
		obj->tei_intno = INT_SCI0_TEI;
	}
	else if ((rx == D6) && (tx == D7)) {
		obj->base_address = (void *)SCI6_BASE;
		obj->rxi_intno = INT_SCI6_RXI;
		obj->tei_intno = INT_SCI6_TEI;
	}
	else if ((rx == D11) && (tx == D12)) {
		obj->base_address = (void *)SCI8_BASE;
		obj->rxi_intno = INT_SCI8_RXI;
		obj->tei_intno = INT_SCI8_TEI;
}
	else {
		obj->base_address = NULL;
		return;
	}
#endif // TOPPERS_GRCITRUS
	dis_int(obj->rxi_intno);
	dis_int(obj->tei_intno);

	/* SCI有効 */
	sci_enable(obj->base_address, sciSerial);

	/* SIMR1.IICMビットを“0”に設定 */
	sil_wrb_mem(SCI_SIMR1(obj->base_address),
		sil_reb_mem(SCI_SIMR1(obj->base_address)) & ~IICM);

	/* SPMR.CKPH, CKPOLビットを“0”に設定 */
	sil_wrb_mem(SCI_SPMR(obj->base_address),
		sil_reb_mem(SCI_SPMR(obj->base_address)) & ~(CKPH | CKPOL));

	sci_set_frequency(obj->base_address, /*9600*/115200);

	/* SCMRレジスタはSDIRビットを“0”、SINVビットを“0”、SMIFビットを“0”に設定 */
	sil_wrb_mem(SCI_SCMR(obj->base_address),
		(sil_reb_mem(SCI_SCMR(obj->base_address)) & ~(SINV | SMIF | SDIR)));

	/* SMRに送信／ 受信フォーマットを設定 */
	serial_format(obj, 8, ParityNone, 1);

	/* 送受信許可 */
	sil_wrb_mem(SCI_SCR(obj->base_address),
		(sil_reb_mem(SCI_SCR(obj->base_address)) | RE | TE));

	ena_int(obj->rxi_intno);
	ena_int(obj->tei_intno);
}

void serial_free(serial_t *obj)
{
	dis_int(obj->rxi_intno);
	dis_int(obj->tei_intno);

	/* SCI無効 */
	sci_disable(obj->base_address);

	obj->base_address = NULL;
}

void serial_baud(serial_t *obj, int baudrate)
{
	uint8_t scr = sil_reb_mem(SCI_SCR(obj->base_address));

	/* 送受信禁止, SCKn端子は入出力ポートとして使用 */
	sil_wrb_mem(SCI_SCR(obj->base_address), 0x00U);

	sci_set_frequency(obj->base_address, baudrate);

	/* 送受信許可 */
	sil_wrb_mem(SCI_SCR(obj->base_address), scr);
}

void serial_format(serial_t *obj, int data_bits, SerialParity parity, int stop_bits)
{
	uint8_t smr = sil_reb_mem(SCI_SMR(obj->base_address));

	smr &= ~(PE | PM | CHR | STOP);

	switch (parity) {
	case ParityNone:
		break;
	case ParityOdd:
		smr |= PE | PM;
		break;
	case ParityEven:
		smr |= PE;
		break;
	default:
		break;
	}

	switch (data_bits) {
	case 7:
		smr |= CHR;
		break;
	case 8:
		break;
	default:
		break;
	}

	switch (stop_bits) {
	case 1:
		break;
	case 2:
		smr |= STOP;
		break;
	default:
		break;
	}

	sil_wrb_mem(SCI_SMR(obj->base_address), smr);
}

int serial_getc(serial_t *obj)
{
	uint8_t ssr;
	int c;

	for (;;) {
		/* SSR.ORER, PER, FERフラグをリード */
		ssr = sil_reb_mem(SCI_SSR(obj->base_address));

		/* 受信エラーが発生したとき */
		if ((ssr & (ORER | PER | FER)) != 0) {
			/* オーバランエラー */
			if ((ssr & ORER) != 0) {
				/* RDRレジスタをリード */
				c = (int)sil_reb_mem(SCI_RDR(obj->base_address));
			}
			/* フレーミングエラー */
			if ((ssr & FER) != 0) {
			}
			/* パリティエラー */
			if ((ssr & PER) != 0) {
			}
		}

		/* SSR.ORER, PER, FERフラグを“0”に設定 */
		sil_wrb_mem(SCI_SSR(obj->base_address), ssr & ~(ORER | PER | FER));

		/* SSR.ORER, PER, FERフラグをリード */
		ssr = sil_reb_mem(SCI_SSR(obj->base_address));

		/* RXI割り込み */
		if (serial_readable(obj)) {
			clear_int(obj->rxi_intno);
			break;
		}

		dly_tsk(10);
	}

	/* RDRレジスタをリード */
	c = (int)sil_reb_mem(SCI_RDR(obj->base_address));

	return c;
}

void serial_putc(serial_t *obj, int c)
{
	while (!serial_writable(obj))
		dly_tsk(10);

	sil_wrb_mem(SCI_TDR(obj->base_address), (char)c);

	clear_int(obj->tei_intno);
}

int serial_readable(serial_t *obj)
{
	//return probe_int(obj->rxi_intno);
	return true;
}

int serial_writable(serial_t *obj)
{
	//return probe_int(obj->tei_intno);
	return (sil_reb_mem(SCI_SSR(obj->base_address)) & TEND) != 0U;
}

void serial_irq_set(serial_t *obj, SerialIrq irq, uint32_t enable)
{
	switch (irq) {
	case RxIrq:
		if (enable) {
			sil_wrb_mem(SCI_SCR(obj->base_address),
				(sil_reb_mem(SCI_SCR(obj->base_address)) | RIE));
		}
		else {
			sil_wrb_mem(SCI_SCR(obj->base_address),
				(sil_reb_mem(SCI_SCR(obj->base_address)) & (~RIE)));
		}
		break;
	case TxIrq:
		if (enable) {
			sil_wrb_mem(SCI_SCR(obj->base_address),
				(sil_reb_mem(SCI_SCR(obj->base_address)) | TEIE));
		}
		else {
			sil_wrb_mem(SCI_SCR(obj->base_address),
				(sil_reb_mem(SCI_SCR(obj->base_address)) & (~TEIE)));
		}
		break;
	}
}

struct uart_irq_info_t {
	uart_irq_handler handler;
	uint32_t id;
};

struct uart_irq_info_t sci_irq_info[7];

void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uint32_t id)
{
	int no = SCI_NUM(obj->base_address);
	bool_t lock = sense_lock();

	if (!lock)
		loc_cpu();

	sci_irq_info[no].handler = handler;
	sci_irq_info[no].id = id;

	if (!lock)
		unl_cpu();
}

void i2c_init(i2c_t *obj, PinName sda, PinName scl)
{
	if ((sda == I2C_SDA) && (scl == I2C_SCL)) {
#ifdef TOPPERS_GRCITRUS
		obj->base_address = (void *)SCI6_BASE;
		obj->rxi_intno = INT_SCI6_RXI;
		obj->txi_intno = INT_SCI6_TXI;
		obj->tei_intno = INT_SCI6_TEI;
#else
		obj->base_address = (void *)SCI2_BASE;
		obj->rxi_intno = INT_SCI2_RXI;
		obj->txi_intno = INT_SCI2_TXI;
		obj->tei_intno = INT_SCI2_TEI;
#endif
	}
	else {
		obj->base_address = NULL;
		return;
	}

	/* SCI有効 */
	sci_enable(obj->base_address, sciI2C);

	/* SIMR3.IICSDAS[1:0](b5-b4), IICSCLS[1:0](b7-b6)ビットを“11b”に設定 */
	sil_wrb_mem(SCI_SIMR3(obj->base_address), 0xF0U);

	sci_set_frequency(obj->base_address, 100000);

	/* SCMRレジスタはSDIRビットを“1”、SINVビットを“0”、SMIFビットを“0”に設定 */
	sil_wrb_mem(SCI_SCMR(obj->base_address),
		(sil_reb_mem(SCI_SCMR(obj->base_address)) & ~(SINV | SMIF)) | SDIR);

	/* SEMR、SNFR、SIMR1、SIMR2、SPMRレジスタに値を設定 */
	/* SEMRレジスタは、NFENビットを設定 */
	sil_wrb_mem(SCI_SEMR(obj->base_address), NFEN);

	/* SNFRレジスタは、NFCS[2:0]ビットを設定*/
	sil_wrb_mem(SCI_SNFR(obj->base_address), 1);

	/* SIMR1レジスタは、IICMビットを“1”に設定し、IICDL[4:0]ビ	ットを設定 */
	sil_wrb_mem(SCI_SIMR1(obj->base_address), IICM | (3 << 3));

	/* SIMR2レジスタは、IICACKT、IICCSCビットを“1”に設定し、IICINTMビットを設定 */
	sil_wrb_mem(SCI_SIMR2(obj->base_address), IICACKT | IICCSC | IICINTM);

	/* SPMRレジスタは、全ビット“0”に設定 */
	sil_wrb_mem(SCI_SPMR(obj->base_address), 0);

	/* 送受信許可 */
	sil_wrb_mem(SCI_SCR(obj->base_address),
		(sil_reb_mem(SCI_SCR(obj->base_address)) | RE | TE));
}

void i2c_frequency(i2c_t *obj, int hz)
{
	uint8_t scr = sil_reb_mem(SCI_SCR(obj->base_address));

	/* 送受信禁止, SCKn端子は入出力ポートとして使用 */
	sil_wrb_mem(SCI_SCR(obj->base_address), 0x00U);

	sci_set_frequency(obj->base_address, hz);

	/* 送受信許可 */
	sil_wrb_mem(SCI_SCR(obj->base_address), scr);
}

int i2c_start(i2c_t *obj)
{
	if (obj->base_address == NULL)
		return 0;

	/* SIMR3.IICSTAREQビットを“1”に設定すると同時に、
	 * SIMR3.IICSCLS[1:0],IICSDAS[1:0]ビットを“01b”に設定 */
	sil_wrb_mem(SCI_SIMR3(obj->base_address), IICSTAREQ | (1 << 4) | (1 << 6));

	return 0;
}

int i2c_restart(i2c_t *obj)
{
	if (obj->base_address == NULL)
		return 0;

	sil_wrb_mem(SCI_SIMR3(obj->base_address), IICRSTAREQ | (1 << 4) | (1 << 6));

	return 0;
}

int i2c_stop(i2c_t *obj)
{
	if (obj->base_address == NULL)
		return 0;

	/* SIMR3.IICSTPREQビットを“1”に設定すると同時に、
	 * SIMR3.IICSCLS[1:0],IICSDAS[1:0]ビットを“01b”に設定 */
	sil_wrb_mem(SCI_SIMR3(obj->base_address), IICSTPREQ | (1 << 4) | (1 << 6));

	return 0;
}

int i2c_read(i2c_t *obj, int address, char *data, int length, int stop)
{
	if (obj->base_address == NULL)
		return 0;

	/* 送信割り込み要求を禁止 */
	sil_wrb_mem(SCI_SCR(obj->base_address),
		(sil_reb_mem(SCI_SCR(obj->base_address)) & ~(RIE)) | TEIE | TIE);

	/* 開始条件の生成 */
	i2c_start(obj);

	/* STI(TEI)割り込み発生確認 */
	while (!probe_int(obj->tei_intno))
		dly_tsk(2);
	clear_int(obj->tei_intno);

	/* SIMR3.IICSTIFフラグを“0”に設定し、 */
	sil_wrb_mem(SCI_SIMR3(obj->base_address),
		sil_reb_mem(SCI_SIMR3(obj->base_address)) & ~IICSTIF);
	/* SIMR3.IICSCLS[1:0], IICSDAS[1:0]ビットを“00b”に設定 */
	sil_wrb_mem(SCI_SIMR3(obj->base_address),
		sil_reb_mem(SCI_SIMR3(obj->base_address)) & ~((3 << 4) | (3 << 6)));

	/* TDRレジスタにスレーブアドレス、R/Wビットをライト */
	sil_wrb_mem(SCI_TDR(obj->base_address), address | 1);

	/* TXI割り込み発生確認 */
	while (!probe_int(obj->txi_intno))
		dly_tsk(10);
	clear_int(obj->txi_intno);

	/* スレーブデバイスからのACKを確認 */
	if ((sil_reb_mem(SCI_SISR(obj->base_address)) & IICACKR) == 0) {
		/* SIMR2.IICACKTビットを“0”に設定 */
		sil_wrb_mem(SCI_SIMR2(obj->base_address), sil_reb_mem(SCI_SIMR2(obj->base_address)) & ~IICACKT);
		/* SCR.RIEビットを“1”に設定 */
		sil_wrb_mem(SCI_SCR(obj->base_address), sil_reb_mem(SCI_SCR(obj->base_address)) | RIE);

		for (int i = 0; i < length; i++) {
			/* 次データが最終 */
			if (i == length - 1) {
				/* SIMR2.IICACKTビットを“1”に設定 */
				sil_wrb_mem(SCI_SIMR2(obj->base_address), sil_reb_mem(SCI_SIMR2(obj->base_address)) | IICACKT);
			}

			/* TDRレジスタにダミーデータ“FFh”をライト */
			sil_wrb_mem(SCI_TDR(obj->base_address), 0xFF);

			/* RXI割り込み発生確認 */
			while (!probe_int(obj->rxi_intno))
				dly_tsk(10);
			clear_int(obj->rxi_intno);

			/* RDRレジスタの受信データをリード */
			*data = sil_reb_mem(SCI_RDR(obj->base_address));
			data++;

			/* TXI割り込み発生確認 */
			while (!probe_int(obj->txi_intno))
				dly_tsk(10);
			clear_int(obj->txi_intno);
		}
	}

	if (stop) {
		/* 停止条件の生成 */
		i2c_stop(obj);

		/* STI(TEI)割り込み発生確認 */
		while (!probe_int(obj->tei_intno))
			dly_tsk(2);
		clear_int(obj->tei_intno);

		/* SIMR3.IICSTIFフラグを“0”に設定し、*/
		sil_wrb_mem(SCI_SIMR3(obj->base_address),
			sil_reb_mem(SCI_SIMR3(obj->base_address)) & ~IICSTIF);
		/* SIMR3.IICSCLS[1:0], IICSDAS[1:0]ビットを“11b”に設定 */
		sil_wrb_mem(SCI_SIMR3(obj->base_address),
			sil_reb_mem(SCI_SIMR3(obj->base_address)) | ((3 << 4) | (3 << 6)));
	}

	return length;
}

int i2c_write(i2c_t *obj, int address, const char *data, int length, int stop)
{
	if (obj->base_address == NULL)
		return 0;

	/* 受信割り込み要求を禁止 */
	sil_wrb_mem(SCI_SCR(obj->base_address),
		(sil_reb_mem(SCI_SCR(obj->base_address)) & ~(RIE)) | TEIE | TIE);

	/* 開始条件の生成 */
	i2c_start(obj);

	/* STI(TEI)割り込み発生確認 */
	while (!probe_int(obj->tei_intno))
		dly_tsk(2);
	clear_int(obj->tei_intno);

	/* SIMR3.IICSTIFフラグを“0”に設定し、 */
	sil_wrb_mem(SCI_SIMR3(obj->base_address),
		sil_reb_mem(SCI_SIMR3(obj->base_address)) & ~IICSTIF);
	/* SIMR3.IICSCLS[1:0], IICSDAS[1:0]ビットを“00b”に設定 */
	sil_wrb_mem(SCI_SIMR3(obj->base_address),
		sil_reb_mem(SCI_SIMR3(obj->base_address)) & ~((3 << 4) | (3 << 6)));

	/* TDRレジスタにスレーブアドレス、R/Wビットをライト */
	sil_wrb_mem(SCI_TDR(obj->base_address), address);

	/* TXI割り込み発生確認 */
	while (!probe_int(obj->txi_intno))
		dly_tsk(10);
	clear_int(obj->txi_intno);

	/* スレーブデバイスからのACKを確認 */
	if ((sil_reb_mem(SCI_SISR(obj->base_address)) & IICACKR) == 0) {

		for (int i = 0; i < length; i++) {
			/* TDRレジスタにスレーブアドレス、R/Wビットをライト */
			sil_wrb_mem(SCI_TDR(obj->base_address), *data);
			data++;

			/* TXI割り込み発生確認 */
			while (!probe_int(obj->txi_intno))
				dly_tsk(10);
			clear_int(obj->txi_intno);
		}
	}

	if (stop) {
		/* 停止条件の生成 */
		i2c_stop(obj);

		/* STI(TEI)割り込み発生確認 */
		while (!probe_int(obj->tei_intno))
			dly_tsk(2);
		clear_int(obj->tei_intno);

		/* SIMR3.IICSTIFフラグを“0”に設定し、*/
		sil_wrb_mem(SCI_SIMR3(obj->base_address),
			sil_reb_mem(SCI_SIMR3(obj->base_address)) & ~IICSTIF);
		/* SIMR3.IICSCLS[1:0], IICSDAS[1:0]ビットを“11b”に設定 */
		sil_wrb_mem(SCI_SIMR3(obj->base_address),
			sil_reb_mem(SCI_SIMR3(obj->base_address)) | ((3 << 4) | (3 << 6)));
	}

	return length;
}

int i2c_byte_read(i2c_t *obj, int last)
{
	if (obj->base_address == NULL)
		return 0;

	return 0;
}

int i2c_byte_write(i2c_t *obj, int data)
{
	if (obj->base_address == NULL)
		return 0;

	return 0;
}

void spi_init(spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel)
{
	if ((mosi == D11) && (miso == D12) && (sclk == D13) && ((ssel == D10) || (ssel == NC))) {
		obj->base_address = (void *)SCI8_BASE;
		obj->txi_intno = INT_SCI8_TXI;
		obj->rxi_intno = INT_SCI8_RXI;
		obj->tei_intno = INT_SCI8_TEI;
	}
	else if ((mosi == D3) && (miso == D6) && (sclk == D4) && (ssel == NC)) {
		obj->base_address = (void *)SCI3_BASE;
		obj->txi_intno = INT_SCI3_TXI;
		obj->rxi_intno = INT_SCI3_RXI;
		obj->tei_intno = INT_SCI3_TEI;
	}
	else {
		obj->base_address = NULL;
		return;
	}

	/* SCI有効 */
	sci_enable(obj->base_address, (ssel != NC) ? sciSPIS : sciSPIM);

	/* SIMR1.IICMビットを“0”に設定 */
	sil_wrb_mem(SCI_SIMR1(obj->base_address),
		sil_reb_mem(SCI_SIMR1(obj->base_address)) & ~IICM);

	if (ssel == NC) {
		/* SPMR.SSE, CTSE, MSS, MFF, CKPH, CKPOLビットを“0”に設定 */
		sil_wrb_mem(SCI_SPMR(obj->base_address), 0);
	}
	else {
		/* SPMR, CTSE, MSS, MFF, CKPH, CKPOLビットを“0”に設定 */
		/* SPMR.SSEビットを“1”に設定 */
		sil_wrb_mem(SCI_SPMR(obj->base_address), SSE);
	}

	/* SCMRレジスタはSDIRビットを“0”、SINVビットを“0”、SMIFビットを“0”に設定 */
	sil_wrb_mem(SCI_SCMR(obj->base_address),
		(sil_reb_mem(SCI_SCMR(obj->base_address)) & ~(SMIF | SDIR)) | SINV);

	/* SMRに送信／ 受信フォーマットをクロック同期式モードで動作で設定 */
	sil_wrb_mem(SCI_SMR(obj->base_address), CM);

	sci_set_frequency(obj->base_address, 1000000);

	/* 送受信許可 */
	sil_wrb_mem(SCI_SCR(obj->base_address),
		(sil_reb_mem(SCI_SCR(obj->base_address)) | RE | TE));
}

void spi_free(spi_t *obj)
{
	/* SCI無効 */
	sci_disable(obj->base_address);

	obj->base_address = NULL;
}

void spi_format(spi_t *obj, int bits, int mode, int slave)
{
	int polarity = (mode & 0x2) ? 1 : 0;
	int phase = (mode & 0x1) ? 1 : 0;
	uint8_t spmr;

	if ((bits != 8) || (mode < 0) || (mode > 3)) {
		return;
	}

	/* 送受信禁止, SCKn端子は入出力ポートとして使用 */
	sil_wrb_mem(SCI_SCR(obj->base_address), 0x00U);

	spmr = sil_reb_mem(SCI_SPMR(obj->base_address));

	/* クロック極性反転あり/なし */
	if (polarity)
		spmr |= CKPOL;
	else
		spmr &= ~CKPOL;

	/* クロック位相反転あり/なし */
	if (phase)
		spmr |= CKPH;
	else
		spmr &= ~CKPH;

	if (slave) {
		/* SCKn端子はクロック入力として使用 */
		sil_wrb_mem(SCI_SCR(obj->base_address), 
			(sil_reb_mem(SCI_SCR(obj->base_address)) & ~CKE) | 2);

		/* SPMR.MSSビットを“1”に設定 */
		sil_wrb_mem(SCI_SPMR(obj->base_address), spmr | MSS);
	}
	else {
		/* SCKn端子はクロック出力として使用 */
		sil_wrb_mem(SCI_SCR(obj->base_address),
			sil_reb_mem(SCI_SCR(obj->base_address)) & ~CKE);

		/* SPMR.MSSビットを“0”に設定 */
		sil_wrb_mem(SCI_SPMR(obj->base_address), spmr & ~MSS);
	}

	/* 送受信許可 */
	sil_wrb_mem(SCI_SCR(obj->base_address),
		(sil_reb_mem(SCI_SCR(obj->base_address)) | RE | TE));
}

void spi_frequency(spi_t *obj, int hz)
{
	uint8_t scr = sil_reb_mem(SCI_SCR(obj->base_address));

	/* 送受信禁止, SCKn端子は入出力ポートとして使用 */
	sil_wrb_mem(SCI_SCR(obj->base_address), 0x00U);

	sci_set_frequency(obj->base_address, hz);

	/* 送受信許可 */
	sil_wrb_mem(SCI_SCR(obj->base_address), scr);
}

int spi_master_write(spi_t *obj, int value)
{
	/* TXI割り込み発生確認 */
	while (!probe_int(obj->txi_intno))
		;

	/* 送信データをライト */
	sil_wrb_mem(SCI_TDR(obj->base_address), (char)value);

	do {
		uint8_t ssr = sil_reb_mem(SCI_SSR(obj->base_address));
		if ((ssr & ORER) != 0) {
			ssr &= ~ORER;
			sil_wrb_mem(SCI_SSR(obj->base_address), ssr);
			return -1;
		}
	}
	/* RXI割り込み発生確認 */
	while (!probe_int(obj->rxi_intno));

	/* 受信データをリード */
	return sil_reb_mem(SCI_RDR(obj->base_address));
}

int spi_master_block_write(spi_t *obj, const char *tx_buffer, int tx_length,
	char *rx_buffer, int rx_length, char write_fill)
{
	int total = (tx_length > rx_length) ? tx_length : rx_length;

	for (int i = 0; i < total; i++) {
		char out = (i < tx_length) ? tx_buffer[i] : write_fill;
		char in = spi_master_write(obj, out);
		if (i < rx_length) {
			rx_buffer[i] = in;
		}
	}

	return total;
}

uint8_t DecToBcd(uint8_t value)
{
	return ((value / 10) << 4) | (value % 10);
}

uint8_t BcdToDec(uint8_t value)
{
	return (10 * (value >> 4)) | (value & 0x0F);
}

void rtc_init(void)
{
}

void rtc_free(void)
{
}

int rtc_isenabled(void)
{
	return 1;
}

long long __tm_to_secs(const struct tm *tm);
int __secs_to_tm(long long t, struct tm *tm);

time_t rtc_read(void)
{
	struct tm tm = { 0 };
	tm.tm_year = 2000 - 1900 + BcdToDec(sil_reb_mem((uint8_t *)RTC_RYRCNT_ADDR));
	tm.tm_mon = BcdToDec(sil_reb_mem((uint8_t *)RTC_RMONCNT_ADDR)) - 1;
	tm.tm_mday = BcdToDec(sil_reb_mem((uint8_t *)RTC_RDAYCNT_ADDR));
	tm.tm_hour = BcdToDec(sil_reb_mem((uint8_t *)RTC_RHRCNT_ADDR));
	tm.tm_min = BcdToDec(sil_reb_mem((uint8_t *)RTC_RMINCNT_ADDR));
	tm.tm_sec = BcdToDec(sil_reb_mem((uint8_t *)RTC_RSECCNT_ADDR));
	return __tm_to_secs(&tm);
}

void rtc_write(time_t t)
{
	struct tm tm;

	__secs_to_tm(t, &tm);
	sil_wrb_mem((uint8_t *)RTC_RYRCNT_ADDR, DecToBcd(tm.tm_year + 1900 - 2000));
	sil_wrb_mem((uint8_t *)RTC_RMONCNT_ADDR, DecToBcd(tm.tm_mon + 1));
	sil_wrb_mem((uint8_t *)RTC_RDAYCNT_ADDR, DecToBcd(tm.tm_mday));
	sil_wrb_mem((uint8_t *)RTC_RHRCNT_ADDR, DecToBcd(tm.tm_hour));
	sil_wrb_mem((uint8_t *)RTC_RMINCNT_ADDR, DecToBcd(tm.tm_min));
	sil_wrb_mem((uint8_t *)RTC_RSECCNT_ADDR, DecToBcd(tm.tm_sec));
}

void sci_rxi_handler(uint32_t no)
{
	struct uart_irq_info_t *info = &sci_irq_info[no];
	if (info->handler == NULL)
		return;

	info->handler(info->id, RxIrq);
}

void sci_txi_handler(uint32_t no)
{
/*	struct uart_irq_info_t *info = &sci_irq_info[no];
	if (info->handler == NULL)
		return;

	info->handler(info->id, TxIrq);*/
}

void sci_tei_handler(int no)
{
	struct uart_irq_info_t *info = &sci_irq_info[no];
	if (info->handler == NULL)
		return;

	info->handler(info->id, TxIrq);
}

void sci0_rxi_handler(void)
{
	sci_rxi_handler(0);
}

void sci0_txi_handler(void)
{
	sci_txi_handler(0);
}

void sci0_tei_handler(void)
{
	sci_tei_handler(0);
}

void sci1_rxi_handler(void)
{
	sci_rxi_handler(1);
}

void sci1_txi_handler(void)
{
	sci_txi_handler(1);
}

void sci1_tei_handler(void)
{
	sci_tei_handler(1);
}

void sci2_rxi_handler(void)
{
	sci_rxi_handler(2);
}

void sci2_txi_handler(void)
{
	sci_txi_handler(2);
}

void sci2_tei_handler(void)
{
	sci_tei_handler(2);
}

void sci3_rxi_handler(void)
{
	sci_rxi_handler(3);
}

void sci3_txi_handler(void)
{
	sci_txi_handler(3);
}

void sci3_tei_handler(void)
{
	sci_tei_handler(3);
}

void sci4_rxi_handler(void)
{
	sci_rxi_handler(4);
}

void sci4_txi_handler(void)
{
	sci_txi_handler(4);
}

void sci4_tei_handler(void)
{
	sci_tei_handler(4);
}

void sci5_rxi_handler(void)
{
	sci_rxi_handler(5);
}

void sci5_txi_handler(void)
{
	sci_txi_handler(5);
}

void sci5_tei_handler(void)
{
	sci_tei_handler(5);
}

void sci6_rxi_handler(void)
{
	sci_rxi_handler(6);
}

void sci6_txi_handler(void)
{
	sci_txi_handler(6);
}

void sci6_tei_handler(void)
{
	sci_tei_handler(6);
}
