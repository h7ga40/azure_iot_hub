//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifRSPI.h 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#ifndef SifRSPIH
#define SifRSPIH

//---------------------------------------------------------------------------
#include "Kernel.h"
#include "RX63NReg.h"
#include "SIFLayer.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#pragma pack(push, 1)
struct TRSPIDataNode {
	int Size;
	uint8_t Data[1];
};
#pragma pack(pop)

typedef list<TRSPIDataNode *> CRSPIDataQueue;

enum TRSPISendTerm
{
	rspistIdle,
	rspistSending,
	rspistWait,
	rspistFlush,
};

enum TRSPIRecvTerm {
	rspirtIdle,
	rspirtReceiving,
	rspirtWait,
};

//------------------------------------------------------------------------------
// にせRSPI クラス
//------------------------------------------------------------------------------
class CSifRSPI : public ISysTimerSync, public ICommunicationPort
{
public:
	CSifRSPI(TRX63NRSPI0Reg *reg, int rxIntNo, int txIntNo, int idIntNo, int devNo);
	virtual ~CSifRSPI();
private:
	TRX63NRSPI0Reg *m_Reg;
	TRX63NRSPI0Reg m_RegInst;
	uint32_t m_SPRX;
	int m_RxIntNo;
	int m_TxIntNo;
	int m_IdIntNo;
	int m_DevNo;
	uint8_t m_SendBuf[64];
	int m_SendBufPos;
	CRITICAL_SECTION m_Lock;
	TRSPISendTerm m_SendTerm;
	LONG m_Send;
	__int64 m_SendTimer;
	CRSPIDataQueue m_RecvQueue;
	TRSPIDataNode *m_RecvBuf;
	int m_RecvBufPos;
	TRSPIRecvTerm m_RecvTerm;
	__int64 m_RecvTimer;
	__int64 CalcTimer();
	void Send(const void *data, int size);
protected:
	class Lock
	{
		CRITICAL_SECTION *m_Lock;
	public:
		Lock(CSifRSPI *_this) { m_Lock = &_this->m_Lock; EnterCriticalSection(m_Lock); }
		~Lock() { LeaveCriticalSection(m_Lock); }
	};
	void CallSendTimeOut();
	void CallRecvTimeOut();
public:
	uint8_t GetByte(intptr_t index);
	void SetByte(intptr_t index, uint8_t value);
	uint32_t GetInt32(intptr_t index);
	void SetInt32(intptr_t index, uint32_t value);
public:
	__int64 GetTimer();
	void Progress(__int64 timer);
	void CallTimeOut();
	int GetDevNo() { return m_DevNo; }
	void Receive(const void *data, int size);
	ICommunicationPort *Port;
};

//---------------------------------------------------------------------------
#endif // SifRSPIH
