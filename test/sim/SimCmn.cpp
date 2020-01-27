//------------------------------------------------------------------------------
// シミュレーション用共通クラス（スレッド関連）
//------------------------------------------------------------------------------
//
// Copyright(C) 2005-2015 Cores Co., Ltd.Japan
// $Id: SimCmn.cpp 1441 2018-03-18 14:19:49Z coas-nagasima $
//
#include "StdAfx.h"
#include "SimCmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SimCmn
{
//IMPLEMENT_DYNAMIC( CThread, CObject )

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  CreateSuspended
//  戻り値：
//------------------------------------------------------------------------------
CThread::CThread()
{
	m_ThreadID = 0;
	m_Handle = 0;
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
CThread::~CThread()
{
	if( !IsFinished() && !m_Suspended ){
		Terminate();
		WaitFor();
	}

	if( m_Handle != 0 ) {
		CloseHandle(m_Handle);
		m_Handle = NULL;
	}
}

#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType;		// Must be 0x1000.
	LPCSTR szName;		// Pointer to name (in user addr space).
	DWORD dwThreadID;	// Thread ID (-1=caller thread).
	DWORD dwFlags;		// Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
void CThread::SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
void CThread::SetThreadName(LPCSTR szThreadName)
{
	SimCmn::CThread::SetThreadName(m_ThreadID, szThreadName);
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  CreateSuspended
//  戻り値：
//------------------------------------------------------------------------------
bool CThread::Start(bool CreateSuspended)
{
	if(m_ThreadID != 0)
		return false;

	m_Terminated = false;
	ReturnValue = false;
	m_Suspended = CreateSuspended;

	m_Handle = CreateThread(NULL, 0, &ThreadProc, (void *)this, CREATE_SUSPENDED, &m_ThreadID);
	if(m_Handle == NULL)
		throw std::exception();

	if(!CreateSuspended)
		::ResumeThread(m_Handle);

	return m_ThreadID != 0;
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  Param
//  戻り値：
//------------------------------------------------------------------------------
unsigned long __stdcall CThread::ThreadProc(void *Param)
{
	CThread *Thread = (CThread *)Param;
	unsigned long Result;

	if(!Thread->m_Terminated){
		try{
			Thread->Execute();
		}catch(...){
		}
		Result = Thread->ReturnValue;
	}
	else{
		Result = 0;
	}

	Thread->m_ThreadID = 0;

	ExitThread(Result);

#if _MSC_VER < 1300
	// .NETではwarningが出力されるので、削除します。
	return Result;
#endif
}

// スレッド優先度
const int Priorities[] = {
	THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL,
	THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL,
	THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL
};

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
TThreadPriority CThread::GetPriority()
{
	TThreadPriority Result;
	int p;
	int i;

	p = GetThreadPriority(m_Handle);
	Result = tpNormal;
	for( i = 0 ; i < sizeof(Priorities) / sizeof(int); i++){
		if( Priorities[i] == p ){
			Result = (TThreadPriority)i;
			break;
		}
	}

	return Result;
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  Value
//  戻り値：
//------------------------------------------------------------------------------
void CThread::SetPriority(TThreadPriority Value)
{
	SetThreadPriority(m_Handle, Priorities[Value]);
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  Value
//  戻り値：
//------------------------------------------------------------------------------
void CThread::SetSuspended(bool Value)
{
	if( Value != m_Suspended )
		if( Value ){
			Suspend();
		}
		else{
			Resume();
		}
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
bool CThread::Suspend()
{
	m_Suspended = true;
	return SuspendThread(m_Handle) != -1;
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
bool CThread::Resume()
{
	DWORD ret = ResumeThread(m_Handle);

	if( ret == 1 )
		m_Suspended = false;

	return ret != -1;
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
void CThread::Terminate()
{
	m_Terminated = true;
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
DWORD CThread::WaitFor()
{
	DWORD Result;
	HANDLE	H;

	H = m_Handle;

	WaitForSingleObject(H, INFINITE);

	GetExitCodeThread(H, &Result);

	return Result;
}

//IMPLEMENT_DYNAMIC( CSynchroObject, CObject )

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
CSynchroObject::CSynchroObject()
{
	m_Handle = NULL;
	m_LastError = 0;
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
CSynchroObject::~CSynchroObject()
{
	if(m_Handle != NULL){
		CloseHandle(m_Handle);
		m_Handle = NULL;
	}
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  Timeout
//  戻り値：
//------------------------------------------------------------------------------
TWaitResult CSynchroObject::WaitFor(DWORD Timeout)
{
	TWaitResult Result;

	switch( WaitForSingleObject(m_Handle, Timeout) ){
	case WAIT_ABANDONED:
		Result = wrAbandoned;
		break;
	case WAIT_OBJECT_0:
		Result = wrSignaled;
		break;
	case WAIT_TIMEOUT:
		Result = wrTimeout;
		break;
	case WAIT_FAILED:
		Result = wrError;
		m_LastError = GetLastError();
		break;
	default:
		Result = wrError;
	}

	return Result;
}
//IMPLEMENT_DYNAMIC( CEvent, CSynchroObject )

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  EventAttributes
//  ManualReset
//  InitialState
//  Name
//  戻り値：
//------------------------------------------------------------------------------
CEvent::CEvent(LPSECURITY_ATTRIBUTES EventAttributes, bool ManualReset,
		bool InitialState, const TCHAR *pName)
{
	m_Handle = CreateEvent(EventAttributes, ManualReset, InitialState, pName);
	if(m_Handle == NULL)
		throw std::exception();
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
void CEvent::SetEvent()
{
	::SetEvent(m_Handle);
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  戻り値：
//------------------------------------------------------------------------------
void CEvent::ResetEvent()
{
	::ResetEvent(m_Handle);
}
//IMPLEMENT_DYNAMIC( CSemaphore, CSynchroObject )

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  SemaphoreAttributes
//  InitialCount
//  MaximumCount
//  Name
//  戻り値：
//------------------------------------------------------------------------------
CSemaphore::CSemaphore(LPSECURITY_ATTRIBUTES SemaphoreAttributes,
  int InitialCount, int MaximumCount, const TCHAR *pName)
{
	m_Handle = CreateSemaphore(SemaphoreAttributes, InitialCount, MaximumCount, pName);
	if(m_Handle == NULL)
		throw std::exception();
}

//------------------------------------------------------------------------------
//  概要：
//  引数：
//  ReleaseCount
//  PreviousCount
//  戻り値：
//------------------------------------------------------------------------------
void CSemaphore::Release(int ReleaseCount, int *PreviousCount)
{
	::ReleaseSemaphore(m_Handle, ReleaseCount, (LPLONG)PreviousCount);
}
//---------------------------------------------------------------------------
}
