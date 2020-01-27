//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SifEtherCtrl.cpp 1970 2019-07-10 00:36:30Z coas-nagasima $
#include "StdAfx.h"
#include "WinKernel.h"
#include "SifEtherCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CKernel *g_Kernel;

CSifEtherCtrl::CSifEtherCtrl(TRX62NEDMACReg *edmacReg, TRX62NETHERCReg *ethercReg,
	int intNo, int devNo)
{
	m_EdmacReg = edmacReg;
	m_EthercReg = ethercReg;
	m_IntNo = intNo;
	m_DevNo = devNo;

	InitializeCriticalSection(&m_Lock);

	m_SendTimer = -1;
	m_SendTerm = ecstIdle;
	m_RecvTimer = -1;
	m_RecvTerm = ecrtIdle;
	m_TxDsp = NULL;
	m_SendReq = false;
	m_SendBufPos = 0;
	m_RxDsp = NULL;
	m_RecvBuf = NULL;
	m_RecvBufPos = 0;

	// 初期値設定
	memset(&m_EdmacRegInst, 0, sizeof(m_EdmacRegInst));
	m_EdmacRegInst.FCFTR = 0x00070007;
	memset(&m_EthercRegInst, 0, sizeof(m_EthercRegInst));
	m_EthercRegInst.IPGR = 0x00000014;

	m_EthercRegInst.PSR = 0x00000001;
}

CSifEtherCtrl::~CSifEtherCtrl()
{
	TEtherCtrlDataNode *Node;

	while(!m_RecvQueue.empty()){
		// このブロックの終わりまで割り込み禁止
		Lock lock(this);

		// キューから次のメッセージを取得
		Node = m_RecvQueue.front();
		m_RecvQueue.pop_front();
		delete (unsigned char *)Node;
	}

	DeleteCriticalSection(&m_Lock);
}

unsigned char CSifEtherCtrl::GetByte(unsigned int addr)
{
	throw std::exception();
}

void CSifEtherCtrl::SetByte(unsigned int addr, unsigned char value)
{
	throw std::exception();
}

unsigned short CSifEtherCtrl::GetUInt16(unsigned int addr)
{
	throw std::exception();
}

void CSifEtherCtrl::SetUInt16(unsigned int addr, unsigned short value)
{
	throw std::exception();
}

