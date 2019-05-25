/*------------------------------------------------------------------------*/
/* FRK-RX62N: MMCv3/SDv1/SDv2 (SPI mode) Control Module                   */
/*------------------------------------------------------------------------*/
/*
/  Copyright (C) 2014, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include <kernel.h>	/* Platform dependent include file */
#include "diskio.h"		/* Common include file for ff.c and disk layer */
#include <sil.h>
#include <prc_rename.h>
#include "target_kernel_impl.h"
#include "mmc_rspi.h"
#include "kernel_cfg.h"
#include <t_syslog.h>
#include <string.h>

static void CS_LOW()
{
	sil_wrb_mem(PORTC_PODR_ADDR, sil_reb_mem(PORTC_PODR_ADDR) & ~PORT_PODR_B0_BIT);		/* Set CS# low */
}

static void CS_HIGH()
{
	sil_wrb_mem(PORTC_PODR_ADDR, sil_reb_mem(PORTC_PODR_ADDR) | PORT_PODR_B0_BIT);		/* Set CS# high */
}

static bool_t WP()
{
	return false;
}

static bool_t INS()
{
	return (sil_reb_mem(PORT1_PIDR_ADDR) & PORT_PIDR_B5_BIT) == 0;
}

static void CTRL_INIT()
{
	/* CS=OUT */
	sil_wrb_mem(PORTC_PDR_ADDR, sil_reb_mem(PORTC_PDR_ADDR) | PORT_PDR_B0_BIT);
	/* INS=IN */
	sil_wrb_mem(PORT1_PDR_ADDR, sil_reb_mem(PORT1_PDR_ADDR) & ~PORT_PDR_B5_BIT);
	/* SPICLK,SPIMOSI=OUT */
	sil_wrb_mem(PORTC_PDR_ADDR, sil_reb_mem(PORTC_PDR_ADDR) | PORT_PDR_B5_BIT | PORT_PDR_B6_BIT);
	/* CS,SPICLK,SPIMOSI=HI */
	sil_wrb_mem(PORTC_PODR_ADDR, sil_reb_mem(PORTC_PODR_ADDR) | PORT_PODR_B0_BIT | PORT_PDR_B5_BIT | PORT_PDR_B6_BIT);
	/* SPIMISO=IN */
	sil_wrb_mem(PORTC_PDR_ADDR, sil_reb_mem(PORTC_PDR_ADDR) & ~PORT_PDR_B7_BIT);
	/* SPIMISO=PULLUP */
	sil_wrb_mem(PORTC_PCR_ADDR, sil_reb_mem(PORTC_PDR_ADDR) | PORT_PDR_B7_BIT);
	/* CMOSを選択 */
	sil_wrb_mem(PORTC_ODR0_ADDR, 0x00);
	/* CMOSを選択 */
	sil_wrb_mem(PORTC_ODR1_ADDR, 0x00);
}

#define F_PCLK		48000000UL	/* PCLK frequency (configured by SCKCR.PCK) */
#define SCLK_FAST	24000000UL	/* SCLK frequency (R/W) */
#define SCLK_SLOW	375000UL	/* SCLK frequency (Init) */

