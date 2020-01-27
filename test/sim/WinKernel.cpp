//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: WinKernel.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
#include "StdAfx.h"
#include "WinKernel.h"
extern "C" {
#include "overrun.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CKernel *g_Kernel;

#define SUSPEND_FLAG_KERNEL_REQUEST 1
#define SUSPEND_FLAG_POP_CONTEXT 2
#define SUSPEND_FLAG_SWITCH_TO_KERNEL 4
#define SUSPEND_FLAG_DELAY_SUSPEND 8
#define SUSPEND_FLAG_END_DELAY 16
#define SUSPEND_FLAG_SUSPENDING 32

#define MS_VC_EXCEPTION 0x406D1388

typedef struct tagTHREADNAME_INFO {
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try {
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(DWORD), (DWORD*)&info);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

class CCPUContext {
public:
	CCPUContext(CKernel *pKernel);
	virtual ~CCPUContext();
private:
	HANDLE m_Thread;
	DWORD m_ThreadID;
	bool m_Terminate;
	bool m_Release;
	CKernel *m_Kernel;
	TCB *m_Task;
	LONG m_SuspendFlag;
	LONG m_DelaySuspend;
	bool m_Suspend;
	CString m_Name;
	static DWORD m_TlsIndex;
#if TASK_SWITCH_DEBUG
	static __int64 m_Frequency;
	struct TSuspendLog {
		double Time;
		DWORD ThreadID;
		enum TState {
			None,
			BeforePushContext,
			AfterPushContext,
			BeforePopContext,
			AfterPopContext,
			BeforeSuspend,
			AfterSuspend,
			BeforeStartDelaySuspend,
			AfterStartDelaySuspend,
			BeforeEndDelaySuspend,
			AfterEndDelaySuspend,
			Timeout,
		}State;
		int Count;
		DWORD Ret;
	};
	static TSuspendLog m_SuspendLog[4092];
	static LONG m_SuspendLogIndex;
	static void Push(TSuspendLog &log)
	{
		LONG Index = InterlockedIncrement(&m_SuspendLogIndex) % 4092;
		m_SuspendLog[Index] = log;
	}
#endif
	static unsigned long __stdcall ThreadProc(void *Param);
public:
	bool IsTerminated();
	bool IsReleased() { return m_Release; }
	bool IsFinished()
	{
		return WaitForSingleObject(m_Thread, 0) == WAIT_OBJECT_0;
	}
	bool IsCurrent() { return m_ThreadID == GetCurrentThreadId(); }
	void Activate(TCB *Task);
	void Dispatch();
	bool PushContext();
	bool PopContext();
	bool Suspend();
	void Terminate() { m_Terminate = true; ResumeThread(m_Thread); }
	void Wait() { ::WaitForSingleObject(m_Thread, INFINITE); }
	void StartDelaySuspend();
	void EndDelaySuspend();
	void Release();
	void SetThreadName(const TCHAR *szThreadName);
	void Timeout()
	{
#if TASK_SWITCH_DEBUG
		__int64 Time;
		TSuspendLog Log;
		QueryPerformanceCounter((LARGE_INTEGER *)&Time);
		Log.Time = (double)Time / (double)m_Frequency;
		Log.ThreadID = m_ThreadID;
		Log.State = TSuspendLog::Timeout;
		Log.Count = m_SuspendFlag;
		Log.Ret = -1;
		Push(Log);
#endif
	}
	static CCPUContext *GetCurrent();
};

inline bool CCPUContext::IsTerminated() { return m_Terminate || m_Kernel->IsTerminated(); }

DWORD CCPUContext::m_TlsIndex = 0xFFFFFFFF;
#if TASK_SWITCH_DEBUG
__int64 CCPUContext::m_Frequency;
CCPUContext::TSuspendLog CCPUContext::m_SuspendLog[4092];
LONG CCPUContext::m_SuspendLogIndex;
#endif

CCPUContext::CCPUContext(CKernel *pKernel)
{
	m_Kernel = pKernel;
	m_Thread = NULL;
	m_ThreadID = 0;
	m_Terminate = false;
	m_Release = false;
	m_Task = NULL;
	m_DelaySuspend = 0;
	m_SuspendFlag = 0;

	if (m_TlsIndex == 0xFFFFFFFF) {
		m_TlsIndex = TlsAlloc();
	}

#if TASK_SWITCH_DEBUG
	m_SuspendLogIndex = 0;
	memset(m_SuspendLog, 0, sizeof(m_SuspendLog));
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_Frequency);
#endif
}

CCPUContext::~CCPUContext()
{
	if (m_Task != NULL) {
		m_Task->tskctxb.cpu_context = NULL;
	}
	if (m_Thread != NULL) {
		CloseHandle(m_Thread);
	}
}

unsigned long __stdcall CCPUContext::ThreadProc(void *Param)
{
	CCPUContext *This = (CCPUContext *)Param;

	TlsSetValue(m_TlsIndex, This);

	if (!This->m_Terminate) {
		::SetThreadName(GetCurrentThreadId(), This->m_Task->p_tinib->name);
		try {
			This->m_Task->tskctxb.start();
			This->Suspend();
			This->m_Task->p_tinib->task(This->m_Task->p_tinib->exinf);
		}
		catch (...) {
		}
	}

	ext_tsk();

	return 0;
}

void CCPUContext::Activate(TCB *Task)
{
	if (m_ThreadID != 0)
		throw std::exception();

	m_Task = Task;
	m_SuspendFlag = SUSPEND_FLAG_KERNEL_REQUEST;

	m_Thread = CreateThread(NULL, 0, &ThreadProc, (void *)this, CREATE_SUSPENDED, &m_ThreadID);
	if (m_Thread == NULL) {
		m_Kernel->OutputLastErrorMessage();
		throw std::exception();
	}

	DWORD ret = ResumeThread(m_Thread);
	if (ret == -1)
		m_Kernel->OutputLastErrorMessage();
	if (ret != 1)
		throw std::exception();

	while ((m_SuspendFlag & SUSPEND_FLAG_SWITCH_TO_KERNEL) == 0)
		SwitchToThread();
}

void CCPUContext::Dispatch()
{
	ASSERT(IsCurrent());

	if (IsTerminated()) {
		if (m_Task->p_winfo != NULL) {
			m_Task->p_winfo->wercd = E_RLWAI;
		}
		return;
	}

	m_Kernel->SwitchKernelMode(this);
}

bool CCPUContext::PushContext()
{
	ASSERT(m_Kernel->InKernelMode());

#if TASK_SWITCH_DEBUG
	__int64 Time;
	TSuspendLog Log;
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::BeforePushContext;
	Log.Count = m_SuspendFlag;
	Log.Ret = -1;
	Push(Log);
#endif

	LONG Count = InterlockedExchangeAdd(&m_SuspendFlag, SUSPEND_FLAG_KERNEL_REQUEST);

	ASSERT((Count & SUSPEND_FLAG_KERNEL_REQUEST) == 0);

	DWORD ret = -1;
	if (Count == SUSPEND_FLAG_DELAY_SUSPEND) {
		for (;;) {
			switch (m_SuspendFlag & (SUSPEND_FLAG_DELAY_SUSPEND | SUSPEND_FLAG_END_DELAY | SUSPEND_FLAG_SUSPENDING)) {
			case 0:
				ret = SuspendThread(m_Thread);
				if (ret == -1)
					m_Kernel->OutputLastErrorMessage();
				if (ret != 0)
					throw std::exception();
				break;
			case SUSPEND_FLAG_END_DELAY:
				continue;
			case SUSPEND_FLAG_SUSPENDING:
			default:
				break;
			}
			break;
		}
	}

#if TASK_SWITCH_DEBUG
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::AfterPushContext;
	Log.Count = m_SuspendFlag;
	Log.Ret = ret;
	Push(Log);
#endif

	return true;
}

bool CCPUContext::PopContext()
{
	ASSERT(m_Kernel->InKernelMode());

#if TASK_SWITCH_DEBUG
	__int64 Time;
	TSuspendLog Log;
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::BeforePopContext;
	Log.Count = m_SuspendFlag;
	Log.Ret = -1;
	Push(Log);
#endif

	LONG Count = InterlockedExchangeAdd(&m_SuspendFlag, -SUSPEND_FLAG_KERNEL_REQUEST + SUSPEND_FLAG_POP_CONTEXT);

	ASSERT((Count & (SUSPEND_FLAG_KERNEL_REQUEST | SUSPEND_FLAG_POP_CONTEXT)) == SUSPEND_FLAG_KERNEL_REQUEST);

	if ((Count & SUSPEND_FLAG_SWITCH_TO_KERNEL) != 0)
		m_Suspend = false;

	DWORD ret = -1;
	if ((m_SuspendFlag & (SUSPEND_FLAG_DELAY_SUSPEND | SUSPEND_FLAG_SWITCH_TO_KERNEL)) != SUSPEND_FLAG_DELAY_SUSPEND) {
		ret = ResumeThread(m_Thread);
		if (ret == -1)
			m_Kernel->OutputLastErrorMessage();
		//if ((ret != 0) && (ret != 1))
		//	throw std::exception();
	}

	Count = InterlockedExchangeAdd(&m_SuspendFlag, -SUSPEND_FLAG_POP_CONTEXT);

	ASSERT((Count & SUSPEND_FLAG_POP_CONTEXT) != 0);

#if TASK_SWITCH_DEBUG
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::AfterPopContext;
	Log.Count = m_SuspendFlag;
	Log.Ret = ret;
	Push(Log);
#endif

	return true;
}

bool CCPUContext::Suspend()
{
	ASSERT(IsCurrent());

#if TASK_SWITCH_DEBUG
	__int64 Time;
	TSuspendLog Log;
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::BeforeSuspend;
	Log.Count = m_SuspendFlag;
	Log.Ret = -1;
	Push(Log);
#endif

	m_Suspend = true;

	LONG Count = InterlockedExchangeAdd(&m_SuspendFlag, SUSPEND_FLAG_SWITCH_TO_KERNEL);

	ASSERT((Count & SUSPEND_FLAG_SWITCH_TO_KERNEL) == 0x0);

	DWORD ret = -1;
	while (m_Suspend && !IsTerminated()) {
		ret = SuspendThread(m_Thread);
		if (ret == -1)
			m_Kernel->OutputLastErrorMessage();
		if ((ret != 0) && (ret != 1))
			throw std::exception();
	}
	if (IsTerminated() && m_Task->p_winfo != NULL) {
		m_Task->p_winfo->wercd = E_RLWAI;
	}

	Count = InterlockedExchangeAdd(&m_SuspendFlag, -SUSPEND_FLAG_SWITCH_TO_KERNEL);

	ASSERT((Count & SUSPEND_FLAG_SWITCH_TO_KERNEL) != 0x0);

#if TASK_SWITCH_DEBUG
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::AfterSuspend;
	Log.Count = m_SuspendFlag;
	Log.Ret = ret;
	Push(Log);
#endif

	return true;
}

void CCPUContext::StartDelaySuspend()
{
	ASSERT(IsCurrent());

#if TASK_SWITCH_DEBUG
	__int64 Time;
	TSuspendLog Log;
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::BeforeStartDelaySuspend;
	Log.Count = m_SuspendFlag;
	Log.Ret = -1;
	Push(Log);
#endif

	m_DelaySuspend++;

	if (m_DelaySuspend == 1) {
		LONG Count = InterlockedExchangeAdd(&m_SuspendFlag, SUSPEND_FLAG_DELAY_SUSPEND);

		ASSERT((Count & SUSPEND_FLAG_DELAY_SUSPEND) == 0);
	}

#if TASK_SWITCH_DEBUG
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::AfterStartDelaySuspend;
	Log.Count = m_SuspendFlag;
	Log.Ret = -1;
	Push(Log);
#endif
}

void CCPUContext::EndDelaySuspend()
{
	ASSERT(IsCurrent());
	ASSERT(m_DelaySuspend > 0);

#if TASK_SWITCH_DEBUG
	__int64 Time;
	TSuspendLog Log;
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::BeforeEndDelaySuspend;
	Log.Count = m_SuspendFlag;
	Log.Ret = -1;
	Push(Log);
#endif

	m_DelaySuspend--;

	DWORD ret = -1;
	if (m_DelaySuspend == 0) {
		LONG Count = InterlockedExchangeAdd(&m_SuspendFlag, -SUSPEND_FLAG_DELAY_SUSPEND + SUSPEND_FLAG_END_DELAY);

		ASSERT((Count & SUSPEND_FLAG_DELAY_SUSPEND) != 0);

		if (m_SuspendFlag == (SUSPEND_FLAG_KERNEL_REQUEST | SUSPEND_FLAG_END_DELAY)) {
			Count = InterlockedExchangeAdd(&m_SuspendFlag, -SUSPEND_FLAG_END_DELAY + SUSPEND_FLAG_SUSPENDING);

			ASSERT((Count & (SUSPEND_FLAG_END_DELAY | SUSPEND_FLAG_SUSPENDING)) == SUSPEND_FLAG_END_DELAY);

			ret = SuspendThread(m_Thread);
			if (ret == -1)
				m_Kernel->OutputLastErrorMessage();
			if ((ret != 0) && (ret != 1))
				throw std::exception();

			Count = InterlockedExchangeAdd(&m_SuspendFlag, -SUSPEND_FLAG_SUSPENDING);

			ASSERT((Count & SUSPEND_FLAG_SUSPENDING) != 0);
		}
		else {
			Count = InterlockedExchangeAdd(&m_SuspendFlag, -SUSPEND_FLAG_END_DELAY);

			ASSERT((Count & SUSPEND_FLAG_END_DELAY) != 0);
		}
	}

#if TASK_SWITCH_DEBUG
	QueryPerformanceCounter((LARGE_INTEGER *)&Time);
	Log.Time = (double)Time / (double)m_Frequency;
	Log.ThreadID = m_ThreadID;
	Log.State = TSuspendLog::AfterEndDelaySuspend;
	Log.Count = m_SuspendFlag;
	Log.Ret = ret;
	Push(Log);
#endif
}

void CCPUContext::Release()
{
	m_Release = true;
}

void CCPUContext::SetThreadName(const TCHAR *szThreadName)
{
	m_Name = szThreadName;
	size_t Size = _tcslen(szThreadName);
	char *Temp = new char[Size + 1];
	try {
		wcstombs_s(&Size, Temp, Size + 1, m_Name.c_str(), Size);

		::SetThreadName(m_ThreadID, Temp);
	}
	catch (...) {
	}
	delete Temp;
}

CCPUContext *CCPUContext::GetCurrent()
{
	return (CCPUContext *)TlsGetValue(m_TlsIndex);
}

extern "C" void *new_context(void *p_tcb)
{
	CCPUContext *CPUContext = new CCPUContext(g_Kernel);
	CPUContext->Activate((TCB *)p_tcb);
	return CPUContext;
}

DWORD CKernel::m_TlsIndex = 0xFFFFFFFF;

CKernel::CKernel(ISifCPU *sifCPU)
{
	m_Frequency = FREQ_ICLK * 1000/*kHz→Hz*/;
	m_SifCPU = sifCPU;
	m_Current = NULL;
	m_ObjOfOnSetEvent = NULL;
	m_OnSetEvent = NULL;
	m_ObjOfOnStart = NULL;
	m_OnStart = NULL;
	m_ObjOfOnTerminate = NULL;
	m_OnTerminate = NULL;
	m_ObjOfOnIdle = NULL;
	m_OnIdle = NULL;
	m_ObjOfOnOutput = NULL;
	m_OnOutput = NULL;
	m_ObjOfOnGetSystemTime = NULL;
	m_OnGetSystemTime = NULL;
	m_OnAddInterface = NULL;
	m_ObjOfOnAddInterface = NULL;
	m_OnRemoveInterface = NULL;
	m_ObjOfOnRemoveInterface = NULL;

	m_Thread = NULL;
	m_ThreadID = 0;
	m_Terminate = false;
	m_IntEvent = NULL;
	m_Locked = 0;
	m_IntMode = false;
	m_TaskMode = false;
	if (m_TlsIndex == 0xFFFFFFFF) {
		m_TlsIndex = TlsAlloc();
	}
	m_SysSem = ::CreateSemaphore(NULL, 1, 1, NULL);
	m_SysSemCount = 0;
	m_CallbackSem = ::CreateSemaphore(NULL, 1, 1, NULL);
	m_Prio = 0;
	m_SetPrio = 0;

	//memset(m_SendBuf, 0, sizeof(m_SendBuf));
	m_SendBufPos = 0;
}

CKernel::~CKernel()
{
	Exit();

	if (m_IntEvent != NULL) {
		CloseHandle(m_IntEvent);
		CloseHandle(m_Thread);
	}

	CloseHandle(m_SysSem);
	CloseHandle(m_CallbackSem);

	TlsFree(m_TlsIndex);

	m_SystemIFList.clear();

	TCallbackEvent callback;

	while (m_EventQueue.size() > 0) {
		callback = m_EventQueue.front();
		m_EventQueue.pop_front();

		if (callback.Data != NULL) {
			delete callback.Data;
		}
	}
}

void CKernel::Exit()
{
	m_Terminate = true;

	if (m_IntEvent != NULL) {
		while (!IsFinished()) {
			SetEvent(m_IntEvent);
			SwitchToThread();
		}
	}
}

void CKernel::Start(bool CreateSuspend)
{
	if (m_ThreadID != 0)
		throw std::exception();

	m_Thread = CreateThread(NULL, 0, &ThreadProc, (void *)this, CREATE_SUSPENDED, &m_ThreadID);
	if (m_Thread == NULL)
		throw std::exception();

	m_IntEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_IntEvent == NULL) {
		CloseHandle(m_Thread);
		throw std::exception();
	}

	SetThreadName(m_ThreadID, _T("Kernel"));

	if (!CreateSuspend) {
		DWORD ret = ResumeThread(m_Thread);
		if (ret == -1)
			OutputLastErrorMessage();
		if (ret != 1)
			throw std::exception();
	}
}

