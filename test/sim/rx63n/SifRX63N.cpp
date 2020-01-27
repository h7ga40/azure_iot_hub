//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifRX63N.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
#include "StdAfx.h"
#include "WinKernel.h"
#include "SifRX63N.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" void(*const vvector_table[])();
extern CKernel *g_Kernel;

//------------------------------------------------------------------------------
// CSifRX63N
//	概要：
//	引数：
//	pKernel
//	戻り値：
//------------------------------------------------------------------------------
CSifRX63N::CSifRX63N(TRX63NReg *CPUReg, TRX63NFLASHReg *FLASHReg) :
	CMT0((TCMTCMSTR0 *)&m_CMSTR0Reg, 0x0001, (TRX63NCMT0Reg *)&CPUReg->CMT0, INT_NO_CMI0),
	CMT1((TCMTCMSTR0 *)&m_CMSTR0Reg, 0x0002, (TRX63NCMT0Reg *)&CPUReg->CMT1, INT_NO_CMI1),
	CMT2((TCMTCMSTR0 *)&m_CMSTR1Reg, 0x0001, (TRX63NCMT0Reg *)&CPUReg->CMT2, INT_NO_CMI2),
	CMT3((TCMTCMSTR0 *)&m_CMSTR1Reg, 0x0002, (TRX63NCMT0Reg *)&CPUReg->CMT3, INT_NO_CMI3),
	SCI0((TRX63NSCI0Reg *)&CPUReg->SCI0, INT_NO_TXI0, INT_NO_RXI0, INT_NO_TEI0, 1),
	SCI1((TRX63NSCI0Reg *)&CPUReg->SCI1, INT_NO_TXI1, INT_NO_RXI1, INT_NO_TEI1, 2),
	SCI2((TRX63NSCI0Reg *)&CPUReg->SCI2, INT_NO_TXI2, INT_NO_RXI2, INT_NO_TEI2, 3),
	SCI3((TRX63NSCI0Reg *)&CPUReg->SCI3, INT_NO_TXI3, INT_NO_RXI3, INT_NO_TEI3, 4),
	SCI4((TRX63NSCI0Reg *)&CPUReg->SCI4, INT_NO_TXI4, INT_NO_RXI4, INT_NO_TEI4, 5),
	SCI5((TRX63NSCI0Reg *)&CPUReg->SCI5, INT_NO_TXI5, INT_NO_RXI5, INT_NO_TEI5, 6),
	SCI6((TRX63NSCI0Reg *)&CPUReg->SCI6, INT_NO_TXI6, INT_NO_RXI6, INT_NO_TEI6, 7),
	SCI7((TRX63NSCI0Reg *)&CPUReg->SCI7, INT_NO_TXI7, INT_NO_RXI7, INT_NO_TEI7, 8),
	SCI8((TRX63NSCI0Reg *)&CPUReg->SCI8, INT_NO_TXI8, INT_NO_RXI8, INT_NO_TEI8, 9),
	SCI9((TRX63NSCI0Reg *)&CPUReg->SCI9, INT_NO_TXI9, INT_NO_RXI9, INT_NO_TEI9, 10),
	SCI10((TRX63NSCI0Reg *)&CPUReg->SCI10, INT_NO_TXI10, INT_NO_RXI10, INT_NO_TEI10, 11),
	SCI11((TRX63NSCI0Reg *)&CPUReg->SCI11, INT_NO_TXI11, INT_NO_RXI11, INT_NO_TEI11, 12),
	SCI12((TRX63NSCI0Reg *)&CPUReg->SCI12, INT_NO_TXI12, INT_NO_RXI12, INT_NO_TEI12, 13),
	RSPI0((TRX63NRSPI0Reg *)&CPUReg->RSPI0, INT_NO_SPRI0, INT_NO_SPTI0, INT_NO_SPII0, 20),
	RSPI1((TRX63NRSPI0Reg *)&CPUReg->RSPI1, INT_NO_SPRI1, INT_NO_SPTI1, INT_NO_SPII1, 21),
	RIIC0((TRX63NRIIC0Reg *)&CPUReg->RIIC0, INT_NO_ICTXI0, INT_NO_ICRXI0, INT_NO_ICTEI0, 22),
	RIIC1((TRX63NRIIC0Reg *)&CPUReg->RIIC1, INT_NO_ICTXI1, INT_NO_ICRXI1, INT_NO_ICTEI1, 23),
	FLASH(m_FLASHReg),
	S12AD(&m_S12ADReg),
	SDCard(24)
{
	m_CPUReg = CPUReg;
	m_FLASHReg = FLASHReg;
	m_Lock = 0;
	RSPI0.Port = &SDCard;
	SDCard.Port = &RSPI0;
	memset(&m_SYSTEMReg1, 0, sizeof(m_SYSTEMReg1));
	memset(&m_SYSTEMReg2, 0, sizeof(m_SYSTEMReg2));
	memset(&m_BSCReg1, 0, sizeof(m_BSCReg1));
	memset(&m_BSCReg2, 0, sizeof(m_BSCReg2));
	memset(&m_CMSTR0Reg, 0, sizeof(m_CMSTR0Reg));
	memset(&m_CMSTR1Reg, 0, sizeof(m_CMSTR1Reg));
	memset(&m_ICUReg, 0, sizeof(m_ICUReg));
	memset(&m_RTCReg, 0, sizeof(m_RTCReg));
	memset(&m_PORT0Reg, 0, sizeof(m_PORT0Reg));
	memset(&m_PORT1Reg, 0, sizeof(m_PORT1Reg));
	memset(&m_PORT2Reg, 0, sizeof(m_PORT2Reg));
	memset(&m_PORT3Reg, 0, sizeof(m_PORT3Reg));
	memset(&m_PORT4Reg, 0, sizeof(m_PORT4Reg));
	memset(&m_PORT5Reg, 0, sizeof(m_PORT5Reg));
	memset(&m_PORT6Reg, 0, sizeof(m_PORT6Reg));
	memset(&m_PORT7Reg, 0, sizeof(m_PORT7Reg));
	memset(&m_PORT8Reg, 0, sizeof(m_PORT8Reg));
	memset(&m_PORT9Reg, 0, sizeof(m_PORT9Reg));
	memset(&m_PORTAReg, 0, sizeof(m_PORTAReg));
	memset(&m_PORTBReg, 0, sizeof(m_PORTBReg));
	memset(&m_PORTCReg, 0, sizeof(m_PORTCReg));
	memset(&m_PORTDReg, 0, sizeof(m_PORTDReg));
	memset(&m_PORTEReg, 0, sizeof(m_PORTEReg));
	memset(&m_PORTFReg, 0, sizeof(m_PORTFReg));
	memset(&m_PORTGReg, 0, sizeof(m_PORTGReg));
	memset(&m_MPCReg, 0, sizeof(m_MPCReg));
	m_InstRomPEAddr = (void *)0x00F80000;
	m_InstRomSize = 512 * 1024;
	Syncs[0] = &CMT0;
	Syncs[1] = &CMT1;
	Syncs[2] = &CMT2;
	Syncs[3] = &CMT3;
	Syncs[4] = &RIIC0;
	Syncs[5] = &RIIC1;
	Syncs[6] = &RSPI0;
	Syncs[7] = &RSPI1;
	Syncs[8] = &SCI0;
	Syncs[9] = &SCI1;
	Syncs[10] = &SCI2;
	Syncs[11] = &SCI3;
	Syncs[12] = &SCI4;
	Syncs[13] = &SCI5;
	Syncs[14] = &SCI6;
	Syncs[15] = &SCI7;
	Syncs[16] = &SCI8;
	Syncs[17] = &SCI9;
	Syncs[18] = &SCI10;
	Syncs[19] = &SCI11;
	Syncs[20] = &SCI12;
	Syncs[21] = &FLASH;
	Syncs[22] = &S12AD;
	Syncs[23] = &SDCard;
	memcpy(m_IntHandler, vvector_table, sizeof(m_IntHandler));
}