static void RSPI_ATTACH()
{
	/*
	 * モジュールストップ機能の設定(RSPI0)
	 */
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA502);	/* 書込み許可 */
	sil_wrw_mem(SYSTEM_MSTPCRB_ADDR,
		sil_rew_mem(SYSTEM_MSTPCRB_ADDR) & ~SYSTEM_MSTPCRB_MSTPB17_BIT);
	sil_wrh_mem(SYSTEM_PRCR_ADDR, (uint16_t)0xA500);	/* 書込み禁止 */

	/* P15をGPIOとして使用 */
	sil_wrb_mem(PORT1_PMR_ADDR,
		sil_reb_mem(PORT1_PMR_ADDR) & ~PORT_PMR_B5_BIT);
	/* PC0をGPIOとして使用 */
	sil_wrb_mem(PORTC_PMR_ADDR,
		sil_reb_mem(PORTC_PMR_ADDR) & ~PORT_PMR_B0_BIT);
	/* PC5, PC6, PC7を周辺機能として使用 */
	sil_wrb_mem(PORTC_PMR_ADDR,
		sil_reb_mem(PORTC_PMR_ADDR) | PORT_PMR_B5_BIT | PORT_PMR_B6_BIT | PORT_PMR_B7_BIT);

	/* 書き込みプロテクトレジスタの設定 PFSWEビットへの書き込みを許可 */
	sil_wrb_mem(MPC_PWPR_ADDR, 0x00);
	/* 書き込みプロテクトレジスタの設定 PxxFSレジスタへの書き込みを許可 */
	sil_wrb_mem(MPC_PWPR_ADDR, 0x40);

	/* RSPCKAを選択 */
	sil_wrb_mem(MPC_PC5PFS_ADDR, 0x0d);
	/* MOSIAを選択 */
	sil_wrb_mem(MPC_PC6PFS_ADDR, 0x0d);
	/* MISOAを選択 */
	sil_wrb_mem(MPC_PC7PFS_ADDR, 0x0d);

	/* 書き込みプロテクトレジスタの設定 書き込みを禁止 */
	sil_wrb_mem(MPC_PWPR_ADDR, 0x80);
}

static void FCLK_FAST()
{
	/* RSPI機能を無効化 */
	sil_wrb_mem(RSPI0_SPCR_ADDR,
		sil_reb_mem(RSPI0_SPCR_ADDR) & ~RSPI_SPCR_SPE_BIT);
	/* ビットレート設定 */
	sil_wrb_mem(RSPI0_SPBR_ADDR, F_PCLK / 2 / SCLK_FAST - 1);
	/* RSPI機能を有効化 */
	sil_wrb_mem(RSPI0_SPCR_ADDR,
		sil_reb_mem(RSPI0_SPCR_ADDR) | RSPI_SPCR_SPE_BIT);
}

#ifdef SIL_ENDIAN_LITTLE
#define LDDW(x) rev_endian_uint32(x)	/* Little endian: swap bytes */
#else
#define LDDW(x) x		/* Big endian: no swap */
#endif



/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* MMC/SD command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */



static volatile
DSTATUS Stat = STA_NOINIT;	/* Physical drive status */

static volatile
WORD Timer1, Timer2;	/* 1000Hz decrement timer stopped at zero (driven by disk_timerproc()) */

static
BYTE CardType;			/* Card type flags */

static
BYTE CSD[16], CID[16];

static
uint64_t CardCapacity;

static
uint32_t CardBlockSize;

/*-----------------------------------------------------------------------*/
/* Control SPI module                                                    */
/*-----------------------------------------------------------------------*/
/* Only these five functions are platform dependent. Any other functions */
/* are portable to different system.                                     */

/*---------------------------------*/
/* Enable SPI and MMC/SDC controls */
/*---------------------------------*/
static
void power_on(void)
{
	/* Initialize CS/INS/WP port */
	CTRL_INIT();

	/* Attach RSPI0 module to I/O pads, disable module stop */
	RSPI_ATTACH();

	/* Initialize RSPI module */
	sil_wrb_mem(RSPI0_SPCR_ADDR, 0);		/* Stop RSPI module */
	sil_wrb_mem(RSPI0_SPPCR_ADDR, 0);	/* Fixed idle value, disable loop-back mode */
	sil_wrb_mem(RSPI0_SPSCR_ADDR, 0);	/* Disable sequence control */
	sil_wrb_mem(RSPI0_SPDCR_ADDR, 0x20);	/* SPLW=1 */
	sil_wrh_mem(RSPI0_SPCMD0_ADDR, 0x0700);	/* LSBF=0, SPB=7, BRDV=0, CPOL=0, CPHA=0 */
	sil_wrb_mem(RSPI0_SPBR_ADDR, F_PCLK / 2 / SCLK_SLOW - 1);	/* Bit rate */
	sil_wrb_mem(RSPI0_SPCR_ADDR, 0xC9);	/* Start RSPI in master mode */

	ena_int(INTNO_MMC_RSPI_SPRI);
	ena_int(INTNO_MMC_RSPI_SPTI);
	ena_int(INTNO_MMC_RSPI_SPII);

	/*sta_cyc(MMC_RSPI_CYC);*/
}


