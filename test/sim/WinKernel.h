//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: WinKernel.h 1970 2019-07-10 00:36:30Z coas-nagasima $
#ifndef KernelH
#define KernelH

//---------------------------------------------------------------------------
#include "SIFLayer.h"

class CCPUContext;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
typedef void CALLBACK TKernelEvent(void *obj);
typedef void CALLBACK TOutputEvent(void *obj, int kind, const void *data, int size);
typedef void CALLBACK TGetSystemTimeEvent(void *obj, __int64 *now);
typedef void CALLBACK TInterfaceEvent(void *obj, const TCHAR *type, const TCHAR *name, int kind);

struct TCallbackEvent
{
	int Func;
	int Kind;
	int Size;
	union{
		void *Data;
		unsigned char Buffer[4];
	};
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CKernel
{
public:
	CKernel(ISifCPU *m_SifCPU);
	virtual ~CKernel();
	void Exit();
private:
	HANDLE m_Thread;
	DWORD m_ThreadID;
	bool m_Terminate;
	HANDLE m_IntEvent;
	HANDLE m_SysSem;
	LONG m_SysSemCount;
	static DWORD m_TlsIndex;
	LONG m_Locked;
	bool m_TaskMode;
	bool m_IntMode;
	__int64 m_Frequency;
	CCPUContext *m_Current;
	void *m_ObjOfOnSetEvent;
	TKernelEvent *m_OnSetEvent;
	void *m_ObjOfOnStart;
	TKernelEvent *m_OnStart;
	void *m_ObjOfOnTerminate;
	TKernelEvent *m_OnTerminate;
	void *m_ObjOfOnIdle;
	TKernelEvent *m_OnIdle;
	void *m_ObjOfOnOutput;
	TOutputEvent *m_OnOutput;
	void *m_ObjOfOnGetSystemTime;
	TGetSystemTimeEvent *m_OnGetSystemTime;
	void *m_ObjOfOnAddInterface;
	TInterfaceEvent *m_OnAddInterface;
	void *m_ObjOfOnRemoveInterface;
	TInterfaceEvent *m_OnRemoveInterface;
	list<TSystemIFItem> m_SystemIFList;
	list<ISysTimerSync *> m_SysTimerSyncList;
	CString m_UnitName;
private:
	int m_Prio;
	int m_IntNo;
	int m_SetPrio;
	ISifCPU *m_SifCPU;
	unsigned char m_SendBuf[256];
	int m_SendBufPos;
	static unsigned long __stdcall ThreadProc(void *Param);
	void Execute();
	void MainLoop();
	void Idle();
	void ClearAllTask();
	void TraceLogPut(char c);
	void TraceLogFlush();
	static void TraceLogPutC(char c);
protected:
	virtual void OnStart();
	virtual void OnTerminate();
	virtual ID OnIdle();
public:
	CCPUContext *GetCurrent(){ return m_Current; }
	struct PFrequency{
		operator __int64() const {
			return ((CKernel *)((intptr_t)this - (intptr_t)&((CKernel *)0)->Frequency))->m_Frequency; 
		}
	}Frequency;
	void SwitchKernelMode(CCPUContext *CPUContext);
	bool CheckIntNoCfg(int IntNo);
	void Interrupt(int IntNo);
	void ProcInterrupt();
	bool InKernelMode(){ return m_ThreadID == GetCurrentThreadId(); }
	bool IsLocked() { return m_Locked > 0; }
	bool CurrentIsLocked(){ return (int)TlsGetValue(m_TlsIndex) > 0; }
	int GetPrio() { return m_Prio; }
	void SetPrio(int prio) { m_SetPrio = prio; }
	DWORD GetThreadID(){ return m_ThreadID; }
	virtual void Prepare() = 0;
	void Start(bool CreateSuspend);
	void Terminate();
	bool IsTerminated(){ return m_Terminate; }
	bool IsFinished(){
		return (m_ThreadID == NULL)
			|| (WaitForSingleObject(m_Thread, 0) == WAIT_OBJECT_0);
	}
	void ExitCPUContext(CCPUContext *CPUContext);
	void Dispatch();
	bool ExitAndDispatch();
	void LockCPU();
	void UnlockCPU();
	CCPUContext *StartDelaySuspend();
	void EndDelaySuspend(CCPUContext *CPUContext);
	bool InTaskContext();
	void AssignOnSetEvent(TKernelEvent *OnSetEvent, void *ObjOfOnSetEvent)
	{
		m_OnSetEvent = OnSetEvent;
		m_ObjOfOnSetEvent = ObjOfOnSetEvent;
	}
	void AssignOnStart(TKernelEvent *OnStart, void *ObjOfOnStart)
	{
		m_OnStart = OnStart;
		m_ObjOfOnStart = ObjOfOnStart;
	}
	void AssignOnTerminate(TKernelEvent *OnTerminate, void *ObjOfOnTerminate)
	{
		m_OnTerminate = OnTerminate;
		m_ObjOfOnTerminate = ObjOfOnTerminate;
	}
	void AssignOnIdle(TKernelEvent *OnIdle, void *ObjOfOnIdle)
	{
		m_OnIdle = OnIdle;
		m_ObjOfOnIdle = ObjOfOnIdle;
	}
	void AssignOnOutput(TOutputEvent *OnOutput, void *ObjOfOnOutput)
	{
		m_OnOutput = OnOutput;
		m_ObjOfOnOutput = ObjOfOnOutput;
	}
	void AssignOnGetSystemTime(TGetSystemTimeEvent *OnGetSystemTime, void *ObjOfOnGetSystemTime)
	{
		m_OnGetSystemTime = OnGetSystemTime;
		m_ObjOfOnGetSystemTime = ObjOfOnGetSystemTime;
	}
	void AssignOnAddInterface(TInterfaceEvent *OnAddInterface, void *ObjOfOnAddInterface)
	{
		m_OnAddInterface = OnAddInterface;
		m_ObjOfOnAddInterface = ObjOfOnAddInterface;
	}
	void AssignOnRemoveInterface(TInterfaceEvent *OnRemoveInterface, void *ObjOfOnRemoveInterface)
	{
		m_OnRemoveInterface = OnRemoveInterface;
		m_ObjOfOnRemoveInterface = ObjOfOnRemoveInterface;
	}
	// Driver⇔Simulator
	bool DefineSysIF(uintptr_t Addr, int Size, ISystemIF *SystemIF);
	void AddSysTimerSync(ISysTimerSync *SysTimerSync);
	unsigned char GetByte(uintptr_t Addr);
	void SetByte(uintptr_t Addr, unsigned char Value);
	unsigned short GetUInt16(uintptr_t Addr);
	void SetUInt16(uintptr_t Addr, unsigned short Value);
	unsigned int GetUInt32(uintptr_t Addr);
	void SetUInt32(uintptr_t Addr, unsigned int Value);
	void Output(int Kind, const void *Data, int Size);
	virtual void Input(int Kind, const void *Data, int Size) = 0;
	__int64 GetTimer();
	void Progress(__int64 Timer);
	void CallTimeOut();
	//
	const TCHAR *GetUnitName(){ return m_UnitName.c_str(); }
	void SetUnitName(const TCHAR *value){ m_UnitName = value; }
	int GetFullPathName(TCHAR *FilePath, int Size, const TCHAR *Extension);
	int ReadFile(TCHAR *Ext, int Pos, void *Data, int Size);
	int WriteFile(TCHAR *Ext, int Pos, void *Data, int Size);
	int ReadAddr(uintptr_t Addr, void *buffer, int count);
	int WriteAddr(uintptr_t Addr, const void *buffer, int count);
	void SetBit(unsigned int bit, unsigned int addr);
	void ClearBit(unsigned int bit, unsigned int addr);
	unsigned int TestBit(unsigned int bit, unsigned int addr);
	// 
	static void SetTaskName(ID tskid, const TCHAR *szName);
	static void SetThreadName(DWORD dwThreadID, const TCHAR *szName);
	//
	void OutputLastErrorMessage();
private:
	HANDLE m_CallbackSem;
	list<TCallbackEvent> m_EventQueue;
public:
	static void Log(const TCHAR *Text);
	void OnSetEvent();
	bool ProcessEvent();
	void GetSystemTime(__int64 *Now);
	void AddInterface(const TCHAR *type, const TCHAR *name, int kind);
	void RemoveInterface(const TCHAR *type, const TCHAR *name, int kind);
public:
	bool InterruptUnmasked(int IntNo) { return m_SifCPU->InterruptUnmasked(IntNo); }
	bool InterruptRaised(int IntNo) { return m_SifCPU->InterruptRaised(IntNo) && KernelFlag(); }
	void ClearInterrupt(int IntNo) { m_SifCPU->ClearInterrupt(IntNo); }
	void SetInterrupt(int IntNo) { m_SifCPU->SetInterrupt(IntNo); }
	unsigned int InterruptPriority(int IntNo) { return m_SifCPU->InterruptPriority(IntNo); }
	bool DefineInterruptHandler(int IntNo, void (*Handler)()) { return m_SifCPU->DefineInterruptHandler(IntNo, Handler); }
	bool ChangeInterruptControlRegister(int IntNo, uint8_t icrcmd) { return m_SifCPU->ChangeInterruptControlRegister(IntNo, icrcmd); }
	void TryProcessInterrupt(int IntNo);
	bool KernelFlag();
};

#ifdef __cplusplus
extern "C" {
#endif
void *Kernel_ReadAddr(void *dest, const void *src, int n);
void *Kernel_WriteAddr(void *dest, const void *src, int n);
void Kernel_Log(const TCHAR *Text);
#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
#endif // KernelH