//------------------------------------------------------------------------------
//	概要：
//	引数：
//	戻り値：
//------------------------------------------------------------------------------
CSifRX63N::~CSifRX63N()
{
}

void CSifRX63N::OnStart()
{
	SDCard.OnStart();
}

void CSifRX63N::OnTerminate()
{
	SDCard.OnTerminate();
}

unsigned char CSifRX63N::GetByte2(intptr_t index)
{
	if ((index >= (intptr_t)&((TRX63NReg *)0)->ICU)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->ICU)[1])) {
		unsigned char *reg = (unsigned char *)&m_ICUReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->ICU;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SYSTEM)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SYSTEM.PRCR)[1])) {
		unsigned char *reg = (unsigned char *)&m_SYSTEMReg1;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->SYSTEM;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SYSTEM.DPSBYCR)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SYSTEM.SCK2)[1])) {
		unsigned char *reg = (unsigned char *)&m_SYSTEMReg2;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->SYSTEM.DPSBYCR;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->BSC)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->BSC.BERSR2)[1])) {
		unsigned char *reg = (unsigned char *)&m_BSCReg1;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->BSC;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->BSC.CS0MOD)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->BSC.SDSR)[1])) {
		unsigned char *reg = (unsigned char *)&m_BSCReg2;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->BSC.CS0MOD;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT0)[1])) {
		return CMT0.GetByte(index - (intptr_t)&((TRX63NReg *)0)->CMT0);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT1)[1])) {
		return CMT1.GetByte(index - (intptr_t)&((TRX63NReg *)0)->CMT1);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT2)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT2)[1])) {
		return CMT2.GetByte(index - (intptr_t)&((TRX63NReg *)0)->CMT2);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT3)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT3)[1])) {
		return CMT3.GetByte(index - (intptr_t)&((TRX63NReg *)0)->CMT3);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RIIC0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RIIC0)[1])) {
		return RIIC0.GetByte(index - (intptr_t)&((TRX63NReg *)0)->RIIC0);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RIIC1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RIIC1)[1])) {
		return RIIC1.GetByte(index - (intptr_t)&((TRX63NReg *)0)->RIIC1);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RSPI0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RSPI0)[1])) {
		return RSPI0.GetByte(index - (intptr_t)&((TRX63NReg *)0)->RSPI0);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RSPI1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RSPI1)[1])) {
		return RSPI1.GetByte(index - (intptr_t)&((TRX63NReg *)0)->RSPI1);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI0)[1])) {
		return SCI0.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI0);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI1)[1])) {
		return SCI1.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI1);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI2)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI2)[1])) {
		return SCI2.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI2);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI3)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI3)[1])) {
		return SCI3.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI3);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI4)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI4)[1])) {
		return SCI4.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI4);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI5)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI5)[1])) {
		return SCI5.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI5);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI6)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI6)[1])) {
		return SCI6.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI6);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI7)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI7)[1])) {
		return SCI7.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI7);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI8)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI8)[1])) {
		return SCI8.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI8);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI9)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI9)[1])) {
		return SCI9.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI9);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI10)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI10)[1])) {
		return SCI10.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI10);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI11)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI11)[1])) {
		return SCI11.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI11);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI12)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI12)[1])) {
		return SCI12.GetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI12);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RTC)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RTC)[1])) {
		TRX63NRTCReg *RCPort = &m_RTCReg;
		unsigned char *reg = (unsigned char *)&m_RTCReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->RTC;
		if ((idx >= (intptr_t)&((TRX63NRTCReg *)0)->R64CNT)
			&& (idx < (intptr_t)&(&((TRX63NRTCReg *)0)->RYRCNT)[1])) {
			__int64 now, freq = 10000000/*[100nsec]=1[sec]*/;
			static __int64 prev = 0;

			g_Kernel->GetSystemTime(&now);

			bool update = (now - prev) > (freq / 1000);
			prev = now;

			if (update) {
				if ((idx >= (intptr_t)&((TRX63NRTCReg *)0)->R64CNT)
					&& (idx < (intptr_t)&(&((TRX63NRTCReg *)0)->R64CNT)[1])) {
					__int64 subc = now % freq;
					RCPort->R64CNT = (64 * subc) / freq;
				}
				else {
					tm tm;
					time_t sec = now / freq;
					gmtime_s(&tm, &sec);
					tm.tm_year -= 100;
					if (tm.tm_year < 0)
						tm.tm_year = 0;
					tm.tm_mon += 1;

					RCPort->RSECCNT = ((tm.tm_sec / 10) << 4) | (tm.tm_sec % 10);
					RCPort->RMINCNT = ((tm.tm_min / 10) << 4) | (tm.tm_min % 10);
					RCPort->RHRCNT = ((tm.tm_hour / 10) << 4) | (tm.tm_hour % 10);
					RCPort->RWKCNT = tm.tm_wday;
					RCPort->RDAYCNT = ((tm.tm_mday / 10) << 4) | (tm.tm_mday % 10);
					RCPort->RMONCNT = ((tm.tm_mon / 10) << 4) | (tm.tm_mon % 10);
					RCPort->RYRCNT = ((tm.tm_year / 10) << 4) | (tm.tm_year % 10);
				}
			}
		}
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT0)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT0Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT0;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT1)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT1Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT1;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT2)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT2)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT2Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT2;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT3)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT3)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT3Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT3;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT4)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT4)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT4Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT4;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT5)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT5)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT5Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT5;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT6)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT6)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT6Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT6;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT7)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT7)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT7Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT7;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT8)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT8)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT8Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT8;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT9)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT9)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT9Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT9;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTA)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTA)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTAReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTA;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTB)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTB)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTBReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTB;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTC)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTC)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTCReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTC;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTD)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTD)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTDReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTD;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTE)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTE)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTEReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTE;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTF)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTF)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTFReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTF;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTG)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTG)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTGReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTG;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->MPC)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->MPC)[1])) {
		unsigned char *reg = (unsigned char *)&m_MPCReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->MPC;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT.CMSTR0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT.CMSTR0)[1])) {
		unsigned char *reg = (unsigned char *)&m_CMSTR0Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->CMT.CMSTR0;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT.CMSTR1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT.CMSTR1)[1])) {
		unsigned char *reg = (unsigned char *)&m_CMSTR1Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->CMT.CMSTR1;
		return reg[idx];
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->S12AD)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->S12AD)[1])) {
		unsigned char *reg = (unsigned char *)&m_S12ADReg;
		return S12AD.GetByte(index - (intptr_t)&((TRX63NReg *)0)->S12AD);
	}

	throw std::exception();
}