unsigned int CSifEtherCtrl::GetUInt32(unsigned int addr)
{
	unsigned int *reg, value;
	int index;

	if((addr >= (intptr_t)m_EdmacReg)
		&& (addr < ((intptr_t)m_EdmacReg + sizeof(TRX62NEDMACReg))))
	{
		reg = (unsigned int *)&m_EdmacRegInst;
		index = (int)addr - (int)m_EdmacReg;

		value = reg[index / sizeof(*reg)];

		switch (index)
		{
		// EDMACモードレジスタ
		case offsetof(TRX62NEDMACReg, EDMR):
			break;
		// EDMAC送信要求レジスタ
		case offsetof(TRX62NEDMACReg, EDTRR):
			break;
		// EDMAC受信要求レジスタ
		case offsetof(TRX62NEDMACReg, EDRRR):
			break;
		// 送信ディスクリプタリスト先頭アドレスレジスタ
		case offsetof(TRX62NEDMACReg, TDLAR):
			break;
		// 受信ディスクリプタリスト先頭アドレスレジスタ
		case offsetof(TRX62NEDMACReg, RDLAR):
			break;
		// ETHERC/EDMACステータスレジスタ
		case offsetof(TRX62NEDMACReg, EESR):
			break;
		// ETHERC/EDMACステータス割り込み許可レジスタ
		case offsetof(TRX62NEDMACReg, EESIPR):
			break;
		// 送受信ステータスコピー指示レジスタ
		case offsetof(TRX62NEDMACReg, TRSCER):
			break;
		// ミスドフレームカウンタレジスタ
		case offsetof(TRX62NEDMACReg, RMFCR):
			break;
		// 送信FIFOしきい値指定レジスタ
		case offsetof(TRX62NEDMACReg, TFTR):
			break;
		// FIFO容量指定レジスタ
		case offsetof(TRX62NEDMACReg, FDR):
			break;
		// 受信方式制御レジスタ
		case offsetof(TRX62NEDMACReg, RMCR):
			break;
		// 送信FIFOアンダランカウント
		case offsetof(TRX62NEDMACReg, TFUCR):
			break;
		// 受信FIFOオーバフローカウント
		case offsetof(TRX62NEDMACReg, RFOCR):
			break;
		// 個別出力信号設定レジスタ
		case offsetof(TRX62NEDMACReg, IOSR):
			break;
		// フロー制御開始FIFOしきい値設定レジスタ
		case offsetof(TRX62NEDMACReg, FCFTR):
			break;
		// 受信データパディング挿入設定レジスタ
		case offsetof(TRX62NEDMACReg, RPADIR):
			break;
		// 送信割り込み設定レジスタ
		case offsetof(TRX62NEDMACReg, TRIMD):
			break;
		// 受信バッファライトアドレスレジスタ
		case offsetof(TRX62NEDMACReg, RBWAR):
			break;
		// 受信ディスクリプタフェッチアドレスレジスタ
		case offsetof(TRX62NEDMACReg, RDFAR):
			break;
		// 送信バッファリードアドレスレジスタ
		case offsetof(TRX62NEDMACReg, TBRAR):
			break;
		// 送信ディスクリプタフェッチアドレスレジスタ
		case offsetof(TRX62NEDMACReg, TDFAR):
			break;
		}
	}
	else if((addr >= (intptr_t)m_EthercReg)
		&& (addr < ((intptr_t)m_EthercReg + sizeof(TRX62NETHERCReg))))
	{
		reg = (unsigned int *)&m_EthercRegInst;
		index = (int)addr - (int)m_EthercReg;

		value = reg[index / sizeof(*reg)];

		switch (index)
		{
		// ETHERCモードレジスタ
		case offsetof(TRX62NETHERCReg, ECMR):
			break;
		// 受信フレーム長上限レジスタ
		case offsetof(TRX62NETHERCReg, RFLR):
			break;
		// ETHERCステータスレジスタ
		case offsetof(TRX62NETHERCReg, ECSR):
			break;
		// ETHERC割り込み許可レジスタ
		case offsetof(TRX62NETHERCReg, ECSIPR):
			break;
		// PHY部インタフェースレジスタ
		case offsetof(TRX62NETHERCReg, PIR):
			break;
		// PHY部ステータスレジスタ
		case offsetof(TRX62NETHERCReg, PSR):
			break;
		// 乱数生成カウンタ上限値設定レジスタ
		case offsetof(TRX62NETHERCReg, RDMLR):
			break;
		// IPG設定レジスタ
		case offsetof(TRX62NETHERCReg, IPGR):
			break;
		// 自動PAUSEフレーム設定レジスタ
		case offsetof(TRX62NETHERCReg, APR):
			break;
		// 手動PAUSEフレーム設定レジスタ
		case offsetof(TRX62NETHERCReg, MPR):
			break;
		// 受信PAUSEフレームカウンタ
		case offsetof(TRX62NETHERCReg, RFCF):
			break;
		// 自動PAUSEフレーム再送回数設定レジスタ
		case offsetof(TRX62NETHERCReg, TPAUSER):
			break;
		// PAUSEフレーム再送回数カウンタ
		case offsetof(TRX62NETHERCReg, TPAUSECR):
			break;
		// Broadcastフレーム受信回数設定レジスタ
		case offsetof(TRX62NETHERCReg, BCFRR):
			break;
		// MACアドレス上位設定レジスタ
		case offsetof(TRX62NETHERCReg, MAHR):
			break;
		// MACアドレス下位設定レジスタ
		case offsetof(TRX62NETHERCReg, MALR):
			break;
		// 送信リトライオーバカウンタレジスタ
		case offsetof(TRX62NETHERCReg, TROCR):
			break;
		// 遅延衝突検出カウンタレジスタ
		case offsetof(TRX62NETHERCReg, CDCR):
			break;
		// キャリア消失カウンタレジスタ
		case offsetof(TRX62NETHERCReg, LCCR):
			break;
		// キャリア未検出カウンタレジスタ
		case offsetof(TRX62NETHERCReg, CNDCR):
			break;
		// CRCエラーフレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, CEFCR):
			break;
		// フレーム受信エラーカウンタレジスタ
		case offsetof(TRX62NETHERCReg, FRECR):
			break;
		// 64バイト未満フレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, TSFRCR):
			break;
		// 指定バイト超フレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, TLFRCR):
			break;
		// 端数ビットフレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, RFCR):
			break;
		// マルチキャストアドレスフレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, MAFCR):
			break;
		}
	}
	else {
		throw std::exception();
	}

	return value;
}

