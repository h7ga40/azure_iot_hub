//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifUSB.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
#include "StdAfx.h"
#include "WinKernel.h"
#include "SifUSB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define SIF_USB_DEBUG

extern CKernel *g_Kernel;

CUsbRingBuf::CUsbRingBuf()
{
	m_WritePos = 0;
	m_ReadPos = 0;
}

int CUsbRingBuf::GetSize()
{
	if(m_ReadPos <= m_WritePos)
		return m_WritePos - m_ReadPos;
	else
		return sizeof(m_Data) + m_WritePos - m_ReadPos;
}

void CUsbRingBuf::PutByte(uint8_t data)
{
	if(GetSize() == (sizeof(m_Data) - 1))
		throw std::exception();

	m_Data[m_WritePos] = data;
	m_WritePos++;
	if(m_WritePos >= sizeof(m_Data))
		m_WritePos = 0;
}

uint8_t CUsbRingBuf::GetByte()
{
	uint8_t result;

	if(GetSize() == 0)
		throw std::exception();

	result = m_Data[m_ReadPos];
	m_ReadPos++;
	if(m_ReadPos >= sizeof(m_Data))
		m_ReadPos = 0;

	return result;
}

void CUsbRingBuf::Clear()
{
	m_ReadPos = m_WritePos;
}

CUsbPipe::CUsbPipe()
{
	m_Owner = NULL;
	m_PipeNo = 0;
	m_Data1 = false;
	m_BVAL = false;
	m_SendReq = false;
	m_Fifo = NULL;
	*(unsigned short *)&m_PIPECFG = 0;
	*(unsigned short *)&m_PIPEMAXP = 0;
	*(unsigned short *)&m_PIPEPERI = 0;
	*(unsigned short *)&m_PIPECTR = 0;
	*(unsigned short *)&m_PIPETRE = 0;
	*(unsigned short *)&m_PIPETRN = 0;
}

CUsbPipe::~CUsbPipe()
{
}

void CUsbPipe::Init(CSifUSB *owner, int pipeNo)
{
	m_Owner = owner;
	m_PipeNo = pipeNo;
}

void CUsbPipe::SetPIPECFG(TUSBPIPECFG value)
{
	m_PIPECFG = value;
}

void CUsbPipe::SetPIPEMAXP(TUSBPIPEMAXP value)
{
	m_PIPEMAXP = value;
}

void CUsbPipe::SetPIPEPERI(TUSBPIPEPERI value)
{
	m_PIPEPERI = value;
}

void CUsbPipe::SetPIPECTR(TUSBPIPECTR value)
{
	CSifUSB::TRunRequest req = CSifUSB::RUN_NONE;

	if(m_PipeNo == 0){
		if(((TUSBDCPCTR *)&value)->SUREQCLR != 0){
			((TUSBDCPCTR *)&m_PIPECTR)->SUREQ = 0;
		}
		if((((TUSBDCPCTR *)&value)->SUREQ != 0) && (((TUSBDCPCTR *)&m_PIPECTR)->SUREQ == 0)){
			req = CSifUSB::RUN_SUREQ;
		}
		if((((TUSBDCPCTR *)&value)->SQSET != 0) && (((TUSBDCPCTR *)&m_PIPECTR)->SQSET == 0)){
			m_Data1 = true;
		}
		if((((TUSBDCPCTR *)&value)->SQCLR != 0) && (((TUSBDCPCTR *)&m_PIPECTR)->SQCLR == 0)){
			m_Data1 = false;
		}
		if((((TUSBDCPCTR *)&value)->PID == 1) && (((TUSBDCPCTR *)&m_PIPECTR)->PID == 0)){
			req = CSifUSB::RUN_PID;
		}
		*((uint16_t *)&value) = (*((uint16_t *)&m_PIPECTR) & 0x8060) | (*((uint16_t *)&value) & ~0x8060);
	}
	else{
		if((value.ACLRM != 0) && (m_PIPECTR.ACLRM == 0)){
			m_Data1 = false;
			m_SendBuf.Clear();
			m_RecvBuf.Clear();
		}
		if((value.SQSET != 0) && (m_PIPECTR.SQSET == 0)){
			m_Data1 = true;
		}
		if((value.SQCLR != 0) && (m_PIPECTR.SQCLR == 0)){
			m_Data1 = false;
		}
		if((value.PID == 1) && (m_PIPECTR.PID == 0)){
			req = CSifUSB::RUN_PID;
			m_SendReq = true;
		}
		*((uint16_t *)&value) = (*((uint16_t *)&m_PIPECTR) & 0xC060) | (*((uint16_t *)&value) & ~0xC060);
	}
	m_PIPECTR = value;

	if(req != CSifUSB::RUN_NONE)
		m_Owner->Run(m_PipeNo, req);
}

void CUsbPipe::SetPIPETRE(TUSBPIPETRE value)
{
	m_PIPETRE = value;
}

void CUsbPipe::SetPIPETRN(TUSBPIPETRN value)
{
	m_PIPETRN = value;
}

void CUsbPipe::PutByte(uint8_t data)
{
	m_SendBuf.PutByte(data);
}

void CUsbPipe::PutShort(uint16_t data)
{
	m_SendBuf.PutByte(data & 0xFF);
	m_SendBuf.PutByte(data >> 8);
}

uint16_t CUsbPipe::GetShort()
{
	return GetByte() | (GetByte() << 8);
}

bool CUsbPipe::IsAssignd()
{
	if(m_PipeNo == 0){
		return ((TUSBDCPCTR *)&m_PIPECTR)->SUREQ != 0;
	}
	else{
		return m_PIPECFG.EPNUM != 0;
	}
}

void CUsbPipe::Receive(const void *data, int size)
{
	int len = GetRecvSize();
	if(len != 0){
#ifdef SIF_USB_DEBUG
		int pos;
		TCHAR *temp = new TCHAR[256];
		pos = _stprintf_s(temp, 256, _T("USB RecvBuf Clear Pipe=%d size=%d "), m_PipeNo, len);
		for(int i = 0; (i < len) && (pos < (256 - 5)); i++, pos += 3)
			_stprintf_s(&temp[pos], 256 - pos, _T("%02x "), GetByte());
		_tcscat_s(temp, 256, _T("\r\n"));
		_tprintf_s(temp);
		delete temp;
#endif
		m_RecvBuf.Clear();
	}

	for(int i = 0; i < size; i++){
		RecvByte(((uint8_t *)data)[i]);
	}
}

void CUsbPipe::SendBytes(void *data, int size)
{
	for(int i = 0; i < size; i++){
		((uint8_t *)data)[i] = SendByte();
	}
}

int CUsbPipe::GetRequest(bool idle)
{
	int ret = 0;

	if(m_PipeNo == 0){
		ret = (((TUSBDCPCTR *)&m_PIPECTR)->SUREQ != 0) ? 1 : 0;
	}
	else if(m_PIPECFG.TYPE != 0){
		// 受信パイプの場合
		if(m_PIPECFG.DIR == 0){
			if(idle && GetRecvSize() == 0){
				ret = 3;
			}
		}
		// 送信パイプの場合
		else{
			if (m_SendReq || ((m_PIPECTR.PID != 0) && (GetSendSize() > 0))){
				ret = 2;
			}
		}
	}

	return ret;
}

