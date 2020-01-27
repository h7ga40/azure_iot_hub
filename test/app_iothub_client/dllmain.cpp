//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: dllmain.cpp 2047 2019-12-23 00:37:52Z coas-nagasima $
#include "stdafx.h"
#include "dllmain.h"
#include "configfile.h"
#include "signode.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#if defined(_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

unsigned int sleep(unsigned int sec)
{
	Sleep(sec * 1000);
	return 0;
}

int usleep(unsigned int usec)
{
	Sleep(usec / 1000);
	return 0;
}

char *basename(char *s)
{
	size_t i;
	if (!s || !*s) return ".";
	i = strlen(s) - 1;
	for (; i&&s[i] == '/'; i--) s[i] = 0;
	for (; i&&s[i - 1] != '/'; i--);
	return s + i;
}

extern CKernel *g_Kernel;

extern "C" void (*const __init_array_start)(void) = 0;

extern "C" void __libc_exit_fini(void)
{
}

extern "C" {
//#include "rom_fs.c"
uint8_t RamDisk[256];
}

void Prepare()
{
	g_Kernel->Prepare();
}

extern "C" void _start_c(const char * const*args);

void Start()
{
	static const char *const args[] = {
		(char *)1,
		"asp3",
		0,
		// envp
		"TZ=JST-9",
		0,
		// auxv
		0
	};
	_start_c(args);

	g_Kernel->Start(false);
}

void Exit()
{
	g_Kernel->Terminate();
}

void Interrupt(int IntNo)
{
	g_Kernel->Interrupt(IntNo);
}

void GetUnitName(TCHAR *value, int size)
{
	_tcscpy_s(value, size, g_Kernel->GetUnitName());
}

void SetUnitName(const TCHAR *value)
{
	g_Kernel->SetUnitName(value);
}

void AssignOnSetEvent(TKernelEvent *OnSetEvent, void *ObjOfOnSetEvent)
{
	g_Kernel->AssignOnSetEvent(OnSetEvent, ObjOfOnSetEvent);
}

void AssignOnStart(TKernelEvent *OnStart, void *ObjOfOnStart)
{
	g_Kernel->AssignOnStart(OnStart, ObjOfOnStart);
}

void AssignOnTerminate(TKernelEvent *OnTerminate, void *ObjOfOnTerminate)
{
	g_Kernel->AssignOnTerminate(OnTerminate, ObjOfOnTerminate);
}

void AssignOnIdle(TKernelEvent *OnIdle, void *ObjOfOnIdle)
{
	g_Kernel->AssignOnIdle(OnIdle, ObjOfOnIdle);
}

void AssignOnOutput(TOutputEvent *OnOutput, void *ObjOfOnOutput)
{
	g_Kernel->AssignOnOutput(OnOutput, ObjOfOnOutput);
}

void AssignOnGetSystemTime(TGetSystemTimeEvent *OnGetSystemTime, void *ObjOfOnGetSystemTime)
{
	g_Kernel->AssignOnGetSystemTime(OnGetSystemTime, ObjOfOnGetSystemTime);
}

void AssignOnAddInterface(TInterfaceEvent *OnAddInterface, void *ObjOfOnAddInterface)
{
	g_Kernel->AssignOnAddInterface(OnAddInterface, ObjOfOnAddInterface);
}

void AssignOnRemoveInterface(TInterfaceEvent *OnRemoveInterface, void *ObjOfOnRemoveInterface)
{
	g_Kernel->AssignOnRemoveInterface(OnRemoveInterface, ObjOfOnRemoveInterface);
}

void *g_ObjOfOnStartExternal;
TStartExternalEvent *g_OnStartExternal;

void AssignOnStartExternal(TStartExternalEvent *OnStartExternal, void *ObjOfOnStartExternal)
{
	g_OnStartExternal = OnStartExternal;
	g_ObjOfOnStartExternal = ObjOfOnStartExternal;
}

extern "C" int Kernel_StartExternal(int argc, char **argv)
{
	if (g_OnStartExternal != NULL)
		return g_OnStartExternal(g_ObjOfOnStartExternal, argc, argv);
	return -1;
}

extern "C" const size_t _DYNAMIC[] = {
	0
};

extern "C" void __cp_begin()
{
}

extern "C" void __cp_end()
{
}

extern "C" void __cp_cancel()
{
}

extern "C" long __syscall_nr(long n, ...);

extern "C" long __syscall_cp_asm(volatile void *cancel, long n,
	long a, long b, long c, long d, long e, long f)
{
	long result = __syscall_nr(n, a, b, c, d, e, f);
	return result;
}

long SystemCall(long n, long a, long b, long c, long d, long e, long f)
{
	return __syscall_nr(n, a, b, c, d, e, f);
}

int ReadAddr(uintptr_t Addr, void *buffer, int count)
{
	return g_Kernel->ReadAddr(Addr, buffer, count);
}

int WriteAddr(uintptr_t Addr, const void *buffer, int count)
{
	return g_Kernel->WriteAddr(Addr, buffer, count);
}

bool GetBit(uintptr_t Addr, int bit)
{
	unsigned char temp = g_Kernel->GetByte(Addr);
	return (temp & (0x01 << bit)) != 0;
}

