//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifRIIC.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#include "StdAfx.h"
#include "WinKernel.h"
#include "SifRIIC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CKernel *g_Kernel;

//------------------------------------------------------------------------------
// CSifRIIC
//	概要：
//	引数：
//	pKernel
//	戻り値：
//------------------------------------------------------------------------------
CSifRIIC::CSifRIIC(TRX62NRIIC0Reg *reg, int txIntNo, int rxIntNo, int teIntNo, int devNo)
{
	m_Reg = reg;
	m_TxIntNo = txIntNo;
	m_RxIntNo = rxIntNo;
	m_TeIntNo = teIntNo;
	m_DevNo = devNo;
	m_SendTerm = riicstIdle;
	m_SendTimer = -1;
	//memset(m_SendBuf, 0, sizeof(m_SendBuf));
	m_SendBufPos = 0;

	memset(&m_RegInst, 0, sizeof(m_RegInst));
}

//------------------------------------------------------------------------------
//	概要：
//	引数：
//	戻り値：
//------------------------------------------------------------------------------
CSifRIIC::~CSifRIIC()
{
}

unsigned char CSifRIIC::GetByte(intptr_t index)
{
	unsigned char *reg = (unsigned char *)&m_RegInst;

	if (index == (intptr_t)&((TRX62NRIIC0Reg *)0)->ICCR2) {
		return m_RegInst.ICCR2;
	}
	// 受信バッファが読み込まれたら
	else if (index == (uintptr_t)&((TRX62NRIIC0Reg *)0)->ICDRR) {
		InterlockedAnd8((char *)&m_RegInst.ICSR2, ~0x20);	// RDRF
	}
	else if (index == (intptr_t)&((TRX62NRIIC0Reg *)0)->ICSR2) {
		return m_RegInst.ICSR2;
	}

	return reg[index];
}

void CSifRIIC::SetByte(intptr_t index, unsigned char value)
{
	unsigned char *reg = (unsigned char *)&m_RegInst;

	if (index == (intptr_t)&((TRX62NRIIC0Reg *)0)->ICCR2) {
		// ST, RS, SP
		unsigned char prev = InterlockedExchange8((char *)&reg[index], value & 0x6E);
		if (((prev & 0x0E) ^ (value & 0x0E)) != 0) {
			bool exit = (m_RegInst.ICCR2 & 0x0E) == 0;
			g_Kernel->OnSetEvent();
			while (!exit) {
				Sleep(0);
				exit = (m_RegInst.ICCR2 & 0x0E) == 0;
			}
		}
		return;
	}

	reg[index] = value;

	// 送信バッファに書き込まれたら
	if (index == (intptr_t)&((TRX62NRIIC0Reg *)0)->ICDRT) {
		// TDRE
		if ((InterlockedAnd8((char *)&m_RegInst.ICSR2, ~0x80) & 0x80) != 0) {
			bool exit = (m_RegInst.ICSR2 & 0x80) != 0;
			g_Kernel->OnSetEvent();
			while (!exit) {
				Sleep(0);
				exit = (m_RegInst.ICSR2 & 0x80) != 0;
			}
		}
	}
}

__int64 CSifRIIC::GetTimer()
{
	return m_SendTimer;
}

void CSifRIIC::Progress(__int64 timer)
{
	if (m_SendTimer != -1) {
		m_SendTimer -= timer;
		if (m_SendTimer < 0) {
			m_SendTimer = 0;
		}
	}
}