void CUsbPipe::SetBVAL(bool value)
{
	//m_BVAL = value;

	if(value)
		m_Owner->Run(m_PipeNo, CSifUSB::RUN_BVAL);
}

void CUsbPipe::SetBRDY(bool value)
{
	if((m_PipeNo < 0) || (m_PipeNo > 9))
		return;

	*(uint16_t *)&m_Owner->m_UsbRegInst.BRDYSTS |= (1 << m_PipeNo);

	if(m_Fifo != NULL)
		m_Fifo->m_FIFOCTR.DTLN = GetRecvSize();
}

void CUsbPipe::SetNRDY(bool value)
{
	if((m_PipeNo < 0) || (m_PipeNo > 9))
		return;

	*(uint16_t *)&m_Owner->m_UsbRegInst.NRDYSTS |= (1 << m_PipeNo);
}

void CUsbPipe::SetBEMP(bool value)
{
	if((m_PipeNo < 0) || (m_PipeNo > 9))
		return;

	*(uint16_t *)&m_Owner->m_UsbRegInst.BEMPSTS |= (1 << m_PipeNo);
}

void CUsbPipe::ClearPID()
{
	if((m_PipeNo < 0) || (m_PipeNo > 9))
		return;

	m_PIPECTR.PID = 0;
}

CUsbFifo::CUsbFifo(CSifUSB *owner)
{
	m_Owner = owner;
	if(this == &m_Owner->m_CFifo)
		m_Pipe = &m_Owner->m_UsbPipe[0];
	else
		m_Pipe = NULL;
}

CUsbFifo::~CUsbFifo()
{
}

void CUsbFifo::SetFIFOSEL(TUSBFIFOSEL value)
{
	if(m_Pipe != NULL)
		m_Pipe->m_Fifo = NULL;

	if(value.CURPIPE == 0){
		if(this == &m_Owner->m_CFifo)
			m_Pipe = &m_Owner->m_UsbPipe[0];
		else
			m_Pipe = NULL;
	}
	else if((value.CURPIPE >= 1) && (value.CURPIPE <= 9)){
		m_Pipe = &m_Owner->m_UsbPipe[value.CURPIPE];
	}
	else{
		m_Pipe = NULL;
	}

	if(m_Pipe != NULL)
		m_Pipe->m_Fifo = this;

	m_FIFOSEL = value;
	if(m_Pipe != NULL){
		m_FIFOCTR.FRDY = 1;
		m_FIFOCTR.DTLN = m_Pipe->GetRecvSize();
		m_FIFOCTR.BVAL = m_Pipe->GetBVAL();
	}
	else{
		m_FIFOCTR.FRDY = 0;
		m_FIFOCTR.DTLN = 0;
		m_FIFOCTR.BVAL = 0;
	}
}

void CUsbFifo::SetFIFOCTR(TUSBFIFOCTR value)
{
	if(value.BCLR != 0){
		value.FRDY = 1;
	}
	if((value.BVAL != m_FIFOCTR.BVAL) && (m_Pipe != NULL)){
		m_Pipe->SetBVAL(value.BVAL);
	}
	m_FIFOCTR = value;
}

TUSBFIFOCTR CUsbFifo::GetFIFOCTR()
{
	if(m_Pipe != NULL){
		m_FIFOCTR.DTLN = m_Pipe->GetRecvSize();
		m_FIFOCTR.BVAL = m_Pipe->GetBVAL();
	}
	else{
		m_FIFOCTR.DTLN = 0;
		m_FIFOCTR.BVAL = 0;
	}
	return m_FIFOCTR;
}

CSifUSB::CSifUSB(TRX62NUSB0Reg *usbReg, int d0FifoIntNo, int d1FifoIntNo,
	int usbioIntNo, int devNo)
	: m_UsbPipe(), m_CFifo(this), m_D0Fifo(this), m_D1Fifo(this),
	m_Bluetooth(this)
{
	for(int i = 0; i < sizeof(m_UsbPipe) / sizeof(*m_UsbPipe); i++)
	{
		m_UsbPipe[i].Init(this, i);
	}

	m_UsbReg = usbReg;
	m_D0FifoIntNo = d0FifoIntNo;
	m_D1FifoIntNo = d1FifoIntNo;
	m_UsbioIntNo = usbioIntNo;
	m_DevNo = devNo;
	m_Stage = 0;
	m_Term = usbtNodevice;
	m_Timer = -1;
	m_CurrentPipe = -1;
	m_Intsts0 = 0;
	m_Intsts1 = 0;

	InitializeCriticalSection(&m_Lock);

	// 初期値設定
	memset(&m_UsbRegInst, 0, sizeof(m_UsbRegInst));
	m_UsbRegInst.DCPMAXP.MXPS = 0x0040;
	m_UsbRegInst.DCPCTR.SQMON = 1;
	m_UsbPipe[0].SetPIPEMAXP(*(TUSBPIPEMAXP *)&m_UsbRegInst.DCPMAXP);
	m_UsbPipe[0].SetPIPECTR(*(TUSBPIPECTR *)&m_UsbRegInst.DCPCTR);
}

CSifUSB::~CSifUSB()
{
	DeleteCriticalSection(&m_Lock);
}

unsigned char CSifUSB::GetByte(unsigned int Addr)
{
	unsigned char *Reg, Value;
	int Index;

	Reg = (unsigned char *)m_UsbReg;
	Index = (int)Addr - (int)Reg;

	Value = ((unsigned char *)&m_UsbRegInst)[Index / sizeof(unsigned char)];

	switch (Index)
	{
	// CFIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, CFIFO):
		Value = m_CFifo.GetByte();
		break;
	// D0FIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, D0FIFO):
		Value = m_D0Fifo.GetByte();
		break;
	// D1FIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, D1FIFO):
		Value = m_D1Fifo.GetByte();
		break;
	default:
		throw std::exception();
	}

	return Value;
}

void CSifUSB::SetByte(unsigned int Addr, unsigned char Value)
{
	unsigned short *Reg;
	int Index;

	Reg = (unsigned short *)m_UsbReg;
	Index = (int)Addr - (int)Reg;

	switch (Index)
	{
	// CFIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, CFIFO):
		m_CFifo.PutByte(Value);
		break;
	// D0FIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, D0FIFO):
		m_D0Fifo.PutByte(Value);
		break;
	// D1FIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, D1FIFO):
		m_D1Fifo.PutByte(Value);
		break;
	default:
		throw std::exception();
	}

	((unsigned char *)&m_UsbRegInst)[Index / sizeof(unsigned char)] = Value;
}

