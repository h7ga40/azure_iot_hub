/// <summary>
/// WaveThrd.cpp: CWaveThrd クラスのインプリメンテーション
/// </summary>
// Copyright(C) 2005-2015 Cores Co., Ltd.Japan
// $Id: WaveThrd.cpp 1441 2018-03-18 14:19:49Z coas-nagasima $

#include "stdafx.h"
#include "WaveThrd.h"
#include "CPUCmn.h"
#include <math.h>

#ifdef _DEBUG
#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

#define WAVE_BUFFER_SIZE		(1024 * sizeof(short))
#define WAVE_BUFFER_COUNT		4

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CWaveThrd::CWaveThrd()
{
	m_SamplesPerSec = 22100;

	m_hWaveIn = NULL;
	m_hWaveOut = NULL;
	m_WaveInHdr = new LPWAVEHDR[WAVE_BUFFER_COUNT];
	m_WaveOutHdr = new LPWAVEHDR[WAVE_BUFFER_COUNT];

	InitializeCriticalSection(&m_Lock);
	m_Event = new SimCmn::CEvent(NULL, false, false, NULL);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CWaveThrd::~CWaveThrd()
{
	if((m_hWaveIn != NULL) || (m_hWaveOut != NULL)){
		CloseWaveIn();
		CloseWaveOut();
	}

	delete m_WaveInHdr;
	delete m_WaveOutHdr;

	delete m_Event;
	DeleteCriticalSection(&m_Lock);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CWaveThrd::Execute()
{
	SimCmn::TWaitResult Result;
	LPWAVEHDR ProcWaveInHdr;
	LPWAVEHDR ProcWaveOutHdr;

	while((m_hWaveIn != NULL) && (m_hWaveOut != NULL)){
		Result = m_Event->WaitFor(INFINITE);
		if(Result != SimCmn::wrSignaled){
			break;
		}

		ProcWaveInHdr = m_ProcWaveInHdr;
		ProcWaveOutHdr = m_ProcWaveOutHdr;
		m_ProcWaveInHdr = NULL;
		m_ProcWaveOutHdr = NULL;

		if(ProcWaveInHdr != NULL){
			ProcWaveIn(ProcWaveInHdr);
		}

		if(ProcWaveOutHdr != NULL){
			ProcWaveOut(ProcWaveOutHdr);
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CWaveThrd::OpenWaveIn()
{
	MMRESULT rc;
	WAVEFORMATEX wfx;
	int i;
	char *h, *d;

	if(m_hWaveIn != NULL){
		return false;
	}

	m_ProcWaveInHdr = NULL;

	m_hGMemInBuf = GlobalAlloc(GPTR, (sizeof(WAVEHDR) + WAVE_BUFFER_SIZE) * WAVE_BUFFER_COUNT);
	if(m_hGMemInBuf == NULL){
		return false;
	}

	h = (char *)m_hGMemInBuf;
	d = &h[WAVE_BUFFER_COUNT * sizeof(WAVEHDR)];
	m_InData = (short *)d;
	for(i = 0; i < WAVE_BUFFER_COUNT; i++){
		m_WaveInHdr[i] = (LPWAVEHDR)h;
		h += sizeof(WAVEHDR);
		m_WaveInHdr[i]->lpData = d;
		m_WaveInHdr[i]->dwBufferLength = WAVE_BUFFER_SIZE;
		d += m_WaveInHdr[i]->dwBufferLength;
	}
	m_InDataEnd = (short *)d;

	wfx.wFormatTag		= WAVE_FORMAT_PCM;		// PCM without any compression
	wfx.nChannels		= 1;					// 1channel (Monaural)
	wfx.nSamplesPerSec	= m_SamplesPerSec;		// (Hz)
	wfx.wBitsPerSample	= sizeof(short) * 8;	// (bits/channel)
	wfx.nBlockAlign		= (WORD)(wfx.nChannels * wfx.wBitsPerSample / 8);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize			= 0;

	m_hWaveIn = NULL;
	rc = waveInOpen(&m_hWaveIn, WAVE_MAPPER, &wfx,
		(DWORD)(void*)WaveInProc, (DWORD)this, CALLBACK_FUNCTION);
	if(rc != MMSYSERR_NOERROR){
		return false;
	}

	// バッファブロックを準備して、入力キューに追加する
	for(i = 0; i < WAVE_BUFFER_COUNT; i++){
		rc = waveInPrepareHeader(m_hWaveIn, m_WaveInHdr[i], sizeof(WAVEHDR));

		// 入力キューにバッファを追加する
		if(rc == MMSYSERR_NOERROR){
			rc = waveInAddBuffer(m_hWaveIn, m_WaveInHdr[i], sizeof(WAVEHDR));
			if(rc == MMSYSERR_NOERROR){
				m_InBufCount++;
			}
		}
	}

	// 録音を開始する
	if(rc == MMSYSERR_NOERROR)
		rc = waveInStart(m_hWaveIn);

	if(rc != MMSYSERR_NOERROR){
		CloseWaveIn();	 // 割り当てられたメモリを解放する
		return false;
	}

	InitIn();

	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CWaveThrd::CloseWaveIn(void)
{
	int i;
	HWAVEIN hWaveIn = m_hWaveIn;

	if(m_hWaveIn == NULL)
		return;

	m_ProcWaveInHdr = NULL;
	m_hWaveIn = NULL;

	i = 0;
	while(m_InBufCount != 0){
		Sleep(100);
		if(i > 10){
			break;
		}
		i++;
	}

	// 録音を停止する
	waveInStop(hWaveIn);
	waveInReset(hWaveIn);

	// ヘッダを非準備状態にする
	for(i = 0; i < WAVE_BUFFER_COUNT; i++) {
		waveInUnprepareHeader(hWaveIn, m_WaveInHdr[i], sizeof(WAVEHDR));
	}

	waveInClose(hWaveIn);

	for(i = 0; i < WAVE_BUFFER_COUNT; i++){
		m_WaveInHdr[i] = NULL;
	}

	if(m_hGMemInBuf != NULL){
		GlobalFree(m_hGMemInBuf);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CWaveThrd::WaveIn(LPWAVEHDR WaveHdr)
{
	MMRESULT rc;

	m_ProcWaveInHdr = WaveHdr;

	// 入力キューにバッファを追加する
	rc = waveInPrepareHeader(m_hWaveIn, WaveHdr, sizeof(WAVEHDR));
	if(rc == MMSYSERR_NOERROR){
		rc = waveInAddBuffer(m_hWaveIn, WaveHdr, sizeof(WAVEHDR));
		if(rc == MMSYSERR_NOERROR){
			m_InBufCount++;
		}
	}
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CALLBACK CWaveThrd::WaveInProc(HWAVEIN hwi, UINT uMsg,
	DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	CWaveThrd *This = (CWaveThrd *)dwInstance;
	LPWAVEHDR WaveHdr = (LPWAVEHDR)dwParam1;

	switch(uMsg){
	case WIM_DATA:
		This->Lock();

		This->m_InBufCount--;
		if(This->m_hWaveIn != NULL){
			This->WaveIn(WaveHdr);
		}

		This->Unlock();

		This->m_Event->SetEvent();
		break;
	case WIM_CLOSE:
		This->m_InBufCount = 0;
		break;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CWaveThrd::OpenWaveOut()
{
	WAVEFORMATEX wfx;
	int i;
	char *h, *d;
	MMRESULT rc = MMSYSERR_NOERROR;

	if(m_hWaveOut != NULL){
		return false;
	}

	m_ProcWaveOutHdr = NULL;

	m_hGMemOutBuf = GlobalAlloc(GPTR, (sizeof(WAVEHDR) + WAVE_BUFFER_SIZE) * WAVE_BUFFER_COUNT);
	if(m_hGMemOutBuf == NULL){
		return false;
	}

	h = (char *)m_hGMemOutBuf;
	d = &h[WAVE_BUFFER_COUNT * sizeof(WAVEHDR)];
	m_OutData = (short *)d;
	for(i = 0; i < WAVE_BUFFER_COUNT; i++){
		m_WaveOutHdr[i] = (LPWAVEHDR)h;
		h += sizeof(WAVEHDR);
		m_WaveOutHdr[i]->lpData = d;
		m_WaveOutHdr[i]->dwBufferLength = WAVE_BUFFER_SIZE;
		d += m_WaveOutHdr[i]->dwBufferLength;
	}
	m_OutDataEnd = (short *)d;

	wfx.wFormatTag		= WAVE_FORMAT_PCM;		// PCM without any compression
	wfx.nChannels		= 1;					// 1channel (Monaural)
	wfx.nSamplesPerSec	= m_SamplesPerSec;		// (Hz)
	wfx.wBitsPerSample	= sizeof(short) * 8;	// (bits/channel)
	wfx.nBlockAlign		= (WORD)(wfx.nChannels * wfx.wBitsPerSample / 8);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize			= 0;

	m_hWaveOut = NULL;
	rc = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &wfx,
		(DWORD)(void*)WaveOutProc, (DWORD)this, CALLBACK_FUNCTION);
	if(rc != MMSYSERR_NOERROR){
		return false;
	}

	waveOutSetVolume(m_hWaveOut, 30000);

	// バッファブロックを準備して、入力キューに追加する
	for(i = 0; i < WAVE_BUFFER_COUNT; i++){
		rc = waveOutPrepareHeader(m_hWaveOut, m_WaveOutHdr[i], sizeof(WAVEHDR));
		if(rc == MMSYSERR_NOERROR){
			rc = waveOutWrite(m_hWaveOut, m_WaveOutHdr[i], sizeof(WAVEHDR));
			if(rc == MMSYSERR_NOERROR){
				m_OutBufCount++;
			}
		}
	}

	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CWaveThrd::CloseWaveOut(void)
{
	int i;
	HWAVEOUT hWaveOut = m_hWaveOut;

	if(hWaveOut == NULL){
		return;
	}

	m_ProcWaveOutHdr = NULL;
	m_hWaveOut = NULL;

	i = 0;
	while(m_OutBufCount != 0){
		Sleep(100);
		if(i > 10){
			break;
		}
		i++;
	}

	// 録音を停止する
	waveOutPause(hWaveOut);
	waveOutReset(hWaveOut);

	// ヘッダを非準備状態にする
	for(i = 0; i < WAVE_BUFFER_COUNT; i++) {
		waveOutUnprepareHeader(hWaveOut, m_WaveOutHdr[i], sizeof(WAVEHDR));
	}

	waveOutClose(hWaveOut);

	for(i = 0; i < WAVE_BUFFER_COUNT; i++){
		m_WaveOutHdr[i] = NULL;
	}

	if(m_hGMemOutBuf != NULL){
		GlobalFree(m_hGMemOutBuf);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CWaveThrd::WaveOut(LPWAVEHDR WaveHdr)
{
	MMRESULT rc = MMSYSERR_NOERROR;

	m_ProcWaveOutHdr = WaveHdr;

	// データバッファブロックを再利用する
	WaveHdr->dwBufferLength = WAVE_BUFFER_SIZE;
	rc = waveOutPrepareHeader(m_hWaveOut, WaveHdr, sizeof(WAVEHDR));
	if(rc == MMSYSERR_NOERROR){
		rc = waveOutWrite(m_hWaveOut, WaveHdr, sizeof(WAVEHDR));
		if(rc == MMSYSERR_NOERROR){
			m_OutBufCount++;
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CALLBACK CWaveThrd::WaveOutProc(HWAVEOUT hwo, UINT uMsg,
	DWORD dwOutstance, DWORD dwParam1, DWORD dwParam2)
{
	CWaveThrd *This = (CWaveThrd *)dwOutstance;
	LPWAVEHDR WaveHdr = (LPWAVEHDR)dwParam1;

	switch(uMsg){
	case WOM_DONE:
		This->Lock();

		This->m_OutBufCount--;
		if(This->m_hWaveOut != NULL){
			This->WaveOut(WaveHdr);
		}

		This->Unlock();

		This->m_Event->SetEvent();
		break;
	case WOM_CLOSE:
		This->m_OutBufCount = 0;
		break;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned short CWaveThrd::GetOutVolume()
{
	DWORD Volume;

	if(waveOutGetVolume(m_hWaveOut, &Volume) == MMSYSERR_NOERROR){
		return (unsigned short)Volume;
	}

	return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CWaveThrd::SetOutVolume(unsigned short Volume)
{
	waveOutSetVolume(m_hWaveOut, Volume);
}