void CSifRIIC::CallTimeOut()
{
	if (m_SendTerm == riicstIdle) {
		if ((((TRIICICSR2 *)&m_RegInst.ICSR2)->TDRE == 0) && (((TRIICICCR2 *)&m_RegInst.ICCR2)->BBSY != 0)) {
			m_SendTimer = 0;
			m_SendTerm = riicstWrite;
		}
		else if (((TRIICICCR2 *)&m_RegInst.ICCR2)->ST ||
			((TRIICICCR2 *)&m_RegInst.ICCR2)->RS) {
			m_SendTimer = 0;
			m_SendTerm = riicstStart;
		}
		else if (((TRIICICCR2 *)&m_RegInst.ICCR2)->SP) {
			m_SendTimer = 0;
			m_SendTerm = riicstStop;
		}
	}

	if (m_SendTimer != 0)
		return;

	switch (m_SendTerm) {
	case riicstIdle:
		m_SendTimer = -1;
		m_SendTerm = riicstIdle;
		break;
	case riicstStart:
		InterlockedOr8((char *)&m_RegInst.ICCR2, 0x80);		// BBUSY
		InterlockedOr8((char *)&m_RegInst.ICSR2, 0x80);		// TDRE
		InterlockedAnd8((char *)&m_RegInst.ICCR2, ~0x0E);	// ST, RS, SP
		m_SendTimer = -1;
		m_SendTerm = riicstIdle;
		break;
	case riicstWrite:
		m_SendBuf[m_SendBufPos++] = m_RegInst.ICDRT;
		InterlockedAnd8((char *)&m_RegInst.ICSR2, ~0x40);	// TEND
		InterlockedOr8((char *)&m_RegInst.ICSR2, 0x80);		// TDRE
		if ((m_SendBuf[0] & 0x01) == 0) {
			m_SendTerm = riicstSending;
			m_SendTimer = 20 * CalcTimer();
		}
		else {
			InterlockedOr8((char *)&m_RegInst.ICSR2, 0x20);	// RDRF
			m_SendTerm = riicstReceiving;
			m_SendTimer = 0;
		}
		break;
	case riicstSending:
		g_Kernel->Interrupt(m_TeIntNo);
		m_SendTimer = 0;
		m_SendTerm = riicstSendWait;
		break;
	case riicstSendWait:
		InterlockedOr8((char *)&m_RegInst.ICSR2, 0x40);		// TEND

		m_SendTimer = -1;
		m_SendTerm = riicstIdle;
		break;
	case riicstReceiving:
		g_Kernel->Interrupt(m_RxIntNo);
		m_SendTimer = 20 * CalcTimer();
		m_SendTerm = riicstReceiveWait;
		break;
	case riicstReceiveWait:
		InterlockedOr8((char *)&m_RegInst.ICSR2, 0x20);		// RDRF

		m_SendTimer = -1;
		m_SendTerm = riicstIdle;
		break;
	case riicstStop:
		InterlockedAnd8((char *)&m_RegInst.ICSR2, ~0xC0);	// TDRE, TEND
		InterlockedOr8((char *)&m_RegInst.ICSR2, 0x08);		// STOP
		InterlockedAnd8((char *)&m_RegInst.ICCR2, ~0x0E);	// ST, RS, SP
		m_SendTimer = CalcTimer();
		m_SendTerm = riicstFlush;
		break;
	case riicstFlush:
		InterlockedAnd8((char *)&m_RegInst.ICCR2, ~0x80);	// BBUSY
		g_Kernel->Output(m_DevNo, m_SendBuf, m_SendBufPos);
		//memset(m_SendBuf, 0, sizeof(m_SendBuf));
		m_SendBufPos = 0;

		m_SendTimer = -1;
		m_SendTerm = riicstIdle;
		break;
	}
}

__int64 CSifRIIC::CalcTimer()
{
	__int64 frequency = 10000000/*[100nsec]=1[sec]*/;
	__int64 bps;
	TRIICICMR1 *ICMR1 = (TRIICICMR1 *)&m_RegInst.ICMR1;
	TRIICICBRL *ICBRL = (TRIICICBRL *)&m_RegInst.ICBRL;
	TRIICICBRH *ICBRH = (TRIICICBRH *)&m_RegInst.ICBRH;

	// 8bitアドレスフォーマットで20bit？
	bps = (frequency * ((ICBRH->BRH + 1) + (ICBRL->BRL + 1))) / (g_Kernel->Frequency / 1 << ICMR1->CKS);

	return bps;
}