unsigned short CSifUSB::GetUInt16(unsigned int Addr)
{
	unsigned short *Reg, Value;
	int Index;

	Reg = (unsigned short *)m_UsbReg;
	Index = (int)Addr - (int)Reg;

	Value = ((unsigned short *)&m_UsbRegInst)[Index / sizeof(unsigned short)];

	switch (Index)
	{
	// システムコンフィギュレーションコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, SYSCFG):
		break;
	// システムコンフィギュレーションステータスレジスタ0
	case offsetof(TRX62NUSB0Reg, SYSSTS0):
		break;
	// デバイスステートコントロールレジスタ0
	case offsetof(TRX62NUSB0Reg, DVSTCTR0):
		break;
	// CFIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, CFIFO):
		Value = m_CFifo.GetShort();
		break;
	// D0FIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, D0FIFO):
		Value = m_D0Fifo.GetShort();
		break;
	// D1FIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, D1FIFO):
		Value = m_D1Fifo.GetShort();
		break;
	// CFIFOポート選択レジスタ
	case offsetof(TRX62NUSB0Reg, CFIFOSEL):
		Value = *(unsigned short *)&m_CFifo.GetFIFOSEL();
		break;
	// CFIFOポートコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, CFIFOCTR):
		Value = *(unsigned short *)&m_CFifo.GetFIFOCTR();
		break;
	// D0FIFOポート選択レジスタ
	case offsetof(TRX62NUSB0Reg, D0FIFOSEL):
		Value = *(unsigned short *)&m_D0Fifo.GetFIFOSEL();
		break;
	// D0FIFOポートコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, D0FIFOCTR):
		Value = *(unsigned short *)&m_D0Fifo.GetFIFOCTR();
		break;
	// D1FIFOポート選択レジスタ
	case offsetof(TRX62NUSB0Reg, D1FIFOSEL):
		Value = *(unsigned short *)&m_D1Fifo.GetFIFOSEL();
		break;
	// D1FIFOポートコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, D1FIFOCTR):
		Value = *(unsigned short *)&m_D1Fifo.GetFIFOCTR();
		break;
	// 割り込み許可レジスタ0
	case offsetof(TRX62NUSB0Reg, INTENB0):
		break;
	// 割り込み許可レジスタ1
	case offsetof(TRX62NUSB0Reg, INTENB1):
		break;
	// BRDY割り込み許可レジスタ
	case offsetof(TRX62NUSB0Reg, BRDYENB):
		break;
	// NRDY割り込み許可レジスタ
	case offsetof(TRX62NUSB0Reg, NRDYENB):
		break;
	// BEMP割り込み許可レジスタ
	case offsetof(TRX62NUSB0Reg, BEMPENB):
		break;
	// SOF出力コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, SOFCFG):
		break;
	// 割り込みステータスレジスタ0
	case offsetof(TRX62NUSB0Reg, INTSTS0):
		break;
	// 割り込みステータスレジスタ1
	case offsetof(TRX62NUSB0Reg, INTSTS1):
		break;
	// BRDY割り込みステータスレジスタ
	case offsetof(TRX62NUSB0Reg, BRDYSTS):
		break;
	// NRDY割り込みステータスレジスタ
	case offsetof(TRX62NUSB0Reg, NRDYSTS):
		break;
	// BEMP割り込みステータスレジスタ
	case offsetof(TRX62NUSB0Reg, BEMPSTS):
		break;
	// フレームナンバレジスタ
	case offsetof(TRX62NUSB0Reg, FRMNUM):
		break;
	// デバイスステート切り替えレジスタ
	case offsetof(TRX62NUSB0Reg, DVCHGR):
		break;
	// USBアドレスレジスタ
	case offsetof(TRX62NUSB0Reg, USBADDR):
		break;
	// USBリクエストタイプレジスタ
	case offsetof(TRX62NUSB0Reg, USBREQ):
		break;
	// USBリクエストバリューレジスタ
	case offsetof(TRX62NUSB0Reg, USBVAL):
		break;
	// USBリクエストインデックスレジスタ
	case offsetof(TRX62NUSB0Reg, USBINDX):
		break;
	// USBリクエストレングスレジスタ
	case offsetof(TRX62NUSB0Reg, USBLENG):
		break;
	// DCPコンフィギュレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DCPCFG):
		Value = *(unsigned short *)&m_UsbPipe[0].GetPIPECFG();
		break;
	// DCPマックスパケットサイズレジスタ
	case offsetof(TRX62NUSB0Reg, DCPMAXP):
		Value = *(unsigned short *)&m_UsbPipe[0].GetPIPEMAXP();
		break;
	// DCPコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, DCPCTR):
		Value = *(unsigned short *)&m_UsbPipe[0].GetPIPECTR();
		break;
	// パイプウィンドウ選択レジスタ
	case offsetof(TRX62NUSB0Reg, PIPESEL):
		break;
	// パイプコンフィギュレーションレジスタ
	case offsetof(TRX62NUSB0Reg, PIPECFG):
		if(m_UsbRegInst.PIPESEL.PIPESEL == 0){
			Value = 0;
		}
		else{
			Value = *(unsigned short *)&m_UsbPipe[m_UsbRegInst.PIPESEL.PIPESEL].GetPIPECFG();
		}
		break;
	// パイプマックスパケットサイズレジスタ
	case offsetof(TRX62NUSB0Reg, PIPEMAXP):
		if(m_UsbRegInst.PIPESEL.PIPESEL == 0){
			Value = 0;
		}
		else{
			Value = *(unsigned short *)&m_UsbPipe[m_UsbRegInst.PIPESEL.PIPESEL].GetPIPEMAXP();
		}
		break;
	// パイプ周期制御レジスタ
	case offsetof(TRX62NUSB0Reg, PIPEPERI):
		if(m_UsbRegInst.PIPESEL.PIPESEL == 0){
			Value = 0;
		}
		else{
			Value = *(unsigned short *)&m_UsbPipe[m_UsbRegInst.PIPESEL.PIPESEL].GetPIPEPERI();
		}
		break;
	// パイプ1コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE1CTR):
		Value = *(unsigned short *)&m_UsbPipe[1].GetPIPECTR();
		break;
	// パイプ2コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE2CTR):
		Value = *(unsigned short *)&m_UsbPipe[2].GetPIPECTR();
		break;
	// パイプ3コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE3CTR):
		Value = *(unsigned short *)&m_UsbPipe[3].GetPIPECTR();
		break;
	// パイプ4コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE4CTR):
		Value = *(unsigned short *)&m_UsbPipe[4].GetPIPECTR();
		break;
	// パイプ5コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE5CTR):
		Value = *(unsigned short *)&m_UsbPipe[5].GetPIPECTR();
		break;
	// パイプ6コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE6CTR):
		Value = *(unsigned short *)&m_UsbPipe[6].GetPIPECTR();
		break;
	// パイプ7コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE7CTR):
		Value = *(unsigned short *)&m_UsbPipe[7].GetPIPECTR();
		break;
	// パイプ8コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE8CTR):
		Value = *(unsigned short *)&m_UsbPipe[8].GetPIPECTR();
		break;
	// パイプ9コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE9CTR):
		Value = *(unsigned short *)&m_UsbPipe[9].GetPIPECTR();
		break;
	// パイプ1トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE1TRE):
		Value = *(unsigned short *)&m_UsbPipe[1].GetPIPETRE();
		break;
	// パイプ1トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE1TRN):
		Value = *(unsigned short *)&m_UsbPipe[1].GetPIPETRN();
		break;
	// パイプ2トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE2TRE):
		Value = *(unsigned short *)&m_UsbPipe[2].GetPIPETRE();
		break;
	// パイプ2トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE2TRN):
		Value = *(unsigned short *)&m_UsbPipe[2].GetPIPETRN();
		break;
	// パイプ3トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE3TRE):
		Value = *(unsigned short *)&m_UsbPipe[3].GetPIPETRE();
		break;
	// パイプ3トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE3TRN):
		Value = *(unsigned short *)&m_UsbPipe[3].GetPIPETRN();
		break;
	// パイプ4トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE4TRE):
		Value = *(unsigned short *)&m_UsbPipe[4].GetPIPETRE();
		break;
	// パイプ4トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE4TRN):
		Value = *(unsigned short *)&m_UsbPipe[4].GetPIPETRN();
		break;
	// パイプ5トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE5TRE):
		Value = *(unsigned short *)&m_UsbPipe[5].GetPIPETRE();
		break;
	// パイプ5トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE5TRN):
		Value = *(unsigned short *)&m_UsbPipe[5].GetPIPETRN();
		break;
	// デバイスアドレス0コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD0):
		break;
	// デバイスアドレス1コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD1):
		break;
	// デバイスアドレス2コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD2):
		break;
	// デバイスアドレス3コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD3):
		break;
	// デバイスアドレス4コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD4):
		break;
	// デバイスアドレス5コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD5):
		break;
	case 0xA4:
	case 0xA6:
		break;
	default:
		throw std::exception();
	}

	return Value;
}

