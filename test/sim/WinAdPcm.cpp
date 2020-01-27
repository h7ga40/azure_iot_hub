/// <summary>
/// WinAdPcm.cpp: CWinAdPcm クラスのインプリメンテーション
/// </summary>
// Copyright(C) 2005-2015 Cores Co., Ltd.Japan
// $Id: WinAdPcm.cpp 1441 2018-03-18 14:19:49Z coas-nagasima $

#include "stdafx.h"
#include "Kernel.h"
#include "WinAdPcm.h"
#include "CPUCmn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CKernel *g_Kernel;

#define SAMPLES_PER_SEC			8000
#define WAVE_BUFFER_SIZE		(320 * sizeof(short))
#define ADPCM_BUFFER_SIZE		60

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
CWinAdPcm::CWinAdPcm(TSCodecBuffer *AdPcmInBuf, TSCodecBuffer *AdPcmOutBuf)
	: m_EncAdPcm(3), m_DecAdPcm(3)
{
	int i;

	m_hWaveIn = NULL;
	m_hWaveOut = NULL;
	m_WaveInHdr = new LPWAVEHDR[2];
	m_WaveOutHdr = new LPWAVEHDR[2];

	m_AdPcmInBuf = AdPcmInBuf;
	m_InBankNo = 0;
	m_OutBankNo = 0;

	m_AdPcmOutBuf = AdPcmOutBuf;
	for(i = 0; i < 4; i++){
		memset(m_AdPcmOutBuf->Bank[i].Data, 0, ADPCM_BUFFER_SIZE);
	}

	memset(&OnWaveInEvent, 0, sizeof(OnWaveInEvent));
	memset(&OnWaveOutEvent, 0, sizeof(OnWaveOutEvent));
}

CWinAdPcm::~CWinAdPcm()
{
	if((m_hWaveIn != NULL) || (m_hWaveOut != NULL)){
		Record(false);
		if(m_hWaveIn != NULL)
			Sleep(100);

		Play(false);
		if(m_hWaveOut != NULL)
			Sleep(100);
	}

	delete m_WaveInHdr;
	delete m_WaveOutHdr;
}

bool CWinAdPcm::OpenWaveIn()
{
	WAVEFORMATEX wfx;
	int		 i;
	char	*h;

	m_hGMemInBuf = GlobalAlloc(GPTR, (sizeof(WAVEHDR) + WAVE_BUFFER_SIZE) * 2);
	if(m_hGMemInBuf == NULL){
		return false;
	}

	h = (char *)m_hGMemInBuf;
	for(i = 0; i < 2; i++){
	   m_WaveInHdr[i] = (LPWAVEHDR)h;
	   h += sizeof(WAVEHDR);
	   m_WaveInHdr[i]->lpData = h;
	   m_WaveInHdr[i]->dwBufferLength = WAVE_BUFFER_SIZE;
	   h += m_WaveInHdr[i]->dwBufferLength;
	}

	wfx.wFormatTag		= WAVE_FORMAT_PCM;		// PCM without any compression
	wfx.nChannels		= 1;					// 1channel (Monaural)
	wfx.nSamplesPerSec	= SAMPLES_PER_SEC;		// (Hz)
	wfx.wBitsPerSample	= sizeof(short) * 8;	// (bits/channel)
	wfx.nBlockAlign		= (WORD)(wfx.nChannels * wfx.wBitsPerSample / 8);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize			= 0;

	m_hWaveIn = NULL;
	return waveInOpen(&m_hWaveIn, WAVE_MAPPER, &wfx,
		(DWORD)(void*)WaveInProc, (DWORD)this, CALLBACK_FUNCTION)
			 == MMSYSERR_NOERROR;
}

void CWinAdPcm::StartWaveIn()
{
	MMRESULT rc = MMSYSERR_NOERROR;
	int i;

	//m_EncAdPcm.Init();

	// バッファブロックを準備して、入力キューに追加する
	for(i = 0; i < 2; i++){
		rc = waveInPrepareHeader(m_hWaveIn, m_WaveInHdr[i], sizeof(WAVEHDR));

		// 入力キューにバッファを追加する
		if(rc == MMSYSERR_NOERROR)
			rc = waveInAddBuffer(m_hWaveIn, m_WaveInHdr[i], sizeof(WAVEHDR));
	}

	// 録音を開始する
	if(rc == MMSYSERR_NOERROR)
		rc = waveInStart(m_hWaveIn);

	if(rc != MMSYSERR_NOERROR){
		CloseWaveIn();	 // 割り当てられたメモリを解放する
		return;
	}
}

