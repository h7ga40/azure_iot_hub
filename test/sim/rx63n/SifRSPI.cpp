//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifRSPI.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#include "StdAfx.h"
#include "WinKernel.h"
#include "SifRSPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CKernel *g_Kernel;

//------------------------------------------------------------------------------
// CSifRSPI
//	概要：
//	引数：
//	pKernel
//	戻り値：
//------------------------------------------------------------------------------
CSifRSPI::CSifRSPI(TRX63NRSPI0Reg *reg, int rxIntNo, int txIntNo, int idIntNo, int devNo)
{
	m_Reg = reg;
	m_TxIntNo = txIntNo;
	m_RxIntNo = rxIntNo;
	m_IdIntNo = idIntNo;
	m_DevNo = devNo;
	m_SendTerm = rspistIdle;
	m_SendTimer = -1;
	m_Send = 0;
	//memset(m_SendBuf, 0, sizeof(m_SendBuf));
	m_SendBufPos = 0;
	m_RecvTerm = rspirtIdle;
	m_RecvTimer = -1;
	m_RecvBuf = NULL;
	m_RecvBufPos = 0;

	InitializeCriticalSection(&m_Lock);
}

//------------------------------------------------------------------------------
//	概要：
//	引数：
//	戻り値：
//------------------------------------------------------------------------------
CSifRSPI::~CSifRSPI()
{
	TRSPIDataNode *node;

	while (!m_RecvQueue.empty()) {
		// このブロックの終わりまで割り込み禁止
		Lock lock(this);

		// キューから次のメッセージを取得
		node = m_RecvQueue.front();
		m_RecvQueue.pop_front();
		delete (unsigned char *)node;
	}

	DeleteCriticalSection(&m_Lock);

	delete m_RecvBuf;
}

uint8_t CSifRSPI::GetByte(intptr_t index)
{
	uint8_t *reg = (uint8_t *)&m_RegInst;

	// 受信バッファが読み込まれたら
	if((index >= (intptr_t)&((TRX63NRSPI0Reg *)0)->SPDR)
		&& (index <= (intptr_t)&(&((TRX63NRSPI0Reg *)0)->SPDR)[1])){
		return ((uint8_t *)&m_SPRX)[index - (intptr_t)&((TRX63NRSPI0Reg *)0)->SPDR];
	}

	return reg[index];
}

void CSifRSPI::SetByte(intptr_t index, uint8_t value)
{
	uint8_t *reg = (uint8_t *)&m_RegInst;

	reg[index] = value;

	// 送信バッファに書き込まれたら
	if(index == (intptr_t)&((TRX63NRSPI0Reg *)0)->SPDR){
	}
}

uint32_t CSifRSPI::GetInt32(intptr_t index)
{
	uint32_t *reg = (uint32_t *)&m_RegInst;

	// 受信バッファが読み込まれたら
	if(index == (uintptr_t)&((TRX63NRSPI0Reg *)0)->SPDR){
		return m_SPRX;
	}

	return reg[index / sizeof(uint32_t)];
}

void CSifRSPI::SetInt32(intptr_t index, uint32_t value)
{
	uint32_t *reg = (uint32_t *)&m_RegInst;

	reg[index / sizeof(uint32_t)] = value;

	// 送信バッファに書き込まれたら
	if (index == (intptr_t)&((TRX63NRSPI0Reg *)0)->SPDR) {
		InterlockedIncrement(&m_Send);
		g_Kernel->OnSetEvent();
	}
}

__int64 CSifRSPI::GetTimer()
{
	__int64 timer = m_SendTimer;
	__int64 timer2 = m_RecvTimer;

	if ((timer == -1) || ((timer2 != -1) && (timer > timer2)))
		timer = timer2;

	return timer;
}

void CSifRSPI::Progress(__int64 timer)
{
	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	if (m_SendTimer != -1) {
		m_SendTimer -= timer;
		if (m_SendTimer < 0) {
			m_SendTimer = 0;
		}
	}
	if (m_RecvTimer != -1) {
		m_RecvTimer -= timer;
		if (m_RecvTimer < 0) {
			m_RecvTimer = 0;
		}
	}
}

void CSifRSPI::CallTimeOut()
{
	CallSendTimeOut();
	CallRecvTimeOut();
}

