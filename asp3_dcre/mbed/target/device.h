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
#ifndef _MBED_API_H_
#define _MBED_API_H_

#include <time.h>
#include "PinNames.h"

#define DEVICE_ANALOGIN		1
#define DEVICE_ANALOGOUT	1
#define DEVICE_PWMOUT		1
#define DEVICE_SERIAL		1
#define DEVICE_SPI			1
#define DEVICE_I2C			1
#define DEVICE_RTC			1
#define DEVICE_SLEEP		1

void mbed_api_init(void);

void wait_ms(int ms);
uint32_t us_ticker_read(void);

typedef struct gpio_s {
	PinName  pin;
	PinDirection dir;
	PinMode mode;
} gpio_t;

typedef struct analogin_s {
	PinName pin;
} analogin_t;

typedef struct dac_s {
	PinName pin;
} dac_t;

typedef struct pwmout_s {
	PinName pin;
} pwmout_t;

void arduino_tick();

typedef struct serial_s {
	void *base_address;
	int rxi_intno;
	int tei_intno;
} serial_t;

typedef struct i2c_s {
	void *base_address;
	int rxi_intno;
	int txi_intno;
	int tei_intno;
	unsigned char rxdata[16];
} i2c_t;

#define SPI_FILL_CHAR         (0xFF)

typedef struct spi_s {
	void *base_address;
	int rxi_intno;
	int txi_intno;
	int tei_intno;
} spi_t;

void sci0_rxi_handler(void);
void sci0_txi_handler(void);
void sci0_tei_handler(void);

void sci1_rxi_handler(void);
void sci1_txi_handler(void);
void sci1_tei_handler(void);

void sci2_rxi_handler(void);
void sci2_txi_handler(void);
void sci2_tei_handler(void);

void sci3_rxi_handler(void);
void sci3_txi_handler(void);
void sci3_tei_handler(void);

void sci4_rxi_handler(void);
void sci4_txi_handler(void);
void sci4_tei_handler(void);

void sci5_rxi_handler(void);
void sci5_txi_handler(void);
void sci5_tei_handler(void);

void sci6_rxi_handler(void);
void sci6_txi_handler(void);
void sci6_tei_handler(void);

#endif /* _MBED_API_H_ */