void CWinAdPcm::CloseWaveIn()
{
	int i;
	HWAVEIN hWaveIn = m_hWaveIn;

	if(hWaveIn == NULL)
		return;

	m_hWaveIn = NULL;

	// 録音を停止する
	waveInStop(hWaveIn);
	waveInReset(hWaveIn);

	// ヘッダを非準備状態にする
	for(i = 0; i < 2; i++) {
		waveInUnprepareHeader(hWaveIn, m_WaveInHdr[i], sizeof(WAVEHDR));
	}

	waveInClose(hWaveIn);

	for(i = 0; i < 2; i++){
		m_WaveInHdr[i] = NULL;
	}

	if(m_hGMemInBuf != NULL){
		GlobalFree(m_hGMemInBuf);
	}
}

void CALLBACK CWinAdPcm::WaveInProc(HWAVEIN hwi, UINT uMsg,
	DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	MMRESULT rc;
	LPWAVEHDR WaveHdr = (LPWAVEHDR)dwParam1;
	CWinAdPcm *This = (CWinAdPcm *)dwInstance;

	if((uMsg ==  WIM_DATA) && (This->m_hWaveIn != NULL)){
		This->m_InBankNo++;
		This->m_InBankNo %= 4;

		This->m_EncAdPcm.Init();
		This->m_EncAdPcm.linear2adpcm((short *)WaveHdr->lpData,
			(unsigned char *)This->m_AdPcmInBuf->Bank[This->m_InBankNo].Data,
			WaveHdr->dwBufferLength / sizeof(short));

		if(This->OnWaveInEvent.Entry != NULL){
			This->OnWaveInEvent.Entry(This->OnWaveInEvent.Object, This);
		}

		// データバッファブロックを再利用する
		rc = waveInPrepareHeader(hwi, WaveHdr, sizeof(WAVEHDR));
		if(rc != MMSYSERR_NOERROR){
			This->Record(false);
		}
		rc = waveInAddBuffer(hwi, WaveHdr, sizeof(WAVEHDR));
		if(rc != MMSYSERR_NOERROR){
			This->Record(false);
		}
	}
}

bool CWinAdPcm::OpenWaveOut()
{
	WAVEFORMATEX wfx;
	int		 i;
	char	*h;

	m_hGMemOutBuf = GlobalAlloc(GPTR, (sizeof(WAVEHDR) + WAVE_BUFFER_SIZE) * 2);
	if(m_hGMemOutBuf == NULL){
		return false;
	}

	h = (char *)m_hGMemOutBuf;
	for(i = 0; i < 2; i++){
	   m_WaveOutHdr[i] = (LPWAVEHDR)h;
	   h += sizeof(WAVEHDR);
	   m_WaveOutHdr[i]->lpData = h;
	   m_WaveOutHdr[i]->dwBufferLength = WAVE_BUFFER_SIZE;
	   h += m_WaveOutHdr[i]->dwBufferLength;
	}

	wfx.wFormatTag		= WAVE_FORMAT_PCM;		// PCM without any compression
	wfx.nChannels		= 1;					// 1channel (Monaural)
	wfx.nSamplesPerSec	= SAMPLES_PER_SEC;		// (Hz)
	wfx.wBitsPerSample	= sizeof(short) * 8;	// (bits/channel)
	wfx.nBlockAlign		= (WORD)(wfx.nChannels * wfx.wBitsPerSample / 8);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize			= 0;

	m_hWaveOut = NULL;
	return waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &wfx,
		(DWORD)(void*)WaveOutProc, (DWORD)this, CALLBACK_FUNCTION)
			 == MMSYSERR_NOERROR;
}

