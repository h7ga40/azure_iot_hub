/// <summary>
/// WaveThrd.h: CWaveThrd クラスのインターフェイス
/// </summary>
// Copyright(C) 2005-2015 Cores Co., Ltd.Japan
// $Id: WaveThrd.h 1441 2018-03-18 14:19:49Z coas-nagasima $
#ifndef WaveThrdH
#define WaveThrdH

#pragma once

#include "Kernel.h"
#include "SimCmn.h"
#include "SysUtils.h"

//------------------------------------------------------------------------------
class CWaveThrd : public SimCmn::CThread
{
public:
	CWaveThrd();
	~CWaveThrd();
private:
	HGLOBAL m_hGMemInBuf;
	HGLOBAL m_hGMemOutBuf;
	HWAVEIN m_hWaveIn;
	HWAVEOUT m_hWaveOut;
	LPWAVEHDR *m_WaveInHdr;
	LPWAVEHDR *m_WaveOutHdr;
	short *m_InData;
	short *m_OutData;
	int m_InBufCount;
	int m_OutBufCount;
	short *m_InDataEnd;
	short *m_OutDataEnd;

	CRITICAL_SECTION m_Lock;
	SimCmn::CEvent *m_Event;
	LPWAVEHDR m_ProcWaveInHdr;
	LPWAVEHDR m_ProcWaveOutHdr;
protected:
	int m_SamplesPerSec;
	short *GetInData(){ return m_InData; }
	short *GetOutData(){ return m_OutData; }
	short *GetInDataEnd(){ return m_InDataEnd; }
	short *GetOutDataEnd(){ return m_OutDataEnd; }

	void Lock() { EnterCriticalSection(&m_Lock); }
	void Unlock() { LeaveCriticalSection(&m_Lock); }

	void Execute();
	virtual void ProcWaveIn(LPWAVEHDR WaveHdr) = 0;
	virtual void ProcWaveOut(LPWAVEHDR WaveHdr) = 0;

	void WaveIn(LPWAVEHDR WaveHdr);
	static void CALLBACK WaveInProc(HWAVEIN hwi, UINT uMsg,
		DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	void WaveOut(LPWAVEHDR WaveHdr);
	static void CALLBACK WaveOutProc(HWAVEOUT hwo, UINT uMsg,
		DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
public:
	HWAVEIN GethWaveIn(){ return m_hWaveIn; }
	HWAVEOUT GethWaveOut(){ return m_hWaveOut; }

	bool OpenWaveIn();
	void CloseWaveIn();
	bool OpenWaveOut();
	void CloseWaveOut();

	virtual void InitIn() = 0;
	virtual void InitOut() = 0;

	unsigned short GetOutVolume();
	void SetOutVolume(unsigned short Volume);
};

#endif // WaveThrdH
