//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifUSB.h 1970 2019-07-10 00:36:30Z coas-nagasima $
#ifndef SifUSBH
#define SifUSBH

//---------------------------------------------------------------------------
#include "Kernel.h"
#include "RX63NReg.h"
#include "SIFLayer.h"
#include "UsbBluetooth.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define USB_SYNC	0x80
#define PID_OUT		0x1
#define PID_IN		0x9
#define PID_SOF		0x5
#define PID_SETUP	0xD
#define PID_DATA0	0x3
#define PID_DATA1	0xB
#define PID_ACK		0x2
#define PID_NAK		0xA
#define PID_STALL	0xE
#define PID_PRE		0xC

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CSifUSB;

class CUsbRingBuf
{
public:
	CUsbRingBuf();
private:
	uint8_t m_Data[2048];
	int m_WritePos;
	int m_ReadPos;
public:
	void PutByte(uint8_t data);
	uint8_t GetByte();
	int GetSize();
	void Clear();
};

class CUsbPipe
{
	friend class CUsbFifo;
public:
	CUsbPipe();
	virtual ~CUsbPipe();
	void Init(CSifUSB *owner, int pipeNo);
private:
	CSifUSB *m_Owner;
	CUsbFifo *m_Fifo;
	int m_PipeNo;
	CUsbRingBuf m_SendBuf;
	CUsbRingBuf m_RecvBuf;
	bool m_Data1;
	bool m_BVAL;
	bool m_SendReq;
	TUSBPIPECFG m_PIPECFG;
	TUSBPIPEMAXP m_PIPEMAXP;
	TUSBPIPEPERI m_PIPEPERI;
	TUSBPIPECTR m_PIPECTR;
	TUSBPIPETRE m_PIPETRE;
	TUSBPIPETRN m_PIPETRN;
public:
	void SetPIPECFG(TUSBPIPECFG value);
	TUSBPIPECFG GetPIPECFG() { return m_PIPECFG; }
	void SetPIPEMAXP(TUSBPIPEMAXP value);
	TUSBPIPEMAXP GetPIPEMAXP() { return m_PIPEMAXP; }
	void SetPIPEPERI(TUSBPIPEPERI value);
	TUSBPIPEPERI GetPIPEPERI() { return m_PIPEPERI; }
	void SetPIPECTR(TUSBPIPECTR value);
	TUSBPIPECTR GetPIPECTR() { return m_PIPECTR; }
	void SetPIPETRE(TUSBPIPETRE value);
	TUSBPIPETRE GetPIPETRE() { return m_PIPETRE; }
	void SetPIPETRN(TUSBPIPETRN value);
	TUSBPIPETRN GetPIPETRN() { return m_PIPETRN; }
	bool IsAssignd();
	int GetPipeNo() { return m_PipeNo; }
	void PutByte(uint8_t data);
	void PutShort(uint16_t data);
	uint8_t GetByte() { return m_RecvBuf.GetByte(); }
	uint16_t GetShort();
	void RecvByte(uint8_t data) { m_RecvBuf.PutByte(data); }
	uint8_t SendByte() { return m_SendBuf.GetByte(); }
	int GetSendSize() { return m_SendBuf.GetSize(); }
	int GetRecvSize() { return m_RecvBuf.GetSize(); }
	bool GetData1() { return m_Data1; }
	void SetData1(bool value) { m_Data1 = value; }
	void Receive(const void *data, int size);
	void SendBytes(void *data, int size);
	int GetRequest(bool idle);
	bool GetBVAL() { return m_BVAL; }
	void SetBVAL(bool value);
	void SetBEMP(bool value);
	void SetBRDY(bool value);
	void SetNRDY(bool value);
	void ClearPID();
	bool GetDir() { return m_PIPECFG.DIR != 0; }
	bool GetSUREQ() { if(m_PipeNo != 0) std::exception(); return ((TUSBDCPCTR *)&m_PIPECTR)->SUREQ != 0; }
	void ClearSUREQ() { if(m_PipeNo != 0) std::exception(); ((TUSBDCPCTR *)&m_PIPECTR)->SUREQ = 0; }
	void SetSendReq(bool value) { m_SendReq = value; }
};