void SetBit(uintptr_t Addr, int bit, bool value)
{
	unsigned char temp = g_Kernel->GetByte(Addr);
	if (value)
		temp |= 0x01 << bit;
	else
		temp &= ~(0x01 << bit);
	g_Kernel->SetByte(Addr, temp);
}

bool ProcessEvent()
{
	return g_Kernel->ProcessEvent();
}

void Input(int Kind, const void *Data, int Size)
{
	g_Kernel->Input(Kind, Data, Size);
}

__int64 GetTimer()
{
	return g_Kernel->GetTimer();
}

void Progress(__int64 Timer)
{
	g_Kernel->Progress(Timer);
}

void CallTimeOut()
{
	g_Kernel->CallTimeOut();
}

CNtShell::CNtShell()
	: CKernel(m_SifCPU = new CSifRX63N((TRX63NReg *)0x00080000, (TRX63NFLASHReg *)0x007FC400))
{
	char path[MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	std::string cfg;
	SetUnitName(_T("app_iothub_client"));

	SGLib_Init();

	if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) {
		path[0] = _T('.');
		path[1] = _T('\0');
	}
	_splitpath(path, drive, dir, NULL, NULL);

	cfg = std::string("[global]\nimagedir: ") + std::string(drive) + std::string(dir) + std::string("\n");
	Config_AddString(cfg.c_str());
	Config_AddString("[card0]\ntype: Canon32M\nfile: app_iothub_client.img\n");

	// レジスタアドレス登録
	DefineSysIF(0x00080000, sizeof(TRX63NReg), m_SifCPU);
	AddSysTimerSync(m_SifCPU);

	m_SifEtherCtrl = new CSifEtherCtrl((TRX63NEDMACReg *)0x000C0000, (TRX63NETHERCReg *)0x000C0100,
		INT_NO_EINT, 256);
	DefineSysIF(0x000C0000, sizeof(TRX63NEDMACReg), m_SifEtherCtrl);
	DefineSysIF(0x000C0100, sizeof(TRX63NETHERCReg), m_SifEtherCtrl);
	AddSysTimerSync(m_SifEtherCtrl);

	//DefineSysIF(0x00090200, sizeof(TRX63NCAN0Reg), );

	m_SifUSB = new CSifUSB((TRX63NUSB0Reg *)0x000A0000, INT_NO_D0FIFO0, INT_NO_D1FIFO0, INT_NO_USBI0, -254);
	DefineSysIF(0x000A0000, sizeof(TRX63NUSB0Reg), m_SifUSB);
	AddSysTimerSync(m_SifUSB);

	//DefineSysIF(0x000A0200, sizeof(TRX63NUSB1Reg), );
	//DefineSysIF(0x000A0400, sizeof(TRX63NUSBReg), );
}

CNtShell::~CNtShell()
{
	if(m_SifCPU != NULL){
		delete m_SifCPU;
	}

	if(m_SifEtherCtrl != NULL){
		delete m_SifEtherCtrl;
	}

	if (m_SifUSB != NULL) {
		delete m_SifUSB;
	}
}

void CNtShell::Prepare()
{
	AddInterface(_T("SIO1"), _T("TSIO"), 1);
	AddInterface(_T("Ethernet"), _T("TEthernet"), 256);
}

extern "C" void hardware_init_hook();
extern "C" void software_init_hook();

void CNtShell::OnStart()
{
	hardware_init_hook();

	software_init_hook();

	SignodesInit();
	ClocksInit();

	__try {
		m_SifCPU->OnStart();
	}
	__finally {
		CKernel::OnStart();
	}
}

void CNtShell::OnTerminate()
{
	__try{
		m_SifCPU->OnTerminate();
	}
	__finally{
		CKernel::OnTerminate();
	}
}

ID CNtShell::OnIdle()
{
	return CKernel::OnIdle();
}

//------------------------------------------------------------------------------
//	概要：
//	引数：
//	戻り値：
//------------------------------------------------------------------------------
void CNtShell::Input(int Kind, const void *Data, int Size)
{
	switch(Kind)
	{
	case 1:
		m_SifCPU->SCI0.Receive(Data, Size);
		break;
	case 2:
		m_SifCPU->SCI1.Receive(Data, Size);
		break;
	case 3:
		m_SifCPU->SCI2.Receive(Data, Size);
		break;
	case 4:
		m_SifCPU->SCI3.Receive(Data, Size);
		break;
	case 6:
		m_SifCPU->SCI5.Receive(Data, Size);
		break;
	case 7:
		m_SifCPU->SCI6.Receive(Data, Size);
		break;
	case 8:
		m_SifCPU->RSPI0.Receive(Data, Size);
		break;
	case 9:
		m_SifCPU->RSPI1.Receive(Data, Size);
		break;
	case 256:
		m_SifEtherCtrl->Receive(Data, Size);
		break;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if(g_Kernel == NULL){
			// DLLアンロード時にメモリリークをチェックするよう設定
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			g_Kernel = new CNtShell();
		}
		break;
	case DLL_PROCESS_DETACH:
		if(g_Kernel != NULL){
			delete g_Kernel;
			g_Kernel = NULL;
			// この時点でメモリリークしているかチェック
			//_CrtDumpMemoryLeaks( );
		}
		break;
	}

	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