void CSifUSB::SetUInt16(unsigned int Addr, unsigned short Value)
{
	unsigned short *Reg;
	int Index;

	Reg = (unsigned short *)m_UsbReg;
	Index = (int)Addr - (int)Reg;

	switch (Index)
	{
	// システムコンフィギュレーションコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, SYSCFG):
		if(((Value & 0x0001) != 0) && ((m_UsbRegInst.SYSCFG & 0x0001) == 0)){
			Attatch();
		}
		break;
	// システムコンフィギュレーションステータスレジスタ0
	case offsetof(TRX62NUSB0Reg, SYSSTS0):
		break;
	// デバイスステートコントロールレジスタ0
	case offsetof(TRX62NUSB0Reg, DVSTCTR0):
		if(((TUSBDVSTCTR0 *)&Value)->USBRST != 0){
			((TUSBDVSTCTR0 *)&Value)->USBRST = 0;
			((TUSBDVSTCTR0 *)&Value)->RHST = 2;
		}
		break;
	// CFIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, CFIFO):
		m_CFifo.PutShort(Value);
		break;
	// D0FIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, D0FIFO):
		m_D0Fifo.PutShort(Value);
		break;
	// D1FIFOポートレジスタ
	case offsetof(TRX62NUSB0Reg, D1FIFO):
		m_D1Fifo.PutShort(Value);
		break;
	// CFIFOポート選択レジスタ
	case offsetof(TRX62NUSB0Reg, CFIFOSEL):
		m_CFifo.SetFIFOSEL(*(TUSBFIFOSEL *)&Value);
		break;
	// CFIFOポートコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, CFIFOCTR):
		m_CFifo.SetFIFOCTR(*(TUSBFIFOCTR *)&Value);
		break;
	// D0FIFOポート選択レジスタ
	case offsetof(TRX62NUSB0Reg, D0FIFOSEL):
		m_D0Fifo.SetFIFOSEL(*(TUSBFIFOSEL *)&Value);
		break;
	// D0FIFOポートコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, D0FIFOCTR):
		m_D0Fifo.SetFIFOCTR(*(TUSBFIFOCTR *)&Value);
		break;
	// D1FIFOポート選択レジスタ
	case offsetof(TRX62NUSB0Reg, D1FIFOSEL):
		m_D1Fifo.SetFIFOSEL(*(TUSBFIFOSEL *)&Value);
		break;
	// D1FIFOポートコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, D1FIFOCTR):
		m_D1Fifo.SetFIFOCTR(*(TUSBFIFOCTR *)&Value);
		break;
	// 割り込み許可レジスタ0
	case offsetof(TRX62NUSB0Reg, INTENB0):
		if(m_Timer == -1){
			Lock lock(this);

			m_Timer = 1;
			g_Kernel->OnSetEvent();
		}
		break;
	// 割り込み許可レジスタ1
	case offsetof(TRX62NUSB0Reg, INTENB1):
		if(m_Timer == -1){
			Lock lock(this);

			m_Timer = 1;
			g_Kernel->OnSetEvent();
		}
		break;
	// BRDY割り込み許可レジスタ
	case offsetof(TRX62NUSB0Reg, BRDYENB):
		if(m_Timer == -1){
			Lock lock(this);

			m_Timer = 1;
			g_Kernel->OnSetEvent();
		}
		break;
	// NRDY割り込み許可レジスタ
	case offsetof(TRX62NUSB0Reg, NRDYENB):
		if(m_Timer == -1){
			Lock lock(this);

			m_Timer = 1;
			g_Kernel->OnSetEvent();
		}
		break;
	// BEMP割り込み許可レジスタ
	case offsetof(TRX62NUSB0Reg, BEMPENB):
		if(m_Timer == -1){
			Lock lock(this);

			m_Timer = 1;
			g_Kernel->OnSetEvent();
		}
		break;
	// SOF出力コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, SOFCFG):
		break;
	// 割り込みステータスレジスタ0
	case offsetof(TRX62NUSB0Reg, INTSTS0):
		Value = (((unsigned short *)&m_UsbRegInst)[Index / sizeof(unsigned short)] & 0x0700) | (Value & ~0x0700);
		break;
	// 割り込みステータスレジスタ1
	case offsetof(TRX62NUSB0Reg, INTSTS1):
		Value = ((unsigned short *)&m_UsbRegInst)[Index / sizeof(unsigned short)] & Value;
		break;
	// BRDY割り込みステータスレジスタ
	case offsetof(TRX62NUSB0Reg, BRDYSTS):
		Value = ((unsigned short *)&m_UsbRegInst)[Index / sizeof(unsigned short)] & Value;
		if((m_UsbRegInst.INTSTS0.BRDY != 0) && ((Value & *(uint16_t *)&m_UsbRegInst.BRDYENB) == 0))
			m_UsbRegInst.INTSTS0.BRDY = 0;
		break;
	// NRDY割り込みステータスレジスタ
	case offsetof(TRX62NUSB0Reg, NRDYSTS):
		Value = ((unsigned short *)&m_UsbRegInst)[Index / sizeof(unsigned short)] & Value;
		if((m_UsbRegInst.INTSTS0.NRDY != 0) && ((Value & *(uint16_t *)&m_UsbRegInst.NRDYENB) == 0))
			m_UsbRegInst.INTSTS0.NRDY = 0;
		break;
	// BEMP割り込みステータスレジスタ
	case offsetof(TRX62NUSB0Reg, BEMPSTS):
		Value = ((unsigned short *)&m_UsbRegInst)[Index / sizeof(unsigned short)] & Value;
		if((m_UsbRegInst.INTSTS0.BEMP != 0) && ((Value & *(uint16_t *)&m_UsbRegInst.BEMPENB) == 0))
			m_UsbRegInst.INTSTS0.BEMP = 0;
		break;
	// フレームナンバレジスタ
	case offsetof(TRX62NUSB0Reg, FRMNUM):
		break;
	// デバイスステート切り替えレジスタ
	case offsetof(TRX62NUSB0Reg, DVCHGR):
		break;
	// USBアドレスレジスタ
	case offsetof(TRX62NUSB0Reg, USBADDR):
		break;
	// USBリクエストタイプレジスタ
	case offsetof(TRX62NUSB0Reg, USBREQ):
		break;
	// USBリクエストバリューレジスタ
	case offsetof(TRX62NUSB0Reg, USBVAL):
		break;
	// USBリクエストインデックスレジスタ
	case offsetof(TRX62NUSB0Reg, USBINDX):
		break;
	// USBリクエストレングスレジスタ
	case offsetof(TRX62NUSB0Reg, USBLENG):
		break;
	// DCPコンフィギュレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DCPCFG):
		m_UsbPipe[0].SetPIPECFG(*(TUSBPIPECFG *)&Value);
		break;
	// DCPマックスパケットサイズレジスタ
	case offsetof(TRX62NUSB0Reg, DCPMAXP):
		m_UsbPipe[0].SetPIPEMAXP(*(TUSBPIPEMAXP *)&Value);
		break;
	// DCPコントロールレジスタ
	case offsetof(TRX62NUSB0Reg, DCPCTR):
		m_UsbPipe[0].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプウィンドウ選択レジスタ
	case offsetof(TRX62NUSB0Reg, PIPESEL):
		if(((TUSBPIPESEL *)&Value)->PIPESEL > 9){
			throw std::exception();
		}
		break;
	// パイプコンフィギュレーションレジスタ
	case offsetof(TRX62NUSB0Reg, PIPECFG):
		if(m_UsbRegInst.PIPESEL.PIPESEL == 0){
			//throw std::exception();
		}
		else{
			m_UsbPipe[m_UsbRegInst.PIPESEL.PIPESEL].SetPIPECFG(*(TUSBPIPECFG *)&Value);
		}
		break;
	// パイプマックスパケットサイズレジスタ
	case offsetof(TRX62NUSB0Reg, PIPEMAXP):
		if(m_UsbRegInst.PIPESEL.PIPESEL == 0){
			//throw std::exception();
		}
		else{
			m_UsbPipe[m_UsbRegInst.PIPESEL.PIPESEL].SetPIPEMAXP(*(TUSBPIPEMAXP *)&Value);
		}
		break;
	// パイプ周期制御レジスタ
	case offsetof(TRX62NUSB0Reg, PIPEPERI):
		if(m_UsbRegInst.PIPESEL.PIPESEL == 0){
			//throw std::exception();
		}
		else{
			m_UsbPipe[m_UsbRegInst.PIPESEL.PIPESEL].SetPIPEPERI(*(TUSBPIPEPERI *)&Value);
		}
		break;
	// パイプ1コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE1CTR):
		m_UsbPipe[1].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプ2コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE2CTR):
		m_UsbPipe[2].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプ3コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE3CTR):
		m_UsbPipe[3].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプ4コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE4CTR):
		m_UsbPipe[4].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプ5コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE5CTR):
		m_UsbPipe[5].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプ6コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE6CTR):
		m_UsbPipe[6].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプ7コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE7CTR):
		m_UsbPipe[7].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプ8コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE8CTR):
		m_UsbPipe[8].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプ9コントロールレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE9CTR):
		m_UsbPipe[9].SetPIPECTR(*(TUSBPIPECTR *)&Value);
		break;
	// パイプ1トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE1TRE):
		m_UsbPipe[1].SetPIPETRE(*(TUSBPIPETRE *)&Value);
		break;
	// パイプ1トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE1TRN):
		m_UsbPipe[1].SetPIPETRN(*(TUSBPIPETRN *)&Value);
		break;
	// パイプ2トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE2TRE):
		m_UsbPipe[2].SetPIPETRE(*(TUSBPIPETRE *)&Value);
		break;
	// パイプ2トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE2TRN):
		m_UsbPipe[2].SetPIPETRN(*(TUSBPIPETRN *)&Value);
		break;
	// パイプ3トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE3TRE):
		m_UsbPipe[3].SetPIPETRE(*(TUSBPIPETRE *)&Value);
		break;
	// パイプ3トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE3TRN):
		m_UsbPipe[3].SetPIPETRN(*(TUSBPIPETRN *)&Value);
		break;
	// パイプ4トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE4TRE):
		m_UsbPipe[4].SetPIPETRE(*(TUSBPIPETRE *)&Value);
		break;
	// パイプ4トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE4TRN):
		m_UsbPipe[4].SetPIPETRN(*(TUSBPIPETRN *)&Value);
		break;
	// パイプ5トランザクションカウンタイネーブルレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE5TRE):
		m_UsbPipe[5].SetPIPETRE(*(TUSBPIPETRE *)&Value);
		break;
	// パイプ5トランザクションカウンタレジスタ
	case offsetof(TRX62NUSB0Reg, PIPE5TRN):
		m_UsbPipe[5].SetPIPETRN(*(TUSBPIPETRN *)&Value);
		break;
	// デバイスアドレス0コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD0):
		break;
	// デバイスアドレス1コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD1):
		break;
	// デバイスアドレス2コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD2):
		break;
	// デバイスアドレス3コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD3):
		break;
	// デバイスアドレス4コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD4):
		break;
	// デバイスアドレス5コンフィグレーションレジスタ
	case offsetof(TRX62NUSB0Reg, DEVADD5):
		break;
	case 0xA4:
	case 0xA6:
		break;
	default:
		throw std::exception();
	}

	((unsigned short *)&m_UsbRegInst)[Index / sizeof(unsigned short)] = Value;
}