void CSifEtherCtrl::SetUInt32(unsigned int addr, unsigned int value)
{
	unsigned int *reg;
	int index;

	if((addr >= (intptr_t)m_EdmacReg)
		&& (addr < ((intptr_t)m_EdmacReg + sizeof(TRX62NEDMACReg))))
	{
		reg = (unsigned int *)&m_EdmacRegInst;
		index = (int)addr - (int)m_EdmacReg;

		switch (index)
		{
		// EDMACモードレジスタ
		case offsetof(TRX62NEDMACReg, EDMR):
			break;
		// EDMAC送信要求レジスタ
		case offsetof(TRX62NEDMACReg, EDTRR):
			if(((TEDMACEDTRR *)&value)->TR != 0){
				// このブロックの終わりまで割り込み禁止
				Lock lock(this);

				m_SendReq = true;
				if(m_SendTerm == ecstIdle)
				{
					m_SendTerm = ecstSend;
					m_SendTimer = 0;
					g_Kernel->OnSetEvent();
				}
			}
			break;
		// EDMAC受信要求レジスタ
		case offsetof(TRX62NEDMACReg, EDRRR):
			if(((TEDMACEDRRR *)&value)->RR != 0){
				// このブロックの終わりまで割り込み禁止
				Lock lock(this);

				if(m_RecvTerm != ecrtRecv)
				{
					if((m_RecvBuf == NULL) && (!m_RecvQueue.empty())){
						m_RecvBuf = m_RecvQueue.front();
						m_RecvQueue.pop_front();
					}
					if((m_RecvBuf != NULL) && (m_RecvTerm == ecrtIdle)){
						m_RecvTerm = ecrtRecv;
						m_RecvTimer = 0;
						g_Kernel->OnSetEvent();
					}
				}
			}
			break;
		// 送信ディスクリプタリスト先頭アドレスレジスタ
		case offsetof(TRX62NEDMACReg, TDLAR):
			{
				// このブロックの終わりまで割り込み禁止
				Lock lock(this);

				m_TxDsp = (TRX62NEtherCTxDescriper *)value;
				if(m_TxDsp != NULL){
					m_EdmacRegInst.EDTRR.TR = m_TxDsp->TACT;
				}
			}
			break;
		// 受信ディスクリプタリスト先頭アドレスレジスタ
		case offsetof(TRX62NEDMACReg, RDLAR):
			{
				// このブロックの終わりまで割り込み禁止
				Lock lock(this);

				m_RxDsp = (TRX62NEtherCRxDescriper *)value;
				if(m_RxDsp != NULL){
					m_EdmacRegInst.EDRRR.RR = m_RxDsp->RACT;
				}
			}
			break;
		// ETHERC/EDMACステータスレジスタ
		case offsetof(TRX62NEDMACReg, EESR):
			reg[index / sizeof(*reg)] &= ~value;
			return;
		// ETHERC/EDMACステータス割り込み許可レジスタ
		case offsetof(TRX62NEDMACReg, EESIPR):
			break;
		// 送受信ステータスコピー指示レジスタ
		case offsetof(TRX62NEDMACReg, TRSCER):
			break;
		// ミスドフレームカウンタレジスタ
		case offsetof(TRX62NEDMACReg, RMFCR):
			break;
		// 送信FIFOしきい値指定レジスタ
		case offsetof(TRX62NEDMACReg, TFTR):
			break;
		// FIFO容量指定レジスタ
		case offsetof(TRX62NEDMACReg, FDR):
			break;
		// 受信方式制御レジスタ
		case offsetof(TRX62NEDMACReg, RMCR):
			break;
		// 送信FIFOアンダランカウント
		case offsetof(TRX62NEDMACReg, TFUCR):
			break;
		// 受信FIFOオーバフローカウント
		case offsetof(TRX62NEDMACReg, RFOCR):
			break;
		// 個別出力信号設定レジスタ
		case offsetof(TRX62NEDMACReg, IOSR):
			break;
		// フロー制御開始FIFOしきい値設定レジスタ
		case offsetof(TRX62NEDMACReg, FCFTR):
			break;
		// 受信データパディング挿入設定レジスタ
		case offsetof(TRX62NEDMACReg, RPADIR):
			break;
		// 送信割り込み設定レジスタ
		case offsetof(TRX62NEDMACReg, TRIMD):
			break;
		// 受信バッファライトアドレスレジスタ
		case offsetof(TRX62NEDMACReg, RBWAR):
			break;
		// 受信ディスクリプタフェッチアドレスレジスタ
		case offsetof(TRX62NEDMACReg, RDFAR):
			break;
		// 送信バッファリードアドレスレジスタ
		case offsetof(TRX62NEDMACReg, TBRAR):
			break;
		// 送信ディスクリプタフェッチアドレスレジスタ
		case offsetof(TRX62NEDMACReg, TDFAR):
			break;
		}
	}
	else if((addr >= (intptr_t)m_EthercReg)
		&& (addr < ((intptr_t)m_EthercReg + sizeof(TRX62NETHERCReg))))
	{
		reg = (unsigned int *)&m_EthercRegInst;
		index = (int)addr - (int)m_EthercReg;

		switch (index)
		{
		// ETHERCモードレジスタ
		case offsetof(TRX62NETHERCReg, ECMR):
			break;
		// 受信フレーム長上限レジスタ
		case offsetof(TRX62NETHERCReg, RFLR):
			break;
		// ETHERCステータスレジスタ
		case offsetof(TRX62NETHERCReg, ECSR):
			reg[index / sizeof(*reg)] &= ~value;
			break;
		// ETHERC割り込み許可レジスタ
		case offsetof(TRX62NETHERCReg, ECSIPR):
			break;
		// PHY部インタフェースレジスタ
		case offsetof(TRX62NETHERCReg, PIR):
			break;
		// PHY部ステータスレジスタ
		case offsetof(TRX62NETHERCReg, PSR):
			break;
		// 乱数生成カウンタ上限値設定レジスタ
		case offsetof(TRX62NETHERCReg, RDMLR):
			break;
		// IPG設定レジスタ
		case offsetof(TRX62NETHERCReg, IPGR):
			break;
		// 自動PAUSEフレーム設定レジスタ
		case offsetof(TRX62NETHERCReg, APR):
			break;
		// 手動PAUSEフレーム設定レジスタ
		case offsetof(TRX62NETHERCReg, MPR):
			break;
		// 受信PAUSEフレームカウンタ
		case offsetof(TRX62NETHERCReg, RFCF):
			break;
		// 自動PAUSEフレーム再送回数設定レジスタ
		case offsetof(TRX62NETHERCReg, TPAUSER):
			break;
		// PAUSEフレーム再送回数カウンタ
		case offsetof(TRX62NETHERCReg, TPAUSECR):
			break;
		// Broadcastフレーム受信回数設定レジスタ
		case offsetof(TRX62NETHERCReg, BCFRR):
			break;
		// MACアドレス上位設定レジスタ
		case offsetof(TRX62NETHERCReg, MAHR):
			m_MacAddr[0] = ((unsigned char *)&value)[3];
			m_MacAddr[1] = ((unsigned char *)&value)[2];
			m_MacAddr[2] = ((unsigned char *)&value)[1];
			m_MacAddr[3] = ((unsigned char *)&value)[0];
			break;
		// MACアドレス下位設定レジスタ
		case offsetof(TRX62NETHERCReg, MALR):
			m_MacAddr[4] = ((unsigned char *)&value)[1];
			m_MacAddr[5] = ((unsigned char *)&value)[0];
			break;
		// 送信リトライオーバカウンタレジスタ
		case offsetof(TRX62NETHERCReg, TROCR):
			break;
		// 遅延衝突検出カウンタレジスタ
		case offsetof(TRX62NETHERCReg, CDCR):
			break;
		// キャリア消失カウンタレジスタ
		case offsetof(TRX62NETHERCReg, LCCR):
			break;
		// キャリア未検出カウンタレジスタ
		case offsetof(TRX62NETHERCReg, CNDCR):
			break;
		// CRCエラーフレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, CEFCR):
			break;
		// フレーム受信エラーカウンタレジスタ
		case offsetof(TRX62NETHERCReg, FRECR):
			break;
		// 64バイト未満フレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, TSFRCR):
			break;
		// 指定バイト超フレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, TLFRCR):
			break;
		// 端数ビットフレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, RFCR):
			break;
		// マルチキャストアドレスフレーム受信カウンタレジスタ
		case offsetof(TRX62NETHERCReg, MAFCR):
			break;
		}
	}
	else {
		throw std::exception();
	}

	reg[index / sizeof(*reg)] = value;
}