void CSifRX63N::SetByte2(intptr_t index, unsigned char value)
{
	if ((index >= (intptr_t)&((TRX63NReg *)0)->ICU)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->ICU)[1])) {
		TRX63NICUReg *ICUPort = &m_ICUReg;
		unsigned char *reg = (unsigned char *)ICUPort;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->ICU;
		reg[idx] = value;
		if ((idx >= (intptr_t)&((TRX63NICUReg *)0)->IER)
			&& (idx < (intptr_t)&(&((TRX63NICUReg *)0)->IER)[1])) {
			unsigned char ier = reg[idx];
			int intNo = 8 * (idx - (intptr_t)&((TRX63NICUReg *)0)->IER);

			if ((value & ~ier) != 0) {
				for (int bit = 0x01; bit < 0x100; bit <<= 1, intNo++) {
					if ((value & bit) != 0 && (ier & bit) == 0 && ((TICUIRReg *)&ICUPort->IR[intNo])->IR != 0) {
						g_Kernel->Interrupt(intNo);
					}
				}
			}
		}
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SYSTEM)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SYSTEM.PRCR)[1])) {
		unsigned char *reg = (unsigned char *)&m_SYSTEMReg1;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->SYSTEM;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SYSTEM.DPSBYCR)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SYSTEM.SCK2)[1])) {
		unsigned char *reg = (unsigned char *)&m_SYSTEMReg2;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->SYSTEM.DPSBYCR;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->BSC)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->BSC.BERSR2)[1])) {
		unsigned char *reg = (unsigned char *)&m_BSCReg1;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->BSC;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->BSC.CS0MOD)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->BSC.SDSR)[1])) {
		unsigned char *reg = (unsigned char *)&m_BSCReg2;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->BSC.CS0MOD;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT0)[1])) {
		CMT0.SetByte(index - (intptr_t)&((TRX63NReg *)0)->CMT0, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT1)[1])) {
		CMT1.SetByte(index - (intptr_t)&((TRX63NReg *)0)->CMT1, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT2)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT2)[1])) {
		CMT2.SetByte(index - (intptr_t)&((TRX63NReg *)0)->CMT2, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT3)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT3)[1])) {
		CMT3.SetByte(index - (intptr_t)&((TRX63NReg *)0)->CMT3, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RIIC0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RIIC0)[1])) {
		RIIC0.SetByte(index - (intptr_t)&((TRX63NReg *)0)->RIIC0, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RIIC1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RIIC1)[1])) {
		RIIC1.SetByte(index - (intptr_t)&((TRX63NReg *)0)->RIIC1, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RSPI0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RSPI0)[1])) {
		RSPI0.SetByte(index - (intptr_t)&((TRX63NReg *)0)->RSPI0, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RSPI1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RSPI1)[1])) {
		RSPI1.SetByte(index - (intptr_t)&((TRX63NReg *)0)->RSPI1, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI0)[1])) {
		SCI0.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI0, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI1)[1])) {
		SCI1.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI1, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI2)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI2)[1])) {
		SCI2.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI2, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI3)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI3)[1])) {
		SCI3.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI3, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI4)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI4)[1])) {
		SCI4.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI4, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI5)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI5)[1])) {
		SCI5.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI5, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI6)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI6)[1])) {
		SCI6.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI6, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI7)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI7)[1])) {
		SCI7.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI7, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI8)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI8)[1])) {
		SCI8.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI8, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI9)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI9)[1])) {
		SCI9.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI9, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI10)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI10)[1])) {
		SCI10.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI10, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI11)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI11)[1])) {
		SCI11.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI11, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->SCI12)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->SCI12)[1])) {
		SCI12.SetByte(index - (intptr_t)&((TRX63NReg *)0)->SCI12, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RTC)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RTC)[1])) {
		TRX63NRTCReg *RCPort = &m_RTCReg;
		unsigned char *reg = (unsigned char *)&m_RTCReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->RTC;
		if ((idx >= (intptr_t)&((TRX63NRTCReg *)0)->R64CNT)
			&& (idx < (intptr_t)&(&((TRX63NRTCReg *)0)->RYRCNT)[1])) {
		}
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT0)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT0Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT0;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT1)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT1Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT1;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT2)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT2)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT2Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT2;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT3)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT3)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT3Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT3;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT4)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT4)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT4Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT4;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT5)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT5)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT5Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT5;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT6)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT6)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT6Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT6;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT7)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT7)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT7Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT7;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT8)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT8)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT8Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT8;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORT9)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORT9)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORT9Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORT9;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTA)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTA)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTAReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTA;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTB)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTB)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTBReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTB;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTC)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTC)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTCReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTC;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTD)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTD)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTDReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTD;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTE)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTE)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTEReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTE;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTF)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTF)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTFReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTF;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->PORTG)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->PORTG)[1])) {
		unsigned char *reg = (unsigned char *)&m_PORTGReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->PORTG;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->MPC)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->MPC)[1])) {
		unsigned char *reg = (unsigned char *)&m_MPCReg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->MPC;
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT.CMSTR0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT.CMSTR0)[1])) {
		unsigned char *reg = (unsigned char *)&m_CMSTR0Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->CMT.CMSTR0;
		if (((TCMTCMSTR0 *)&m_CMSTR0Reg)->STR0 != ((TCMTCMSTR0 *)&((TRX63NCMTReg *)&value)->CMSTR0)->STR0)
			CMT0.Enabled(((TCMTCMSTR0 *)&((TRX63NCMTReg *)&value)->CMSTR0)->STR0 != 0);
		if (((TCMTCMSTR0 *)&m_CMSTR0Reg)->STR1 != ((TCMTCMSTR0 *)&((TRX63NCMTReg *)&value)->CMSTR0)->STR1)
			CMT1.Enabled(((TCMTCMSTR0 *)&((TRX63NCMTReg *)&value)->CMSTR0)->STR1 != 0);
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->CMT.CMSTR1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->CMT.CMSTR1)[1])) {
		unsigned char *reg = (unsigned char *)&m_CMSTR1Reg;
		intptr_t idx = index - (intptr_t)&((TRX63NReg *)0)->CMT.CMSTR1;
		if (((TCMTCMSTR0 *)&m_CMSTR1Reg)->STR0 != ((TCMTCMSTR0 *)&((TRX63NCMTReg *)&value)->CMSTR1)->STR0)
			CMT2.Enabled(((TCMTCMSTR0 *)&((TRX63NCMTReg *)&value)->CMSTR1)->STR0 != 0);
		if (((TCMTCMSTR0 *)&m_CMSTR1Reg)->STR1 != ((TCMTCMSTR0 *)&((TRX63NCMTReg *)&value)->CMSTR1)->STR1)
			CMT3.Enabled(((TCMTCMSTR0 *)&((TRX63NCMTReg *)&value)->CMSTR1)->STR1 != 0);
		reg[idx] = value;
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->S12AD)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->S12AD)[1])) {
		unsigned char *reg = (unsigned char *)&m_S12ADReg;
		S12AD.SetByte(index - (intptr_t)&((TRX63NReg *)0)->S12AD, value);
	}
	else {
		throw std::exception();
	}
}