void CSifRSPI::CallSendTimeOut()
{
	uint8_t *spdr;
	int len;

	if(((m_SendTerm == rspistIdle) || (m_SendTerm == rspistFlush)) && (m_Send != 0)){
		m_SendTimer = 0;
		m_SendTerm = rspistIdle;
	}

	if(m_SendTimer != 0)
		return;

	switch(m_SendTerm){
	case rspistIdle:
		spdr = (uint8_t *)&m_RegInst.SPDR;
		len = 1;
		for (int i = 0; i < len; i++) {
			m_SendBuf[m_SendBufPos++] = *spdr++;
			if (m_SendBufPos == (sizeof(m_SendBuf) / sizeof(m_SendBuf[0]))) {
				Send(m_SendBuf, m_SendBufPos);
				//memset(m_SendBuf, 0, sizeof(m_SendBuf));
				m_SendBufPos = 0;
			}
		}
		m_SendTerm = rspistSending;
		m_SendTimer = CalcTimer();
		break;
	case rspistSending:
		g_Kernel->Interrupt(m_TxIntNo);
		m_SendTimer = 0;
		m_SendTerm = rspistWait;
		break;
	case rspistWait:
		// 送信データがなければ
		if(InterlockedDecrement(&m_Send) == 0){
			// 少し待って送信
			m_SendTimer = 2 * CalcTimer();
			m_SendTerm = rspistFlush;

			int data = 0;
			Receive(&data, sizeof(data));
		}
		else{
			m_SendTimer = -1;
			m_SendTerm = rspistIdle;
		}
		break;
	case rspistFlush:
		Send(m_SendBuf, m_SendBufPos);
		//memset(m_SendBuf, 0, sizeof(m_SendBuf));
		m_SendBufPos = 0;

		m_SendTimer = -1;
		m_SendTerm = rspistIdle;
		break;
	}
}

void CSifRSPI::CallRecvTimeOut()
{
	uint8_t *spdr;
	int len;

	if (m_RecvTimer != 0)
		return;

	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	switch (m_RecvTerm) {
	case rspirtIdle:
		m_RecvTimer = -1;
		break;
	case rspirtReceiving:
		spdr = (uint8_t *)&m_SPRX;
		len = 1;
		memcpy(spdr, &m_RecvBuf->Data[m_RecvBufPos], len);
		m_RecvBufPos += len;
		if (m_RecvBufPos >= m_RecvBuf->Size) {
			delete m_RecvBuf;
			if (m_RecvQueue.empty()) {
				m_RecvBuf = NULL;
			}
			else {
				// キューから次のメッセージを取得
				m_RecvBuf = m_RecvQueue.front();
				m_RecvQueue.pop_front();
			}
			m_RecvBufPos = 0;
		}
		g_Kernel->Interrupt(m_RxIntNo);
		m_RecvTimer = 0;
		m_RecvTerm = rspirtWait;
		break;
	case rspirtWait:
		// 受信データがなければ
		if (m_RecvBuf != NULL) {
			// 少し待って受信
			m_RecvTimer = CalcTimer();
			m_RecvTerm = rspirtReceiving;
		}
		else {
			m_RecvTimer = -1;
			m_RecvTerm = rspirtIdle;
		}
		break;
	}
}

__int64 CSifRSPI::CalcTimer()
{
	__int64 frequency = 10000000/*[100nsec]=1[sec]*/;
	int bits;
	int n = m_RegInst.SPBR;
	int N = m_RegInst.SPDR & 0x03;

	return (frequency * 2 * (n + 1) * (1ll << N)) / g_Kernel->Frequency;
}

void CSifRSPI::Receive(const void *data, int size)
{
	TRSPIDataNode *node = (TRSPIDataNode *)new unsigned char[offsetof(TRSPIDataNode, Data) + size];

	node->Size = size;
	memcpy(node->Data, data, size);

	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	if (m_RecvBuf == NULL) {
		m_RecvBuf = node;
		m_RecvBufPos = 0;
		m_RecvTerm = rspirtReceiving;
		m_RecvTimer = 0;

		g_Kernel->OnSetEvent();
	}
	else {
		// メッセージをキューイング
		m_RecvQueue.push_back(node);
	}
}

void CSifRSPI::Send(const void *data, int size)
{
	if (Port != NULL)
		Port->Receive(data, size);
	else
		g_Kernel->Output(m_DevNo, data, size);
}
