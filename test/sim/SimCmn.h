//------------------------------------------------------------------------------
// シミュレーション用共通クラス（スレッド関連）
//------------------------------------------------------------------------------
//
// Copyright(C) 2005-2015 Cores Co., Ltd.Japan
// $Id: SimCmn.h 1441 2018-03-18 14:19:49Z coas-nagasima $
//
#ifndef SimCmnH
#define SimCmnH

namespace SimCmn
{
//---------------------------------------------------------------------------
// CThread
//---------------------------------------------------------------------------
enum TThreadPriority {
	tpIdle,
	tpLowest,
	tpLower,
	tpNormal,
	tpHigher,
	tpHighest,
	tpTimeCritical
};

//------------------------------------------------------------------------------
// Windows スレッド クラス
//  概要：
//  Windows スレッド クラス
//------------------------------------------------------------------------------
class CThread/* : public CObject*/
{
	friend unsigned long __stdcall ThreadProc(void *Param);

	//DECLARE_DYNAMIC( CThread )

public:
	CThread();
	virtual ~CThread();
private:
	HANDLE m_Handle;
	DWORD m_ThreadID;
	bool m_Terminated;
	bool m_Suspended;
	static unsigned long __stdcall ThreadProc(void *Param);
protected:
	int ReturnValue;
	//------------------------------------------------------------------------------
	//  概要：
	//  引数：
	//  戻り値：
	//------------------------------------------------------------------------------
	virtual void Execute() = 0;
public:
	static void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName);
	void SetThreadName(LPCSTR szThreadName);
	//------------------------------------------------------------------------------
	//  概要：
	//  引数：
	//  戻り値：
	//------------------------------------------------------------------------------
	bool IsTerminated(){ return m_Terminated; };
	bool IsSuspended(){ return m_Suspended; }
	bool IsFinished()
	{
		return (m_ThreadID == NULL)
			|| (WaitForSingleObject(m_Handle, 0) == WAIT_OBJECT_0);
	}
	bool Start(bool CreateSuspended);
	bool Resume();
	bool Suspend();
	virtual void Terminate();
	DWORD WaitFor();
	TThreadPriority GetPriority();
	void SetPriority(TThreadPriority Value);
	//------------------------------------------------------------------------------
	//  概要：
	//  引数：
	//  戻り値：
	//------------------------------------------------------------------------------
	HANDLE GetHandle(){ return m_Handle; };
	//------------------------------------------------------------------------------
	//  概要：
	//  引数：
	//  戻り値：
	//------------------------------------------------------------------------------
	bool GetSuspended(){ return m_Suspended; };
	void SetSuspended(bool Value);
	//------------------------------------------------------------------------------
	//  概要：
	//  引数：
	//  戻り値：
	//------------------------------------------------------------------------------
	DWORD GetThreadID(){ return m_ThreadID; };
};
//---------------------------------------------------------------------------
// CSynchroObject
//---------------------------------------------------------------------------
enum TWaitResult {
	wrSignaled,
	wrTimeout,
	wrAbandoned,
	wrError
};

//------------------------------------------------------------------------------
// Windows 同期オブジェクト 基底クラス
//  概要：
//  Windows 同期オブジェクト 基底クラス
//------------------------------------------------------------------------------
class CSynchroObject/* : public CObject*/
{
	//DECLARE_DYNAMIC( CSynchroObject )
public:
	CSynchroObject();
	virtual ~CSynchroObject();
protected:
	HANDLE m_Handle;
	int m_LastError;
public:
	TWaitResult WaitFor(DWORD Timeout);
	//------------------------------------------------------------------------------
	//  概要：
	//  引数：
	//  戻り値：
	//------------------------------------------------------------------------------
	int GetLastError() { return m_LastError; };
	//------------------------------------------------------------------------------
	//  概要：
	//  引数：
	//  戻り値：
	//------------------------------------------------------------------------------
	HANDLE GetHandle() { return m_Handle; };
};
//------------------------------------------------------------------------------
// Windows イベント クラス
//  概要：
//  Windows イベント クラス
//------------------------------------------------------------------------------
class CEvent : public CSynchroObject
{
	//DECLARE_DYNAMIC( CEvent )
public:
	CEvent(LPSECURITY_ATTRIBUTES EventAttributes, bool ManualReset,
		bool InitialState, const TCHAR *pName);
	void SetEvent();
	void ResetEvent();
};
//------------------------------------------------------------------------------
// Windows セマフォ クラス
//  概要：
//  Windows セマフォ クラス
//------------------------------------------------------------------------------
class CSemaphore : public CSynchroObject
{
	//DECLARE_DYNAMIC( CSemaphore )
public:
	CSemaphore(LPSECURITY_ATTRIBUTES SemaphoreAttributes, int InitialCount,
		int MaximumCount, const TCHAR *pName);
	void Release(int ReleaseCount = 1, int *PreviousCount = NULL);
};
//---------------------------------------------------------------------------
}
#endif