unsigned char CSifRX63N::GetByte(uintptr_t addr)
{
	intptr_t index;

	index = addr - (uintptr_t)m_CPUReg;
	if ((index >= 0) && (index < sizeof(TRX63NReg))) {
		return GetByte2(index);
	}

	index = addr - (uintptr_t)m_FLASHReg;
	if ((index >= 0) && (index < sizeof(TRX63NFLASHReg))) {
		return FLASH.GetByte(index);
	}

	index = addr - (uintptr_t)m_InstRomPEAddr;
	if ((index >= 0) && (index < m_InstRomSize)) {
		return 0xFFFF;
	}

	throw std::exception();
}

void CSifRX63N::SetByte(uintptr_t addr, unsigned char value)
{
	intptr_t index;

	index = addr - (uintptr_t)m_CPUReg;
	if ((index >= 0) && (index < sizeof(TRX63NReg))) {
		SetByte2(index, value);
		return;
	}

	index = addr - (uintptr_t)m_FLASHReg;
	if ((index >= 0) && (index < sizeof(TRX63NFLASHReg))) {
		FLASH.SetByte(index, value);
		return;
	}

	index = addr - (uintptr_t)m_InstRomPEAddr;
	if ((index >= 0) && (index < m_InstRomSize)) {
		return;
	}

	throw std::exception();
}

