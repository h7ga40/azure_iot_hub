//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifS12AD.h 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#ifndef SifS12ADH
#define SifS12ADH

//---------------------------------------------------------------------------
#include "WinKernel.h"
#include "RX63NReg.h"
#include "SIFLayer.h"

enum TS12ADTerm {
	s12adtInterrupt,
	s12adtWait,
};

//------------------------------------------------------------------------------
// にせS12ADクラス
//------------------------------------------------------------------------------
class CSifS12AD : public ISysTimerSync {
public:
	CSifS12AD(TRX63NS12ADReg *flash);
	virtual ~CSifS12AD();
private:
	TRX63NS12ADReg *m_S12AD;
	TRX63NS12ADReg m_S12ADInst;
	TS12ADTerm m_Term;
	__int64 m_Timer;
public:
	unsigned char GetByte(intptr_t index);
	void SetByte(intptr_t index, unsigned char value);
	unsigned short GetUInt16(uintptr_t index);
	void SetUInt16(uintptr_t index, unsigned short value);
public:
	__int64 GetTimer();
	void Progress(__int64 timer);
	void CallTimeOut();
};

//---------------------------------------------------------------------------
#endif // SifS12ADH
