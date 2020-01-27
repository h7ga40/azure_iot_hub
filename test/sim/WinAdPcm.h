/// <summary>
/// WinAdPcm.h: CWinAdPcm クラスのインターフェイス
/// </summary>
// Copyright(C) 2005-2015 Cores Co., Ltd.Japan
// $Id: WinAdPcm.h 1441 2018-03-18 14:19:49Z coas-nagasima $
#ifndef WinAdPcmH
#define WinAdPcmH

#pragma once

#include "SCodecPacket.h"
#include "AdPcm.h"

//
class CWinAdPcm;

//
typedef void (*TWinAdPcmEventEntry)(void *This, CWinAdPcm *Sender);

struct TWinAdPcmEvent{
	void *Object;
	TWinAdPcmEventEntry Entry;
};

//------------------------------------------------------------------------------
class CWinAdPcm/* : public CObject*/
{
public:
	CWinAdPcm(TSCodecBuffer *AdPcmInBuf, TSCodecBuffer *AdPcmOutBuf);
	virtual ~CWinAdPcm();
private:
	CAdPcm m_EncAdPcm;
	CAdPcm m_DecAdPcm;
	HGLOBAL m_hGMemInBuf;
	HGLOBAL m_hGMemOutBuf;
	HWAVEIN m_hWaveIn;
	HWAVEOUT m_hWaveOut;
	LPWAVEHDR *m_WaveInHdr;
	LPWAVEHDR *m_WaveOutHdr;
	int m_InBankNo;
	int m_OutBankNo;
	TSCodecBuffer *m_AdPcmInBuf;
	TSCodecBuffer *m_AdPcmOutBuf;
protected:
	bool OpenWaveIn();
	void StartWaveIn();
	void CloseWaveIn();
	static void CALLBACK WaveInProc(HWAVEIN hwi, UINT uMsg,
		DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	bool OpenWaveOut();
	void StartWaveOut();
	void CloseWaveOut();
	static void CALLBACK WaveOutProc(HWAVEOUT hwo, UINT uMsg,
		DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	void OnNext();
public:
	int GetInBankNo(){ return m_InBankNo; }
	int GetOutBankNo(){ return m_OutBankNo; }
	TWinAdPcmEvent OnWaveInEvent;
	TWinAdPcmEvent OnWaveOutEvent;
	void Record(bool Enable);
	bool IsRecord(){ return m_hWaveIn != NULL; }
	void Play(bool Enable);
	bool IsPlay(){ return m_hWaveOut != NULL; }
};

#endif // WinAdPcmH