unsigned short CSifRX63N::GetUInt16(uintptr_t addr)
{
	unsigned short result;
	intptr_t index;

	index = addr - (uintptr_t)m_CPUReg;
	if ((index >= 0) && ((index + 1) < sizeof(TRX63NReg))) {
		((unsigned char *)&result)[0] = GetByte2(index);
		((unsigned char *)&result)[1] = GetByte2(index + 1);
		return result;
	}

	index = addr - (uintptr_t)m_FLASHReg;
	if ((index >= 0) && ((index + 1) < sizeof(TRX63NFLASHReg))) {
		return FLASH.GetUInt16(index);
	}

	index = addr - (uintptr_t)m_InstRomPEAddr;
	if ((index >= 0) && (index < m_InstRomSize)) {
		return 0xFFFF;
	}

	throw std::exception();
}

void CSifRX63N::SetUInt16(uintptr_t addr, unsigned short value)
{
	intptr_t index;

	index = addr - (uintptr_t)m_CPUReg;
	if ((index >= 0) && ((index + 1) < sizeof(TRX63NReg))) {
		SetByte2(index, ((unsigned char *)&value)[0]);
		SetByte2(index + 1, ((unsigned char *)&value)[1]);
		return;
	}

	index = addr - (uintptr_t)m_FLASHReg;
	if ((index >= 0) && ((index + 1) < sizeof(TRX63NFLASHReg))) {
		FLASH.SetUInt16(index, value);
		return;
	}

	index = addr - (uintptr_t)m_InstRomPEAddr;
	if ((index >= 0) && (index < m_InstRomSize)) {
		return;
	}

	throw std::exception();
}