void CKernel::Terminate()
{
	if (m_Terminate && m_ThreadID == 0) {
		OnTerminate();
	}
	else {
		m_Terminate = true;
		SetEvent(m_IntEvent);
	}
	if ((m_Current != NULL) && m_Current->IsCurrent()) {
		ExitCPUContext(m_Current);
	}
}

unsigned long __stdcall CKernel::ThreadProc(void *Param)
{
	CKernel *This = (CKernel *)Param;

	if (!This->m_Terminate) {
		try {
			This->Execute();
		}
		catch (...) {
		}
	}

	This->m_ThreadID = 0;

	return 0;
}

void CKernel::OnSetEvent()
{
	if (m_OnSetEvent != NULL)
		m_OnSetEvent(m_ObjOfOnSetEvent);
}

void CKernel::OnStart()
{
	TCallbackEvent callback;
	DWORD ret;

	memset(&callback, 0, sizeof(callback));

	callback.Func = 0;

	if ((ret = WaitForSingleObject(m_CallbackSem, INFINITE)) != WAIT_OBJECT_0)
		throw std::exception();

	__try {
		m_EventQueue.push_back(callback);
	}
	__finally {
		ReleaseSemaphore(m_CallbackSem, 1, NULL);
	}

	OnSetEvent();
}

