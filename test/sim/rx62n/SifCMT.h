//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifCMT.h 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#ifndef SifCMTH
#define SifCMTH

//---------------------------------------------------------------------------
#include "WinKernel.h"
#include "RX62NReg.h"
#include "SIFLayer.h"

enum TTimerTerm {
	ttInterrupt,
	ttWait,
};

//------------------------------------------------------------------------------
// にせSCI クラス
//------------------------------------------------------------------------------
class CSifCMT : public ISysTimerSync {
public:
	CSifCMT(TCMTCMSTR0 *cmstr, uint16_t str, TRX62NCMT0Reg *cmt, int txIntNo);
	virtual ~CSifCMT();
private:
	TCMTCMSTR0 *m_CMSTR;
	uint16_t m_STR;
	TRX62NCMT0Reg *m_CMT;
	TRX62NCMT0Reg m_CMTInst;
	int m_IntNo;
	TTimerTerm m_Term;
	int m_StartReq;
	int m_StartRes;
	__int64 m_TimerOrigin;
	__int64 m_Timer;
	__int64 CalcTimer();
public:
	unsigned char GetByte(intptr_t index);
	void SetByte(intptr_t index, unsigned char value);
	void Enabled(bool enabled);
public:
	__int64 GetTimer();
	void Progress(__int64 timer);
	void CallTimeOut();
};

//---------------------------------------------------------------------------
#endif // SifCMTH
