//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifFLASH.h 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#ifndef SifFLASHH
#define SifFLASHH

//---------------------------------------------------------------------------
#include "WinKernel.h"
#include "RX63NReg.h"
#include "SIFLayer.h"

enum TFlashTerm {
	ftInterrupt,
	ftWait,
};

//------------------------------------------------------------------------------
// にせROM クラス
//------------------------------------------------------------------------------
class CSifFLASH : public ISysTimerSync {
public:
	CSifFLASH(TRX63NFLASHReg *flash);
	virtual ~CSifFLASH();
private:
	TRX63NFLASHReg *m_FLASH;
	TRX63NFLASHReg m_FLASHInst;
	TFlashTerm m_Term;
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
#endif // SifFLASHH