void CKernel::OnTerminate()
{
	TCallbackEvent callback;
	DWORD ret;

	memset(&callback, 0, sizeof(callback));

	callback.Func = 1;

	if ((ret = WaitForSingleObject(m_CallbackSem, INFINITE)) != WAIT_OBJECT_0)
		throw std::exception();

	__try {
		m_EventQueue.push_back(callback);
	}
	__finally {
		ReleaseSemaphore(m_CallbackSem, 1, NULL);
	}

	OnSetEvent();
}

void CKernel::Execute()
{
	__try {
		LockCPU();
		__try {
			OnStart();

			sta_ker();

			MainLoop();
		}
		__finally {
			UnlockCPU();
		}

		exit_kernel();

		ClearAllTask();
	}
	__finally {
		OnTerminate();
	}
}

void CKernel::TraceLogPut(char c)
{
	m_SendBuf[m_SendBufPos++] = c;
	if (m_SendBufPos == (sizeof(m_SendBuf) / sizeof(m_SendBuf[0]))) {
		Output(1, m_SendBuf, sizeof(m_SendBuf));
		//memset(m_SendBuf, 0, sizeof(m_SendBuf));
		m_SendBufPos = 0;
	}
}

void CKernel::TraceLogFlush()
{
	Output(1, m_SendBuf, m_SendBufPos * sizeof(m_SendBuf[0]));
	//memset(m_SendBuf, 0, sizeof(m_SendBuf));
	m_SendBufPos = 0;
}

