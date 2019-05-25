/*
 *  TOPPERS PROJECT Home Network Working Group Software
 * 
 *  Copyright (C) 2014-2017 Cores Co., Ltd. Japan
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
 */

/* 
 *  サンプルプログラム(1)の本体
 */

#include "shellif.h"
#include <kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "t_stdlib.h"
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "target_syssvc.h"
#include "target_stddef.h"
#include "kernel_cfg.h"
#include "diskio.h"
#include "mmc_rspi.h"
#include "ff.h"
#include "ffarch.h"
#include <stdio.h>
#include "gpio_api.h"
#include "rtc_api.h"

enum ffarch_state_t {
	FFS_IDLE,
	FFS_RETRY_WAIT,
	FFS_DISABLE,
};

enum ffarch_state_t ffarch_state;
volatile int ffarch_timer;
int ffarch_retry_count;
uint8_t mmc_rspi_prev_status;

static void ffarch_initialize();

/*
 * FatFSタスク
 */
void ffarch_task(intptr_t exinf)
{
	ER ret, ret2;
	SYSTIM prev, now;
	int timer;

	ffarch_initialize();

	ret2 = get_tim(&now);
	if (ret2 != E_OK){
		syslog(LOG_NOTICE, "[ffarch] get_tim error: %s",
			itron_strerror(ret2));
		return;
	}

	for(;;){
		prev = now;

		/* タイマー取得 */
		timer = ffarch_get_timer();

		/* 待ち */
		ret = tslp_tsk(timer);
		if ((ret != E_OK) && (ret != E_TMOUT)) {
			syslog(LOG_NOTICE, "[ffarch] tslp_tsk error: %s %d",
				itron_strerror(ret), timer);
			break;
		}

		ret2 = get_tim(&now);
		if (ret2 != E_OK) {
			syslog(LOG_NOTICE, "[ffarch] get_tim error: %s",
				itron_strerror(ret2));
			break;
		}

		/* 時間経過 */
		int elapse = now - prev;
		ffarch_progress(elapse);

		/* タイムアウト処理 */
		ffarch_timeout();
	}
}

FATFS RomDisk;

#define WP() false

typedef struct SD {
	DSTATUS dst;
	BYTE type;
	FATFS FatFs;
} SD;
SD Sd;

bool_t romdisk_init();
bool_t SD_begin();

void ffarch_init(void)
{
#ifdef TOPPERS_GRCITRUS
	ffarch_state = FFS_DISABLE;
	ffarch_timer = TMO_FEVR;
	mmc_rspi_prev_status = STA_NODISK;
#else
	ffarch_state = FFS_IDLE;
	ffarch_timer = 0;
	ffarch_retry_count = 3;
	mmc_rspi_prev_status = STA_NODISK;

	ffarch_timeout();

	act_tsk(FFARCH_TASK);
#endif
	if (romdisk_init()) {
		syslog(LOG_NOTICE, "ROM disk (0:) OK!");
	}
	else {
		syslog(LOG_NOTICE, "ROM disk (0:) NG!");
	}
}

void ffarch_initialize()
{
#ifdef TOPPERS_GRCITRUS
	ffarch_state = FFS_DISABLE;
	ffarch_timer = TMO_FEVR;
	mmc_rspi_prev_status = STA_NODISK;
#else
	sta_cyc(MMC_RSPI_CYC);
#endif
}

bool_t romdisk_init()
{
	DSTATUS dst;
	FRESULT res;

	if (RomDisk.fs_type != 0)
		return true;

	if ((dst = ramdisk_initialize()) != RES_OK) {
		return false;
	}

	if ((res = f_mount(&RomDisk, "0:", 1)) != FR_OK) {
		return false;
	}

	return true;
}

bool_t SD_begin()
{
	DSTATUS dst;
	FRESULT res;
	BYTE pdrv = 1, type;

	if (Sd.FatFs.fs_type != 0)
		return true;

	if ((dst = disk_initialize(pdrv)) != RES_OK) {
		return false;
	}

	if ((dst = disk_ioctl(pdrv, MMC_GET_TYPE, &type)) != RES_OK) {
		Sd.dst = dst;
		Sd.type = 0;
	}
	else {
		Sd.dst = RES_OK;
		Sd.type = type;
	}

	if ((res = f_mount(&Sd.FatFs, "1:", 1)) != FR_OK) {
		return false;
	}

	return true;
}