/*---------------------*/
/*    Disable SPI      */
/*---------------------*/
static
void power_off(void)	/* Disable MMC/SDC interface */
{
	/*stp_cyc(MMC_RSPI_CYC);*/

	dis_int(INTNO_MMC_RSPI_SPRI);
	dis_int(INTNO_MMC_RSPI_SPTI);
	dis_int(INTNO_MMC_RSPI_SPII);

	sil_wrb_mem(RSPI0_SPCR_ADDR, 0);		/* Stop RSPI module */
}


/*---------------------*/
/* Send/Receive a byte */
/*---------------------*/
static
BYTE xchg_spi(
	BYTE dat	/* Data to send */
	)
{
	sil_wrb_mem(RSPI0_SPSR_ADDR, 0xA0);
	sil_wrw_mem(RSPI0_SPDR_ADDR, dat);			/* Start transmission (lower 8bits) */
	/*while ((sil_reb_mem(RSPI0_SPSR_ADDR) & RSPI_SPSR_SPRF_BIT) == 0);	/* Wait for end of transfer */
	if (twai_sem(MMC_RSPI_SEMAPHORE, 1000) != E_OK)
		return 0;
	return sil_rew_mem(RSPI0_SPDR_ADDR);			/* Returen received byte (lower 8bits) */
}


/*---------------------*/
/* Send multiple bytes */
/*---------------------*/
static
void xmit_spi_multi(
	const BYTE *buff,	/* Pointer to the data */
	UINT btx			/* Number of bytes to send (multiple of 4) */
	)
{
	const DWORD *lp = (const DWORD*)buff;

	sil_wrh_mem(RSPI0_SPCMD0_ADDR,
		(sil_reh_mem(RSPI0_SPCMD0_ADDR) & ~RSPI_SPCMD_SPB_MASK) | (3 << RSPI_SPCMD_SPB_OFFSET));	/* Set 32-bit mode */

	do {
		sil_wrb_mem(RSPI0_SPSR_ADDR, 0xA0);
		sil_wrw_mem(RSPI0_SPDR_ADDR, LDDW(*lp++));	/* Send four data bytes */
		/*while ((sil_reb_mem(RSPI0_SPSR_ADDR) & RSPI_SPSR_SPRF_BIT) == 0);	/* Wait for end of transfer */
		wai_sem(MMC_RSPI_SEMAPHORE);
		sil_rew_mem(RSPI0_SPDR_ADDR);					/* Discard four received bytes */
	} while (btx -= 4);					/* Repeat until all data sent */

	sil_wrh_mem(RSPI0_SPCMD0_ADDR,
		(sil_reh_mem(RSPI0_SPCMD0_ADDR) & ~RSPI_SPCMD_SPB_MASK) | (7 << RSPI_SPCMD_SPB_OFFSET));	/* Set 8-bit mode */
}