/*
 * システムログの低レベル出力のための文字出力
 */
void CKernel::TraceLogPutC(char c)
{
	if (c == '\n') {
		g_Kernel->TraceLogPut('\r');
	}
	g_Kernel->TraceLogPut(c);
}

void CKernel::ProcInterrupt()
{
	if (/*m_Locked == 1*/true) {
		m_IntMode = true;
		int setPrio = m_SetPrio;
		// 割り込み処理
		while (m_Prio >= m_SetPrio) {
			int prio = m_Prio;
			m_Prio--;
			while (m_IntNo < m_SifCPU->GetInterruptCount()) {
				int intNo = m_IntNo;
				m_IntNo++;
				if ((InterruptPriority(intNo) == prio) && InterruptRaised(intNo)) {
					ClearInterrupt(intNo);
					if (InterruptUnmasked(intNo)) {
#ifdef LOG_INH_ENTER
						LOG_INH_ENTER(intNo);
#endif /* LOG_INH_ENTER */
						try {
							m_SifCPU->InvokeInterrupt(intNo);
						}
						catch (...) {
						}
#ifdef LOG_INH_LEAVE
						LOG_INH_LEAVE(intNo);
#endif /* LOG_INH_LEAVE */
					}
				}
			}
			m_IntNo = 0;
		}
		m_IntMode = false;
		m_SetPrio = setPrio;
	}
}

void CKernel::MainLoop()
{
	TCB	*preruntsk;
#if TIPM_LOCK != -15
	lock_flag = false;
#endif	/* TIPM_LOCK != -15	*/
	DWORD ret;

	for (;;) {
		m_Prio = 15;
		m_IntNo = 0;
		ProcInterrupt();
		m_Prio = m_SetPrio;

		preruntsk = p_runtsk;
		if (dspflg)
			p_runtsk = p_schedtsk;
		if (preruntsk != p_runtsk) {
#ifdef LOG_DSP_LEAVE
			LOG_DSP_LEAVE(preruntsk);
#endif /* LOG_DSP_LEAVE */
#ifdef LOG_DSP_ENTER
			LOG_DSP_ENTER(p_runtsk);
#endif /* LOG_DSP_LEAVE */
		}

		if (p_runtsk == NULL) {
			if (IsTerminated())
				break;
			Idle();
		}
		else {
			m_Current = (CCPUContext *)p_runtsk->tskctxb.cpu_context;

#if TIPM_LOCK != -15
			lock_flag = false;
#endif	/* TIPM_LOCK != -15	*/

			m_Current->PopContext();

#ifdef TOPPERS_SUPPORT_OVRHDR /* 6.6.1 */
			ovrtimer_start();
#endif /* TOPPERS_SUPPORT_OVRHDR */
			UnlockCPU();

			m_TaskMode = true;

			if ((ret = WaitForSingleObject(m_IntEvent, 1000)) != WAIT_OBJECT_0) {
				if (ret == WAIT_TIMEOUT) {
					m_Current->Timeout();
				}
				else{
					Terminate();
				}
			}

			m_TaskMode = false;

			LockCPU();

			CCPUContext *CPUContext = (CCPUContext *)m_Current;
			m_Current = NULL;

			if (CPUContext->IsReleased()) {
				CPUContext->Wait();
				delete CPUContext;
			}
			else
				CPUContext->PushContext();
		}

		//trace_dump((intptr_t)CKernel::TraceLogPutC);
	}

	TraceLogFlush();
}