class CUsbFifo
{
	friend class CUsbPipe;
public:
	CUsbFifo(CSifUSB *owner);
	virtual ~CUsbFifo();
private:
	CSifUSB *m_Owner;
	CUsbPipe *m_Pipe;
	TUSBFIFOSEL m_FIFOSEL;
	TUSBFIFOCTR m_FIFOCTR;
public:
	void SetFIFOSEL(TUSBFIFOSEL value);
	TUSBFIFOSEL GetFIFOSEL() { return m_FIFOSEL; }
	void SetFIFOCTR(TUSBFIFOCTR value);
	TUSBFIFOCTR GetFIFOCTR();
	void PutByte(uint8_t data) { m_Pipe->PutByte(data); }
	void PutShort(uint16_t data) { m_Pipe->PutShort(data); }
	uint8_t GetByte() { return m_Pipe->GetByte(); }
	uint16_t GetShort() { return m_Pipe->GetShort(); }
	int GetPipeNo() { if(m_Pipe == NULL) return -1; else return m_Pipe->GetPipeNo(); }
};

enum TUSB0Term
{
	usbtNodevice,
	usbtAttatch,
	usbtDettatch,
	usbtIdle,
	usbtStartSetupStage,
	usbtSetupStageToken,
	usbtSetupStageData,
	usbtSetupStageHandshake,
	usbtStartDataStage,
	usbtDataStageTokenIn,
	usbtDataStageTokenOut,
	usbtDataStageDataIn,
	usbtDataStageDataOut,
	usbtDataStageDataNak,
	usbtDataStageHandshakeIn,
	usbtDataStageHandshakeOut,
	usbtStartStatusStage,
	usbtStatusStageTokenIn,
	usbtStatusStageTokenOut,
	usbtStatusStageDataIn,
	usbtStatusStageDataOut,
	usbtStatusStageHandshakeIn = usbtIdle,
	usbtStatusStageHandshakeOut = usbtStatusStageDataOut + 1,
	usbtStartData,
	usbtDataTokenIn,
	usbtDataTokenOut,
	usbtDataDataIn,
	usbtDataDataOut,
	usbtDataDataNak,
	usbtDataHandshakeIn = usbtIdle,
	usbtDataHandshakeOut = usbtDataDataNak + 1,
};

//------------------------------------------------------------------------------
// にせUSB0 クラス
//------------------------------------------------------------------------------
class CSifUSB : public ISystemIF, public ISysTimerSync
{
	friend class CUsbPipe;
	friend class CUsbFifo;
public:
	CSifUSB(TRX63NUSB0Reg *usbReg, int d0FifoIntNo, int d1FifoIntNo,
		int usbioIntNo, int devNo);
	virtual ~CSifUSB();
private:
	enum TRunRequest{
		RUN_NONE,
		RUN_SUREQ,
		RUN_BVAL,
		RUN_PID,
	};
	TRX63NUSB0Reg *m_UsbReg;
	TRX63NUSB0Reg m_UsbRegInst;
	int m_D0FifoIntNo;
	int m_D1FifoIntNo;
	int m_UsbioIntNo;
	int m_DevNo;
	CRITICAL_SECTION m_Lock;
	int m_Stage;
	TUSB0Term m_Term;
#ifdef _DEBUG
	TUSB0Term m_PrevTerm;
#endif
	__int64 m_Timer;
	int m_CurrentPipe;
	uint16_t m_Intsts0;
	uint16_t m_Intsts1;
	bool m_Interrupting;
	CUsbPipe m_UsbPipe[10];
	CUsbFifo m_CFifo;
	CUsbFifo m_D0Fifo;
	CUsbFifo m_D1Fifo;
	CUSBBluetooth m_Bluetooth;
	void TimeOut();
	void Attatch();
	void SendSetup(CUsbPipe *pipe);
	void SendDataIn(CUsbPipe *pipe);
	void SendDataOut(CUsbPipe *pipe);
	void SendSetupData(CUsbPipe *pipe);
	void SendData(CUsbPipe *pipe);
	void SendStatusData(CUsbPipe *pipe);
	void SendHandshake(CUsbPipe *pipe);
	void Run(int pipeNo, TRunRequest req);
	void NextPipe();
	void Send(const void *data, int size);
public:
	class Lock{
		CRITICAL_SECTION *m_Lock;
	public:
		Lock(CSifUSB *_this) { m_Lock = &_this->m_Lock; EnterCriticalSection(m_Lock); }
		~Lock() { LeaveCriticalSection(m_Lock); }
	};
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
public:
	void Receive(const void *data, int size);
};

//---------------------------------------------------------------------------
#endif // SifUSBH