unsigned int CSifRX63N::GetUInt32(uintptr_t addr)
{
	unsigned int value;
	unsigned char *offset = (unsigned char *)m_CPUReg;
	intptr_t index;

	index = addr - (uintptr_t)offset;

	if ((index < 0) || ((index + 3) >= sizeof(TRX63NReg))) {
		throw std::exception();
	}

	if ((index >= (intptr_t)&((TRX63NReg *)0)->RSPI0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RSPI0)[1])) {
		return RSPI0.GetInt32(index - (intptr_t)&((TRX63NReg *)0)->RSPI0);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RSPI1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RSPI1)[1])) {
		return RSPI1.GetInt32(index - (intptr_t)&((TRX63NReg *)0)->RSPI1);
	}
	else {
		((unsigned char *)&value)[0] = GetByte2(index);
		((unsigned char *)&value)[1] = GetByte2(index + 1);
		((unsigned char *)&value)[2] = GetByte2(index + 2);
		((unsigned char *)&value)[3] = GetByte2(index + 3);
	}

	return value;
}

void CSifRX63N::SetUInt32(uintptr_t addr, unsigned int value)
{
	unsigned char *offset = (unsigned char *)m_CPUReg;
	intptr_t index;

	index = addr - (uintptr_t)offset;

	if ((index < 0) || ((index + 3) >= sizeof(TRX63NReg))) {
		throw std::exception();
	}

	// 受信バッファを読み込まれたら
	if ((index >= (intptr_t)&((TRX63NReg *)0)->RSPI0)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RSPI0)[1])) {
		RSPI0.SetInt32(index - (intptr_t)&((TRX63NReg *)0)->RSPI0, value);
	}
	else if ((index >= (intptr_t)&((TRX63NReg *)0)->RSPI1)
		&& (index < (intptr_t)&(&((TRX63NReg *)0)->RSPI1)[1])) {
		RSPI1.SetInt32(index - (intptr_t)&((TRX63NReg *)0)->RSPI1, value);
	}
	else {
		SetByte2(index, ((unsigned char *)&value)[0]);
		SetByte2(index + 1, ((unsigned char *)&value)[1]);
		SetByte2(index + 2, ((unsigned char *)&value)[2]);
		SetByte2(index + 3, ((unsigned char *)&value)[3]);
	}
}

