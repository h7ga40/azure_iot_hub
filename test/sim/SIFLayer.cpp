//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SIFLayer.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
#include "StdAfx.h"
#include "SIFLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSystemIF::CSystemIF(intptr_t BaseAddr, int Size)
{
	m_BaseAddr = BaseAddr;
	m_Size = Size;
}

CSystemIF::~CSystemIF()
{
}

unsigned char CSystemIF::GetByte(uintptr_t Addr)
{
	unsigned char Result;
	intptr_t Index;
	bool DoEvent;

	Index = Addr - m_BaseAddr;

	if((Index < 0) && (Index >= m_Size)){
		throw std::exception();
	}

	DoEvent = HasReadEvent(Index);

	if(DoEvent)
		ReadEvent(Index);

	GetByte2(Index, ((unsigned char *)&Result)[0]);

	return Result;
}

void CSystemIF::SetByte(uintptr_t Addr, unsigned char Value)
{
	intptr_t Index;
	bool DoEvent;

	Index = Addr - m_BaseAddr;

	if((Index < 0) && (Index >= m_Size)){
		throw std::exception();
	}

	DoEvent = SetByte2(Index, Value);

	if(DoEvent)
		WriteEvent(Index);
}

unsigned short CSystemIF::GetUInt16(uintptr_t Addr)
{
	unsigned short Result;
	intptr_t Index;
	bool DoEvent = false;

	Index = Addr - m_BaseAddr;

	if((Index < 0) && (Index >= m_Size)){
		throw std::exception();
	}

	DoEvent = HasReadEvent(Index    ) ? true : DoEvent;
	DoEvent = HasReadEvent(Index + 1) ? true : DoEvent;

	if(DoEvent)
		ReadEvent(Index);

	GetByte2(Index ,    ((unsigned char *)&Result)[0]);
	GetByte2(Index + 1, ((unsigned char *)&Result)[1]);

	return Result;
}

void CSystemIF::SetUInt16(uintptr_t Addr, unsigned short Value)
{
	intptr_t Index;
	bool DoEvent = false;

	Index = Addr - m_BaseAddr;

	if((Index < 0) && (Index >= m_Size)){
		throw std::exception();
	}

	DoEvent = SetByte2(Index,     ((unsigned char *)&Value)[0]) ? true : DoEvent;
	DoEvent = SetByte2(Index + 1, ((unsigned char *)&Value)[1]) ? true : DoEvent;

	if(DoEvent)
		WriteEvent(Index);
}

unsigned int CSystemIF::GetUInt32(uintptr_t Addr)
{
	unsigned int Result;
	intptr_t Index;
	bool DoEvent = false;

	Index = Addr - m_BaseAddr;

	if((Index < 0) && (Index >= m_Size)){
		throw std::exception();
	}

	DoEvent = HasReadEvent(Index    ) ? true : DoEvent;
	DoEvent = HasReadEvent(Index + 1) ? true : DoEvent;
	DoEvent = HasReadEvent(Index + 2) ? true : DoEvent;
	DoEvent = HasReadEvent(Index + 3) ? true : DoEvent;

	if(DoEvent)
		ReadEvent(Index);

	GetByte2(Index ,    ((unsigned char *)&Result)[0]);
	GetByte2(Index + 1, ((unsigned char *)&Result)[1]);
	GetByte2(Index + 2, ((unsigned char *)&Result)[2]);
	GetByte2(Index + 3, ((unsigned char *)&Result)[3]);

	return Result;
}

void CSystemIF::SetUInt32(uintptr_t Addr, unsigned int Value)
{
	intptr_t Index;
	bool DoEvent = false;

	Index = Addr - m_BaseAddr;

	if((Index < 0) && (Index >= m_Size)){
		throw std::exception();
	}

	DoEvent = SetByte2(Index,     ((unsigned char *)&Value)[0]) ? true : DoEvent;
	DoEvent = SetByte2(Index + 1, ((unsigned char *)&Value)[1]) ? true : DoEvent;
	DoEvent = SetByte2(Index + 2, ((unsigned char *)&Value)[2]) ? true : DoEvent;
	DoEvent = SetByte2(Index + 3, ((unsigned char *)&Value)[3]) ? true : DoEvent;

	if(DoEvent)
		WriteEvent(Index);
}