unsigned int CSifUSB::GetUInt32(unsigned int Addr)
{
	throw std::exception();
}

void CSifUSB::SetUInt32(unsigned int Addr, unsigned int Value)
{
	throw std::exception();
}

__int64 CSifUSB::GetTimer()
{
	if(m_Interrupting)
		return 0;

	__int64 timer = m_Timer;
	__int64 timer2 = m_Bluetooth.GetTimer();

	if((timer == -1) || ((timer2 != -1) && (timer > timer2)))
		timer = timer2;

	return timer;
}

void CSifUSB::Progress(__int64 Timer)
{
	m_Bluetooth.Progress(Timer);

	if(m_Timer != -1){
		m_Timer -= Timer;
		if(m_Timer < 0){
			m_Timer = 0;
		}
	}
}

void CSifUSB::CallTimeOut()
{
	m_Bluetooth.CallTimeOut();

	// 割り込みの受付が終了するまでスレッドスイッチ
	if(m_Interrupting){
		m_Interrupting = false;
#ifdef SIF_USB_DEBUG
		int pos;
		TCHAR *temp = new TCHAR[256];
		pos = _stprintf_s(temp, 256, _T("USB Interrupt End intsts0=%04x:%04x intsts1=%04x:%04x\r\n"),
			m_Intsts0, *(uint16_t *)&m_UsbRegInst.INTSTS0, m_Intsts1, *(uint16_t *)&m_UsbRegInst.INTSTS1);
		_tprintf_s(temp);
		delete temp;
#endif
		return;
	}

	if(m_Timer != 0)
		return;

	// このブロックの終わりまで割り込み禁止
	CSifUSB::Lock lock(this);

	TimeOut();

	m_UsbRegInst.INTSTS0.BRDY = ((*(uint16_t *)&m_UsbRegInst.BRDYSTS & *(uint16_t *)&m_UsbRegInst.BRDYENB) == 0) ? 0 : 1;
	m_UsbRegInst.INTSTS0.NRDY = ((*(uint16_t *)&m_UsbRegInst.NRDYSTS & *(uint16_t *)&m_UsbRegInst.NRDYENB) == 0) ? 0 : 1;
	m_UsbRegInst.INTSTS0.BEMP = ((*(uint16_t *)&m_UsbRegInst.BEMPSTS & *(uint16_t *)&m_UsbRegInst.BEMPENB) == 0) ? 0 : 1;

	uint16_t intsts0 = *(uint16_t *)&m_UsbRegInst.INTSTS0 & *(uint16_t *)&m_UsbRegInst.INTENB0;
	uint16_t intsts1 = *(uint16_t *)&m_UsbRegInst.INTSTS1 & *(uint16_t *)&m_UsbRegInst.INTENB1;

	if((intsts0 != m_Intsts0) || (intsts1 != m_Intsts1)){
		if(((intsts0 & (intsts0 ^ m_Intsts0)) != 0) || ((intsts1 & (intsts1 ^ m_Intsts1)) != 0)){
#ifdef SIF_USB_DEBUG
			int pos;
			TCHAR *temp = new TCHAR[256];
			pos = _stprintf_s(temp, 256, _T("USB Interrupt Start intsts0=%04x intsts1=%04x\r\n"), intsts0, intsts1);
			_tprintf_s(temp);
			delete temp;
#endif
			m_Interrupting = true;
			g_Kernel->Interrupt(m_UsbioIntNo);
		}
		m_Intsts0 = intsts0;
		m_Intsts1 = intsts1;
	}
}

