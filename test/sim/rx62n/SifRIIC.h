//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifRIIC.h 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#ifndef SifRIICH
#define SifRIICH

//---------------------------------------------------------------------------
#include "WinKernel.h"
#include "RX62NReg.h"
#include "SIFLayer.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#pragma pack(push, 1)
struct TRIICDataNode {
	int Size;
	unsigned char Data[1];
};
#pragma pack(pop)

typedef list<TRIICDataNode *> CRIICDataQueue;

enum TRIICSendTerm
{
	riicstIdle,
	riicstStart,
	riicstWrite,
	riicstSending,
	riicstSendWait,
	riicstReceiving,
	riicstReceiveWait,
	riicstStop,
	riicstFlush,
};

//------------------------------------------------------------------------------
// にせRIIC クラス
//------------------------------------------------------------------------------
class CSifRIIC : public ISysTimerSync {
public:
	CSifRIIC(TRX62NRIIC0Reg *reg, int txIntNo, int rxIntNo, int teIntNo, int devNo);
	virtual ~CSifRIIC();
private:
	TRX62NRIIC0Reg *m_Reg;
	TRX62NRIIC0Reg m_RegInst;
	int m_TxIntNo;
	int m_RxIntNo;
	int m_TeIntNo;
	int m_DevNo;
	unsigned char m_SendBuf[256];
	int m_SendBufPos;
	TRIICSendTerm m_SendTerm;
	__int64 m_SendTimer;
	__int64 CalcTimer();
public:
	unsigned char GetByte(intptr_t index);
	void SetByte(intptr_t index, unsigned char value);
public:
	__int64 GetTimer();
	void Progress(__int64 timer);
	void CallTimeOut();
};

//---------------------------------------------------------------------------
#endif // SifRIICH