__int64 CSifRX63N::GetTimer()
{
	__int64 timer = -1;

	for (int i = 0; i < sizeof(Syncs) / sizeof(Syncs[0]); i++) {
		__int64 timer2 = Syncs[i]->GetTimer();

		if ((timer == -1) || ((timer2 != -1) && (timer > timer2)))
			timer = timer2;
	}

	return timer;
}

void CSifRX63N::Progress(__int64 Timer)
{
	for (int i = 0; i < sizeof(Syncs) / sizeof(Syncs[0]); i++) {
		Syncs[i]->Progress(Timer);
	}
}

void CSifRX63N::CallTimeOut()
{
	for (int i = 0; i < sizeof(Syncs) / sizeof(Syncs[0]); i++) {
		Syncs[i]->CallTimeOut();
	}
}

bool CSifRX63N::InterruptUnmasked(int intNo)
{
	TRX63NICUReg *ICU = &m_ICUReg;

	return ((ICU->IER[intNo / 8] & (1 << (intNo % 8))) != 0);
}

bool CSifRX63N::InterruptRaised(int intNo)
{
	TRX63NICUReg *ICU = &m_ICUReg;
	TICUIRReg *IR = (TICUIRReg *)ICU->IR;

	return (IR[intNo].IR != 0);
}

void CSifRX63N::ClearInterrupt(int intNo)
{
	TRX63NICUReg *ICU = &m_ICUReg;
	TICUIRReg *IR = (TICUIRReg *)ICU->IR;

	SpinLock();
	__try {
		IR[intNo].IR = 0;
	}
	__finally {
		SpinUnlock();
	}
}

void CSifRX63N::SetInterrupt(int intNo)
{
	TRX63NICUReg *ICU = &m_ICUReg;
	TICUIRReg *IR = (TICUIRReg *)ICU->IR;

	SpinLock();
	__try {
		IR[intNo].IR = 1;
	}
	__finally {
		SpinUnlock();
	}
}

