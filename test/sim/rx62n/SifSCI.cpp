//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifSCI.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#include "StdAfx.h"
#include "WinKernel.h"
#include "SifSCI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SIF_SCI_TYPE2	// 受信バッファの読み込みで次のバイトが設定される

extern CKernel *g_Kernel;

//------------------------------------------------------------------------------
// CSifSCI
//	概要：
//	引数：
//	pKernel
//	戻り値：
//------------------------------------------------------------------------------
CSifSCI::CSifSCI(TRX62NSCI0Reg *reg, int txIntNo, int rxIntNo, int teIntNo, int devNo)
{
	m_Reg = reg;
	m_TxIntNo = txIntNo;
	m_RxIntNo = rxIntNo;
	m_TeIntNo = teIntNo;
	m_DevNo = devNo;
	m_SendTerm = scistIdle;
	m_SendTimer = -1;
	//memset(m_SendBuf, 0, sizeof(m_SendBuf));
	m_SendBufPos = 0;
	m_RecvTerm = scirtIdle;
	m_RecvTimer = -1;
	m_RecvBuf = NULL;
	m_RecvBufPos = 0;

	InitializeCriticalSection(&m_Lock);

	memset(&m_RegInst, 0, sizeof(m_RegInst));
	((TSCISSR *)&m_RegInst.SSR)->TDRE = 1;
	((TSCISSR *)&m_RegInst.SSR)->TEND = 1;
}

