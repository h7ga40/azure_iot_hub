//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifS12AD.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#include "StdAfx.h"
#include "WinKernel.h"
#include "SifS12AD.h"
#include "RX62NReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CKernel *g_Kernel;

//------------------------------------------------------------------------------
// CSifS12AD
//	概要：
//	引数：
//	pKernel
//	戻り値：
//------------------------------------------------------------------------------
CSifS12AD::CSifS12AD(TRX62NS12ADReg *flash)
{
	m_S12AD = flash;
	m_Term = s12adtInterrupt;
	m_Timer = -1;
	memset(&m_S12ADInst, 0, sizeof(m_S12ADInst));
}

//------------------------------------------------------------------------------
//	概要：
//	引数：
//	戻り値：
//------------------------------------------------------------------------------
CSifS12AD::~CSifS12AD()
{
}

unsigned char CSifS12AD::GetByte(intptr_t index)
{
	unsigned char *reg = (unsigned char *)&m_S12ADInst;

	return reg[index];
}

void CSifS12AD::SetByte(intptr_t index, unsigned char value)
{
	unsigned char *reg = (unsigned char *)&m_S12ADInst;

	reg[index] = value;
}

unsigned short CSifS12AD::GetUInt16(uintptr_t index)
{
	unsigned char *reg = (unsigned char *)&m_S12ADInst;

	return reg[index];
}

void CSifS12AD::SetUInt16(uintptr_t index, unsigned short value)
{
	unsigned char *reg = (unsigned char *)&m_S12ADInst;

	reg[index] = value;
}

__int64 CSifS12AD::GetTimer()
{
	return m_Timer;
}

void CSifS12AD::Progress(__int64 timer)
{
	if (m_Timer != -1) {
		m_Timer -= timer;
		if (m_Timer < 0) {
			m_Timer = 0;
		}
	}
}

void CSifS12AD::CallTimeOut()
{
	if (m_Timer != 0)
		return;

	switch (m_Term) {
	case s12adtInterrupt:
		m_Timer = 0;
		m_Term = s12adtWait;
		break;
	case s12adtWait:
		m_Timer = -1;
		m_Term = s12adtInterrupt;
		break;
	}
}
