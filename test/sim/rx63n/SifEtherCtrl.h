//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifEtherCtrl.h 1970 2019-07-10 00:36:30Z coas-nagasima $
#ifndef SifEtherCtrlH
#define SifEtherCtrlH

//---------------------------------------------------------------------------
#include "WinKernel.h"
#include "RX63NReg.h"
#include "SIFLayer.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#pragma pack(push, 1)
struct TEthernetData
{
	unsigned char DstAddr[6];
	unsigned char SrcAddr[6];
	unsigned short Length;
	unsigned char Data[46];
	//unsigned char FrameChkSeq[4];
};
#pragma pack(pop)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#pragma pack(push, 1)
struct TEtherCtrlDataNode
{
	int Size;
	union{
		TEthernetData EthernetData;
		unsigned char Data[sizeof(TEthernetData)];
	};
};
#pragma pack(pop)

typedef list<TEtherCtrlDataNode *> CEtherCtrlDataQueue;

enum TEtherCtrlSendTerm
{
	ecstIdle,
	ecstSend,
	ecstTxInt,
};

enum TEtherCtrlRecvTerm
{
	ecrtIdle,
	ecrtRecv,
	ecrtRxInt,
};

//------------------------------------------------------------------------------
// にせEtherCtrl クラス
//------------------------------------------------------------------------------
class CSifEtherCtrl : public ISystemIF, public ISysTimerSync
{
public:
	CSifEtherCtrl(TRX63NEDMACReg *edmacReg, TRX63NETHERCReg *ethercReg,
		int intNo, int devNo);
	virtual ~CSifEtherCtrl();
private:
	TRX63NEDMACReg *m_EdmacReg;
	TRX63NEDMACReg m_EdmacRegInst;
	TRX63NETHERCReg *m_EthercReg;
	TRX63NETHERCReg m_EthercRegInst;
	int m_IntNo;
	int m_DevNo;
	CRITICAL_SECTION m_Lock;
	bool m_SendReq;
	TRX63NEtherCTxDescriper *m_TxDsp;
	unsigned char m_SendBuf[2048];
	int m_SendBufPos;
	TRX63NEtherCRxDescriper *m_RxDsp;
	CEtherCtrlDataQueue m_RecvQueue;
	TEtherCtrlDataNode *m_RecvBuf;
	int m_RecvBufPos;
	TEtherCtrlSendTerm m_SendTerm;
	TEtherCtrlRecvTerm m_RecvTerm;
	__int64 m_SendTimer;
	__int64 m_RecvTimer;

	bool m_MacReset;
	bool m_FifoReset;
	bool m_SoftReset;
	unsigned short m_PhyReg[32];
	unsigned char m_MacAddr[6];
protected:
	class Lock{
		CRITICAL_SECTION *m_Lock;
	public:
		Lock(CSifEtherCtrl *_this) { m_Lock = &_this->m_Lock; EnterCriticalSection(m_Lock); }
		~Lock() { LeaveCriticalSection(m_Lock); }
	};
	void CallSendTimeOut();
	void CallRecvTimeOut();
public:	// ISystemIF
	virtual unsigned char GetByte(unsigned int Addr);
	virtual void SetByte(unsigned int Addr, unsigned char Value);
	virtual unsigned short GetUInt16(unsigned int Addr);
	virtual void SetUInt16(unsigned int Addr, unsigned short Value);
	virtual unsigned int GetUInt32(unsigned int Addr);
	virtual void SetUInt32(unsigned int Addr, unsigned int Value);
public:	// ISysTimerSync
	__int64 GetTimer();
	void Progress(__int64 Timer);
	void CallTimeOut();
	void Receive(const void *Data, int Size);
};

//---------------------------------------------------------------------------
#endif // SifEtherCtrlH
