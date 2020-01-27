//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SIFLayer.h 1970 2019-07-10 00:36:30Z coas-nagasima $
#ifndef SIFLayerH
#define SIFLayerH

//---------------------------------------------------------------------------
extern "C" {
#include <kernel.h>
#include <sil.h>
}

//------------------------------------------------------------------------------
// Driver⇔Simulator
//------------------------------------------------------------------------------
class ISystemIF
{
public:
	virtual unsigned char GetByte(uintptr_t Addr) = 0;
	virtual void SetByte(uintptr_t Addr, unsigned char Value) = 0;
	virtual unsigned short GetUInt16(uintptr_t Addr) = 0;
	virtual void SetUInt16(uintptr_t Addr, unsigned short Value) = 0;
	virtual unsigned int GetUInt32(uintptr_t Addr) = 0;
	virtual void SetUInt32(uintptr_t Addr, unsigned int Value) = 0;
};

//-------------------------------------------------------------------------
// TSystemIFItem
//-------------------------------------------------------------------------
struct TSystemIFItem
{
	uintptr_t Addr;
	int Size;
	ISystemIF *SystemIF;
};

//------------------------------------------------------------------------------
// Driver⇔Simulator
//------------------------------------------------------------------------------
class CSystemIF : public ISystemIF
{
public:
	CSystemIF(intptr_t BaseAddr, int Size);
	virtual ~CSystemIF();
private:
	intptr_t m_BaseAddr;
	int m_Size;
protected:
	virtual bool HasReadEvent(intptr_t Index) = 0;
	virtual void GetByte2(intptr_t Index, unsigned char &Value) = 0;
	virtual bool SetByte2(intptr_t Index, unsigned char Value) = 0;
	virtual void ReadEvent(intptr_t Index) = 0;
	virtual void WriteEvent(intptr_t Index) = 0;
public:
	unsigned char GetByte(uintptr_t Addr);
	void SetByte(uintptr_t Addr, unsigned char Value);
	unsigned short GetUInt16(uintptr_t Addr);
	void SetUInt16(uintptr_t Addr, unsigned short Value);
	unsigned int GetUInt32(uintptr_t Addr);
	void SetUInt32(uintptr_t Addr, unsigned int Value);
};

//------------------------------------------------------------------------------
// Driver⇔Simulator
//------------------------------------------------------------------------------
class ISysTimerSync
{
public:
	virtual __int64 GetTimer() = 0;
	virtual void Progress(__int64 Timer) = 0;
	virtual void CallTimeOut() = 0;
};

//------------------------------------------------------------------------------
// CPU
//------------------------------------------------------------------------------
class ISifCPU
{
public:
	virtual int GetInterruptCount() = 0;
	virtual bool InterruptUnmasked(int IntNo) = 0;
	virtual bool InterruptRaised(int IntNo) = 0;
	virtual void ClearInterrupt(int IntNo) = 0;
	virtual void SetInterrupt(int IntNo) = 0;
	virtual unsigned int InterruptPriority(int IntNo) = 0;
	virtual bool DefineInterruptHandler(int IntNo, void (*Handler)()) = 0;
	virtual bool ChangeInterruptControlRegister(int IntNo, uint8_t icrcmd) = 0;
	virtual void InvokeInterrupt(int IntNo) = 0;
};

//------------------------------------------------------------------------------
// Communication port
//------------------------------------------------------------------------------
class ICommunicationPort
{
public:
	virtual int GetDevNo() = 0;
	virtual void Receive(const void *data, int size) = 0;
};

//---------------------------------------------------------------------------
#endif // SIFLayerH