const int IPR_Table[INTERRUPT_COUNT] = {
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	0x00,   -1,   -1,   -1,   -1, 0x01,   -1, 0x02,
	  -1,   -1,   -1, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x14, 0x14, 0x14,
	0x15, 0x15, 0x15, 0x15,   -1,   -1,   -1,   -1,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1, 0x3A, 0x3B, 0x3C,   -1,   -1,   -1,
	0x40,   -1, 0x44, 0x45,   -1,   -1, 0x48,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1, 0x51, 0x51, 0x51, 0x51, 0x52, 0x52,
	0x52, 0x53, 0x53, 0x54, 0x54, 0x55, 0x55, 0x56,
	0x56, 0x57, 0x57, 0x57, 0x57, 0x58, 0x59, 0x59,
	0x59, 0x59, 0x5A, 0x5B, 0x5B, 0x5B, 0x5C, 0x5C,
	0x5C, 0x5C, 0x5D, 0x5D, 0x5D, 0x5E, 0x5E, 0x5F,
	0x5F, 0x60, 0x60, 0x61, 0x61, 0x62, 0x62, 0x62,
	0x62, 0x63, 0x64, 0x64, 0x64, 0x64, 0x65, 0x66,
	0x66, 0x66, 0x67, 0x67, 0x67, 0x67, 0x68, 0x68,
	0x68, 0x69, 0x69, 0x69, 0x6A, 0x6A, 0x6A, 0x6B,
	0x6B, 0x6B,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1, 0x70, 0x71,
	0x72, 0x73, 0x74, 0x75,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1, 0x80, 0x80,
	0x80, 0x80, 0x81, 0x81, 0x81, 0x81, 0x82, 0x82,
	0x82, 0x82, 0x83, 0x83, 0x83, 0x83,   -1,   -1,
	  -1,   -1, 0x85, 0x85, 0x85, 0x85, 0x86, 0x86,
	0x86, 0x86,   -1,   -1,   -1,   -1, 0x88, 0x89,
	0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91,
};

unsigned int CSifRX63N::InterruptPriority(int intNo)
{
	TRX63NICUReg *ICU = &m_ICUReg;
	TICUIPRReg *IPR = (TICUIPRReg *)ICU->IPR;

	if (intNo >= (sizeof(IPR_Table) / sizeof(*IPR_Table)))
		return 0;

	int IprNo = IPR_Table[intNo];
	if (IprNo < 0)
		return 0;

	return IPR[IprNo].IPR;
}

bool CSifRX63N::DefineInterruptHandler(int intNo, void(*handler)())
{
	if (/*(intNo < 0) || */(intNo >= sizeof(m_IntHandler) / sizeof(m_IntHandler[0])))
		return false;

	m_IntHandler[intNo] = handler;

	return true;
}

bool CSifRX63N::ChangeInterruptControlRegister(int intNo, uint8_t icrcmd)
{
	if (/*(intNo < 0) || */(intNo >= sizeof(m_IntHandler) / sizeof(m_IntHandler[0])))
		return false;

	TRX63NICUReg *ICU = &m_ICUReg;
	TICUIRReg *IR = (TICUIRReg *)ICU->IR;
	TICUIPRReg *IPR = (TICUIPRReg *)ICU->IPR;
	int IprNo = IPR_Table[intNo];

	if ((icrcmd & 0x80) == 0)
		IR[intNo].IR = 0;
	else
		IR[intNo].IR = 1;

	if ((icrcmd & 0x40) == 0)
		ICU->IER[intNo / 8] &= ~(1 << (intNo % 8));
	else
		ICU->IER[intNo / 8] |= (1 << (intNo % 8));

	if (IprNo < 0)
		return false;

	IPR[IprNo].IPR = icrcmd & 0x0F;

	return true;
}

void CSifRX63N::InvokeInterrupt(int intNo)
{
	if (/*(intNo < 0) || */(intNo >= sizeof(m_IntHandler) / sizeof(m_IntHandler[0])))
		return;

	if (m_IntHandler[intNo] != NULL)
		m_IntHandler[intNo]();
}