/*------------------------*/
/* Receive multiple bytes */
/*------------------------*/
static
void rcvr_spi_multi(
	BYTE *buff,		/* Pointer to data buffer */
	UINT btr		/* Number of bytes to receive (multiple of 4) */
	)
{
	DWORD *lp = (DWORD*)buff;

	sil_wrh_mem(RSPI0_SPCMD0_ADDR,
		(sil_reh_mem(RSPI0_SPCMD0_ADDR) & ~RSPI_SPCMD_SPB_MASK) | (3 << RSPI_SPCMD_SPB_OFFSET));	/* Set 32-bit mode */

	do {
		sil_wrb_mem(RSPI0_SPSR_ADDR, 0xA0);
		sil_wrw_mem(RSPI0_SPDR_ADDR, 0xFFFFFFFF);	/* Send four 0xFFs */
		/*while ((sil_reb_mem(RSPI0_SPSR_ADDR) & RSPI_SPSR_SPRF_BIT) == 0);	/* Wait for end of transfer */
		wai_sem(MMC_RSPI_SEMAPHORE);
		*lp++ = LDDW(sil_rew_mem(RSPI0_SPDR_ADDR));	/* Store four received bytes */
	} while (btr -= 4);					/* Repeat until all data received */

	sil_wrh_mem(RSPI0_SPCMD0_ADDR,
		(sil_reh_mem(RSPI0_SPCMD0_ADDR) & ~RSPI_SPCMD_SPB_MASK) | (7 << RSPI_SPCMD_SPB_OFFSET));	/* Set 8-bit mode */
}




/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
int wait_ready(	/* 1:Ready, 0:Timeout */
	UINT wt			/* Timeout [ms] */
	)
{
	Timer2 = (WORD)wt;

	do {
		if (xchg_spi(0xFF) == 0xFF) return 1;	/* Card goes ready */
		/* This loop takes a time. Insert rot_rdq() here for multitask envilonment. */
	} while (Timer2);	/* Wait until card goes ready or timeout */

	return 0;	/* Timeout occured */
}



/*-----------------------------------------------------------------------*/
/* Deselect card and release SPI                                         */
/*-----------------------------------------------------------------------*/

static
void deselect(void)
{
	CS_HIGH();		/* Set CS# high */
	xchg_spi(0xFF);	/* Dummy clock (force DO hi-z for multiple slave SPI) */
}



/*-----------------------------------------------------------------------*/
/* Select card and wait for ready                                        */
/*-----------------------------------------------------------------------*/