void CKernel::ClearAllTask()
{
	TCB *Task;
	CCPUContext *CPUContext;
	bool end;

	do {
		end = true;
		for (uint_t i = 0; i < tnum_tsk; i++) {
			Task = &tcb_table[i];
			if (Task->p_tinib != NULL) {
				CPUContext = (CCPUContext *)Task->tskctxb.cpu_context;
				if ((CPUContext != NULL) && !CPUContext->IsFinished()) {
					CPUContext->Terminate();
					end = false;
					continue;
				}
				Task->tskctxb.cpu_context = NULL;
				delete CPUContext;
			}
		}
		SwitchToThread();
	} while (!end);
}

void CKernel::Idle()
{
	ID tskid;

	tskid = OnIdle();

	if (tskid != TSK_NONE) {
		TCB *Task;

		Task = get_tcb(tskid);
		if (Task != NULL) {
			make_runnable(Task);
		}
	}
	else {
#if TIPM_LOCK != -15
		lock_flag = false;
#endif	/* TIPM_LOCK != -15	*/
		DWORD ret;

		UnlockCPU();

		if ((ret = WaitForSingleObject(m_IntEvent, INFINITE)) != WAIT_OBJECT_0) {
			Terminate();
		}

		LockCPU();
	}
}

void CKernel::Dispatch()
{
	CCPUContext *CPUContext = CCPUContext::GetCurrent();

	CPUContext->Dispatch();
}

bool CKernel::ExitAndDispatch()
{
	ExitCPUContext(CCPUContext::GetCurrent());
	return true;
}

ID CKernel::OnIdle()
{
	ID tskid = TSK_NONE;
#if 0
	TCallbackEvent callback;
	DWORD ret;

	memset(&callback, 0, sizeof(callback));

	callback.Func = 2;

	if ((ret = WaitForSingleObject(m_CallbackSem, INFINITE)) != WAIT_OBJECT_0)
		throw std::exception();

	__try{
		m_EventQueue.push_back(callback);
	}
	__finally{
		ReleaseSemaphore(m_CallbackSem, 1, NULL);
	}

	OnSetEvent();
#endif
	return tskid;
}

void CKernel::ExitCPUContext(CCPUContext *CPUContext)
{
	ASSERT(CPUContext->IsCurrent());

	CPUContext->Release();

	// UnlockCPUでは、deleteしたCPUContextに対してEndDelaySuspendを
	// 呼んでしまうので、その手前の処理のみ実行。
	int TlsLockCount = (int)TlsGetValue(m_TlsIndex);
	TlsLockCount--;

	// ロック解除
	if (TlsLockCount == 0) {
		ReleaseSemaphore(m_SysSem, 1, &m_SysSemCount);
		InterlockedDecrement(&m_Locked);
	}

	TlsSetValue(m_TlsIndex, (LPVOID)TlsLockCount);

	ExitThread(0);
}

void CKernel::LockCPU()
{
	int TlsLockCount = (int)TlsGetValue(m_TlsIndex);

	// 他のスレッドが動かないようロック
	if (TlsLockCount == 0) {
		InterlockedIncrement(&m_Locked);
		if (InKernelMode()) {
#if 0
			for (;;) {
				HANDLE handles[2] = { m_IntEvent, m_SysSem };

				switch (WaitForMultipleObjects(2, handles, FALSE, INFINITE)) {
				case WAIT_OBJECT_0:
					// 割り込み処理
					ProcInterrupt();
					continue;
				case WAIT_OBJECT_0 + 1:
					break;
				default:
					Terminate();
					break;
				}
				break;
			}
#else
			if (WaitForSingleObject(m_SysSem, INFINITE) != WAIT_OBJECT_0) {
				Terminate();
			}
#endif
		}
		else {
			for (;;) {
				DWORD ret;
				if ((ret = WaitForSingleObject(m_SysSem, INFINITE)) != WAIT_OBJECT_0) {
					Terminate();
					break;
				}
				// 実行を意図したスレッドかチェック
				CCPUContext *Context = CCPUContext::GetCurrent();
				if ((Context == NULL) || (Context == m_Current)
					|| ((m_Current == NULL) && m_TaskMode))
					break;

				SetEvent(m_IntEvent);

				// 実行したくないスレッドはもう一度待つ
				ReleaseSemaphore(m_SysSem, 1, &m_SysSemCount);
				SwitchToThread();
			}
		}
	}

	TlsLockCount++;
	TlsSetValue(m_TlsIndex, (LPVOID)TlsLockCount);

	if (TlsLockCount == 1) {
		CCPUContext *LockCPUContext = CCPUContext::GetCurrent();
		if (LockCPUContext != NULL)
			LockCPUContext->StartDelaySuspend();
	}
}

void CKernel::UnlockCPU()
{
	int TlsLockCount = (int)TlsGetValue(m_TlsIndex);
	TlsLockCount--;

	ASSERT(TlsLockCount >= 0);

	// ロック解除
	if (TlsLockCount == 0) {
		ReleaseSemaphore(m_SysSem, 1, &m_SysSemCount);
		InterlockedDecrement(&m_Locked);
	}

	TlsSetValue(m_TlsIndex, (LPVOID)TlsLockCount);

	if (TlsLockCount == 0) {
		CCPUContext *LockCPUContext = CCPUContext::GetCurrent();
		if (LockCPUContext != NULL)
			LockCPUContext->EndDelaySuspend();
	}
}