__int64 CSifEtherCtrl::GetTimer()
{
	__int64 timer = m_SendTimer;
	__int64 timer2 = m_RecvTimer;

	if((timer == -1) || ((timer2 != -1) && (timer > timer2)))
		timer = timer2;

	return timer;
}

void CSifEtherCtrl::Progress(__int64 Timer)
{
	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	if(m_SendTimer != -1){
		m_SendTimer -= Timer;
		if(m_SendTimer < 0){
			m_SendTimer = 0;
		}
	}
	if(m_RecvTimer != -1){
		m_RecvTimer -= Timer;
		if(m_RecvTimer < 0){
			m_RecvTimer = 0;
		}
	}
}

void CSifEtherCtrl::CallTimeOut()
{
	uint32_t eesr = *(uint32_t *)&m_EdmacRegInst.EESR
		& *(uint32_t *)&m_EdmacRegInst.EESIPR;

	CallSendTimeOut();
	CallRecvTimeOut();

	if(eesr != (*(uint32_t *)&m_EdmacRegInst.EESR & *(uint32_t *)&m_EdmacRegInst.EESIPR)){
		g_Kernel->Interrupt(m_IntNo);
	}
}

void CSifEtherCtrl::CallSendTimeOut()
{
	if(m_SendTimer != 0)
		return;

	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	switch(m_SendTerm){
	case ecstIdle:
		break;
	case ecstSend:
		for(;;){
			// 送信済みのバッファ・ディスクリプタ
			if((m_TxDsp == NULL) || (m_TxDsp->TACT == 0))
			{
				m_EdmacRegInst.EESR.TDE = 1;
				m_SendTerm = ecstTxInt;
				m_SendTimer = 1;
			}
			else{
				m_SendReq = false;

				unsigned short len = m_TxDsp->Size;
				if(m_SendBufPos + len > sizeof(m_SendBuf))
					len = sizeof(m_SendBuf) - m_SendBufPos;
				// 送信
				memcpy(&m_SendBuf[m_SendBufPos], m_TxDsp->Pointer, len);
				m_SendBufPos += len;
				if(((m_TxDsp->TFP & 0x2) != 0) || (m_SendBufPos == sizeof(m_SendBuf))){
					g_Kernel->Output(m_DevNo, m_SendBuf, m_SendBufPos);
					m_SendBufPos = 0;
				}
				if(len != m_TxDsp->Size){
					m_EdmacRegInst.EESR.TFUF = 1;
					m_EdmacRegInst.EESR.TWB = 1;
				}
				m_TxDsp->TACT = 0;
				if (m_TxDsp->TDLE != 0)
					m_TxDsp = (TRX62NEtherCTxDescriper *)m_EdmacRegInst.TDLAR;
				else
					m_TxDsp++;

				if (m_TxDsp != NULL)
					m_EdmacRegInst.EDTRR.TR = m_TxDsp->TACT;

				m_EdmacRegInst.EESR.TC = 1;
				m_SendTerm = ecstTxInt;
				m_SendTimer = 1;
			}
			return;
		}
		break;
	case ecstTxInt:
		m_SendReq = m_EdmacRegInst.EDTRR.TR != 0;
		break;
	default:
		return;
	}

	// 送信処理あり
	if(m_SendReq) {
		m_SendTimer = 1;
		m_SendTerm = ecstSend;
	}
	else{
		m_SendTimer = -1;
		m_SendTerm = ecstIdle;
	}
}