static
int select(void)	/* 1:OK, 0:Timeout */
{
	CS_LOW();		/* Set CS# low */
	xchg_spi(0xFF);	/* Dummy clock (force DO enabled) */

	if (wait_ready(500)) return 1;	/* Wait for card ready */

	deselect();
	return 0;	/* Failed to select the card due to timeout */
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from the MMC                                    */
/*-----------------------------------------------------------------------*/

static
int rcvr_datablock(	/* 1:OK, 0:Error */
	BYTE *buff,			/* Data buffer */
	UINT btr			/* Data block length (byte) */
	)
{
	BYTE token;


	Timer1 = 200;
	do {							/* Wait for DataStart token in timeout of 200ms */
		token = xchg_spi(0xFF);
		/* This loop will take a time. Insert rot_rdq() here for multitask envilonment. */
	} while ((token == 0xFF) && Timer1);
	if (token != 0xFE) return 0;	/* Function fails if invalid DataStart token or timeout */

	rcvr_spi_multi(buff, btr);		/* Store trailing data to the buffer */
	xchg_spi(0xFF); xchg_spi(0xFF);	/* Discard CRC */

	return 1;						/* Function succeeded */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to the MMC                                         */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
static
int xmit_datablock(	/* 1:OK, 0:Failed */
	const BYTE *buff,	/* Ponter to 512 byte data to be sent */
	BYTE token			/* Token */
	)
{
	BYTE resp;


	if (!wait_ready(500)) return 0;		/* Wait for card ready */

	xchg_spi(token);					/* Send token */
	if (token != 0xFD) {				/* Send data if token is other than StopTran */
		xmit_spi_multi(buff, 512);		/* Data */
		xchg_spi(0xFF); xchg_spi(0xFF);	/* Dummy CRC */

		resp = xchg_spi(0xFF);			/* Receive data resp */
		if ((resp & 0x1F) != 0x05)		/* Function fails if the data packet was not accepted */
			return 0;
	}
	return 1;
}
#endif /* _USE_WRITE */



/*-----------------------------------------------------------------------*/
/* Send a command packet to the MMC                                      */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd(		/* Return value: R1 resp (bit7==1:Failed to send) */
	BYTE cmd,		/* Command index */
	DWORD arg		/* Argument */
	)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* Send a CMD55 prior to ACMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		deselect();
		if (!select()) return 0xFF;
	}

	/* Send command packet */
	xchg_spi(0x40 | cmd);			/* Start + command index */
	xchg_spi((BYTE)(arg >> 24));	/* Argument[31..24] */
	xchg_spi((BYTE)(arg >> 16));	/* Argument[23..16] */
	xchg_spi((BYTE)(arg >> 8));		/* Argument[15..8] */
	xchg_spi((BYTE)arg);			/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;		/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;		/* Valid CRC for CMD8(0x1AA) */
	xchg_spi(n);

	/* Receive command resp */
	if (cmd == CMD12) xchg_spi(0xFF);	/* Diacard stuff byte on CMD12 */
	n = 10;							/* Wait for response (10 bytes max) */
	do
		res = xchg_spi(0xFF);
	while ((res & 0x80) && --n);

	return res;						/* Return received response */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize disk drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS mmc_rspi_initialize()
{
	BYTE n, cmd, ty, ocr[4];

	power_on();							/* Initialize RSPI */
	for (n = 10; n; n--) xchg_spi(0xFF);/* Send 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Put the card SPI/Idle state */
		Timer1 = 1000;						/* Initialization timeout = 1 sec */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
			for (n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF);	/* Get 32 bit return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* Is the card supports vcc of 2.7-3.6V? */
				while (Timer1 && send_cmd(ACMD41, 1UL << 30));	/* Wait for end of initialization with ACMD41(HCS) */
				if (Timer1 && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF);
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* Card id SDv2 */
				}
			}
		}
		else {	/* Not SDv2 card */
			if (send_cmd(ACMD41, 0) <= 1) {	/* SDv1 or MMC? */
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 (ACMD41(0)) */
			}
			else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 (CMD1(0)) */
			}
			while (Timer1 && send_cmd(cmd, 0));		/* Wait for end of initialization */
			//if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set block length: 512 */
			//	ty = 0;
		}
		if (ty) {
			if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
				&& rcvr_datablock(CSD, 16)){
				/* CSD Ver.1.0 */
				if ((CSD[0] & 0xC0) == 0) {
					CardBlockSize = 1 << (CSD[5] & 0x0F);
					CardCapacity = (((CSD[6] & 0x03) << 10) | (CSD[7] << 2) | ((CSD[8] & 0xC0) >> 6)) + 1;
					CardCapacity *= (((uint64_t)1) << ((((CSD[9] & 0x03) << 1) | (CSD[10] & 0x80) >> 7) + 2));
					CardCapacity *= (uint64_t)CardBlockSize;
				}
				else {
					CardBlockSize = 512;
					CardCapacity = ((uint64_t)((((CSD[7] & 0x3F) << 16) | (CSD[8] << 8) | CSD[9]) + 1)) * 512 * 1024;
				}
				if (send_cmd(CMD10, 0) != 0		/* READ_CID */
					|| !rcvr_datablock(CID, 16)) {
					ty = 0;
				}
			}
			else {
				ty = 0;
			}
		}
	}
	CardType = ty;	/* Card type */
	deselect();

	if (ty) {			/* OK */
		FCLK_FAST();			/* Set fast clock */
		Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
	}
	else {			/* Failed */
		power_off();
		Stat = STA_NOINIT;
	}

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS mmc_rspi_status()
{
	return Stat;	/* Return disk status */
}