void CSifUSB::TimeOut()
{
	CUsbPipe *pipe;

#ifdef _DEBUG
	m_PrevTerm = m_Term;
#endif

	switch(m_Term){
	case usbtAttatch:
		m_UsbRegInst.SYSSTS0.LNST = 1; // Full Speed
		m_UsbRegInst.DVSTCTR0.RHST = 2; // Full Speed
		m_UsbRegInst.INTSTS1.ATTCH = 1;
		m_CurrentPipe = -1;
		m_Timer = -1;
		m_Term = usbtIdle;
		return;
	case usbtStartSetupStage:
		NextPipe();
		break;
	}

	if((m_CurrentPipe >= 0) && (m_CurrentPipe <= 9))
		pipe = &m_UsbPipe[m_CurrentPipe];
	else
		pipe = NULL;

	switch(m_Term){
	case usbtIdle:
		m_Timer = -1;
		break;
	case usbtSetupStageToken:
		SendSetupData(pipe);
		m_Term = usbtSetupStageData;
		m_Timer = -1;
		break;
	case usbtSetupStageHandshake:
		m_UsbRegInst.INTSTS1.SACK = 1;
		{
			pipe->ClearSUREQ();
		}
		m_Timer = -1;
		break;
	case usbtStartDataStage:
		if(pipe->GetDir()){
			SendDataOut(pipe);
			m_Term = usbtDataStageTokenOut;
			m_Timer = 1;
		}
		else{
			SendDataIn(pipe);
			m_Term = usbtDataStageTokenIn;
			m_Timer = -1;
		}
		break;
	case usbtDataStageTokenOut:
		SendData(pipe);
		m_Term = usbtDataStageDataOut;
		m_Timer = -1;
		break;
	case usbtDataStageDataIn:
		pipe->SetBRDY(true);
		pipe->ClearPID();
		SendHandshake(pipe);
		m_Term = usbtDataStageHandshakeIn;
		m_Timer = -1;
		break;
	case usbtDataStageDataNak:
		//pipe->SetNRDY(true);
		pipe->ClearPID();
		m_Term = usbtDataStageHandshakeIn;
		m_Timer = -1;
		break;
	case usbtStartStatusStage:
		pipe->SetBEMP(true);
		SendDataOut(pipe);
		m_Term = usbtStatusStageTokenOut;
		m_Timer = 1;
		break;
	case usbtDataStageHandshakeOut:
		pipe->SetBEMP(true);
		SendDataIn(pipe);
		m_Term = usbtStatusStageTokenIn;
		m_Timer = -1;
		break;
	case usbtStatusStageTokenOut:
		SendStatusData(pipe);
		m_Term = usbtStatusStageDataOut;
		m_Timer = -1;
		break;
	case usbtStatusStageDataIn:
		pipe->SetBRDY(true);
		SendHandshake(pipe);
		m_Term = usbtStatusStageHandshakeIn;
		m_Timer = -1;
		break;
	case usbtStatusStageHandshakeOut:
		pipe->SetBEMP(true);
		m_Term = usbtIdle;
		m_Timer = -1;
		break;
	case usbtStartData:
		if(pipe->GetDir()){
			SendDataOut(pipe);
			m_Term = usbtDataTokenOut;
			m_Timer = 1;
		}
		else{
			SendDataIn(pipe);
			m_Term = usbtDataTokenIn;
			m_Timer = -1;
		}
		break;
	case usbtDataDataIn:
		pipe->SetBRDY(true);
		pipe->ClearPID();
		SendHandshake(pipe);
		m_Term = usbtDataHandshakeIn;
		m_Timer = -1;
		break;
	case usbtDataDataNak:
		//pipe->SetNRDY(true);
		pipe->ClearPID();
		m_Term = usbtIdle;
		m_Timer = -1;
		break;
	case usbtDataTokenOut:
		SendData(pipe);
		m_Term = usbtDataDataOut;
		m_Timer = -1;
		break;
	case usbtDataHandshakeOut:
		//if(pipe->GetBVAL())
			pipe->SetBEMP(true);
		//else
			pipe->SetBRDY(true);
		m_Term = usbtIdle;
		m_Timer = -1;
		break;
	}

	if(m_Term == usbtIdle){
		NextPipe();
	}
}