void CKernel::SwitchKernelMode(CCPUContext *CPUContext)
{
	int i = 0, TlsLockCount;
	DWORD ret;

	TlsLockCount = (int)TlsGetValue(m_TlsIndex);

	ASSERT(TlsLockCount > 0);

	while ((m_Locked == 0) && !m_TaskMode)
		SwitchToThread();

	// ロック解除
	ReleaseSemaphore(m_SysSem, 1, &m_SysSemCount);
	InterlockedExchangeAdd(&m_Locked, -TlsLockCount);

	TlsSetValue(m_TlsIndex, (LPVOID)0);

	SetEvent(m_IntEvent);

	CPUContext->Suspend();

	// 他のスレッドが動かないようロック
	InterlockedExchangeAdd(&m_Locked, TlsLockCount);
	if ((ret = WaitForSingleObject(m_SysSem, INFINITE)) != WAIT_OBJECT_0) {
		Terminate();
	}

	TlsSetValue(m_TlsIndex, (LPVOID)TlsLockCount);
}

bool CKernel::CheckIntNoCfg(int IntNo)
{
	return (IntNo >= 0) && (IntNo < m_SifCPU->GetInterruptCount());
}

void CKernel::Interrupt(int IntNo)
{
	SetInterrupt(IntNo);
	SetEvent(m_IntEvent);
}

void CKernel::TryProcessInterrupt(int IntNo)
{
	if (m_Locked > 0) {
#if 0
		SetEvent(m_IntEvent);
#endif
		SwitchToThread();
	}
}

CCPUContext *CKernel::StartDelaySuspend()
{
	CCPUContext *CPUContext = CCPUContext::GetCurrent();

	if (CPUContext != NULL) {
		CPUContext->StartDelaySuspend();
	}

	return CPUContext;
}

void CKernel::EndDelaySuspend(CCPUContext *CPUContext)
{
	if (CPUContext == NULL)
		return;

	CPUContext->EndDelaySuspend();
}

bool CKernel::InTaskContext()
{
	return CCPUContext::GetCurrent() != NULL;
}

bool CKernel::DefineSysIF(uintptr_t Addr, int Size, ::ISystemIF *SystemIF)
{
	TSystemIFItem Item;

	Item.Addr = Addr;
	Item.Size = Size;
	Item.SystemIF = SystemIF;

	m_SystemIFList.push_back(Item);

	return E_OK;
}

void CKernel::AddSysTimerSync(ISysTimerSync *SysTimerSync)
{
	m_SysTimerSyncList.push_back(SysTimerSync);
}

unsigned char CKernel::GetByte(uintptr_t Addr)
{
	bool exist = false;
	unsigned char result = 0xFF;

	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		for (list<TSystemIFItem>::iterator Item = m_SystemIFList.begin(); Item != m_SystemIFList.end(); Item++) {
			if ((Addr >= Item->Addr) && (Addr < Item->Addr + Item->Size)) {
				result = Item->SystemIF->GetByte(Addr);
				exist = true;
			}
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);

	if (!exist)
		result = *(unsigned char *)Addr;

	return result;
}

void CKernel::SetByte(uintptr_t Addr, unsigned char Value)
{
	bool exist = false;

	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		for (list<TSystemIFItem>::iterator Item = m_SystemIFList.begin(); Item != m_SystemIFList.end(); Item++) {
			if ((Addr >= Item->Addr) && (Addr < Item->Addr + Item->Size)) {
				Item->SystemIF->SetByte(Addr, Value);
				exist = true;
			}
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);

	if (!exist)
		*(unsigned char *)Addr = Value;
}

unsigned short CKernel::GetUInt16(uintptr_t Addr)
{
	bool exist = false;
	unsigned short result = 0xFFFF;

	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		for (list<TSystemIFItem>::iterator Item = m_SystemIFList.begin(); Item != m_SystemIFList.end(); Item++) {
			if ((Addr >= Item->Addr) && (Addr < Item->Addr + Item->Size)) {
				result = Item->SystemIF->GetUInt16(Addr);
				exist = true;
			}
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);

	if (!exist)
		result = *(unsigned short *)Addr;

	return result;
}

void CKernel::SetUInt16(uintptr_t Addr, unsigned short Value)
{
	bool exist = false;

	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		for (list<TSystemIFItem>::iterator Item = m_SystemIFList.begin(); Item != m_SystemIFList.end(); Item++) {
			if ((Addr >= Item->Addr) && (Addr < Item->Addr + Item->Size)) {
				Item->SystemIF->SetUInt16(Addr, Value);
				exist = true;
			}
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);

	if (!exist)
		*(unsigned short *)Addr = Value;
}

unsigned int CKernel::GetUInt32(uintptr_t Addr)
{
	bool exist = false;
	unsigned int result = 0xFFFFFFFF;

	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		for (list<TSystemIFItem>::iterator Item = m_SystemIFList.begin(); Item != m_SystemIFList.end(); Item++) {
			if ((Addr >= Item->Addr) && (Addr < Item->Addr + Item->Size)) {
				result = Item->SystemIF->GetUInt32(Addr);
				exist = true;
			}
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);

	if (!exist)
		result = *(unsigned int *)Addr;

	return result;
}

void CKernel::SetUInt32(uintptr_t Addr, unsigned int Value)
{
	bool exist = false;

	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		for (list<TSystemIFItem>::iterator Item = m_SystemIFList.begin(); Item != m_SystemIFList.end(); Item++) {
			if ((Addr >= Item->Addr) && (Addr < Item->Addr + Item->Size)) {
				Item->SystemIF->SetUInt32(Addr, Value);
				exist = true;
			}
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);

	if (!exist)
		*(unsigned int *)Addr = Value;
}

