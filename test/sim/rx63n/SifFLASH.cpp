//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifFLASH.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
//
#include "StdAfx.h"
#include "WinKernel.h"
#include "SifFLASH.h"
#include "RX63NReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CKernel *g_Kernel;

//------------------------------------------------------------------------------
// CSifFLASH
//	概要：
//	引数：
//	pKernel
//	戻り値：
//------------------------------------------------------------------------------
CSifFLASH::CSifFLASH(TRX63NFLASHReg *flash)
{
	m_FLASH = flash;
	m_Term = ftInterrupt;
	m_Timer = -1;
	memset(&m_FLASHInst, 0, sizeof(m_FLASHInst));
}

//------------------------------------------------------------------------------
//	概要：
//	引数：
//	戻り値：
//------------------------------------------------------------------------------
CSifFLASH::~CSifFLASH()
{
}

unsigned char CSifFLASH::GetByte(intptr_t index)
{
	unsigned char *reg = (unsigned char *)&m_FLASHInst;

	if ((index >= (intptr_t)&((TRX63NFLASHReg *)0)->FSTATR0)
		&& (index < (intptr_t)&(&((TRX63NFLASHReg *)0)->FSTATR0)[1])) {
		m_FLASHInst.FSTATR0.FRDY = 1;
	}

	return reg[index];
}

void CSifFLASH::SetByte(intptr_t index, unsigned char value)
{
	unsigned char *reg = (unsigned char *)&m_FLASHInst;

	reg[index] = value;
}

unsigned short CSifFLASH::GetUInt16(uintptr_t index)
{
	unsigned char *reg = (unsigned char *)&m_FLASHInst;

	return reg[index];
}

void CSifFLASH::SetUInt16(uintptr_t index, unsigned short value)
{
	unsigned char *reg = (unsigned char *)&m_FLASHInst;

	reg[index] = value;
}

__int64 CSifFLASH::GetTimer()
{
	return m_Timer;
}

void CSifFLASH::Progress(__int64 timer)
{
	if (m_Timer != -1) {
		m_Timer -= timer;
		if (m_Timer < 0) {
			m_Timer = 0;
		}
	}
}

void CSifFLASH::CallTimeOut()
{
	if (m_Timer != 0)
		return;

	switch (m_Term) {
	case ftInterrupt:
		m_Timer = 0;
		m_Term = ftWait;
		break;
	case ftWait:
		m_Timer = -1;
		m_Term = ftInterrupt;
		break;
	}
}