const TCHAR *GetPID(uint8_t pid)
{
	switch(pid){
	case (PID_OUT	 << 4) | ((~PID_OUT		) & 0xF):
		return _T("OUT  ");
	case (PID_IN	 << 4) | ((~PID_IN		) & 0xF):
		return _T("IN   ");
	case (PID_SOF	 << 4) | ((~PID_SOF		) & 0xF):
		return _T("SOF  ");
	case (PID_SETUP	 << 4) | ((~PID_SETUP	) & 0xF):
		return _T("SETUP");
	case (PID_DATA0	 << 4) | ((~PID_DATA0	) & 0xF):
		return _T("DATA0");
	case (PID_DATA1	 << 4) | ((~PID_DATA1	) & 0xF):
		return _T("DATA1");
	case (PID_ACK	 << 4) | ((~PID_ACK		) & 0xF):
		return _T("ACK  ");
	case (PID_NAK	 << 4) | ((~PID_NAK		) & 0xF):
		return _T("NAK  ");
	case (PID_STALL	 << 4) | ((~PID_STALL	) & 0xF):
		return _T("STALL");
	case (PID_PRE	 << 4) | ((~PID_PRE		) & 0xF):
		return _T("PRE  ");
	default:
		return _T("???  ");
	}
}

void CSifUSB::Receive(const void *data, int size)
{
#ifdef SIF_USB_DEBUG
	int pos;
	TCHAR *temp = new TCHAR[256];
	pos = _stprintf_s(temp, 256, _T("USB Recv Pipe=%d size=%d %s: "), m_CurrentPipe, size - 2, GetPID(((uint8_t *)data)[1]));
	for(int i = 2; (i < size) && (pos < (256 - 5)); i++, pos += 3)
		_stprintf_s(&temp[pos], 256 - pos, _T("%02x "), ((uint8_t *)data)[i]);
	_tcscat_s(temp, 256, _T("\r\n"));
	_tprintf_s(temp);
	delete temp;
#endif
	// SYNC
	if((((uint8_t *)data)[0] != USB_SYNC) || (size < 2) || (size >= 2 + 1024))
		return;

	if(m_CurrentPipe == -1)
		throw std::exception();

	CUsbPipe *pipe = &m_UsbPipe[m_CurrentPipe];

	// PID
	switch(((uint8_t *)data)[1])
	{
	// DATA0
	case (PID_DATA0 << 4) | ((~PID_DATA0) & 0xF):
		switch(m_Term){
		case usbtDataStageTokenIn:
			pipe->Receive(&((uint8_t *)data)[2], size - 2);
			m_Term = usbtDataStageDataIn;
			m_Timer = 1;
			break;
		case usbtStatusStageTokenIn:
			pipe->Receive(&((uint8_t *)data)[2], size - 2);
			m_Term = usbtStatusStageDataIn;
			m_Timer = 1;
			break;
		case usbtDataTokenIn:
			pipe->Receive(&((uint8_t *)data)[2], size - 2);
			m_Term = usbtDataDataIn;
			m_Timer = 1;
			break;
		}
		break;
	// DATA1
	case (PID_DATA1 << 4) | ((~PID_DATA1) & 0xF):
		switch(m_Term){
		case usbtDataStageTokenIn:
			pipe->Receive(&((uint8_t *)data)[2], size - 2);
			m_Term = usbtDataStageDataIn;
			m_Timer = 1;
			break;
		case usbtStatusStageTokenIn:
			pipe->Receive(&((uint8_t *)data)[2], size - 2);
			m_Term = usbtStatusStageDataIn;
			m_Timer = 1;
			break;
		case usbtDataTokenIn:
			pipe->Receive(&((uint8_t *)data)[2], size - 2);
			m_Term = usbtDataDataIn;
			m_Timer = 1;
			break;
		}
		break;
	// ACK
	case (PID_ACK << 4) | ((~PID_ACK) & 0xF):
		switch(m_Term){
		case usbtSetupStageData:
			m_Term = usbtSetupStageHandshake;
			m_Timer = 1;
			break;
		case usbtDataStageDataOut:
			m_Term = usbtDataStageHandshakeOut;
			m_Timer = 1;
			break;
		case usbtStatusStageDataOut:
			m_Term = usbtStatusStageHandshakeOut;
			m_Timer = 1;
			break;
		case usbtDataDataOut:
			m_Term = usbtDataHandshakeOut;
			m_Timer = 1;
			break;
		}
		break;
	// NAK
	case (PID_NAK << 4) | ((~PID_NAK) & 0xF):
		switch(m_Term){
		case usbtSetupStageData:
			m_Term = usbtSetupStageHandshake;
			m_Timer = 1;
			break;
		case usbtDataStageTokenIn:
			//pipe->Receive(NULL, 0);
			m_Term = usbtDataStageDataNak;
			m_Timer = 1;
			break;
		case usbtDataStageDataOut:
			m_Term = usbtDataStageHandshakeOut;
			m_Timer = 1;
			break;
		case usbtStatusStageTokenIn:
			//pipe->Receive(NULL, 0);
			m_Term = usbtIdle;
			m_Timer = -1;
			break;
		case usbtStatusStageDataOut:
			m_Term = usbtStatusStageHandshakeOut;
			m_Timer = 1;
			break;
		case usbtDataTokenIn:
			//pipe->Receive(NULL, 0);
			m_Term = usbtDataDataNak;
			m_Timer = 1;
			break;
		case usbtDataDataOut:
			m_Term = usbtDataHandshakeOut;
			m_Timer = 1;
			break;
		}
		break;
	// STALL
	case (PID_STALL << 4) | ((~PID_STALL) & 0xF):
		switch(m_Term){
		case usbtSetupStageData:
			m_Term = usbtSetupStageHandshake;
			m_Timer = 1;
			break;
		case usbtDataStageTokenIn:
			//pipe->Receive(NULL, 0);
			m_Term = usbtDataStageDataNak;
			m_Timer = 1;
			break;
		case usbtDataStageDataOut:
			m_Term = usbtDataStageHandshakeOut;
			m_Timer = 1;
			break;
		case usbtStatusStageTokenIn:
			//pipe->Receive(NULL, 0);
			m_Term = usbtDataDataNak;
			m_Timer = 1;
			break;
		case usbtStatusStageDataOut:
			m_Term = usbtStatusStageHandshakeOut;
			m_Timer = 1;
			break;
		case usbtDataTokenIn:
			//pipe->Receive(NULL, 0);
			m_Term = usbtIdle;
			m_Timer = -1;
			break;
		case usbtDataDataOut:
			m_Term = usbtDataHandshakeOut;
			m_Timer = 1;
			break;
		}
		break;
	}
}

void CSifUSB::Attatch()
{
	Lock lock(this);

	m_Term = usbtAttatch;
	m_Timer = 1000 * 10000;
}

void CSifUSB::SendSetup(CUsbPipe *pipe)
{
	uint8_t data[4];
	int pos = 2;
	uint8_t addr, endp;

	// SYNC
	data[0] = USB_SYNC;
	// PID
	data[1] = (PID_SETUP << 4) | ((~PID_SETUP) & 0xF);
	// ADDR/ENDP
	addr = pipe->GetPIPEMAXP().DEVSEL;
	endp = pipe->GetPIPECFG().EPNUM;
	data[pos++] = (addr << 1) | (endp >> 3);
	data[pos++] = (endp << 5) | (/*CRC*/0);

	Send(data, pos);
}