void CKernel::Output(int Kind, const void *Data, int Size)
{
	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		TCallbackEvent callback;
		DWORD ret;

		memset(&callback, 0, sizeof(callback));

		if (Size > sizeof(callback.Buffer)) {
			callback.Func = 3;
			callback.Kind = Kind;
			callback.Data = new char[Size];
			memcpy(callback.Data, Data, Size);
			callback.Size = Size;
		}
		else {
			callback.Func = 4;
			callback.Kind = Kind;
			memcpy(callback.Buffer, Data, Size);
			callback.Size = Size;
		}

		if ((ret = WaitForSingleObject(m_CallbackSem, INFINITE)) != WAIT_OBJECT_0)
			throw std::exception();

		//__try{
		m_EventQueue.push_back(callback);
		//}
		//__finally{
		ReleaseSemaphore(m_CallbackSem, 1, NULL);
		//}

		OnSetEvent();
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);
}

__int64 CKernel::GetTimer()
{
	__int64 result = -1;

	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		for (list<ISysTimerSync *>::iterator Item = m_SysTimerSyncList.begin(); Item != m_SysTimerSyncList.end(); Item++) {
			__int64 temp = (*Item)->GetTimer();
			if ((result == -1) || ((temp != -1) && (result > temp)))
				result = temp;
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);

	return result;
}

void CKernel::Progress(__int64 Timer)
{
	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		for (list<ISysTimerSync *>::iterator Item = m_SysTimerSyncList.begin(); Item != m_SysTimerSyncList.end(); Item++) {
			(*Item)->Progress(Timer);
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);
}

void CKernel::CallTimeOut()
{
	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		for (list<ISysTimerSync *>::iterator Item = m_SysTimerSyncList.begin(); Item != m_SysTimerSyncList.end(); Item++) {
			(*Item)->CallTimeOut();
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);
}

void CKernel::Log(const TCHAR *Text)
{
	g_Kernel->Output(0, Text, (_tcslen(Text) + 1) * sizeof(TCHAR));
}

