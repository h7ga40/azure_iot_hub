//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SDCard.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#include "StdAfx.h"
#include "WinKernel.h"
#include "SDCard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CKernel *g_Kernel;

/*-----------------------------------------------------------------------*/
/* Low level disk control module for Win32              (C)ChaN, 2013    */
/*-----------------------------------------------------------------------*/
#define	FF_MIN_SS		512
#define	FF_MAX_SS		2048

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at FF_FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at FF_USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at FF_USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */

#define SZ_RAMDISK	768	/* Size of RAM disk [kB] */
#define SS_RAMDISK	512	/* Sector size of RAM disk [byte] */

#define	BUFSIZE 262144UL	/* Size of data transfer buffer */

typedef struct STAT
{
	DSTATUS status;
	WORD sz_sector;
	DWORD n_sectors;
	HANDLE h_drive;
} STAT;

/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */

CSDCard::CSDCard(int devNo)
{
	m_DevNo = devNo;
	Stat = new struct STAT[1];
	RamDisk = NULL;
	m_Timer = -1;
	m_MMCard = NULL;
	m_SDSpi = NULL;
}

CSDCard::~CSDCard()
{
	delete Stat;
	if (RamDisk != NULL)
		VirtualFree(RamDisk, 0, MEM_RELEASE);
}

void CSDCard::OnStart()
{
	m_MMCard = MMCard_New("card0");
	if (m_MMCard) {
		m_SDSpi = SDSpi_New("sdspi0", m_MMCard);
	}
}

void CSDCard::OnTerminate()
{
}

__int64 CSDCard::GetTimer()
{
	return m_Timer;
}

void CSDCard::Progress(__int64 timer)
{
	if (m_Timer == -1)
		return;

	m_Timer -= timer;
	if (m_Timer < 0)
		m_Timer = 0;
}

void CSDCard::CallTimeOut()
{
	if (m_Timer != 0)
		return;

}

void CSDCard::Receive(const void *data, int size)
{
	uint8_t *send = new uint8_t[size];
	for (int i = 0; i < size; i++) {
		send[i] = SDSpi_ByteExchange(m_SDSpi, ((uint8_t *)data)[i]);
	}
	Send(send, size);
	delete send;
}

void CSDCard::Send(const void *data, int size)
{
	if (Port != NULL)
		Port->Receive(data, size);
	else
		g_Kernel->Output(m_DevNo, data, size);
}

int CSDCard::get_status(BYTE pdrv)
{
	volatile STAT *stat = &Stat[pdrv];
	HANDLE h = stat->h_drive;
	DWORD dw;

	if (pdrv == 0) {	/* RAMDISK */
		stat->sz_sector = SS_RAMDISK;
		if (stat->sz_sector < FF_MIN_SS || stat->sz_sector > FF_MAX_SS) return 0;
		stat->n_sectors = SZ_RAMDISK * 1024 / SS_RAMDISK;
		stat->status = 0;
		return 1;
	}

	return 0;
}

/*-----------------------------------------------------------------------*/
/* Initialize Windows disk accesss layer                                 */
/*-----------------------------------------------------------------------*/
int CSDCard::assign_drives(void)
{
	BYTE pdrv, ndrv;
	TCHAR str[30];
	HANDLE h;
	OSVERSIONINFO vinfo = { sizeof(OSVERSIONINFO) };

	RamDisk = (BYTE *)VirtualAlloc(0, SZ_RAMDISK * 1024, MEM_COMMIT, PAGE_READWRITE);
	if (!RamDisk) return 0;

	pdrv = 0;
	/* \\.\PhysicalDrive0 is not mapped to disk function and map a RAM disk instead. */
	_stprintf_s(str, sizeof(str) / sizeof(str[0]), _T("RAM Disk"));
	_tprintf(_T("PD#%u <== %s"), pdrv, str);
	if (get_status(pdrv))
		_tprintf(_T(" (%uMB, %u bytes * %u sectors)\n"), (UINT)((LONGLONG)Stat[pdrv].sz_sector * Stat[pdrv].n_sectors / 1024 / 1024), Stat[pdrv].sz_sector, Stat[pdrv].n_sectors);
	else
		_tprintf(_T(" (Not Ready)\n"));
	pdrv++;

	Drives = pdrv;
	return pdrv;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS CSDCard::disk_initialize(
	BYTE pdrv		/* Physical drive nmuber */
	)
{
	DSTATUS sta;

	if (pdrv >= Drives) {
		sta = STA_NOINIT;
	}
	else {
		get_status(pdrv);
		sta = Stat[pdrv].status;
	}

	return sta;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS CSDCard::disk_status(
	BYTE pdrv		/* Physical drive nmuber (0) */
	)
{
	DSTATUS sta;

	if (pdrv >= Drives) {
		sta = STA_NOINIT;
	}
	else {
		sta = Stat[pdrv].status;
	}

	return sta;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT CSDCard::disk_read(
	BYTE pdrv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to read */
	)
{
	DWORD nc, rnc;
	LARGE_INTEGER ofs;
	DRESULT res;

	if (pdrv >= Drives || Stat[pdrv].status & STA_NOINIT)
		return RES_NOTRDY;

	nc = (DWORD)count * Stat[pdrv].sz_sector;
	ofs.QuadPart = (LONGLONG)sector * Stat[pdrv].sz_sector;
	memcpy(buff, RamDisk + ofs.LowPart, nc);
	res = RES_OK;

	return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
DRESULT CSDCard::disk_write(
	BYTE pdrv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to write */
	)
{
	DWORD nc, rnc;
	LARGE_INTEGER ofs;
	DRESULT res;

	if (pdrv >= Drives || Stat[pdrv].status & STA_NOINIT)
		return RES_NOTRDY;

	res = RES_OK;
	if (Stat[pdrv].status & STA_PROTECT) {
		return RES_WRPRT;
	}
	else {
		nc = (DWORD)count * Stat[pdrv].sz_sector;
		if (nc > BUFSIZE)
			res = RES_PARERR;
	}

	ofs.QuadPart = (LONGLONG)sector * Stat[pdrv].sz_sector;
	memcpy(RamDisk + ofs.LowPart, buff, nc);
	res = RES_OK;

	return res;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT CSDCard::disk_ioctl(
	BYTE pdrv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
	)
{
	DRESULT res = RES_PARERR;

	if (pdrv >= Drives || (Stat[pdrv].status & STA_NOINIT))
		return RES_NOTRDY;

	switch (ctrl) {
	case CTRL_SYNC:
		res = RES_OK;
		break;

	case GET_SECTOR_COUNT:
		*(DWORD*)buff = Stat[pdrv].n_sectors;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE:
		*(WORD*)buff = Stat[pdrv].sz_sector;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 128;
		res = RES_OK;
		break;

	case 200: {
			HANDLE h;
			DWORD br;

			h = CreateFile((TCHAR*)buff, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
			if (!pdrv && h != INVALID_HANDLE_VALUE) {
				if (ReadFile(h, RamDisk, SZ_RAMDISK * 1024, &br, 0))
					res = RES_OK;
				CloseHandle(h);
			}
		}
		break;

	case 201: {
			HANDLE h;
			DWORD br;

			h = CreateFile((TCHAR*)buff, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if (!pdrv && h != INVALID_HANDLE_VALUE) {
				if (WriteFile(h, RamDisk, SZ_RAMDISK * 1024, &br, 0))
					res = RES_OK;
				CloseHandle(h);
			}
		}
		break;
	}

	return res;
}