void CSifEtherCtrl::CallRecvTimeOut()
{
	if(m_RecvTimer != 0)
		return;

	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	switch(m_RecvTerm){
	case ecrtIdle:
		break;
	case ecrtRecv:
		for(;;){
			// 受信済みのバッファ・ディスクリプタ
			if((m_RxDsp == NULL) || (m_RxDsp->RACT == 0))
			{
				m_EdmacRegInst.EESR.RDE = 1;
				m_RecvTerm = ecrtRxInt;
				m_RecvTimer = 1;
			}
			else{
				unsigned int len = m_RxDsp->Capacity;
				if(len > (m_RecvBuf->Size - m_RecvBufPos))
					len = m_RecvBuf->Size - m_RecvBufPos;
				m_RxDsp->Size = len;
				// 受信
				memcpy(m_RxDsp->Pointer, &m_RecvBuf->Data[m_RecvBufPos], len);
				m_RecvBufPos += len;
				if(m_RecvBufPos >= m_RecvBuf->Size){
					delete m_RecvBuf;
					if(m_RecvQueue.empty()){
						m_RecvBuf = NULL;
					}
					else{
						m_RecvBuf = m_RecvQueue.front();
						m_RecvQueue.pop_front();
					}
					m_RecvBufPos = 0;

					m_RxDsp->RFP = 0x3;
				}
				m_RxDsp->RACT = 0;
				m_RxDsp->Status = 0;
				if(m_RxDsp->RDLE != 0)
					m_RxDsp = (TRX62NEtherCRxDescriper *)m_EdmacRegInst.RDLAR;
				else
					m_RxDsp++;

				if(m_RxDsp != NULL)
					m_EdmacRegInst.EDRRR.RR = m_RxDsp->RACT;

				m_EdmacRegInst.EESR.FR = 1;
				m_RecvTerm = ecrtRxInt;
				m_RecvTimer = 1;
			}
			return;
		}
		break;
	case ecrtRxInt:
		break;
	default:
		return;
	}

	// 受信処理あり
	if(m_RecvBuf != NULL){
		m_RecvTimer = 1;
		m_RecvTerm = ecrtRecv;
	}
	else{
		m_RecvTimer = -1;
		m_RecvTerm = ecrtIdle;
	}
}

