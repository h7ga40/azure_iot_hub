//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifRX63N.h 1970 2019-07-10 00:36:30Z coas-nagasima $
#ifndef SifRX63NH
#define SifRX63NH

//---------------------------------------------------------------------------
#include "WinKernel.h"
#include "RX63NReg.h"
#include "SifCMT.h"
#include "SifRIIC.h"
#include "SifRSPI.h"
#include "SifSCI.h"
#include "SifFLASH.h"
#include "SifS12AD.h"
#include "SDCard.h"

#define INTERRUPT_COUNT 256

//------------------------------------------------------------------------------
// 贋 PBXスレッド クラス
//  概要：
//  贋 PBXスレッド クラス
//------------------------------------------------------------------------------
class CSifRX63N : public ISifCPU, public ISystemIF, public ISysTimerSync
{
public:
	CSifRX63N(TRX63NReg *RX63NReg, TRX63NFLASHReg *FLASHReg);
	virtual ~CSifRX63N();
private:
	TRX63NReg *m_CPUReg;
	TRX63NFLASHReg *m_FLASHReg;
	//unsigned char m_SYSTEMReg1[(intptr_t)&(&((TRX63NReg *)0)->SYSTEM.OSTDCR)[1] - (intptr_t)&((TRX63NReg *)0)->SYSTEM];
	unsigned char m_SYSTEMReg1[0x00080042 - 0x00080000];
	//unsigned char m_SYSTEMReg2[(intptr_t)&(&((TRX63NReg *)0)->SYSTEM.DPSBKR31)[1] - (intptr_t)&((TRX63NReg *)0)->SYSTEM.DPSBYCR];
	unsigned char m_SYSTEMReg2[0x0008C2B0 - 0x0008C280];
	//unsigned char m_BSCReg1[(intptr_t)&(&((TRX63NReg *)0)->BSC.BERSR2)[1] - (intptr_t)&((TRX63NReg *)0)->BSC];
	unsigned char m_BSCReg1[0x0008130C - 0x00081300];
	//unsigned char m_BSCReg2[(intptr_t)&(&((TRX63NReg *)0)->BSC.SDSR)[1] - (intptr_t)&((TRX63NReg *)0)->BSC.CS0MOD];
	unsigned char m_BSCReg2[0x00083C52 - 0x00083002];
	TCMTCMSTR0 m_CMSTR0Reg;
	TCMTCMSTR0 m_CMSTR1Reg;
	TRX63NICUReg m_ICUReg;
	TRX63NRTCReg m_RTCReg;
	TRX63NPORT0Reg m_PORT0Reg;
	TRX63NPORT1Reg m_PORT1Reg;
	TRX63NPORT2Reg m_PORT2Reg;
	TRX63NPORT3Reg m_PORT3Reg;
	TRX63NPORT4Reg m_PORT4Reg;
	TRX63NPORT5Reg m_PORT5Reg;
	TRX63NPORT6Reg m_PORT6Reg;
	TRX63NPORT7Reg m_PORT7Reg;
	TRX63NPORT8Reg m_PORT8Reg;
	TRX63NPORT9Reg m_PORT9Reg;
	TRX63NPORTAReg m_PORTAReg;
	TRX63NPORTBReg m_PORTBReg;
	TRX63NPORTCReg m_PORTCReg;
	TRX63NPORTDReg m_PORTDReg;
	TRX63NPORTEReg m_PORTEReg;
	TRX63NPORTFReg m_PORTFReg;
	TRX63NPORTGReg m_PORTGReg;
	TRX63NMPCReg m_MPCReg;
	TRX63NS12ADReg m_S12ADReg;
	void *m_InstRomPEAddr;
	int m_InstRomSize;
	LONG m_Lock;
	ISysTimerSync *Syncs[24];
	typedef void (*IntHandler)();
	IntHandler m_IntHandler[INTERRUPT_COUNT];
	unsigned char GetByte2(intptr_t index);
	void SetByte2(intptr_t index, unsigned char Value);
protected:
	void SpinLock(){ while(InterlockedCompareExchange(&m_Lock, 1, 0) == 1) SwitchToThread(); }
	void SpinUnlock(){ InterlockedExchange(&m_Lock, 0); }
public:
	int GetInterruptCount() { return INTERRUPT_COUNT; }
	bool InterruptUnmasked(int IntNo);
	bool InterruptRaised(int IntNo);
	void ClearInterrupt(int IntNo);
	void SetInterrupt(int IntNo);
	unsigned int InterruptPriority(int IntNo);
	bool DefineInterruptHandler(int IntNo, void(*Handler)());
	bool ChangeInterruptControlRegister(int IntNo, uint8_t icrcmd);
	void InvokeInterrupt(int IntNo);
public:	// ISystemIF
	virtual unsigned char GetByte(uintptr_t Addr);
	virtual void SetByte(uintptr_t Addr, unsigned char Value);
	virtual unsigned short GetUInt16(uintptr_t Addr);
	virtual void SetUInt16(uintptr_t Addr, unsigned short Value);
	virtual unsigned int GetUInt32(uintptr_t Addr);
	virtual void SetUInt32(uintptr_t Addr, unsigned int Value);
public:	// ISysTimerSync
	__int64 GetTimer();
	void Progress(__int64 Timer);
	void CallTimeOut();
public:
	void OnStart();
	void OnTerminate();
public:
	CSifCMT CMT0;
	CSifCMT CMT1;
	CSifCMT CMT2;
	CSifCMT CMT3;
	CSifRIIC RIIC0;
	CSifRIIC RIIC1;
	CSifRSPI RSPI0;
	CSifRSPI RSPI1;
	CSifSCI SCI0;
	CSifSCI SCI1;
	CSifSCI SCI2;
	CSifSCI SCI3;
	CSifSCI SCI4;
	CSifSCI SCI5;
	CSifSCI SCI6;
	CSifSCI SCI7;
	CSifSCI SCI8;
	CSifSCI SCI9;
	CSifSCI SCI10;
	CSifSCI SCI11;
	CSifSCI SCI12;
	CSifFLASH FLASH;
	CSifS12AD S12AD;
	CSDCard SDCard;
};

//---------------------------------------------------------------------------
#endif // SifRX63NH
