//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifSCI.h 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#ifndef SifSCIH
#define SifSCIH

//---------------------------------------------------------------------------
#include "WinKernel.h"
#include "RX63NReg.h"
#include "SIFLayer.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#pragma pack(push, 1)
struct TSCIDataNode {
	int Size;
	unsigned char Data[1];
};
#pragma pack(pop)

typedef list<TSCIDataNode *> CSCIDataQueue;

enum TSCISendTerm
{
	scistIdle,
	scistSending,
};

enum TSCIRecvTerm {
	scirtIdle,
	scirtReceiving,
	scirtWait,
};

//------------------------------------------------------------------------------
// にせSCI クラス
//------------------------------------------------------------------------------
class CSifSCI : public ISysTimerSync {
public:
	CSifSCI(TRX63NSCI0Reg *reg, int txIntNo, int rxIntNo, int teIntNo, int devNo);
	virtual ~CSifSCI();
private:
	TRX63NSCI0Reg *m_Reg;
	TRX63NSCI0Reg m_RegInst;
	int m_TxIntNo;
	int m_RxIntNo;
	int m_TeIntNo;
	int m_DevNo;
	unsigned char m_SendBuf[256];
	int m_SendBufPos;
	CRITICAL_SECTION m_Lock;
	TSCISendTerm m_SendTerm;
	__int64 m_SendTimer;
	CSCIDataQueue m_RecvQueue;
	TSCIDataNode *m_RecvBuf;
	int m_RecvBufPos;
	TSCIRecvTerm m_RecvTerm;
	__int64 m_RecvTimer;
	__int64 CalcTimer();
protected:
	class Lock {
		CRITICAL_SECTION *m_Lock;
	public:
		Lock(CSifSCI *_this) { m_Lock = &_this->m_Lock; EnterCriticalSection(m_Lock); }
		~Lock() { LeaveCriticalSection(m_Lock); }
	};
	void CallSendTimeOut();
	void CallRecvTimeOut();
public:
	unsigned char GetByte(intptr_t index);
	void SetByte(intptr_t index, unsigned char value);
public:
	__int64 GetTimer();
	void Progress(__int64 timer);
	void CallTimeOut();
	void Receive(const void *data, int size);
};

//---------------------------------------------------------------------------
#endif // SifSCIH
