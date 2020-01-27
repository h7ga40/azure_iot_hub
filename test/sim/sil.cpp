//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: sil.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
#include "StdAfx.h"
#include "sil.h"
#include "WinKernel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CKernel *g_Kernel;

//---------------------------------------------------------------------------
//  エンディアンの反転
//---------------------------------------------------------------------------
#ifndef SIL_REV_ENDIAN_H
#define	SIL_REV_ENDIAN_H(data) \
	((uint16_t)((((uint16_t)(data) & 0xff) << 8) | (((uint16_t)(data) >> 8) & 0xff)))
#endif /* SIL_REV_ENDIAN_H */

#ifndef SIL_REV_ENDIAN_W
#define	SIL_REV_ENDIAN_W(data) \
	((uint32_t)((((uint32_t)(data) & 0xff) << 24) | (((uint32_t)(data) & 0xff00) << 8) \
		| (((uint32_t)(data)>> 8) & 0xff00) | (((uint32_t)(data) >> 24) & 0xff)))
#endif /* SIL_REV_ENDIAN_H */

//---------------------------------------------------------------------------
//  8ビット単位の読出し／書込み
//---------------------------------------------------------------------------
uint8_t sil_reb_mem(const volatile uint8_t *mem)
{
	return g_Kernel->GetByte((uintptr_t)mem);
}

void sil_wrb_mem(volatile uint8_t *mem, uint8_t data)
{
	g_Kernel->SetByte((uintptr_t)mem, data);
}

//---------------------------------------------------------------------------
//  16ビット単位の読出し／書込み
//---------------------------------------------------------------------------
uint16_t sil_reh_mem(const volatile uint16_t *mem)
{
	return g_Kernel->GetUInt16((uintptr_t)mem);
}

void sil_wrh_mem(volatile uint16_t *mem, uint16_t data)
{
	g_Kernel->SetUInt16((uintptr_t)mem, data);
}

uint16_t sil_reh_bem(const volatile uint16_t *mem)
{
	uint16_t	data;

	data = g_Kernel->GetUInt16((uintptr_t)mem);

	return(SIL_REV_ENDIAN_H(data));
}

void sil_wrh_bem(volatile uint16_t *mem, uint16_t data)
{
	sil_wrh_mem(mem, SIL_REV_ENDIAN_H(data));
}

//---------------------------------------------------------------------------
//  32ビット単位の読出し／書込み
//---------------------------------------------------------------------------
uint32_t sil_rew_mem(const volatile uint32_t *mem)
{
	return g_Kernel->GetUInt32((uintptr_t)mem);
}

void sil_wrw_mem(volatile uint32_t *mem, uint32_t data)
{
	g_Kernel->SetUInt32((uintptr_t)mem, data);
}

uint32_t sil_rew_bem(const volatile uint32_t *mem)
{
	uint32_t data;

	data = g_Kernel->GetUInt32((uintptr_t)mem);

	return(SIL_REV_ENDIAN_W(data));
}

void sil_wrw_bem(volatile uint32_t *mem, uint32_t data)
{
	g_Kernel->SetUInt32((uintptr_t)mem, SIL_REV_ENDIAN_W(data));
}