int CKernel::GetFullPathName(TCHAR *FilePath, int Size, const TCHAR *Extension)
{
	TCHAR Buffer[2024];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	int i, j;
	CString str = m_UnitName;

	i = GetModuleFileName(NULL, Buffer, sizeof(Buffer) / sizeof(TCHAR));
	if (i <= 0) {
		return 0;
	}

	_tsplitpath_s(Buffer, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	if (str.empty()) {
		str = fname;
		if ((j = str.find(_T(".vshost"))) > 0) {
			str = str.substr(0, j);
		}
		m_UnitName = str;
	}

	i = i - _tcslen(ext) + _tcslen(Extension);
	if (FilePath != NULL) {
		if (i > Size) {
			return 0;
		}
		_tmakepath_s(FilePath, Size / sizeof(TCHAR), drive, dir, str.c_str(), Extension);
	}

	return i;
}

int CKernel::ReadFile(TCHAR *Ext, int Pos, void *Data, int Size)
{
	int result = 0;
	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		int i;
		TCHAR FileName[2024];
		fstream File;

		i = GetFullPathName(FileName, sizeof(FileName), Ext);
		if (i > 0) {
			//__try{
			File.open(FileName, fstream::binary | fstream::in);
			File.seekg(Pos, fstream::beg);
			result = (int)File.read((char *)Data, Size).gcount();
			//}
			//__finally{
			File.close();
			//}
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);

	return result;
}

int CKernel::WriteFile(TCHAR *Ext, int Pos, void *Data, int Size)
{
	int result = 0;
	CCPUContext *CPUContext = StartDelaySuspend();
	try {
		int i;
		TCHAR FileName[2024];
		fstream File;

		i = GetFullPathName(FileName, sizeof(FileName), Ext);
		if (i > 0) {
			//__try{
			File.open(FileName, fstream::binary | fstream::in | fstream::out);
			if (File.fail())
				File.open(FileName, fstream::binary | fstream::in | fstream::out | fstream::trunc);
			File.seekp(Pos, fstream::beg);
			File.write((char *)Data, Size);
			result = (int)File.tellp() - Pos;
			//}
			//__finally{
			File.close();
			//}
		}
	}
	catch (...) {
	}
	EndDelaySuspend(CPUContext);

	return result;
}

int CKernel::ReadAddr(uintptr_t Addr, void *buffer, int count)
{
	int *di;
	short *ds;
	char *dc;
	int i, n = count;

	// ２バイト境界へアライメント調整
	dc = (char *)buffer;
	if (Addr & 0x01) {
		*dc++ = GetByte(Addr);
		Addr++;
		n--;
	}

	if (n > 1) {
		// ４バイト境界へアライメント調整
		ds = (short *)dc;
		if (Addr & 0x02) {
			*ds++ = GetUInt16(Addr);
			Addr += 2;
			n -= 2;
		}

		di = (int *)ds;
		i = n >> 2;
		n -= i << 2;

		// ４バイト単位でデータバイト数分繰り返し
		while (--i >= 0) {
			// ４バイトコピー
			*di++ = GetUInt32(Addr);
			Addr += 4;
		}

		ds = (short *)di;
		i = n >> 1;
		n -= i << 1;

		// ２バイト単位でデータバイト数分繰り返し
		while (--i >= 0) {
			// ２バイトコピー
			*ds++ = GetUInt16(Addr);
			Addr += 2;
		}

		dc = (char *)ds;
	}

	// 残りのデータバイト数分繰り返し
	while (--n >= 0) {
		// １バイトコピー
		*dc++ = GetByte(Addr);
		Addr++;
	}

	return count;
}

int CKernel::WriteAddr(uintptr_t Addr, const void *buffer, int count)
{
	int *di;
	short *ds;
	char *dc;
	int i, n = count;

	// ２バイト境界へアライメント調整
	dc = (char *)buffer;
	if (Addr & 0x01) {
		SetByte(Addr, *dc++);
		Addr++;
		n--;
	}

	if (n > 1) {
		// ４バイト境界へアライメント調整
		ds = (short *)dc;
		if (Addr & 0x02) {
			SetUInt16(Addr, *ds++);
			Addr += 2;
			n -= 2;
		}

		di = (int *)ds;
		i = n >> 2;
		n -= i << 2;

		// ４バイト単位でデータバイト数分繰り返し
		while (--i >= 0) {
			// ４バイトコピー
			SetUInt32(Addr, *di++);
			Addr += 4;
		}

		ds = (short *)di;
		i = n >> 1;
		n -= i << 1;

		// ２バイト単位でデータバイト数分繰り返し
		while (--i >= 0) {
			// ２バイトコピー
			SetUInt16(Addr, *ds++);
			Addr += 2;
		}

		dc = (char *)ds;
	}

	// 残りのデータバイト数分繰り返し
	while (--n >= 0) {
		// １バイトコピー
		SetByte(Addr, *dc++);
		Addr++;
	}

	return count;
}

bool CKernel::ProcessEvent()
{
	TCallbackEvent callback;

	while (m_EventQueue.size() > 0) {
		DWORD ret;
		if ((ret = WaitForSingleObject(m_CallbackSem, INFINITE)) != WAIT_OBJECT_0)
			return false;

		__try {
			callback = m_EventQueue.front();
			m_EventQueue.pop_front();
		}
		__finally {
			ReleaseSemaphore(m_CallbackSem, 1, NULL);
		}

		switch (callback.Func) {
		case 0:
			if (m_OnStart != NULL) {
				m_OnStart(m_ObjOfOnStart);
			}
			break;
		case 1:
			if (m_OnTerminate != NULL) {
				m_OnTerminate(m_ObjOfOnTerminate);
			}
			return false;
		case 2:
			if (m_OnIdle != NULL) {
				m_OnIdle(m_ObjOfOnIdle);
			}
			break;
		case 3:
			if (m_OnOutput != NULL) {
				m_OnOutput(m_ObjOfOnOutput, callback.Kind, callback.Data, callback.Size);
			}
			delete callback.Data;
			break;
		case 4:
			if (m_OnOutput != NULL) {
				m_OnOutput(m_ObjOfOnOutput, callback.Kind, callback.Buffer, callback.Size);
			}
			break;
		}
	}

	return !IsTerminated();
}

void CKernel::SetTaskName(ID tskid, const TCHAR *szName)
{
	CCPUContext *CPUContext = g_Kernel->StartDelaySuspend();
	try {
		CString Name = g_Kernel->m_UnitName + _T("::") + szName;

		TCB *Task = get_tcb(tskid);

		CCPUContext *tc;
		for (;;) {
			tc = (CCPUContext *)Task->tskctxb.cpu_context;
			if (tc != NULL) {
				tc->SetThreadName(Name.c_str());
				break;
			}
			//if(Task->tstat == TS_DORMANT)
			//	break;
			SwitchToThread();
		}
	}
	catch (...) {
	}
	g_Kernel->EndDelaySuspend(CPUContext);
}

void CKernel::SetThreadName(DWORD dwThreadID, const TCHAR *szName)
{
	char *Temp = NULL;

	CCPUContext *CPUContext = g_Kernel->StartDelaySuspend();
	try {
		CString Name = g_Kernel->m_UnitName + _T("::") + szName;
		size_t Size = Name.length() + 1;
		Temp = new char[Size];
		try {
			wcstombs_s(&Size, Temp, Size, Name.c_str(), Size - 1);

			::SetThreadName(dwThreadID, Temp);
		}
		catch (...) {
		}
		delete Temp;
	}
	catch (...) {
	}
	g_Kernel->EndDelaySuspend(CPUContext);
}

void CKernel::GetSystemTime(__int64 *Now)
{
	if (m_OnGetSystemTime != NULL)
		m_OnGetSystemTime(m_ObjOfOnGetSystemTime, Now);
}

void CKernel::AddInterface(const TCHAR *type, const TCHAR *name, int kind)
{
	if (m_OnAddInterface != NULL)
		m_OnAddInterface(m_ObjOfOnAddInterface, type, name, kind);
}

void CKernel::RemoveInterface(const TCHAR *type, const TCHAR *name, int kind)
{
	if (m_OnRemoveInterface != NULL)
		m_OnRemoveInterface(m_ObjOfOnRemoveInterface, type, name, kind);
}

void CKernel::SetBit(unsigned int bit, unsigned int addr)
{
	SetByte(addr, GetByte(addr) | (1 << (bit & 7)));
}

void CKernel::ClearBit(unsigned int bit, unsigned int addr)
{
	SetByte(addr, GetByte(addr) & (~(1 << (bit & 7))));
}

unsigned int CKernel::TestBit(unsigned int bit, unsigned int addr)
{
	return (GetByte(addr) & (1 << (bit & 7))) != 0;
}

void CKernel::OutputLastErrorMessage()
{
	LPVOID lpMsgBuf;

	BOOL ret = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	Output(-1, lpMsgBuf, (_tcslen((LPTSTR)lpMsgBuf) + 1) * sizeof(TCHAR));

	LocalFree(lpMsgBuf);
}

bool CKernel::KernelFlag()
{
	return kerflg != 0;
}

extern "C" void software_term_hook()
{
}

extern "C" void TOPPERS_assert_abort(void)
{
	if (g_Kernel->IsTerminated())
		return;

	DebugBreak();
}

void *Kernel_ReadAddr(void *dest, const void *src, int n)
{
	g_Kernel->ReadAddr((unsigned int)src, dest, n);

	return dest;
}

void *Kernel_WriteAddr(void *dest, const void *src, int n)
{
	g_Kernel->WriteAddr((unsigned int)dest, src, n);

	return dest;
}

void Kernel_Log(const TCHAR *Text)
{
	g_Kernel->Output(-256, Text, (_tcslen(Text) + 1) * sizeof(TCHAR));
}
