//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifCMT.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#include "StdAfx.h"
#include "WinKernel.h"
#include "SifCMT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CKernel *g_Kernel;

CSifCMT::CSifCMT(TCMTCMSTR0 *cmstr, uint16_t str, TRX63NCMT0Reg *cmt, int txIntNo)
{
	m_CMSTR = cmstr;
	m_STR = str;
	m_CMT = cmt;
	m_IntNo = txIntNo;
	m_Term = ttInterrupt;
	m_Timer = -1;
	m_StartReq = 0;
	m_StartRes = 0;
	memset(&m_CMTInst, 0, sizeof(m_CMTInst));
}

CSifCMT::~CSifCMT()
{
}

unsigned char CSifCMT::GetByte(intptr_t index)
{
	unsigned char *reg = (unsigned char *)&m_CMTInst;

	if (index == (intptr_t)&((TRX63NCMT0Reg *)0)->CMCNT) {
		__int64 time, frequency = 10000000/*[100nsec]=1[sec]*/;
		g_Kernel->GetSystemTime(&time);
		time -= m_TimerOrigin;
		m_CMTInst.CMCNT = (((g_Kernel->Frequency / 2) * time) / ((8 << (2 * ((TCMTCMCR *)&m_CMTInst.CMCR)->CKS)) * frequency)) % (m_CMTInst.CMCOR + 1);
		return m_CMTInst.CMCNT;
	}

	return reg[index];
}

void CSifCMT::SetByte(intptr_t index, unsigned char value)
{
	unsigned char *reg = (unsigned char *)&m_CMTInst;

	reg[index] = value;
}

void CSifCMT::Enabled(bool enabled)
{
	if (m_StartReq == m_StartRes) {
		if (enabled)
			g_Kernel->GetSystemTime(&m_TimerOrigin);
		m_StartReq++;
		g_Kernel->OnSetEvent();
	}
}

__int64 CSifCMT::CalcTimer()
{
	__int64 result;
	__int64 frequency = 10000000/*[100nsec]=1[sec]*/;
	TCMTCMCR *CMCR = (TCMTCMCR *)&m_CMTInst.CMCR;

	return ((8 << (2 * CMCR->CKS)) * (m_CMTInst.CMCOR + 1) * frequency) / (g_Kernel->Frequency / 2);
}

__int64 CSifCMT::GetTimer()
{
	return m_Timer;
}

void CSifCMT::Progress(__int64 timer)
{
	if (m_Timer != -1) {
		m_Timer -= timer;
		if (m_Timer < 0) {
			m_Timer = 0;
		}
	}
}

void CSifCMT::CallTimeOut()
{
	if (m_StartReq != m_StartRes) {
		m_StartRes++;
		m_Timer = 0;
		m_Term = ttWait;
	}

	if (m_Timer != 0)
		return;

	switch (m_Term) {
	case ttInterrupt:
		g_Kernel->Interrupt(m_IntNo);
		m_Timer = 0;
		m_Term = ttWait;
		break;
	case ttWait:
		if (((*(uint16_t *)m_CMSTR) & m_STR) != 0)
			m_Timer = CalcTimer();
		else
			m_Timer = -1;
		m_Term = ttInterrupt;
		break;
	}
}
