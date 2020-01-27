//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

void *operator new(size_t size, const char *filename, int linenumber);
void operator delete(void *_P, const char *filename, int linenumber);

#define DEBUG_NEW new(__FILE__, __LINE__)

#endif
