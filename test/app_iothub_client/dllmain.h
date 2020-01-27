//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: dllmain.h 1970 2019-07-10 00:36:30Z coas-nagasima $
#pragma once

#ifndef ntshellH
#define ntshellH

#include "WinKernel.h"
#include "SifRX63N.h"
#include "SifEtherCtrl.h"
#include "SifUSB.h"

#ifdef __cplusplus
extern "C"{
#endif

#if defined(_USRDLL)
#define DLL_EXPORT	__declspec(dllexport)
#else
#define DLL_EXPORT	__declspec(dllimport)
#endif

typedef int CALLBACK TStartExternalEvent(void *obj, int argc, char **argv);

DLL_EXPORT void Prepare();

DLL_EXPORT void Start();

DLL_EXPORT void Exit();

DLL_EXPORT void Interrupt(int IntNo);

DLL_EXPORT void GetUnitName(TCHAR *value, int size);

DLL_EXPORT void SetUnitName(const TCHAR *value);

DLL_EXPORT void AssignOnSetEvent(TKernelEvent *OnSetEvent, void *ObjOfOnSetEvent);

DLL_EXPORT void AssignOnStart(TKernelEvent *OnStart, void *ObjOfOnStart);

DLL_EXPORT void AssignOnTerminate(TKernelEvent *OnTerminate, void *ObjOfOnTerminate);

DLL_EXPORT void AssignOnIdle(TKernelEvent *OnIdle, void *ObjOfOnIdle);

DLL_EXPORT void AssignOnOutput(TOutputEvent *OnOutput, void *ObjOfOnOutput);

DLL_EXPORT void AssignOnGetSystemTime(TGetSystemTimeEvent *OnGetSystemTime, void *ObjOfOnGetSystemTime);

DLL_EXPORT void AssignOnAddInterface(TInterfaceEvent *OnAddInterface, void *ObjOfOnAddInterface);

DLL_EXPORT void AssignOnRemoveInterface(TInterfaceEvent *OnRemoveInterface, void *ObjOfOnRemoveInterface);

DLL_EXPORT void AssignOnStartExternal(TStartExternalEvent *StartExternal, void *ObjOfStartExternal);

DLL_EXPORT long SystemCall(long n, long a, long b, long c, long d, long e, long f);

DLL_EXPORT int ReadAddr(uintptr_t Addr, void *buffer, int count);

DLL_EXPORT int WriteAddr(uintptr_t Addr, const void *buffer, int count);

DLL_EXPORT bool GetBit(uintptr_t Addr, int bit);

DLL_EXPORT void SetBit(uintptr_t Addr, int bit, bool value);

DLL_EXPORT bool ProcessEvent();

DLL_EXPORT void Input(int Kind, const void *Data, int Size);

DLL_EXPORT __int64 GetTimer();

DLL_EXPORT void Progress(__int64 Timer);

DLL_EXPORT void CallTimeOut();

#ifdef __cplusplus
}
#endif

class CNtShell : public CKernel
{
public:
	CNtShell();
	virtual ~CNtShell();
protected:
	void OnStart();
	void OnTerminate();
	ID OnIdle();
	void Input(int Kind, const void *Data, int Size);
public:
	void Prepare();
private:
	CSifRX63N *m_SifCPU;
	CSifEtherCtrl *m_SifEtherCtrl;
	CSifUSB *m_SifUSB;
};

#endif ntshellH