/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT mmc_rspi_read(
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
	)
{
	if (!count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* LBA ot BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, 512))
			count = 0;
	}
	else {				/* Multiple sector read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}



/*-----------------------------------------------------------------------*/
/* Write sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT mmc_rspi_write(
	const BYTE *buff,	/* Ponter to the data to write */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
	)
{
	if (!count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check drive status */
	if (Stat & STA_PROTECT) return RES_WRPRT;	/* Check write protect */

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* LBA ==> BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple sector write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}
#endif /* _USE_WRITE */



/*-----------------------------------------------------------------------*/
/* Miscellaneous drive controls other than data read/write               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT mmc_rspi_ioctl(
	BYTE ctrl,		/* Control command code */
	void *buff		/* Pointer to the conrtol data */
	)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	DWORD *dp, st, ed;


	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	res = RES_ERROR;

	switch (ctrl) {
	case CTRL_SYNC:		/* Wait for end of internal write process of the drive */
		if (select()) res = RES_OK;
		break;

	case GET_SECTOR_SIZE:
		*(WORD*)buff = 512;
		res = RES_OK;
		break;

	case GET_SECTOR_COUNT:
		*(DWORD*)buff = CardCapacity / 512;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE:
		*(DWORD*)buff = CardBlockSize;
		res = RES_OK;
		break;

	case CTRL_TRIM:	/* Erase a block of sectors (used when FF_USE_TRIM == 1) */
		if (!(CardType & CT_SDC)) break;				/* Check if the card is SDC */
		if (mmc_rspi_ioctl(MMC_GET_CSD, csd)) break;	/* Get CSD */
		if (!(csd[0] >> 6) && !(csd[10] & 0x40)) break;	/* Check if sector erase can be applied to the card */
		dp = buff; st = dp[0]; ed = dp[1];				/* Load sector block */
		if (!(CardType & CT_BLOCK)) {
			st *= 512; ed *= 512;
		}
		if (send_cmd(CMD32, st) == 0 && send_cmd(CMD33, ed) == 0 && send_cmd(CMD38, 0) == 0 && wait_ready(30000))	/* Erase sector block */
			res = RES_OK;
		break;

		/* Following command are not used by FatFs module */

	case MMC_GET_TYPE:		/* Get MMC/SDC type (BYTE) */
		*ptr = CardType;
		res = RES_OK;
		break;

	case MMC_GET_CSD:		/* Read CSD (16 bytes) */
		memcpy(ptr, CSD, sizeof(CSD));
		res = RES_OK;
		break;

	case MMC_GET_CID:		/* Read CID (16 bytes) */
		memcpy(ptr, CID, sizeof(CID));
		res = RES_OK;
		break;

	case MMC_GET_OCR:		/* Read OCR (4 bytes) */
		if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
			for (n = 4; n; n--) *ptr++ = xchg_spi(0xFF);
			res = RES_OK;
		}
		break;

	case MMC_GET_SDSTAT:	/* Read SD status (64 bytes) */
		if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
			xchg_spi(0xFF);
			if (rcvr_datablock(ptr, 64))
				res = RES_OK;
		}
		break;

	default:
		res = RES_PARERR;
	}

	deselect();

	return res;
}
#endif /* _USE_IOCTL */


/*-----------------------------------------------------------------------*/
/* Device timer function                                                 */
/*-----------------------------------------------------------------------*/
/* This function must be called from system timer process
/  in period of 1 ms to generate card control timing.
*/

void mmc_rspi_cychdr(intptr_t exinf)
{
	WORD n;
	BYTE s;


	n = Timer1;						/* 1kHz decrement timer stopped at 0 */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;

	s = Stat;
	if (WP())		/* Write protected */
		s |= STA_PROTECT;
	else		/* Write enabled */
		s &= ~STA_PROTECT;
	if (INS())	/* Card is in socket */
		s &= ~STA_NODISK;
	else		/* Socket empty */
		s |= (STA_NODISK | STA_NOINIT);

	if (Stat != s) {
		mmc_rspi_status_changed(s);
		Stat = s;
	}
}

void mmc_rspi_spri_handler(void)
{
	isig_sem(MMC_RSPI_SEMAPHORE);
}

void mmc_rspi_spti_handler(void)
{
}

void mmc_rspi_spii_handler(void)
{
}

