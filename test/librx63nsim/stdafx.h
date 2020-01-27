// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、
// または、参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#include "DebugHeap.h"
#include <new.h>
// Windows ヘッダー ファイル:
#include <windows.h>
#include <tchar.h>

#include <vector>
#include <list>
#include <map>
#include <string>
using namespace std;
typedef basic_string<char> CStringA;
typedef basic_string<wchar_t> CStringW;
typedef basic_string<TCHAR> CString;
#include <fstream>

#include "RX63NReg.h"
#define TOPPERS_size	tagSIZE

#ifdef __cplusplus
extern "C" {
#include "kernel_impl.h"
#include "task.h"
}
#else
#include "kernel_impl.h"
#include "task.h"
#endif

#ifndef _DEBUG
#define ASSERT(cond) 
#else
#ifndef ASSERT
#define ASSERT(cond) try{ if(!(cond)) throw std::exception(); }catch(...){ OutputDebugString(_T("Assertion failed !")); }
#endif
#endif

#undef exception
