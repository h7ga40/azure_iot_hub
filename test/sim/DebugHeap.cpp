//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "DebugHeap.h"

//-------------------------------------------------------------------------
// メモリリーク調査用
//-------------------------------------------------------------------------
#ifdef _DEBUG
void* operator new(size_t size, const char *filename, int linenumber)
{
	return _malloc_dbg(size, _NORMAL_BLOCK, filename, linenumber);
}

void operator delete(void * _P, const char *filename, int linenumber)
{
	_free_dbg(_P, _NORMAL_BLOCK);
	return;
}
#endif
