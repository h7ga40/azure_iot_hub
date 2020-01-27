//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SDCard.h 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#ifndef SDCardH
#define SDCardH

//---------------------------------------------------------------------------
#include "Kernel.h"
#include "RX62NReg.h"
#include "SIFLayer.h"
#include "sd_spi.h"

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum
{
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

class CSDCard : public ISysTimerSync, public ICommunicationPort
{
public:
	CSDCard(int devNo);
	virtual ~CSDCard();
private:
	int m_DevNo;
	__int64 m_Timer;
	MMCDev *m_MMCard;
	SD_Spi *m_SDSpi;
	int Drives;
	struct STAT *Stat;
	BYTE *RamDisk;
	void Send(const void *data, int size);
	int get_status(BYTE pdrv);
	int assign_drives(void);
	DSTATUS disk_initialize(BYTE pdrv);
	DSTATUS disk_status(BYTE pdrv);
	DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
	DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
	DRESULT disk_ioctl(BYTE pdrv, BYTE ctrl, void *buff);
public:	// ISysTimerSync
	void OnStart();
	void OnTerminate();
	__int64 GetTimer();
	void Progress(__int64 Timer);
	void CallTimeOut();
	int GetDevNo() { return m_DevNo; }
	void Receive(const void *Data, int Size);
	ICommunicationPort *Port;
};

#endif // SDCardH