void CWinAdPcm::StartWaveOut()
{
	MMRESULT rc = MMSYSERR_NOERROR;
	int i;

	//m_DecAdPcm.Init();

	// バッファブロックを準備して、入力キューに追加する
	for(i = 0; i < 2; i++){
		m_WaveOutHdr[i]->dwBufferLength = WAVE_BUFFER_SIZE;

		m_DecAdPcm.Init();
		m_DecAdPcm.adpcm2linear((unsigned char *)m_AdPcmOutBuf->Bank[i].Data,
			(short *)m_WaveOutHdr[i]->lpData, ADPCM_BUFFER_SIZE);

		// データバッファブロックを再利用する
		rc = waveOutPrepareHeader(m_hWaveOut, m_WaveOutHdr[i], sizeof(WAVEHDR));
		if(rc == MMSYSERR_NOERROR){
			rc = waveOutWrite(m_hWaveOut, m_WaveOutHdr[i], sizeof(WAVEHDR));
		}
	}

	if(rc != MMSYSERR_NOERROR){
		CloseWaveOut();	 // 割り当てられたメモリを解放する
		return;
	}
}

void CWinAdPcm::CloseWaveOut()
{
	int i;
	HWAVEOUT hWaveOut = m_hWaveOut;

	if(hWaveOut == NULL)
		return;

	m_hWaveOut = NULL;

	// 録音を停止する
	waveOutPause(hWaveOut);
	waveOutReset(hWaveOut);

	// ヘッダを非準備状態にする
	for(i = 0; i < 2; i++) {
		waveOutUnprepareHeader(hWaveOut, m_WaveOutHdr[i], sizeof(WAVEHDR));
	}

	waveOutClose(hWaveOut);

	for(i = 0; i < 2; i++){
		m_WaveOutHdr[i] = NULL;
	}

	if(m_hGMemOutBuf != NULL){
		GlobalFree(m_hGMemOutBuf);
	}
}

void CALLBACK CWinAdPcm::WaveOutProc(HWAVEOUT hwo, UINT uMsg,
	DWORD dwOutstance, DWORD dwParam1, DWORD dwParam2)
{
	MMRESULT rc;
	LPWAVEHDR WaveHdr = (LPWAVEHDR)dwParam1;
	CWinAdPcm *This = (CWinAdPcm *)dwOutstance;
	int BankNo = This->m_OutBankNo;
	//static char *Text[4] = {"0 WaveOut\n", "1 WaveOut\n", "2 WaveOut\n", "3 WaveOut\n"};

	if((uMsg ==  WOM_DONE) && (This->m_hWaveOut != NULL)){
		This->m_OutBankNo++;
		This->m_OutBankNo %= 4;

		//CKernel::Log(Text[BankNo]);

		WaveHdr->dwBufferLength = WAVE_BUFFER_SIZE;

		This->m_DecAdPcm.Init();
		This->m_DecAdPcm.adpcm2linear(
			(unsigned char *)This->m_AdPcmOutBuf->Bank[BankNo].Data,
			(short *)WaveHdr->lpData, ADPCM_BUFFER_SIZE);

		// データバッファブロックを再利用する
		rc = waveOutPrepareHeader(hwo, WaveHdr, sizeof(WAVEHDR));
		if(rc != MMSYSERR_NOERROR){
			This->Play(true);
		}

		rc = waveOutWrite(hwo, WaveHdr, sizeof(WAVEHDR));

		if(This->OnWaveOutEvent.Entry != NULL){
			This->OnWaveOutEvent.Entry(This->OnWaveOutEvent.Object, This);
		}
	}
}

void CWinAdPcm::Record(bool Enable)
{
	if(Enable){
		if((m_hWaveIn == NULL)){
			OpenWaveIn();
			if(m_hWaveIn != NULL){
				StartWaveIn();
			}
		}
	}
	else{
		if((m_hWaveIn != NULL)){
			CloseWaveIn();
		}
	}
}

void CWinAdPcm::Play(bool Enable)
{
	if(Enable){
		if((m_hWaveOut == NULL)){
			OpenWaveOut();
			if(m_hWaveOut != NULL){
				StartWaveOut();
			}
		}
	}
	else{
		if((m_hWaveOut != NULL)){
			CloseWaveOut();
		}
	}
}