void CSifUSB::SendDataOut(CUsbPipe *pipe)
{
	uint8_t data[4];
	int pos = 2;
	uint8_t addr, endp;

	pipe->SetSendReq(false);

	// SYNC
	data[0] = USB_SYNC;
	// PID
	data[1] = (PID_OUT << 4) | ((~PID_OUT) & 0xF);
	// ADDR/ENDP
	addr = pipe->GetPIPEMAXP().DEVSEL;
	endp = pipe->GetPIPECFG().EPNUM;
	data[pos++] = (addr << 1) | (endp >> 3);
	data[pos++] = (endp << 5) | (/*CRC*/0);

	Send(data, pos);
}

void CSifUSB::SendDataIn(CUsbPipe *pipe)
{
	uint8_t data[4];
	int pos = 2;
	uint8_t addr, endp;

	// SYNC
	data[0] = USB_SYNC;
	// PID
	data[1] = (PID_IN << 4) | ((~PID_IN) & 0xF);
	// ADDR/ENDP
	addr = pipe->GetPIPEMAXP().DEVSEL;
	endp = pipe->GetPIPECFG().EPNUM;
	data[pos++] = (addr << 1) | (endp >> 3);
	data[pos++] = (endp << 5) | (/*CRC*/0);

	Send(data, pos);
}

void CSifUSB::SendSetupData(CUsbPipe *pipe)
{
	uint8_t data[10];

	// SYNC
	data[0] = USB_SYNC;
	// PID
	data[1] = (PID_DATA0 << 4) | ((~PID_DATA0) & 0xF);
	// bRequestType/bRequest
	*(uint16_t *)&data[2] = m_UsbRegInst.USBREQ;
	// wValue
	*(uint16_t *)&data[4] = m_UsbRegInst.USBVAL;
	// wIndex
	*(uint16_t *)&data[6] = m_UsbRegInst.USBINDX;
	// wLength
	*(uint16_t *)&data[8] = m_UsbRegInst.USBLENG;

	Send(data, sizeof(data));
}

void CSifUSB::SendData(CUsbPipe *pipe)
{
	int size = 2 + pipe->GetSendSize();
	uint8_t *data = new uint8_t[size];
	uint8_t pid;

	// SYNC
	data[0] = USB_SYNC;
	// PID
	if(!pipe->GetData1())
		pid = PID_DATA0;
	else
		pid = PID_DATA1;
	data[1] = (pid << 4) | ((~pid) & 0xF);

	pipe->SendBytes(&data[2], size - 2);

	Send(data, size);

	delete[] data;
}

void CSifUSB::SendStatusData(CUsbPipe *pipe)
{
	uint8_t data[2];

	// SYNC
	data[0] = USB_SYNC;
	// PID
	data[1] = (PID_DATA1 << 4) | ((~PID_DATA1) & 0xF);

	Send(data, sizeof(data));
}

void CSifUSB::SendHandshake(CUsbPipe *pipe)
{
	uint8_t data[2];
	uint8_t pid;

	// SYNC
	data[0] = USB_SYNC;
	// PID
	switch(pipe->GetPIPECTR().PID){
	case 0: pid = PID_NAK; break;
	case 1: pid = PID_ACK; break;
	default: pid = PID_STALL; break;
	}
	data[1] = (pid << 4) | ((~pid) & 0xF);

	Send(data, sizeof(data));
}

void CSifUSB::Run(int pipeNo, TRunRequest req)
{
	TUSB0Term term = usbtIdle;

	Lock lock(this);

	switch(m_Term){
	case usbtIdle:
		if(req == RUN_SUREQ)
			term = usbtStartSetupStage;
		else if(req == RUN_PID)
			term = usbtStartData;
		break;
	case usbtSetupStageHandshake:
		if(req == RUN_PID)
			term = usbtStartDataStage;
		break;
	case usbtDataStageHandshakeIn:
		if((req == RUN_BVAL) || (req == RUN_SUREQ))
			term = usbtStartStatusStage;
		break;
	}

	if(term == usbtIdle)
		return;

	if(m_CurrentPipe == -1)
		m_CurrentPipe = pipeNo;
	else if(m_CurrentPipe != pipeNo){
		if(req == RUN_PID)
			m_UsbPipe[pipeNo].SetSendReq(true);
		return;
	}

	m_Term = term;
	m_Timer = 1;
	g_Kernel->OnSetEvent();
}

void CSifUSB::NextPipe()
{
	switch(m_Term){
	case usbtIdle:
	case usbtStartSetupStage:
	//case usbtDataStageHandshakeOut:
	//case usbtDataStageHandshakeIn:
		break;
	default:
		return;
	}

	int pos = m_CurrentPipe;
	if((pos == -1) || (pos >= sizeof(m_UsbPipe) / sizeof(*m_UsbPipe)))
		pos = 0;
	else
		pos++;

	m_CurrentPipe = -1;
	for(int i = 0; i < sizeof(m_UsbPipe) / sizeof(*m_UsbPipe); pos++, i++)
	{
		if(pos >= sizeof(m_UsbPipe) / sizeof(*m_UsbPipe))
			pos = 0;

		CUsbPipe *pipe = &m_UsbPipe[pos];

		if(!pipe->IsAssignd())
			continue;

		switch(pipe->GetRequest(m_Term == usbtIdle))
		{
		case 1:
			m_CurrentPipe = pos;
			SendSetup(pipe);
			m_Term = usbtSetupStageToken;
			m_Timer = 1;
			break;
		case 2:
			m_CurrentPipe = pos;
			SendDataOut(pipe);
			m_Term = usbtDataTokenOut;
			m_Timer = 1;
			break;
		case 3:
			if(m_Term != usbtIdle || !m_Bluetooth.HasEvent())
				continue;

			m_CurrentPipe = pos;
			SendDataIn(pipe);
			m_Term = usbtDataTokenIn;
			m_Timer = -1;
			break;
		default:
			continue;
		}
		break;
	}

	if(m_CurrentPipe == -1){
		m_Timer = -1;
		m_Term = usbtIdle;
		return;
	}
}

void CSifUSB::Send(const void *data, int size)
{
#ifdef SIF_USB_DEBUG
	int pos;
	TCHAR *temp = new TCHAR[256];
	pos = _stprintf_s(temp, 256, _T("USB Send Pipe=%d size=%d %s: "), m_CurrentPipe, size - 2, GetPID(((uint8_t *)data)[1]));
	for(int i = 2; (i < size) && (pos < (256 - 5)); i++, pos += 3)
		_stprintf_s(&temp[pos], 256 - pos, _T("%02x "), ((uint8_t *)data)[i]);
	_tcscat_s(temp, 256, _T("\r\n"));
	_tprintf_s(temp);
	delete temp;
#endif
	m_Bluetooth.Receive(data, size);
}