DSTATUS disk_initialize(BYTE pdrv)
{
	switch (pdrv) {
	case 0:
		return ramdisk_initialize();
	case 1:
		return mmc_rspi_initialize();
	}
	return STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv)
{
	switch (pdrv) {
	case 0:
		return ramdisk_get_status();
	case 1:
		return mmc_rspi_status();
	}
	return STA_NOINIT;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
	switch (pdrv) {
	case 0:
		return ramdisk_read(buff, sector, count);
	case 1:
		return mmc_rspi_read(buff, sector, count);
	}
	return RES_PARERR;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
	switch (pdrv) {
	case 0:
		return ramdisk_write(buff, sector, count);
	case 1:
		return mmc_rspi_write(buff, sector, count);
	}
	return RES_PARERR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
	switch (pdrv) {
	case 0:
		return RES_PARERR; /* ramdisk_ioctl(cmd, buff); */
	case 1:
		return mmc_rspi_ioctl(cmd, buff);
	}
	return RES_PARERR;
}

void mmc_rspi_status_changed(DSTATUS newst)
{
	if (ffarch_state == FFS_IDLE) {
		mmc_rspi_prev_status = mmc_rspi_status();
		ffarch_timer = 0;
		wup_tsk(FFARCH_TASK);
	}
}

int ffarch_get_timer(void)
{
	return ffarch_timer;
}

void ffarch_progress(int elapse)
{
	if (ffarch_timer != TMO_FEVR) {
		ffarch_timer -= elapse;
		if (ffarch_timer < 0) {
			ffarch_timer = 0;
		}
	}
}

void ffarch_timeout(void)
{
	if (ffarch_timer != 0)
		return;

	switch (ffarch_state) {
	case FFS_RETRY_WAIT:
		if (ffarch_retry_count == 0) {
			syslog(LOG_NOTICE, "SD card (1:) initialize tired...");

			ffarch_state = FFS_IDLE;
			ffarch_timer = TMO_FEVR;
			ffarch_retry_count = 3;
			break;
		}
		ffarch_retry_count--;
		//goto case FFS_IDLE;
	case FFS_IDLE:
		/* SDカードが入れられた場合 */
		if (((mmc_rspi_prev_status & STA_NODISK) != 0) && ((mmc_rspi_status() & STA_NODISK) == 0)) {
			syslog(LOG_NOTICE, "SD card initializing ...");

			Sd.FatFs.fs_type = 0;
			if (SD_begin()) {
				syslog(LOG_NOTICE, "SD card (1:) OK!");

				/* uploadディレクトリを作成しておく */
				f_mkdir("1:/upload");

				ffarch_state = FFS_IDLE;
				ffarch_timer = TMO_FEVR;
				ffarch_retry_count = 3;
			}
			else {
				syslog(LOG_NOTICE, "SD card (1:) NG!");
				ffarch_state = FFS_RETRY_WAIT;
				ffarch_timer = 1000 * 1000;
			}
		}
		/* SDカードが抜かれた場合 */
		else if (((mmc_rspi_prev_status & STA_NODISK) == 0) && ((mmc_rspi_status() & STA_NODISK) != 0)) {
			syslog(LOG_NOTICE, "SD card unmount");

			f_mount(&Sd.FatFs, "1:", 0);
			ffarch_state = FFS_IDLE;
			ffarch_timer = TMO_FEVR;
			ffarch_retry_count = 3;
		}
		mmc_rspi_prev_status = mmc_rspi_status();
		break;
	}
}

int ff_cre_syncobj(BYTE vol, FF_SYNC_t* sobj)
{
	return 1;
}

int ff_req_grant(FF_SYNC_t sobj)
{
	ER ret;
	ret = wai_sem(SEM_FILESYSTEM);
	return ret == E_OK;
}

void ff_rel_grant(FF_SYNC_t sobj)
{
	sig_sem(SEM_FILESYSTEM);
}

int ff_del_syncobj(FF_SYNC_t sobj)
{
	return 1;
}

void* ff_memalloc (UINT msize)
{
	return malloc(msize);
}

void ff_memfree (void* mblock)
{
	free(mblock);
}

DWORD get_fattime(void)
{
	time_t temp;
	struct tm _tm;

	temp = rtc_read();
	gmtime_r(&temp, &_tm);

	return	  ((DWORD)(_tm.tm_year - 1980) << 25)
		| ((DWORD)_tm.tm_mon << 21)
		| ((DWORD)_tm.tm_mday << 16)
		| ((DWORD)_tm.tm_hour << 11)
		| ((DWORD)_tm.tm_min << 5)
		| ((DWORD)_tm.tm_sec >> 1);
}