void CSifEtherCtrl::Receive(const void *Data, int Size)
{
	if(m_EdmacRegInst.EDRRR.RR == 0)
		return;

	static const unsigned char BrodcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	TEtherCtrlDataNode *Node = (TEtherCtrlDataNode *)new unsigned char[offsetof(TEtherCtrlDataNode, Data) + Size];

	Node->Size = Size;
	memcpy((unsigned char *)&Node->Data, Data, Size);

	bool recv = (m_EthercRegInst.ECMR.PRM == 1);

	// MACアドレスを確認
	if(!recv
		&& (memcmp(Node->EthernetData.DstAddr, m_MacAddr, sizeof(m_MacAddr)) == 0))
	{
		recv = true;
	}

	// マルチキャストを確認
	if(!recv
		&& ((Node->EthernetData.DstAddr[0] & 0x01) != 0) && (Node->EthernetData.DstAddr[0] != 0xFF))
	{
		recv = true;
	}

	// ブロードキャストアドレスを確認
	if(!recv
		&& (memcmp(Node->EthernetData.DstAddr, BrodcastAddr, sizeof(BrodcastAddr)) == 0))
	{
		recv = true;
	}

	if(!recv)
	{
		delete[] Node;
		return;
	}

	// このブロックの終わりまで割り込み禁止
	Lock lock(this);

	if(m_RecvBuf == NULL){
		m_RecvBuf = Node;
		m_RecvBufPos = 0;
	}
	else{
		// メッセージをキューイング
		m_RecvQueue.push_back(Node);
	}

	if(m_RecvTerm == ecrtIdle){
		m_RecvTimer = 1;
		m_RecvTerm = ecrtRecv;
		g_Kernel->OnSetEvent();
	}
}
