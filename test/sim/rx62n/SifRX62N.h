//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifRX62N.h 1970 2019-07-10 00:36:30Z coas-nagasima $
#ifndef SifRX62NH
#define SifRX62NH

//---------------------------------------------------------------------------
#include "WinKernel.h"
#include "RX62NReg.h"
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
class CSifRX62N : public ISifCPU, public ISystemIF, public ISysTimerSync
{
public:
	CSifRX62N(TRX62NReg *RX62NReg, TRX62NFLASHReg *FLASHReg);
	virtual ~CSifRX62N();
private:
	TRX62NReg *m_CPUReg;
	TRX62NFLASHReg *m_FLASHReg;
	//unsigned char m_SYSTEMReg1[(intptr_t)&(&((TRX62NReg *)0)->SYSTEM.OSTDCR)[1] - (intptr_t)&((TRX62NReg *)0)->SYSTEM];
	unsigned char m_SYSTEMReg1[0x00080042 - 0x00080000];
	//unsigned char m_SYSTEMReg2[(intptr_t)&(&((TRX62NReg *)0)->SYSTEM.DPSBKR31)[1] - (intptr_t)&((TRX62NReg *)0)->SYSTEM.DPSBYCR];
	unsigned char m_SYSTEMReg2[0x0008C2B0 - 0x0008C280];
	//unsigned char m_BSCReg1[(intptr_t)&(&((TRX62NReg *)0)->BSC.BERSR2)[1] - (intptr_t)&((TRX62NReg *)0)->BSC];
	unsigned char m_BSCReg1[0x0008130C - 0x00081300];
	//unsigned char m_BSCReg2[(intptr_t)&(&((TRX62NReg *)0)->BSC.SDSR)[1] - (intptr_t)&((TRX62NReg *)0)->BSC.CS0MOD];
	unsigned char m_BSCReg2[0x00083C52 - 0x00083002];
	TCMTCMSTR0 m_CMSTR0Reg;
	TCMTCMSTR0 m_CMSTR1Reg;
	TRX62NICUReg m_ICUReg;
	TRX62NRTCReg m_RTCReg;
	TRX62NPORT0Reg m_PORT0Reg;
	TRX62NPORT1Reg m_PORT1Reg;
	TRX62NPORT2Reg m_PORT2Reg;
	TRX62NPORT3Reg m_PORT3Reg;
	TRX62NPORT4Reg m_PORT4Reg;
	TRX62NPORT5Reg m_PORT5Reg;
	TRX62NPORT6Reg m_PORT6Reg;
	TRX62NPORT7Reg m_PORT7Reg;
	TRX62NPORT8Reg m_PORT8Reg;
	TRX62NPORT9Reg m_PORT9Reg;
	TRX62NPORTAReg m_PORTAReg;
	TRX62NPORTBReg m_PORTBReg;
	TRX62NPORTCReg m_PORTCReg;
	TRX62NPORTDReg m_PORTDReg;
	TRX62NPORTEReg m_PORTEReg;
	TRX62NPORTFReg m_PORTFReg;
	TRX62NPORTGReg m_PORTGReg;
	TRX62NIOPORTReg m_IOPORTReg;
	TRX62NS12ADReg m_S12ADReg;
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
	CSifSCI SCI5;
	CSifSCI SCI6;
	CSifFLASH FLASH;
	CSifS12AD S12AD;
	CSDCard SDCard;
};

//---------------------------------------------------------------------------
#endif // SifRX62NH