//------------------------------------------------------------------------------
//	概要：
//	引数：
//	戻り値：
//------------------------------------------------------------------------------
CSifSCI::~CSifSCI()
{
	TSCIDataNode *node;

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

unsigned char CSifSCI::GetByte(intptr_t index)
{
	unsigned char *reg = (unsigned char *)&m_RegInst;

	// 受信バッファが読み込まれたら
	if (index == (uintptr_t)&((TRX62NSCI0Reg *)0)->RDR) {
#ifdef SIF_SCI_TYPE2
		// このブロックの終わりまで割り込み禁止
		Lock lock(this);

		InterlockedAnd8((char *)&m_RegInst.SSR, ~0x40);
		if (m_RecvBuf != NULL) {
			m_RegInst.RDR = m_RecvBuf->Data[m_RecvBufPos++];
			if (m_RecvBufPos == m_RecvBuf->Size) {
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
		}
#else
		InterlockedAnd8((char *)&m_RegInst.SSR, ~0x40);
#endif
	}
	else if (index == (intptr_t)&((TRX62NSCI0Reg *)0)->SSR) {
		return m_RegInst.SSR;
	}

	return reg[index];
}

void CSifSCI::SetByte(intptr_t index, unsigned char value)
{
	unsigned char *reg = (unsigned char *)&m_RegInst;

	reg[index] = value;

	// 送信バッファに書き込まれたら
	if (index == (intptr_t)&((TRX62NSCI0Reg *)0)->TDR) {
		if ((InterlockedAnd8((char *)&m_RegInst.SSR, ~0x80) & 0x80) != 0)
			g_Kernel->OnSetEvent();
	}
}

__int64 CSifSCI::GetTimer()
{
	__int64 timer = m_SendTimer;
	__int64 timer2 = m_RecvTimer;

	if ((timer == -1) || ((timer2 != -1) && (timer > timer2)))
		timer = timer2;

	return timer;
}

void CSifSCI::Progress(__int64 Timer)
{
	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	if (m_SendTimer != -1) {
		m_SendTimer -= Timer;
		if (m_SendTimer < 0) {
			m_SendTimer = 0;
		}
	}
	if (m_RecvTimer != -1) {
		m_RecvTimer -= Timer;
		if (m_RecvTimer < 0) {
			m_RecvTimer = 0;
		}
	}
}

void CSifSCI::CallTimeOut()
{
	CallSendTimeOut();
	CallRecvTimeOut();
}

void CSifSCI::CallSendTimeOut()
{
	if ((m_SendTerm == scistIdle) && ((m_RegInst.SSR & 0x80) == 0)) {
		m_SendTimer = 0;
		m_SendTerm = scistIdle;
	}

	if (m_SendTimer != 0)
		return;

	switch (m_SendTerm) {
	case scistIdle:
		m_SendBuf[m_SendBufPos++] = m_RegInst.TDR;
		InterlockedAnd8((char *)&m_RegInst.SSR, ~0x04);
		InterlockedOr8((char *)&m_RegInst.SSR, 0x80);
		g_Kernel->Interrupt(m_TxIntNo);
		if (m_SendBufPos == (sizeof(m_SendBuf) / sizeof(m_SendBuf[0]))) {
			g_Kernel->Output(m_DevNo, m_SendBuf, sizeof(m_SendBuf));
			//memset(m_SendBuf, 0, sizeof(m_SendBuf));
			m_SendBufPos = 0;
		}
		m_SendTerm = scistSending;
		m_SendTimer = CalcTimer();
		break;
	case scistSending:
		InterlockedOr8((char *)&m_RegInst.SSR, 0x04);
		g_Kernel->Interrupt(m_TeIntNo);
		g_Kernel->Output(m_DevNo, m_SendBuf, m_SendBufPos);
		//memset(m_SendBuf, 0, sizeof(m_SendBuf));
		m_SendBufPos = 0;
		m_SendTimer = -1;
		m_SendTerm = scistIdle;
		break;
	}
}

void CSifSCI::CallRecvTimeOut()
{
	if (m_RecvTimer != 0)
		return;

	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	switch (m_RecvTerm) {
	case scirtIdle:
		m_RecvTimer = -1;
		break;
	case scirtReceiving:
#ifdef SIF_SCI_TYPE2
		InterlockedOr8((char *)&m_RegInst.SSR, 0x40);
#else
		m_RegInst.RDR = m_RecvBuf->Data[m_RecvBufPos++];
		InterlockedOr8((char *)&m_RegInst.SSR, 0x40);
		if (m_RecvBufPos == m_RecvBuf->Size) {
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
#endif
		g_Kernel->Interrupt(m_RxIntNo);
		m_RecvTimer = CalcTimer();
		m_RecvTerm = scirtWait;
		break;
	case scirtWait:
		// 受信データがあれば続けて受信
		if (m_RecvBuf != NULL) {
#ifdef SIF_SCI_TYPE2
			if ((m_RegInst.SSR & 0x40) == 0) {
				m_RecvTimer = 0;
				m_RecvTerm = scirtReceiving;
			}
			else {
				m_RecvTimer = CalcTimer();
				m_RecvTerm = scirtWait;
			}
#else
			m_RecvTimer = 0;
			m_RecvTerm = scirtReceiving;
#endif
		}
		// 受信データがなければ待機
		else {
			m_RecvTimer = -1;
			m_RecvTerm = scirtIdle;
		}
		break;
	}
}

__int64 CSifSCI::CalcTimer()
{
	__int64 frequency = 10000000/*[100nsec]=1[sec]*/;
	__int64 bits;
	__int64 abcs;
	TSCISMR *SMR = (TSCISMR *)&m_RegInst.SMR;
	TSCISEMR *SEMR = (TSCISEMR *)&m_RegInst.SEMR;

	// データビット数
	if (SMR->CHR == 0)
		bits = 7;
	else
		bits = 8;

	// パリティありの場合
	if (SMR->PE != 0)
		// 1bit追加
		bits += 1;

	// ストップビット数加算
	if (SMR->STOP == 0)
		bits += 1;
	else
		bits += 2;

	if (SEMR->ABCS == 0)
		abcs = 64;
	else
		abcs = 32;

	return (frequency * bits * abcs * (1ll << (2 * SMR->CSK)) * (m_RegInst.BRR + 1)) / g_Kernel->Frequency;
}

void CSifSCI::Receive(const void *data, int size)
{
	TSCIDataNode *node = (TSCIDataNode *)new unsigned char[offsetof(TSCIDataNode, Data) + size];

	node->Size = size;
	memcpy(node->Data, data, size);

	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	if (m_RecvBuf == NULL) {
		m_RecvBuf = node;
		m_RecvBufPos = 0;
		m_RecvTerm = scirtReceiving;
		m_RecvTimer = 0;

		g_Kernel->OnSetEvent();
	}
	else {
		// メッセージをキューイング
		m_RecvQueue.push_back(node);
	}
}
