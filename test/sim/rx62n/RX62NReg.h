//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: RX62NReg.h 1970 2019-07-10 00:36:30Z coas-nagasima $
#ifndef RX62NRegH
#define RX62NRegH

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

#pragma pack(push, 1)

#define SPACE(end, size) unsigned char _SPACE_TO_##end[size];
#define FIELD_S(start, end, type, name) type name; SPACE(end, end + 1 - start - sizeof(type))
#define FIELD_0(start, end, type, name) type name;
#define ARRAY_S(start, end, type, name, count) type name[count]; SPACE(end, end + 1 - start - sizeof(type) * count)
#define ARRAY_0(start, end, type, name, count) type name[count];

//------------------------------------------------------------------------------
// CPUレジスタ
//------------------------------------------------------------------------------
struct TRX62NSYSTEMReg
{
	FIELD_0(0x00080000, 0x00080001, unsigned short, MDMONR)	// モードモニタレジスタ
	FIELD_S(0x00080002, 0x00080005, unsigned short, MDSR)	// モードステータスレジスタ
	FIELD_0(0x00080006, 0x00080007, unsigned short, SYSCR0)	// システムコントロールレジスタ0
	FIELD_S(0x00080008, 0x0008000B, unsigned short, SYSCR1)	// システムコントロールレジスタ1
	FIELD_S(0x0008000C, 0x0008000F, unsigned short, SBYCR)	// スタンバイコントロールレジスタ
	FIELD_0(0x00080010, 0x00080013, unsigned int, MSTPCRA)	// モジュールストップコントロールレジスタA
	FIELD_0(0x00080014, 0x00080017, unsigned int, MSTPCRB)	// モジュールストップコントロールレジスタB
	FIELD_S(0x00080018, 0x0008001F, unsigned int, MSTPCRC)	// モジュールストップコントロールレジスタC
	FIELD_S(0x00080020, 0x0008002F, unsigned int, SCKCR)	// システムクロックコントロールレジスタ
	FIELD_S(0x00080030, 0x0008003F, unsigned char, BCKCR)	// 外部バスクロックコントロールレジスタ
	FIELD_S(0x00080040, 0x0008C27F, unsigned short, OSTDCR)	// 発振停止検出コントロールレジスタ
	FIELD_0(0x0008C280, 0x0008C280, unsigned char, DPSBYCR)	// ディープスタンバイコントロールレジスタ
	FIELD_0(0x0008C281, 0x0008C281, unsigned char, DPSWCR)	// ディープスタンバイウェイトコントロールレジスタ
	FIELD_0(0x0008C282, 0x0008C282, unsigned char, DPSIER)	// ディープスタンバイインタラプトイネーブルレジスタ
	FIELD_0(0x0008C283, 0x0008C283, unsigned char, DPSIFR)	// ディープスタンバイインタラプトフラグレジスタ
	FIELD_0(0x0008C284, 0x0008C284, unsigned char, DPSIEGR)	// ディープスタンバイインタラプトエッジレジスタ
	FIELD_S(0x0008C285, 0x0008C289, unsigned char, RSTSR)	// リセットステータスレジスタ
	FIELD_S(0x0008C28A, 0x0008C28B, unsigned char, SUBOSCCR)	// サブクロック発振器コントロールレジスタ
	FIELD_0(0x0008C28C, 0x0008C28C, unsigned char, LVDKEYR)	// 低電圧検出コントロール用キーコードレジスタ
	FIELD_S(0x0008C28D, 0x0008C28F, unsigned char, LVDCR)	// 低電圧検出コントロールレジスタ
	FIELD_0(0x0008C290, 0x0008C290, unsigned char, DPSBKR0)	// ディープスタンバイバックアップレジスタ0
	FIELD_0(0x0008C291, 0x0008C291, unsigned char, DPSBKR1)	// ディープスタンバイバックアップレジスタ1
	FIELD_0(0x0008C292, 0x0008C292, unsigned char, DPSBKR2)	// ディープスタンバイバックアップレジスタ2
	FIELD_0(0x0008C293, 0x0008C293, unsigned char, DPSBKR3)	// ディープスタンバイバックアップレジスタ3
	FIELD_0(0x0008C294, 0x0008C294, unsigned char, DPSBKR4)	// ディープスタンバイバックアップレジスタ4
	FIELD_0(0x0008C295, 0x0008C295, unsigned char, DPSBKR5)	// ディープスタンバイバックアップレジスタ5
	FIELD_0(0x0008C296, 0x0008C296, unsigned char, DPSBKR6)	// ディープスタンバイバックアップレジスタ6
	FIELD_0(0x0008C297, 0x0008C297, unsigned char, DPSBKR7)	// ディープスタンバイバックアップレジスタ7
	FIELD_0(0x0008C298, 0x0008C298, unsigned char, DPSBKR8)	// ディープスタンバイバックアップレジスタ8
	FIELD_0(0x0008C299, 0x0008C299, unsigned char, DPSBKR9)	// ディープスタンバイバックアップレジスタ9
	FIELD_0(0x0008C29A, 0x0008C29A, unsigned char, DPSBKR10)	// ディープスタンバイバックアップレジスタ10
	FIELD_0(0x0008C29B, 0x0008C29B, unsigned char, DPSBKR11)	// ディープスタンバイバックアップレジスタ11
	FIELD_0(0x0008C29C, 0x0008C29C, unsigned char, DPSBKR12)	// ディープスタンバイバックアップレジスタ12
	FIELD_0(0x0008C29D, 0x0008C29D, unsigned char, DPSBKR13)	// ディープスタンバイバックアップレジスタ13
	FIELD_0(0x0008C29E, 0x0008C29E, unsigned char, DPSBKR14)	// ディープスタンバイバックアップレジスタ14
	FIELD_0(0x0008C29F, 0x0008C29F, unsigned char, DPSBKR15)	// ディープスタンバイバックアップレジスタ15
	FIELD_0(0x0008C2A0, 0x0008C2A0, unsigned char, DPSBKR16)	// ディープスタンバイバックアップレジスタ16
	FIELD_0(0x0008C2A1, 0x0008C2A1, unsigned char, DPSBKR17)	// ディープスタンバイバックアップレジスタ17
	FIELD_0(0x0008C2A2, 0x0008C2A2, unsigned char, DPSBKR18)	// ディープスタンバイバックアップレジスタ18
	FIELD_0(0x0008C2A3, 0x0008C2A3, unsigned char, DPSBKR19)	// ディープスタンバイバックアップレジスタ19
	FIELD_0(0x0008C2A4, 0x0008C2A4, unsigned char, DPSBKR20)	// ディープスタンバイバックアップレジスタ20
	FIELD_0(0x0008C2A5, 0x0008C2A5, unsigned char, DPSBKR21)	// ディープスタンバイバックアップレジスタ21
	FIELD_0(0x0008C2A6, 0x0008C2A6, unsigned char, DPSBKR22)	// ディープスタンバイバックアップレジスタ22
	FIELD_0(0x0008C2A7, 0x0008C2A7, unsigned char, DPSBKR23)	// ディープスタンバイバックアップレジスタ23
	FIELD_0(0x0008C2A8, 0x0008C2A8, unsigned char, DPSBKR24)	// ディープスタンバイバックアップレジスタ24
	FIELD_0(0x0008C2A9, 0x0008C2A9, unsigned char, DPSBKR25)	// ディープスタンバイバックアップレジスタ25
	FIELD_0(0x0008C2AA, 0x0008C2AA, unsigned char, DPSBKR26)	// ディープスタンバイバックアップレジスタ26
	FIELD_0(0x0008C2AB, 0x0008C2AB, unsigned char, DPSBKR27)	// ディープスタンバイバックアップレジスタ27
	FIELD_0(0x0008C2AC, 0x0008C2AC, unsigned char, DPSBKR28)	// ディープスタンバイバックアップレジスタ28
	FIELD_0(0x0008C2AD, 0x0008C2AD, unsigned char, DPSBKR29)	// ディープスタンバイバックアップレジスタ29
	FIELD_0(0x0008C2AE, 0x0008C2AE, unsigned char, DPSBKR30)	// ディープスタンバイバックアップレジスタ30
	FIELD_0(0x0008C2AF, 0x0008C2AF, unsigned char, DPSBKR31)	// ディープスタンバイバックアップレジスタ31
};

struct TRX62NBSCReg
{
	FIELD_S(0x00081300, 0x00081303, unsigned char, BERCLR)	// バスエラーステータスクリアレジスタ
	FIELD_S(0x00081304, 0x00081307, unsigned char, BEREN)	// バスエラー監視許可レジスタ
	FIELD_S(0x00081308, 0x00081309, unsigned char, BERSR1)	// バスエラーステータスレジスタ1
	FIELD_S(0x0008130A, 0x00083001, unsigned short, BERSR2)	// バスエラーステータスレジスタ2
	FIELD_0(0x00083002, 0x00083003, unsigned short, CS0MOD)	// CS0モードレジスタ
	FIELD_0(0x00083004, 0x00083007, unsigned int, CS0WCR1)	// CS0ウェイト制御レジスタ1
	FIELD_S(0x00083008, 0x00083011, unsigned int, CS0WCR2)	// CS0ウェイト制御レジスタ2
	FIELD_0(0x00083012, 0x00083013, unsigned short, CS1MOD)	// CS1モードレジスタ
	FIELD_0(0x00083014, 0x00083017, unsigned int, CS1WCR1)	// CS1ウェイト制御レジスタ1
	FIELD_S(0x00083018, 0x00083021, unsigned int, CS1WCR2)	// CS1ウェイト制御レジスタ2
	FIELD_0(0x00083022, 0x00083023, unsigned short, CS2MOD)	// CS2モードレジスタ
	FIELD_0(0x00083024, 0x00083027, unsigned int, CS2WCR1)	// CS2ウェイト制御レジスタ1
	FIELD_S(0x00083028, 0x00083031, unsigned int, CS2WCR2)	// CS2ウェイト制御レジスタ2
	FIELD_0(0x00083032, 0x00083033, unsigned short, CS3MOD)	// CS3モードレジスタ
	FIELD_0(0x00083034, 0x00083037, unsigned int, CS3WCR1)	// CS3ウェイト制御レジスタ1
	FIELD_S(0x00083038, 0x00083041, unsigned int, CS3WCR2)	// CS3ウェイト制御レジスタ2
	FIELD_0(0x00083042, 0x00083043, unsigned short, CS4MOD)	// CS4モードレジスタ
	FIELD_0(0x00083044, 0x00083047, unsigned int, CS4WCR1)	// CS4ウェイト制御レジスタ1
	FIELD_S(0x00083048, 0x00083051, unsigned int, CS4WCR2)	// CS4ウェイト制御レジスタ2
	FIELD_0(0x00083052, 0x00083053, unsigned short, CS5MOD)	// CS5モードレジスタ
	FIELD_0(0x00083054, 0x00083057, unsigned int, CS5WCR1)	// CS5ウェイト制御レジスタ1
	FIELD_S(0x00083058, 0x00083061, unsigned int, CS5WCR2)	// CS5ウェイト制御レジスタ2
	FIELD_0(0x00083062, 0x00083063, unsigned short, CS6MOD)	// CS6モードレジスタ
	FIELD_0(0x00083064, 0x00083067, unsigned int, CS6WCR1)	// CS6ウェイト制御レジスタ1
	FIELD_S(0x00083068, 0x00083071, unsigned int, CS6WCR2)	// CS6ウェイト制御レジスタ2
	FIELD_0(0x00083072, 0x00083073, unsigned short, CS7MOD)	// CS7モードレジスタ
	FIELD_0(0x00083074, 0x00083077, unsigned int, CS7WCR1)	// CS7ウェイト制御レジスタ1
	FIELD_S(0x00083078, 0x00083801, unsigned int, CS7WCR2)	// CS7ウェイト制御レジスタ2
	FIELD_S(0x00083802, 0x00083809, unsigned short, CS0CR)	// CS0制御レジスタ
	FIELD_S(0x0008380A, 0x00083811, unsigned short, CS0REC)	// CS0リカバリサイクル設定レジスタ
	FIELD_S(0x00083812, 0x00083819, unsigned short, CS1CR)	// CS1制御レジスタ
	FIELD_S(0x0008381A, 0x00083821, unsigned short, CS1REC)	// CS1リカバリサイクル設定レジスタ
	FIELD_S(0x00083822, 0x00083829, unsigned short, CS2CR)	// CS2制御レジスタ
	FIELD_S(0x0008382A, 0x00083831, unsigned short, CS2REC)	// CS2リカバリサイクル設定レジスタ
	FIELD_S(0x00083832, 0x00083839, unsigned short, CS3CR)	// CS3制御レジスタ
	FIELD_S(0x0008383A, 0x00083841, unsigned short, CS3REC)	// CS3リカバリサイクル設定レジスタ
	FIELD_S(0x00083842, 0x00083849, unsigned short, CS4CR)	// CS4制御レジスタ
	FIELD_S(0x0008384A, 0x00083851, unsigned short, CS4REC)	// CS4リカバリサイクル設定レジスタ
	FIELD_S(0x00083852, 0x00083859, unsigned short, CS5CR)	// CS5制御レジスタ
	FIELD_S(0x0008385A, 0x00083861, unsigned short, CS5REC)	// CS5リカバリサイクル設定レジスタ
	FIELD_S(0x00083862, 0x00083869, unsigned short, CS6CR)	// CS6制御レジスタ
	FIELD_S(0x0008386A, 0x00083871, unsigned short, CS6REC)	// CS6リカバリサイクル設定レジスタ
	FIELD_S(0x00083872, 0x00083879, unsigned short, CS7CR)	// CS7制御レジスタ
	FIELD_S(0x0008387A, 0x00083BFF, unsigned short, CS7REC)	// CS7リカバリサイクル設定レジスタ
	FIELD_0(0x00083C00, 0x00083C00, unsigned char, SDCCR)	// SDC制御レジスタ
	FIELD_0(0x00083C01, 0x00083C01, unsigned char, SDCMOD)	// SDCモードレジスタ
	FIELD_S(0x00083C02, 0x00083C0F, unsigned char, SDAMOD)	// SDRAMアクセスモードレジスタ
	FIELD_S(0x00083C10, 0x00083C13, unsigned char, SDSELF)	// SDRAMセルフリフレッシュ制御レジスタ
	FIELD_0(0x00083C14, 0x00083C15, unsigned short, SDRFCR)	// SDRAMリフレッシュ制御レジスタ
	FIELD_S(0x00083C16, 0x00083C1F, unsigned char, SDRFEN)	// SDRAMオートリフレッシュ制御レジスタ
	FIELD_S(0x00083C20, 0x00083C23, unsigned char, SDICR)	// SDRAM初期化シーケンス制御レジスタ
	FIELD_S(0x00083C24, 0x00083C3F, unsigned short, SDIR)	// SDRAM初期化レジスタ
	FIELD_S(0x00083C40, 0x00083C43, unsigned char, SDADR)	// SDRAMアドレスレジスタ
	FIELD_0(0x00083C44, 0x00083C47, unsigned int, SDTR)	// SDRAMタイミングレジスタ
	FIELD_S(0x00083C48, 0x00083C4F, unsigned short, SDMOD)	// SDRAMモードレジスタ
	FIELD_0(0x00083C50, 0x00083C50, unsigned char, SDSR)	// SDRAMステータスレジスタ
};

struct TRX62NDMAC0Reg
{
	FIELD_0(0x00082000, 0x00082003, unsigned int, DMSAR)	// DMA転送元アドレスレジスタ
	FIELD_0(0x00082004, 0x00082007, unsigned int, DMDAR)	// DMA転送先アドレスレジスタ
	FIELD_0(0x00082008, 0x0008200B, unsigned int, DMCRA)	// DMA転送カウントレジスタ
	FIELD_S(0x0008200C, 0x0008200F, unsigned short, DMCRB)	// DMAブロック転送カウントレジスタ
	FIELD_S(0x00082010, 0x00082012, unsigned short, DMTMD)	// DMA転送モードレジスタ
	FIELD_0(0x00082013, 0x00082013, unsigned char, DMINT)	// DMA割り込み設定レジスタ
	FIELD_S(0x00082014, 0x00082017, unsigned short, DMAMD)	// DMAアドレスモードレジスタ
	FIELD_0(0x00082018, 0x0008201B, unsigned int, DMOFR)	// DMAオフセットレジスタ
	FIELD_0(0x0008201C, 0x0008201C, unsigned char, DMCNT)	// DMA転送許可レジスタ
	FIELD_0(0x0008201D, 0x0008201D, unsigned char, DMREQ)	// DMAソフトウェア起動レジスタ
	FIELD_0(0x0008201E, 0x0008201E, unsigned char, DMSTS)	// DMAステータスレジスタ
	FIELD_0(0x0008201F, 0x0008201F, unsigned char, DMCSL)	// DMA起動要因フラグ制御レジスタ
};

struct TRX62NDMAC1Reg
{
	FIELD_0(0x00082040, 0x00082043, unsigned int, DMSAR)	// DMA転送元アドレスレジスタ
	FIELD_0(0x00082044, 0x00082047, unsigned int, DMDAR)	// DMA転送先アドレスレジスタ
	FIELD_0(0x00082048, 0x0008204B, unsigned int, DMCRA)	// DMA転送カウントレジスタ
	FIELD_S(0x0008204C, 0x0008204F, unsigned short, DMCRB)	// DMAブロック転送カウントレジスタ
	FIELD_S(0x00082050, 0x00082052, unsigned short, DMTMD)	// DMA転送モードレジスタ
	FIELD_0(0x00082053, 0x00082053, unsigned char, DMINT)	// DMA割り込み設定レジスタ
	FIELD_S(0x00082054, 0x0008205B, unsigned short, DMAMD)	// DMAアドレスモードレジスタ
	FIELD_0(0x0008205C, 0x0008205C, unsigned char, DMCNT)	// DMA転送許可レジスタ
	FIELD_0(0x0008205D, 0x0008205D, unsigned char, DMREQ)	// DMAソフトウェア起動レジスタ
	FIELD_0(0x0008205E, 0x0008205E, unsigned char, DMSTS)	// DMAステータスレジスタ
	FIELD_0(0x0008205F, 0x0008205F, unsigned char, DMCSL)	// DMA起動要因フラグ制御レジスタ
};

struct TRX62NDMAC2Reg
{
	FIELD_0(0x00082080, 0x00082083, unsigned int, DMSAR)	// DMA転送元アドレスレジスタ
	FIELD_0(0x00082084, 0x00082087, unsigned int, DMDAR)	// DMA転送先アドレスレジスタ
	FIELD_0(0x00082088, 0x0008208B, unsigned int, DMCRA)	// DMA転送カウントレジスタ
	FIELD_S(0x0008208C, 0x0008208F, unsigned short, DMCRB)	// DMAブロック転送カウントレジスタ
	FIELD_S(0x00082090, 0x00082092, unsigned short, DMTMD)	// DMA転送モードレジスタ
	FIELD_0(0x00082093, 0x00082093, unsigned char, DMINT)	// DMA割り込み設定レジスタ
	FIELD_S(0x00082094, 0x0008209B, unsigned short, DMAMD)	// DMAアドレスモードレジスタ
	FIELD_0(0x0008209C, 0x0008209C, unsigned char, DMCNT)	// DMA転送許可レジスタ
	FIELD_0(0x0008209D, 0x0008209D, unsigned char, DMREQ)	// DMAソフトウェア起動レジスタ
	FIELD_0(0x0008209E, 0x0008209E, unsigned char, DMSTS)	// DMAステータスレジスタ
	FIELD_0(0x0008209F, 0x0008209F, unsigned char, DMCSL)	// DMA起動要因フラグ制御レジスタ
};

struct TRX62NDMAC3Reg
{
	FIELD_0(0x000820C0, 0x000820C3, unsigned int, DMSAR)	// DMA転送元アドレスレジスタ
	FIELD_0(0x000820C4, 0x000820C7, unsigned int, DMDAR)	// DMA転送先アドレスレジスタ
	FIELD_0(0x000820C8, 0x000820CB, unsigned int, DMCRA)	// DMA転送カウントレジスタ
	FIELD_S(0x000820CC, 0x000820CF, unsigned short, DMCRB)	// DMAブロック転送カウントレジスタ
	FIELD_S(0x000820D0, 0x000820D2, unsigned short, DMTMD)	// DMA転送モードレジスタ
	FIELD_0(0x000820D3, 0x000820D3, unsigned char, DMINT)	// DMA割り込み設定レジスタ
	FIELD_S(0x000820D4, 0x000820DB, unsigned short, DMAMD)	// DMAアドレスモードレジスタ
	FIELD_0(0x000820DC, 0x000820DC, unsigned char, DMCNT)	// DMA転送許可レジスタ
	FIELD_0(0x000820DD, 0x000820DD, unsigned char, DMREQ)	// DMAソフトウェア起動レジスタ
	FIELD_0(0x000820DE, 0x000820DE, unsigned char, DMSTS)	// DMAステータスレジスタ
	FIELD_0(0x000820DF, 0x000820DF, unsigned char, DMCSL)	// DMA起動要因フラグ制御レジスタ
};

struct TRX62NDMACReg
{
	FIELD_0(0x00082200, 0x00082200, unsigned char, DMAST)	// DMACAモジュール起動レジスタ
};

struct TRX62NDTCReg
{
	FIELD_0(0x00082400, 0x00082403, unsigned char, DTCCR)	// DTCコントロールレジスタ
	FIELD_0(0x00082404, 0x00082407, unsigned int, DTCVBR)	// DTCベクタベースレジスタ
	FIELD_S(0x00082408, 0x0008240B, unsigned char, DTCADMOD)	// DTCアドレスモードレジスタ
	FIELD_S(0x0008240C, 0x0008240D, unsigned char, DTCST)	// DTCモジュール起動レジスタ
	FIELD_0(0x0008240E, 0x0008240F, unsigned short, DTCSTS)	// DTCステータスレジスタ
};

struct TRX62NEXDMAC0Reg
{
	FIELD_0(0x00082800, 0x00082803, unsigned int, EDMSAR)	// EXDMA転送元アドレスレジスタ
	FIELD_0(0x00082804, 0x00082807, unsigned int, EDMDAR)	// EXDMA転送先アドレスレジスタ
	FIELD_0(0x00082808, 0x0008280B, unsigned int, EDMCRA)	// EXDMA転送カウントレジスタ
	FIELD_S(0x0008280C, 0x0008280F, unsigned short, EDMCRB)	// EXDMAブロック転送カウントレジスタ
	FIELD_0(0x00082810, 0x00082811, unsigned short, EDMTMD)	// EXDMA転送モードレジスタ
	FIELD_0(0x00082812, 0x00082812, unsigned char, EDMOMD)	// EXDMA出力設定レジスタ
	FIELD_0(0x00082813, 0x00082813, unsigned char, EDMINT)	// EXDMA割り込み設定レジスタ
	FIELD_0(0x00082814, 0x00082817, unsigned int, EDMAMD)	// EXDMAアドレスモードレジスタ
	FIELD_0(0x00082818, 0x0008281B, unsigned int, EDMOFR)	// EXDMAオフセットレジスタ
	FIELD_0(0x0008281C, 0x0008281C, unsigned char, EDMCNT)	// EXDMA転送許可レジスタ
	FIELD_0(0x0008281D, 0x0008281D, unsigned char, EDMREQ)	// EXDMAソフトウェア起動レジスタ
	FIELD_S(0x0008281E, 0x0008281F, unsigned char, EDMSTS)	// EXDMAステータスレジスタ
	FIELD_0(0x00082820, 0x00082820, unsigned char, EDMRMD)	// EXDMA外部要求センスモードレジスタ
	FIELD_0(0x00082821, 0x00082821, unsigned char, EDMERF)	// EXDMA外部要求フラグレジスタ
	FIELD_0(0x00082822, 0x00082822, unsigned char, EDMPRF)	// EXDMA周辺要求フラグレジスタ
};

struct TRX62NEXDMAC1Reg
{
	FIELD_0(0x00082840, 0x00082843, unsigned int, EDMSAR)	// EXDMA転送元アドレスレジスタ
	FIELD_0(0x00082844, 0x00082847, unsigned int, EDMDAR)	// EXDMA転送先アドレスレジスタ
	FIELD_0(0x00082848, 0x0008284B, unsigned int, EDMCRA)	// EXDMA転送カウントレジスタ
	FIELD_S(0x0008284C, 0x0008284F, unsigned short, EDMCRB)	// EXDMAブロック転送カウントレジスタ
	FIELD_0(0x00082850, 0x00082851, unsigned short, EDMTMD)	// EXDMA転送モードレジスタ
	FIELD_0(0x00082852, 0x00082852, unsigned char, EDMOMD)	// EXDMA出力設定レジスタ
	FIELD_0(0x00082853, 0x00082853, unsigned char, EDMINT)	// EXDMA割り込み設定レジスタ
	FIELD_S(0x00082854, 0x0008285B, unsigned int, EDMAMD)	// EXDMAアドレスモードレジスタ
	FIELD_0(0x0008285C, 0x0008285C, unsigned char, EDMCNT)	// EXDMA転送許可レジスタ
	FIELD_0(0x0008285D, 0x0008285D, unsigned char, EDMREQ)	// EXDMAソフトウェア起動レジスタ
	FIELD_S(0x0008285E, 0x0008285F, unsigned char, EDMSTS)	// EXDMAステータスレジスタ
	FIELD_0(0x00082860, 0x00082860, unsigned char, EDMRMD)	// EXDMA外部要求センスモードレジスタ
	FIELD_0(0x00082861, 0x00082861, unsigned char, EDMERF)	// EXDMA外部要求フラグレジスタ
	FIELD_0(0x00082862, 0x00082862, unsigned char, EDMPRF)	// EXDMA周辺要求フラグレジスタ
};

struct TRX62NEXDMACReg
{
	FIELD_0(0x00082A00, 0x00082BDF, unsigned char, EDMAST)	// EXDMAモジュール起動レジスタ
	FIELD_0(0x00082BE0, 0x00082BE3, unsigned int, CLSBR0)	// クラスタバッファレジスタ0
	FIELD_0(0x00082BE4, 0x00082BE7, unsigned int, CLSBR1)	// クラスタバッファレジスタ1
	FIELD_0(0x00082BE8, 0x00082BEB, unsigned int, CLSBR2)	// クラスタバッファレジスタ2
	FIELD_0(0x00082BEC, 0x00082BEF, unsigned int, CLSBR3)	// クラスタバッファレジスタ3
	FIELD_0(0x00082BF0, 0x00082BF3, unsigned int, CLSBR4)	// クラスタバッファレジスタ4
	FIELD_0(0x00082BF4, 0x00082BF7, unsigned int, CLSBR5)	// クラスタバッファレジスタ5
	FIELD_0(0x00082BF8, 0x00082BFB, unsigned int, CLSBR6)	// クラスタバッファレジスタ6
};

struct TRX62NMPUReg
{
	FIELD_0(0x00086400, 0x00086403, unsigned int, RSPAGE0)	// 領域0開始ページ番号レジスタ
	FIELD_0(0x00086404, 0x00086407, unsigned int, REPAGE0)	// 領域0終了ページ番号レジスタ
	FIELD_0(0x00086408, 0x0008640B, unsigned int, RSPAGE1)	// 領域1開始ページ番号レジスタ
	FIELD_0(0x0008640C, 0x0008640F, unsigned int, REPAGE1)	// 領域1終了ページ番号レジスタ
	FIELD_0(0x00086410, 0x00086413, unsigned int, RSPAGE2)	// 領域2開始ページ番号レジスタ
	FIELD_0(0x00086414, 0x00086417, unsigned int, REPAGE2)	// 領域2終了ページ番号レジスタ
	FIELD_0(0x00086418, 0x0008641B, unsigned int, RSPAGE3)	// 領域3開始ページ番号レジスタ
	FIELD_0(0x0008641C, 0x0008641F, unsigned int, REPAGE3)	// 領域3終了ページ番号レジスタ
	FIELD_0(0x00086420, 0x00086423, unsigned int, RSPAGE4)	// 領域4開始ページ番号レジスタ
	FIELD_0(0x00086424, 0x00086427, unsigned int, REPAGE4)	// 領域4終了ページ番号レジスタ
	FIELD_0(0x00086428, 0x0008642B, unsigned int, RSPAGE5)	// 領域5開始ページ番号レジスタ
	FIELD_0(0x0008642C, 0x0008642F, unsigned int, REPAGE5)	// 領域5終了ページ番号レジスタ
	FIELD_0(0x00086430, 0x00086433, unsigned int, RSPAGE6)	// 領域6開始ページ番号レジスタ
	FIELD_0(0x00086434, 0x00086437, unsigned int, REPAGE6)	// 領域6終了ページ番号レジスタ
	FIELD_0(0x00086438, 0x0008643B, unsigned int, RSPAGE7)	// 領域7開始ページ番号レジスタ
	FIELD_S(0x0008643C, 0x000864FF, unsigned int, REPAGE7)	// 領域7終了ページ番号レジスタ
	FIELD_0(0x00086500, 0x00086503, unsigned int, MPEN)	// メモリプロテクション機能有効化レジスタ
	FIELD_0(0x00086504, 0x00086507, unsigned int, MPBAC)	// バッググラウンドアクセス制御レジスタ
	FIELD_0(0x00086508, 0x0008650B, unsigned int, MPECLR)	// メモリプロテクションエラーステータスクリアレジスタ
	FIELD_S(0x0008650C, 0x00086513, unsigned int, MPESTS)	// メモリプロテクションエラーステータスレジスタ
	FIELD_S(0x00086514, 0x0008651F, unsigned int, MPDEA)	// データメモリプロテクションエラーアドレスレジスタ
	FIELD_0(0x00086520, 0x00086523, unsigned int, MPSA)	// 領域サーチアドレスレジスタ
	FIELD_0(0x00086524, 0x00086525, unsigned short, MPOPS)	// 領域サーチオペレーションレジスタ
	FIELD_0(0x00086526, 0x00086527, unsigned short, MPOPI)	// 領域インバリデートオペレーションレジスタ
	FIELD_0(0x00086528, 0x0008652B, unsigned int, MHITI)	// 命令ヒット領域レジスタ
	FIELD_0(0x0008652C, 0x0008652F, unsigned int, MHITD)	// データヒット領域レジスタ
};

struct TICUIRReg
{
	unsigned char IR : 1;			// 割り込みステータスフラグ
	unsigned char : 7;
};

struct TICUIPRReg
{
	unsigned char IPR : 4;			// 割り込み優先レベル設定ビット
	unsigned char : 4;
};

struct TRX62NICUReg
{
	ARRAY_0(0x00087000, 0x000870FF, unsigned char, IR, 256)	// 割り込み要求レジスタ
	ARRAY_0(0x00087100, 0x000871FF, unsigned char, DTCER, 256)	// DTC起動許可レジスタ
	ARRAY_0(0x00087200, 0x0008721F, unsigned char, IER, 32)	// 割り込み要求許可レジスタ
	SPACE(0x000872DF, 0x000872E0 - 0x00087220)
	FIELD_S(0x000872E0, 0x000872EF, unsigned char, SWINTR)	// ソフトウェア割り込み起動レジスタ
	FIELD_S(0x000872F0, 0x000872FF, unsigned short, FIR)	// 高速割り込み設定レジスタ
	ARRAY_0(0x00087300, 0x0008738F, unsigned char, IPR, 144)	// 割り込み要因プライオリティレジスタ
	SPACE(0x000873FF, 0x00087400 - 0x00087290)
	FIELD_S(0x00087400, 0x00087403, unsigned char, DMRSR0)	// DMACA起動要因選択レジスタ0
	FIELD_S(0x00087404, 0x00087407, unsigned char, DMRSR1)	// DMACA起動要因選択レジスタ1
	FIELD_S(0x00087408, 0x0008740B, unsigned char, DMRSR2)	// DMACA起動要因選択レジスタ2
	FIELD_S(0x0008740C, 0x000874FF, unsigned char, DMRSR3)	// DMACA起動要因選択レジスタ3
	ARRAY_0(0x00087500, 0x0008750F, unsigned char, IRQCR, 16)	// IRQコントロールレジスタ
	FIELD_0(0x00087580, 0x00087580, unsigned char, NMISR)	// ノンマスカブル割り込みステータスレジスタ
	FIELD_0(0x00087581, 0x00087581, unsigned char, NMIER)	// ノンマスカブル割り込み許可レジスタ
	FIELD_0(0x00087582, 0x00087582, unsigned char, NMICLR)	// ノンマスカブル割り込みクリアレジスタ
	FIELD_0(0x00087583, 0x00087583, unsigned char, NMICR)	// NMI端子割り込みコントロールレジスタ
};

struct TCMTCMSTR0
{
	unsigned short STR0 : 1;		// カウントスタート0ビット
	unsigned short STR1 : 1;		// カウントスタート1ビット
	unsigned short : 14;
};

struct TRX62NCMTReg
{
	FIELD_S(0x00088000, 0x0008800F, unsigned short, CMSTR0)	// コンペアマッチタイマスタートレジスタ0
	FIELD_0(0x00088010, 0x00088011, unsigned short, CMSTR1)	// コンペアマッチタイマスタートレジスタ1
};

struct TCMTCMCR
{
	unsigned short CKS : 2;		// クロック選択ビット
	unsigned short : 4;
	unsigned short CMIE : 1;	// コンペアマッチ割り込み許可ビット
	unsigned short : 9;
};

struct TRX62NCMT0Reg
{
	FIELD_0(0x00088002, 0x00088003, unsigned short, CMCR)	// コンペアマッチタイマコントロールレジスタ
	FIELD_0(0x00088004, 0x00088005, unsigned short, CMCNT)	// コンペアマッチタイマカウンタ
	FIELD_0(0x00088006, 0x00088007, unsigned short, CMCOR)	// コンペアマッチタイマコンスタントレジスタ
};

struct TRX62NCMT1Reg
{
	FIELD_0(0x00088008, 0x00088009, unsigned short, CMCR)	// コンペアマッチタイマコントロールレジスタ
	FIELD_0(0x0008800A, 0x0008800B, unsigned short, CMCNT)	// コンペアマッチタイマカウンタ
	FIELD_0(0x0008800C, 0x0008800D, unsigned short, CMCOR)	// コンペアマッチタイマコンスタントレジスタ
};

struct TRX62NCMT2Reg
{
	FIELD_0(0x00088012, 0x00088013, unsigned short, CMCR)	// コンペアマッチタイマコントロールレジスタ
	FIELD_0(0x00088014, 0x00088015, unsigned short, CMCNT)	// コンペアマッチタイマカウンタ
	FIELD_0(0x00088016, 0x00088017, unsigned short, CMCOR)	// コンペアマッチタイマコンスタントレジスタ
};

struct TRX62NCMT3Reg
{
	FIELD_0(0x00088018, 0x00088019, unsigned short, CMCR)	// コンペアマッチタイマコントロールレジスタ
	FIELD_0(0x0008801A, 0x0008801B, unsigned short, CMCNT)	// コンペアマッチタイマカウンタ
	FIELD_0(0x0008801C, 0x0008801D, unsigned short, CMCOR)	// コンペアマッチタイマコンスタントレジスタ
};

struct TRX62NWDTReg
{
	union{
		struct {
			FIELD_0(0x00088028, 0x00088028, unsigned char, TCSR)	// タイマコントロール／ステータスレジスタ
			FIELD_0(0x00088029, 0x00088029, unsigned char, TCNT)	// タイマカウンタ
			FIELD_0(0x0008802B, 0x0008802B, unsigned char, RSTCSR)	// リセットコントロール／ステータスレジスタ
		}READ;
		struct {
			FIELD_0(0x00088028, 0x00088029, unsigned short, WINA)	// ライトウインドウAレジスタ
			FIELD_0(0x0008802A, 0x0008802B, unsigned short, WINB)	// ライトウインドウBレジスタ
		}WRITE;
	};
};

struct TRX62NIWDTReg
{
	FIELD_0(0x00088030, 0x00088031, unsigned char, IWDTRR)	// IWDTリフレッシュレジスタ
	FIELD_0(0x00088032, 0x00088033, unsigned short, IWDTCR)	// IWDTコントロールレジスタ
	FIELD_0(0x00088034, 0x00088035, unsigned short, IWDTSR)	// IWDTステータスレジスタ
};

struct TRX62NAD0Reg
{
	FIELD_0(0x00088040, 0x00088041, unsigned short, ADDRA)	// A/DデータレジスタA
	FIELD_0(0x00088042, 0x00088043, unsigned short, ADDRB)	// A/DデータレジスタB
	FIELD_0(0x00088044, 0x00088045, unsigned short, ADDRC)	// A/DデータレジスタC
	FIELD_S(0x00088046, 0x0008804F, unsigned short, ADDRD)	// A/DデータレジスタD
	FIELD_0(0x00088050, 0x00088050, unsigned char, ADCSR)	// A/Dコントロール／ステータスレジスタ
	FIELD_0(0x00088051, 0x00088051, unsigned char, ADCR)	// A/Dコントロールレジスタ
	FIELD_0(0x00088052, 0x00088052, unsigned char, ADDPR)	// ADDRnフォーマット選択レジスタ
	FIELD_S(0x00088053, 0x0008805E, unsigned char, ADSSTR)	// A/Dサンプリングステートレジスタ
	FIELD_0(0x0008805F, 0x0008805F, unsigned char, ADDIAGR)	// A/D自己診断レジスタ
};

struct TRX62NAD1Reg
{
	FIELD_0(0x00088060, 0x00088061, unsigned short, ADDRA)	// A/DデータレジスタA
	FIELD_0(0x00088062, 0x00088063, unsigned short, ADDRB)	// A/DデータレジスタB
	FIELD_0(0x00088064, 0x00088065, unsigned short, ADDRC)	// A/DデータレジスタC
	FIELD_S(0x00088066, 0x0008806F, unsigned short, ADDRD)	// A/DデータレジスタD
	FIELD_0(0x00088070, 0x00088070, unsigned char, ADCSR)	// A/Dコントロール／ステータスレジスタ
	FIELD_0(0x00088071, 0x00088071, unsigned char, ADCR)	// A/Dコントロールレジスタ
	FIELD_0(0x00088072, 0x00088072, unsigned char, ADDPR)	// ADDRnフォーマット選択レジスタ
	FIELD_S(0x00088073, 0x0008807E, unsigned char, ADSSTR)	// A/Dサンプリングステートレジスタ
	FIELD_0(0x0008807F, 0x0008807F, unsigned char, ADDIAGR)	// A/D自己診断レジスタ
};

struct TRX62NDAReg
{
	FIELD_0(0x000880C0, 0x000880C1, unsigned short, DADR0)	// D/Aデータレジスタ0
	FIELD_0(0x000880C2, 0x000880C3, unsigned short, DADR1)	// D/Aデータレジスタ1
	FIELD_0(0x000880C4, 0x000880C4, unsigned char, DACR)	// D/Aコントロールレジスタ
	FIELD_0(0x000880C5, 0x000880C5, unsigned char, DADPR)	// DADRmフォーマット選択レジスタ
};

struct TRX62NPPG0Reg
{
	FIELD_0(0x000881E6, 0x000881E6, unsigned char, PCR)	// PPG出力コントロールレジスタ
	FIELD_0(0x000881E7, 0x000881E7, unsigned char, PMR)	// PPG出力モードレジスタ
	FIELD_0(0x000881E8, 0x000881E8, unsigned char, NDERH)	// ネクストデータイネーブルレジスタH
	FIELD_0(0x000881E9, 0x000881E9, unsigned char, NDERL)	// ネクストデータイネーブルレジスタL
	FIELD_0(0x000881EA, 0x000881EA, unsigned char, PODRH)	// アウトプットデータレジスタH
	FIELD_0(0x000881EB, 0x000881EB, unsigned char, PODRL)	// アウトプットデータレジスタL
	FIELD_0(0x000881EC, 0x000881EC, unsigned char, NDRH)	// ネクストデータレジスタH
	FIELD_0(0x000881ED, 0x000881ED, unsigned char, NDRL)	// ネクストデータレジスタL
	FIELD_0(0x000881EE, 0x000881EE, unsigned char, NDRH2)	// ネクストデータレジスタH2
	FIELD_0(0x000881EF, 0x000881EF, unsigned char, NDRL2)	// ネクストデータレジスタL2
};

struct TRX62NPPG1Reg
{
	FIELD_0(0x000881F0, 0x000881F5, unsigned char, PTRSLR)	// PPGトリガセレクトレジスタ
	FIELD_0(0x000881F6, 0x000881F6, unsigned char, PCR)	// PPG出力コントロールレジスタ
	FIELD_0(0x000881F7, 0x000881F7, unsigned char, PMR)	// PPG出力モードレジスタ
	FIELD_0(0x000881F8, 0x000881F8, unsigned char, NDERH)	// ネクストデータイネーブルレジスタH
	FIELD_0(0x000881F9, 0x000881F9, unsigned char, NDERL)	// ネクストデータイネーブルレジスタL
	FIELD_0(0x000881FA, 0x000881FA, unsigned char, PODRH)	// アウトプットデータレジスタH
	FIELD_0(0x000881FB, 0x000881FB, unsigned char, PODRL)	// アウトプットデータレジスタL
	FIELD_0(0x000881FC, 0x000881FC, unsigned char, NDRH)	// ネクストデータレジスタH
	FIELD_0(0x000881FD, 0x000881FD, unsigned char, NDRL)	// ネクストデータレジスタL
	FIELD_0(0x000881FE, 0x000881FE, unsigned char, NDRH2)	// ネクストデータレジスタH2
	FIELD_0(0x000881FF, 0x000881FF, unsigned char, NDRL2)	// ネクストデータレジスタL2
};

struct TRX62NTMR0Reg
{
	FIELD_0(0x00088200, 0x00088201, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_S(0x00088202, 0x00088203, unsigned char, TCSR)	// タイマコントロール／ステータスレジスタ
	FIELD_S(0x00088204, 0x00088205, unsigned char, TCORA)	// タイムコンスタントレジスタA
	FIELD_S(0x00088206, 0x00088207, unsigned char, TCORB)	// タイムコンスタントレジスタB
	FIELD_S(0x00088208, 0x00088209, unsigned char, TCNT)	// タイマカウンタ
	FIELD_0(0x0008820A, 0x0008820A, unsigned char, TCCR)	// タイマカウンタコントロールレジスタ
};

struct TRX62NTMR1Reg
{
	FIELD_0(0x00088201, 0x00088202, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_S(0x00088203, 0x00088204, unsigned char, TCSR)	// タイマコントロール／ステータスレジスタ
	FIELD_S(0x00088205, 0x00088206, unsigned char, TCORA)	// タイムコンスタントレジスタA
	FIELD_S(0x00088207, 0x00088208, unsigned char, TCORB)	// タイムコンスタントレジスタB
	FIELD_S(0x00088209, 0x0008820A, unsigned char, TCNT)	// タイマカウンタ
	FIELD_0(0x0008820B, 0x0008820B, unsigned char, TCCR)	// タイマカウンタコントロールレジスタ
};

struct TRX62NTMR01Reg
{
	FIELD_0(0x00088204, 0x00088205, unsigned short, TCORA)	// タイムコンスタントレジスタA
	FIELD_0(0x00088206, 0x00088207, unsigned short, TCORB)	// タイムコンスタントレジスタB
	FIELD_0(0x00088208, 0x00088209, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x0008820A, 0x0008820B, unsigned short, TCCR)	// タイマカウンタコントロールレジスタ
};

struct TRX62NTMR2Reg
{
	FIELD_0(0x00088210, 0x00088211, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_S(0x00088212, 0x00088213, unsigned char, TCSR)	// タイマコントロール／ステータスレジスタ
	FIELD_S(0x00088214, 0x00088215, unsigned char, TCORA)	// タイムコンスタントレジスタA
	FIELD_S(0x00088216, 0x00088217, unsigned char, TCORB)	// タイムコンスタントレジスタB
	FIELD_S(0x00088218, 0x00088219, unsigned char, TCNT)	// タイマカウンタ
	FIELD_0(0x0008821A, 0x0008821A, unsigned char, TCCR)	// タイマカウンタコントロールレジスタ
};

struct TRX62NTMR3Reg
{
	FIELD_0(0x00088211, 0x00088212, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_S(0x00088213, 0x00088214, unsigned char, TCSR)	// タイマコントロール／ステータスレジスタ
	FIELD_S(0x00088215, 0x00088216, unsigned char, TCORA)	// タイムコンスタントレジスタA
	FIELD_S(0x00088217, 0x00088218, unsigned char, TCORB)	// タイムコンスタントレジスタB
	FIELD_S(0x00088219, 0x0008821A, unsigned char, TCNT)	// タイマカウンタ
	FIELD_0(0x0008821B, 0x0008821B, unsigned char, TCCR)	// タイマカウンタコントロールレジスタ
};

struct TRX62NTMR23Reg
{
	FIELD_0(0x00088214, 0x00088215, unsigned short, TCORA)	// タイムコンスタントレジスタA
	FIELD_0(0x00088216, 0x00088217, unsigned short, TCORB)	// タイムコンスタントレジスタB
	FIELD_0(0x00088218, 0x00088219, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x0008821A, 0x0008821B, unsigned short, TCCR)	// タイマカウンタコントロールレジスタ
};

struct TRX62NSMCI0Reg
{
	FIELD_0(0x00088240, 0x00088240, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088241, 0x00088241, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x00088242, 0x00088242, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x00088243, 0x00088243, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x00088244, 0x00088244, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x00088245, 0x00088245, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x00088246, 0x00088246, unsigned char, SCMR)	// スマートカードモードレジスタ
};

struct TSCISSR
{
	unsigned char MPBT : 1;				// マルチプロセッサビット転送ビット
	unsigned char MPB : 1;				// マルチプロセッサビット
	unsigned char TEND : 1;				// 送信完了フラグ
	unsigned char PER : 1;				// パリティエラーフラグ
	unsigned char FER : 1;				// フレーミングエラーフラグ
	unsigned char ORER : 1;				// オーバランエラーフラグ
	unsigned char RDRF : 1;				// 受信データフルフラグ
	unsigned char TDRE : 1;				// 送信データエンプティフラグ
};

struct TSCISMR
{
	unsigned char CSK : 2;				// クロック選択ビット
	unsigned char MP : 1;				// マルチプロセッサモードビット
	unsigned char STOP : 1;				// ストップビット長選択ビット
	unsigned char PM : 1;				// パリティモードビット
	unsigned char PE : 1;				// パリティ許可ビット
	unsigned char CHR : 1;				// キャラクタ長ビット
	unsigned char CM : 1;				// コミュニケーションモードビット
};

struct TSCISEMR
{
	unsigned char ACS0 : 1;				// 調歩同期クロックソース選択ビット
	unsigned char : 3;
	unsigned char ABCS : 1;				// 調歩同期基本クロック選択ビット
	unsigned char : 3;
};

struct TRX62NSCI0Reg
{
	FIELD_0(0x00088240, 0x00088240, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088241, 0x00088241, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x00088242, 0x00088242, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x00088243, 0x00088243, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x00088244, 0x00088244, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x00088245, 0x00088245, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x00088246, 0x00088246, unsigned char, SCMR)	// スマートカードモードレジスタ
	FIELD_0(0x00088247, 0x00088247, unsigned char, SEMR)	// シリアル拡張モードレジスタ
};

struct TRX62NSMCI1Reg
{
	FIELD_0(0x00088248, 0x00088248, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088249, 0x00088249, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x0008824A, 0x0008824A, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x0008824B, 0x0008824B, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x0008824C, 0x0008824C, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x0008824D, 0x0008824D, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x0008824E, 0x0008824E, unsigned char, SCMR)	// スマートカードモードレジスタ
};

struct TRX62NSCI1Reg
{
	FIELD_0(0x00088248, 0x00088248, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088249, 0x00088249, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x0008824A, 0x0008824A, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x0008824B, 0x0008824B, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x0008824C, 0x0008824C, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x0008824D, 0x0008824D, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x0008824E, 0x0008824E, unsigned char, SCMR)	// スマートカードモードレジスタ
	FIELD_0(0x0008824F, 0x0008824F, unsigned char, SEMR)	// シリアル拡張モードレジスタ
};

struct TRX62NSMCI2Reg
{
	FIELD_0(0x00088250, 0x00088250, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088251, 0x00088251, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x00088252, 0x00088252, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x00088253, 0x00088253, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x00088254, 0x00088254, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x00088255, 0x00088255, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x00088256, 0x00088256, unsigned char, SCMR)	// スマートカードモードレジスタ
};

struct TRX62NSCI2Reg
{
	FIELD_0(0x00088250, 0x00088250, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088251, 0x00088251, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x00088252, 0x00088252, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x00088253, 0x00088253, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x00088254, 0x00088254, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x00088255, 0x00088255, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x00088256, 0x00088256, unsigned char, SCMR)	// スマートカードモードレジスタ
	FIELD_0(0x00088257, 0x00088257, unsigned char, SEMR)	// シリアル拡張モードレジスタ
};

struct TRX62NSMCI3Reg
{
	FIELD_0(0x00088258, 0x00088258, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088259, 0x00088259, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x0008825A, 0x0008825A, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x0008825B, 0x0008825B, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x0008825C, 0x0008825C, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x0008825D, 0x0008825D, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x0008825E, 0x0008825E, unsigned char, SCMR)	// スマートカードモードレジスタ
};

struct TRX62NSCI3Reg
{
	FIELD_0(0x00088258, 0x00088258, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088259, 0x00088259, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x0008825A, 0x0008825A, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x0008825B, 0x0008825B, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x0008825C, 0x0008825C, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x0008825D, 0x0008825D, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x0008825E, 0x0008825E, unsigned char, SCMR)	// スマートカードモードレジスタ
	FIELD_0(0x0008825F, 0x0008825F, unsigned char, SEMR)	// シリアル拡張モードレジスタ
};

struct TRX62NSMCI5Reg
{
	FIELD_0(0x00088268, 0x00088268, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088269, 0x00088269, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x0008826A, 0x0008826A, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x0008826B, 0x0008826B, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x0008826C, 0x0008826C, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x0008826D, 0x0008826D, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x0008826E, 0x0008826E, unsigned char, SCMR)	// スマートカードモードレジスタ
};

struct TRX62NSCI5Reg
{
	FIELD_0(0x00088268, 0x00088268, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088269, 0x00088269, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x0008826A, 0x0008826A, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x0008826B, 0x0008826B, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x0008826C, 0x0008826C, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x0008826D, 0x0008826D, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x0008826E, 0x0008826E, unsigned char, SCMR)	// スマートカードモードレジスタ
	FIELD_0(0x0008826F, 0x0008826F, unsigned char, SEMR)	// シリアル拡張モードレジスタ
};

struct TRX62NSMCI6Reg
{
	FIELD_0(0x00088270, 0x00088270, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088271, 0x00088271, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x00088272, 0x00088272, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x00088273, 0x00088273, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x00088274, 0x00088274, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x00088275, 0x00088275, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x00088276, 0x00088276, unsigned char, SCMR)	// スマートカードモードレジスタ
};

struct TRX62NSCI6Reg
{
	FIELD_0(0x00088270, 0x00088270, unsigned char, SMR)	// シリアルモードレジスタ
	FIELD_0(0x00088271, 0x00088271, unsigned char, BRR)	// ビットレートレジスタ
	FIELD_0(0x00088272, 0x00088272, unsigned char, SCR)	// シリアルコントロールレジスタ
	FIELD_0(0x00088273, 0x00088273, unsigned char, TDR)	// トランスミットデータレジスタ
	FIELD_0(0x00088274, 0x00088274, unsigned char, SSR)	// シリアルステータスレジスタ
	FIELD_0(0x00088275, 0x00088275, unsigned char, RDR)	// レシーブデータレジスタ
	FIELD_0(0x00088276, 0x00088276, unsigned char, SCMR)	// スマートカードモードレジスタ
	FIELD_0(0x00088277, 0x00088277, unsigned char, SEMR)	// シリアル拡張モードレジスタ
};

struct TRX62NCRCReg
{
	FIELD_0(0x00088280, 0x00088280, unsigned char, CRCCR)	// CRCコントロールレジスタ
	FIELD_0(0x00088281, 0x00088281, unsigned char, CRCDIR)	// CRCデータ入力レジスタ
	FIELD_0(0x00088282, 0x00088283, unsigned short, CRCDOR)	// CRCデータ出力レジスタ
};

struct TRIICICMR1
{
	unsigned char BC : 3;				// ビットカウンタ
	unsigned char BCWP : 1;			// BCライトプロテクトビット
	unsigned char CKS : 3;			// 内部基準クロック選択ビット
	unsigned char MTWP : 1;			// MST/TRSライトプロテクトビット};
};

struct TRIICICCR2
{
	unsigned char : 1;				// 予約ビット
	unsigned char ST : 1;				// スタートコンディション発行要求ビット
	unsigned char RS : 1;				// リスタートコンディション発行要求ビット
	unsigned char SP : 1;				// ストップコンディション発行要求ビット
	unsigned char : 1;				// 予約ビット
	unsigned char TRS : 1;			// 送信/受信モードビット
	unsigned char MST : 1;			// マスタ/スレーブモードビット
	unsigned char BBSY : 1;			// バスビジー検出フラグ
};

struct TRIICICSR2 
{
	unsigned char TMOF : 1;		// タイムアウト検出フラグ
	unsigned char AL : 1;			// アービトレーションロストフラグ
	unsigned char START : 1;		// スタートコンディション検出フラグ
	unsigned char STOP : 1;		// ストップコンディション検出フラグ
	unsigned char NACKF : 1;		// NACK検出フラグ
	unsigned char RDRF : 1;		// 受信データフルフラグ
	unsigned char TEND : 1;		// 送信終了フラグ
	unsigned char TDRE : 1;		// 送信データエンプティフラグ
};

struct TRIICICBRL {
	unsigned char BRL : 5;			// ビットレートLow幅設定ビット
	unsigned char : 3;				// 予約ビット
};

struct TRIICICBRH {
	unsigned char BRH : 5;			// ビットレートHigh幅設定ビット
	unsigned char : 3;				// 予約ビット
};

struct TRX62NRIIC0Reg
{
	FIELD_0(0x00088300, 0x00088300, unsigned char, ICCR1)	// I2Cバスコントロールレジスタ1
	FIELD_0(0x00088301, 0x00088301, unsigned char, ICCR2)	// I2Cバスコントロールレジスタ2
	FIELD_0(0x00088302, 0x00088302, unsigned char, ICMR1)	// I2Cバスモードレジスタ1
	FIELD_0(0x00088303, 0x00088303, unsigned char, ICMR2)	// I2Cバスモードレジスタ2
	FIELD_0(0x00088304, 0x00088304, unsigned char, ICMR3)	// I2Cバスモードレジスタ3
	FIELD_0(0x00088305, 0x00088305, unsigned char, ICFER)	// I2Cバスファンクションイネーブルレジスタ
	FIELD_0(0x00088306, 0x00088306, unsigned char, ICSER)	// I2Cバスステータスイネーブルレジスタ
	FIELD_0(0x00088307, 0x00088307, unsigned char, ICIER)	// I2Cバスインタラプトイネーブルレジスタ
	FIELD_0(0x00088308, 0x00088308, unsigned char, ICSR1)	// I2Cバスステータスレジスタ1
	FIELD_0(0x00088309, 0x00088309, unsigned char, ICSR2)	// I2Cバスステータスレジスタ2
	FIELD_0(0x0008830A, 0x0008830A, unsigned char, SARL0)	// スレーブアドレスレジスタL0
	FIELD_0(0x0008830B, 0x0008830B, unsigned char, SARU0)	// スレーブアドレスレジスタU0
	FIELD_0(0x0008830C, 0x0008830C, unsigned char, SARL1)	// スレーブアドレスレジスタL1
	FIELD_0(0x0008830D, 0x0008830D, unsigned char, SARU1)	// スレーブアドレスレジスタU1
	FIELD_0(0x0008830E, 0x0008830E, unsigned char, SARL2)	// スレーブアドレスレジスタL2
	FIELD_0(0x0008830F, 0x0008830F, unsigned char, SARU2)	// スレーブアドレスレジスタU2
	FIELD_0(0x00088310, 0x00088310, unsigned char, ICBRL)	// I2Cバスビットレートローレベルレジスタ
	FIELD_0(0x00088311, 0x00088311, unsigned char, ICBRH)	// I2Cバスビットレートハイレベルレジスタ
	FIELD_0(0x00088312, 0x00088312, unsigned char, ICDRT)	// I2Cバス送信データレジスタ
	FIELD_0(0x00088313, 0x00088313, unsigned char, ICDRR)	// I2Cバス受信データレジスタ
};

struct TRX62NRIIC1Reg
{
	FIELD_0(0x00088320, 0x00088320, unsigned char, ICCR1)	// I2Cバスコントロールレジスタ1
	FIELD_0(0x00088321, 0x00088321, unsigned char, ICCR2)	// I2Cバスコントロールレジスタ2
	FIELD_0(0x00088322, 0x00088322, unsigned char, ICMR1)	// I2Cバスモードレジスタ1
	FIELD_0(0x00088323, 0x00088323, unsigned char, ICMR2)	// I2Cバスモードレジスタ2
	FIELD_0(0x00088324, 0x00088324, unsigned char, ICMR3)	// I2Cバスモードレジスタ3
	FIELD_0(0x00088325, 0x00088325, unsigned char, ICFER)	// I2Cバスファンクションイネーブルレジスタ
	FIELD_0(0x00088326, 0x00088326, unsigned char, ICSER)	// I2Cバスステータスイネーブルレジスタ
	FIELD_0(0x00088327, 0x00088327, unsigned char, ICIER)	// I2Cバスインタラプトイネーブルレジスタ
	FIELD_0(0x00088328, 0x00088328, unsigned char, ICSR1)	// I2Cバスステータスレジスタ1
	FIELD_0(0x00088329, 0x00088329, unsigned char, ICSR2)	// I2Cバスステータスレジスタ2
	FIELD_0(0x0008832A, 0x0008832A, unsigned char, SARL0)	// スレーブアドレスレジスタL0
	FIELD_0(0x0008832B, 0x0008832B, unsigned char, SARU0)	// スレーブアドレスレジスタU0
	FIELD_0(0x0008832C, 0x0008832C, unsigned char, SARL1)	// スレーブアドレスレジスタL1
	FIELD_0(0x0008832D, 0x0008832D, unsigned char, SARU1)	// スレーブアドレスレジスタU1
	FIELD_0(0x0008832E, 0x0008832E, unsigned char, SARL2)	// スレーブアドレスレジスタL2
	FIELD_0(0x0008832F, 0x0008832F, unsigned char, SARU2)	// スレーブアドレスレジスタU2
	FIELD_0(0x00088330, 0x00088330, unsigned char, ICBRL)	// I2Cバスビットレートローレベルレジスタ
	FIELD_0(0x00088331, 0x00088331, unsigned char, ICBRH)	// I2Cバスビットレートハイレベルレジスタ
	FIELD_0(0x00088332, 0x00088332, unsigned char, ICDRT)	// I2Cバス送信データレジスタ
	FIELD_0(0x00088333, 0x00088333, unsigned char, ICDRR)	// I2Cバス受信データレジスタ
};

struct TRX62NRSPI0Reg
{
	FIELD_0(0x00088380, 0x00088380, unsigned char, SPCR)	// RSPI制御レジスタ
	FIELD_0(0x00088381, 0x00088381, unsigned char, SSLP)	// RSPIスレーブセレクト極性レジスタ
	FIELD_0(0x00088382, 0x00088382, unsigned char, SPPCR)	// RSPI端子制御レジスタ
	FIELD_0(0x00088383, 0x00088383, unsigned char, SPSR)	// RSPIステータスレジスタ
	FIELD_0(0x00088384, 0x00088387, unsigned int, SPDR)	// RSPIデータレジスタ
	FIELD_0(0x00088388, 0x00088388, unsigned char, SPSCR)	// RSPIシーケンス制御レジスタ
	FIELD_0(0x00088389, 0x00088389, unsigned char, SPSSR)	// RSPIシーケンスステータスレジスタ
	FIELD_0(0x0008838A, 0x0008838A, unsigned char, SPBR)	// RSPIビットレートレジスタ
	FIELD_0(0x0008838B, 0x0008838B, unsigned char, SPDCR)	// RSPIデータコントロールレジスタ
	FIELD_0(0x0008838C, 0x0008838C, unsigned char, SPCKD)	// RSPIクロック遅延レジスタ
	FIELD_0(0x0008838D, 0x0008838D, unsigned char, SSLND)	// RSPIスレーブセレクトネゲート遅延レジスタ
	FIELD_0(0x0008838E, 0x0008838E, unsigned char, SPND)	// RSPI次アクセス遅延レジスタ
	FIELD_0(0x0008838F, 0x0008838F, unsigned char, SPCR2)	// RSPI制御レジスタ2
	FIELD_0(0x00088390, 0x00088391, unsigned short, SPCMD0)	// RSPIコマンドレジスタ0
	FIELD_0(0x00088392, 0x00088393, unsigned short, SPCMD1)	// RSPIコマンドレジスタ1
	FIELD_0(0x00088394, 0x00088395, unsigned short, SPCMD2)	// RSPIコマンドレジスタ2
	FIELD_0(0x00088396, 0x00088397, unsigned short, SPCMD3)	// RSPIコマンドレジスタ3
	FIELD_0(0x00088398, 0x00088399, unsigned short, SPCMD4)	// RSPIコマンドレジスタ4
	FIELD_0(0x0008839A, 0x0008839B, unsigned short, SPCMD5)	// RSPIコマンドレジスタ5
	FIELD_0(0x0008839C, 0x0008839D, unsigned short, SPCMD6)	// RSPIコマンドレジスタ6
	FIELD_0(0x0008839E, 0x0008839F, unsigned short, SPCMD7)	// RSPIコマンドレジスタ7
};

struct TRX62NRSPI1Reg
{
	FIELD_0(0x000883A0, 0x000883A0, unsigned char, SPCR)	// RSPI制御レジスタ
	FIELD_0(0x000883A1, 0x000883A1, unsigned char, SSLP)	// RSPIスレーブセレクト極性レジスタ
	FIELD_0(0x000883A2, 0x000883A2, unsigned char, SPPCR)	// RSPI端子制御レジスタ
	FIELD_0(0x000883A3, 0x000883A3, unsigned char, SPSR)	// RSPIステータスレジスタ
	FIELD_0(0x000883A4, 0x000883A7, unsigned int, SPDR)	// RSPIデータレジスタ
	FIELD_0(0x000883A8, 0x000883A8, unsigned char, SPSCR)	// RSPIシーケンス制御レジスタ
	FIELD_0(0x000883A9, 0x000883A9, unsigned char, SPSSR)	// RSPIシーケンスステータスレジスタ
	FIELD_0(0x000883AA, 0x000883AA, unsigned char, SPBR)	// RSPIビットレートレジスタ
	FIELD_0(0x000883AB, 0x000883AB, unsigned char, SPDCR)	// RSPIデータコントロールレジスタ
	FIELD_0(0x000883AC, 0x000883AC, unsigned char, SPCKD)	// RSPIクロック遅延レジスタ
	FIELD_0(0x000883AD, 0x000883AD, unsigned char, SSLND)	// RSPIスレーブセレクトネゲート遅延レジスタ
	FIELD_0(0x000883AE, 0x000883AE, unsigned char, SPND)	// RSPI次アクセス遅延レジスタ
	FIELD_0(0x000883AF, 0x000883AF, unsigned char, SPCR2)	// RSPI制御レジスタ2
	FIELD_0(0x000883B0, 0x000883B1, unsigned short, SPCMD0)	// RSPIコマンドレジスタ0
	FIELD_0(0x000883B2, 0x000883B3, unsigned short, SPCMD1)	// RSPIコマンドレジスタ1
	FIELD_0(0x000883B4, 0x000883B5, unsigned short, SPCMD2)	// RSPIコマンドレジスタ2
	FIELD_0(0x000883B6, 0x000883B7, unsigned short, SPCMD3)	// RSPIコマンドレジスタ3
	FIELD_0(0x000883B8, 0x000883B9, unsigned short, SPCMD4)	// RSPIコマンドレジスタ4
	FIELD_0(0x000883BA, 0x000883BB, unsigned short, SPCMD5)	// RSPIコマンドレジスタ5
	FIELD_0(0x000883BC, 0x000883BD, unsigned short, SPCMD6)	// RSPIコマンドレジスタ6
	FIELD_0(0x000883BE, 0x000883BF, unsigned short, SPCMD7)	// RSPIコマンドレジスタ7
};

struct TRX62NMTU3Reg
{
	FIELD_S(0x00088600, 0x00088601, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_S(0x00088602, 0x00088603, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_0(0x00088604, 0x00088604, unsigned char, TIORH)	// タイマI/OコントロールレジスタH
	FIELD_S(0x00088605, 0x00088607, unsigned char, TIORL)	// タイマI/OコントロールレジスタL
	FIELD_S(0x00088608, 0x0008860F, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_S(0x00088610, 0x00088617, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x00088618, 0x00088619, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_S(0x0008861A, 0x00088623, unsigned short, TGRB)	// タイマジェネラルレジスタB
	FIELD_0(0x00088624, 0x00088625, unsigned short, TGRC)	// タイマジェネラルレジスタC
	FIELD_S(0x00088626, 0x0008862B, unsigned short, TGRD)	// タイマジェネラルレジスタD
	FIELD_S(0x0008862C, 0x00088637, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_0(0x00088638, 0x00088638, unsigned char, TBTM)	// タイマバッファ動作転送モードレジスタ
};

struct TRX62NMTU4Reg
{
	FIELD_S(0x00088601, 0x00088602, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_S(0x00088603, 0x00088605, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_0(0x00088606, 0x00088606, unsigned char, TIORH)	// タイマI/OコントロールレジスタH
	FIELD_S(0x00088607, 0x00088608, unsigned char, TIORL)	// タイマI/OコントロールレジスタL
	FIELD_S(0x00088609, 0x00088611, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_S(0x00088612, 0x0008861B, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x0008861C, 0x0008861D, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_S(0x0008861E, 0x00088627, unsigned short, TGRB)	// タイマジェネラルレジスタB
	FIELD_0(0x00088628, 0x00088629, unsigned short, TGRC)	// タイマジェネラルレジスタC
	FIELD_S(0x0008862A, 0x0008862C, unsigned short, TGRD)	// タイマジェネラルレジスタD
	FIELD_S(0x0008862D, 0x00088638, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_S(0x00088639, 0x0008863F, unsigned char, TBTM)	// タイマバッファ動作転送モードレジスタ
	FIELD_S(0x00088640, 0x00088643, unsigned short, TADCR)	// タイマA/D変換開始要求コントロールレジスタ
	FIELD_0(0x00088644, 0x00088645, unsigned short, TADCORA)	// タイマA/D変換開始要求周期設定レジスタA
	FIELD_0(0x00088646, 0x00088647, unsigned short, TADCORB)	// タイマA/D変換開始要求周期設定レジスタB
	FIELD_0(0x00088648, 0x00088649, unsigned short, ATADCOBRA)	// タイマA/D変換開始要求周期設定バッファレジスタ
	FIELD_0(0x0008864A, 0x0008864B, unsigned short, BTADCOBRB)	// タイマA/D変換開始要求周期設定バッファレジスタ
};

struct TRX62NMTUAReg
{
	FIELD_S(0x0008860A, 0x0008860C, unsigned char, TOER)	// タイマアウトプットマスタイネーブルレジスタ
	FIELD_0(0x0008860D, 0x0008860D, unsigned char, TGCR)	// タイマゲートコントロールレジスタ
	FIELD_0(0x0008860E, 0x0008860E, unsigned char, TOCR1)	// タイマアウトプットコントロールレジスタ1
	FIELD_S(0x0008860F, 0x00088613, unsigned char, TOCR2)	// タイマアウトプットコントロールレジスタ2
	FIELD_0(0x00088614, 0x00088615, unsigned short, TCDR)	// タイマ周期データレジスタ
	FIELD_S(0x00088616, 0x0008861F, unsigned short, TDDR)	// タイマデッドタイムデータレジスタ
	FIELD_0(0x00088620, 0x00088621, unsigned short, TCNTS)	// タイマサブカウンタ
	FIELD_S(0x00088622, 0x0008862F, unsigned short, TCBR)	// タイマ周期バッファレジスタ
	FIELD_0(0x00088630, 0x00088630, unsigned char, TITCR)	// タイマ割り込み間引き設定レジスタ
	FIELD_0(0x00088631, 0x00088631, unsigned char, TITCNT)	// タイマ割り込み間引き回数カウンタ
	FIELD_S(0x00088632, 0x00088633, unsigned char, TBTER)	// タイマバッファ転送設定レジスタ
	FIELD_S(0x00088634, 0x00088635, unsigned char, TDER)	// タイマデッドタイムイネーブルレジスタ
	FIELD_S(0x00088636, 0x0008865F, unsigned char, TOLBR)	// タイマアウトプットレベルバッファレジスタ
	FIELD_S(0x00088660, 0x0008867F, unsigned char, TWCR)	// タイマ波形コントロールレジスタ
	FIELD_0(0x00088680, 0x00088680, unsigned char, TSTR)	// タイマスタートレジスタ
	FIELD_S(0x00088681, 0x00088683, unsigned char, TSYR)	// タイマシンクロレジスタ
	FIELD_0(0x00088684, 0x00088684, unsigned char, TRWER)	// タイマリードライトイネーブルレジスタ
};

struct TRX62NMTU0Reg
{
	FIELD_0(0x00088700, 0x00088700, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_0(0x00088701, 0x00088701, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_0(0x00088702, 0x00088702, unsigned char, TIORH)	// タイマI/OコントロールレジスタH
	FIELD_0(0x00088703, 0x00088703, unsigned char, TIORL)	// タイマI/OコントロールレジスタL
	FIELD_0(0x00088704, 0x00088704, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_0(0x00088705, 0x00088705, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_0(0x00088706, 0x00088707, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x00088708, 0x00088709, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_0(0x0008870A, 0x0008870B, unsigned short, TGRB)	// タイマジェネラルレジスタB
	FIELD_0(0x0008870C, 0x0008870D, unsigned short, TGRC)	// タイマジェネラルレジスタC
	FIELD_S(0x0008870E, 0x0008871F, unsigned short, TGRD)	// タイマジェネラルレジスタD
	FIELD_0(0x00088720, 0x00088721, unsigned short, TGRE)	// タイマジェネラルレジスタE
	FIELD_0(0x00088722, 0x00088723, unsigned short, TGRF)	// タイマジェネラルレジスタF
	FIELD_S(0x00088724, 0x00088725, unsigned char, TIER2)	// タイマインタラプトイネーブルレジスタ2
	FIELD_0(0x00088726, 0x00088726, unsigned char, TBTM)	// タイマバッファ動作転送モードレジスタ
};

struct TRX62NMTU1Reg
{
	FIELD_0(0x00088780, 0x00088780, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_0(0x00088781, 0x00088781, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_S(0x00088782, 0x00088783, unsigned char, TIOR)	// タイマI/Oコントロールレジスタ
	FIELD_0(0x00088784, 0x00088784, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_0(0x00088785, 0x00088785, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_0(0x00088786, 0x00088787, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x00088788, 0x00088789, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_S(0x0008878A, 0x0008878F, unsigned short, TGRB)	// タイマジェネラルレジスタB
	FIELD_0(0x00088790, 0x00088790, unsigned char, TICCR)	// タイマインプットキャプチャコントロールレジスタ
};

struct TRX62NMTU2Reg
{
	FIELD_0(0x00088800, 0x00088800, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_0(0x00088801, 0x00088801, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_S(0x00088802, 0x00088803, unsigned char, TIOR)	// タイマI/Oコントロールレジスタ
	FIELD_0(0x00088804, 0x00088804, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_0(0x00088805, 0x00088805, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_0(0x00088806, 0x00088807, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x00088808, 0x00088809, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_0(0x0008880A, 0x0008880B, unsigned short, TGRB)	// タイマジェネラルレジスタB
};

struct TRX62NMTU5Reg
{
	FIELD_0(0x00088880, 0x00088881, unsigned short, TCNTU)	// タイマカウンタU
	FIELD_0(0x00088882, 0x00088883, unsigned short, TGRU)	// タイマジェネラルレジスタU
	FIELD_S(0x00088884, 0x00088885, unsigned char, TCRU)	// タイマコントロールレジスタU
	FIELD_S(0x00088886, 0x0008888F, unsigned char, TIORU)	// タイマI/OコントロールレジスタU
	FIELD_0(0x00088890, 0x00088891, unsigned short, TCNTV)	// タイマカウンタV
	FIELD_0(0x00088892, 0x00088893, unsigned short, TGRV)	// タイマジェネラルレジスタV
	FIELD_S(0x00088894, 0x00088895, unsigned char, TCRV)	// タイマコントロールレジスタV
	FIELD_S(0x00088896, 0x0008889F, unsigned char, TIORV)	// タイマI/OコントロールレジスタV
	FIELD_0(0x000888A0, 0x000888A1, unsigned short, TCNTW)	// タイマカウンタW
	FIELD_0(0x000888A2, 0x000888A3, unsigned short, TGRW)	// タイマジェネラルレジスタW
	FIELD_S(0x000888A4, 0x000888A5, unsigned char, TCRW)	// タイマコントロールレジスタW
	FIELD_S(0x000888A6, 0x000888B1, unsigned char, TIORW)	// タイマI/OコントロールレジスタW
	FIELD_S(0x000888B2, 0x000888B3, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_S(0x000888B4, 0x000888B5, unsigned char, TSTR)	// タイマスタートレジスタ
	FIELD_0(0x000888B6, 0x000888B6, unsigned char, TCNTCMPCLR)	// タイマコンペアマッチクリアレジスタ
};

struct TRX62NPOEReg
{
	FIELD_0(0x00088900, 0x00088901, unsigned short, ICSR1)	// 入力レベルコントロール／ステータスレジスタ1
	FIELD_0(0x00088902, 0x00088903, unsigned short, OCSR1)	// 出力レベルコントロール／ステータスレジスタ1
	FIELD_0(0x00088904, 0x00088905, unsigned short, ICSR2)	// 入力レベルコントロール／ステータスレジスタ2
	FIELD_0(0x00088906, 0x00088907, unsigned short, OCSR2)	// 出力レベルコントロール／ステータスレジスタ2
	FIELD_0(0x00088908, 0x00088909, unsigned short, ICSR3)	// 入力レベルコントロール／ステータスレジスタ3
	FIELD_0(0x0008890A, 0x0008890A, unsigned char, SPOER)	// ソフトウェアポートアウトプットイネーブルレジスタ
	FIELD_0(0x0008890B, 0x0008890B, unsigned char, POECR1)	// ポートアウトプットイネーブルコントロールレジスタ1
	FIELD_0(0x0008890C, 0x0008890D, unsigned short, POECR2)	// ポートアウトプットイネーブルコントロールレジスタ2
	FIELD_0(0x0008890E, 0x0008890F, unsigned short, ICSR4)	// 入力レベルコントロール／ステータスレジスタ4
};

struct TRX62NMTU9Reg
{
	FIELD_S(0x00088A00, 0x00088A01, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_S(0x00088A02, 0x00088A03, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_0(0x00088A04, 0x00088A04, unsigned char, TIORH)	// タイマI/OコントロールレジスタH
	FIELD_S(0x00088A05, 0x00088A07, unsigned char, TIORL)	// タイマI/OコントロールレジスタL
	FIELD_S(0x00088A08, 0x00088A0F, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_S(0x00088A10, 0x00088A17, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x00088A18, 0x00088A19, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_S(0x00088A1A, 0x00088A23, unsigned short, TGRB)	// タイマジェネラルレジスタB
	FIELD_0(0x00088A24, 0x00088A25, unsigned short, TGRC)	// タイマジェネラルレジスタC
	FIELD_S(0x00088A26, 0x00088A2B, unsigned short, TGRD)	// タイマジェネラルレジスタD
	FIELD_S(0x00088A2C, 0x00088A37, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_0(0x00088A38, 0x00088A38, unsigned char, TBTM)	// タイマバッファ動作転送モードレジスタ
};

struct TRX62NMTU10Reg
{
	FIELD_S(0x00088A01, 0x00088A02, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_S(0x00088A03, 0x00088A05, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_0(0x00088A06, 0x00088A06, unsigned char, TIORH)	// タイマI/OコントロールレジスタH
	FIELD_S(0x00088A07, 0x00088A08, unsigned char, TIORL)	// タイマI/OコントロールレジスタL
	FIELD_S(0x00088A09, 0x00088A11, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_S(0x00088A12, 0x00088A1B, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x00088A1C, 0x00088A1D, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_S(0x00088A1E, 0x00088A27, unsigned short, TGRB)	// タイマジェネラルレジスタB
	FIELD_0(0x00088A28, 0x00088A29, unsigned short, TGRC)	// タイマジェネラルレジスタC
	FIELD_S(0x00088A2A, 0x00088A2C, unsigned short, TGRD)	// タイマジェネラルレジスタD
	FIELD_S(0x00088A2D, 0x00088A38, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_S(0x00088A39, 0x00088A3F, unsigned char, TBTM)	// タイマバッファ動作転送モードレジスタ
	FIELD_S(0x00088A40, 0x00088A43, unsigned short, TADCR)	// タイマA/D変換開始要求コントロールレジスタ
	FIELD_0(0x00088A44, 0x00088A45, unsigned short, TADCORA)	// タイマA/D変換開始要求周期設定レジスタA
	FIELD_0(0x00088A46, 0x00088A47, unsigned short, TADCORB)	// タイマA/D変換開始要求周期設定レジスタB
	FIELD_0(0x00088A48, 0x00088A49, unsigned short, ATADCOBRA)	// タイマA/D変換開始要求周期設定バッファレジスタ
	FIELD_0(0x00088A4A, 0x00088A4B, unsigned short, BTADCOBRB)	// タイマA/D変換開始要求周期設定バッファレジスタ
};

struct TRX62NMTUBReg
{
	FIELD_S(0x00088A0A, 0x00088A0C, unsigned char, TOER)	// タイマアウトプットマスタイネーブルレジスタ
	FIELD_0(0x00088A0D, 0x00088A0D, unsigned char, TGCR)	// タイマゲートコントロールレジスタ
	FIELD_0(0x00088A0E, 0x00088A0E, unsigned char, TOCR1)	// タイマアウトプットコントロールレジスタ1
	FIELD_S(0x00088A0F, 0x00088A13, unsigned char, TOCR2)	// タイマアウトプットコントロールレジスタ2
	FIELD_0(0x00088A14, 0x00088A15, unsigned short, TCDR)	// タイマ周期データレジスタ
	FIELD_S(0x00088A16, 0x00088A1F, unsigned short, TDDR)	// タイマデッドタイムデータレジスタ
	FIELD_0(0x00088A20, 0x00088A21, unsigned short, TCNTS)	// タイマサブカウンタ
	FIELD_S(0x00088A22, 0x00088A2F, unsigned short, TCBR)	// タイマ周期バッファレジスタ
	FIELD_0(0x00088A30, 0x00088A30, unsigned char, TITCR)	// タイマ割り込み間引き設定レジスタ
	FIELD_0(0x00088A31, 0x00088A31, unsigned char, TITCNT)	// タイマ割り込み間引き回数カウンタ
	FIELD_S(0x00088A32, 0x00088A33, unsigned char, TBTER)	// タイマバッファ転送設定レジスタ
	FIELD_S(0x00088A34, 0x00088A35, unsigned char, TDER)	// タイマデッドタイムイネーブルレジスタ
	FIELD_S(0x00088A36, 0x00088A5F, unsigned char, TOLBR)	// タイマアウトプットレベルバッファレジスタ
	FIELD_S(0x00088A60, 0x00088A7F, unsigned char, TWCR)	// タイマ波形コントロールレジスタ
	FIELD_0(0x00088A80, 0x00088A80, unsigned char, TSTR)	// タイマスタートレジスタ
	FIELD_S(0x00088A81, 0x00088A83, unsigned char, TSYR)	// タイマシンクロレジスタ
	FIELD_0(0x00088A84, 0x00088A84, unsigned char, TRWER)	// タイマリードライトイネーブルレジスタ
};

struct TRX62NMTU6Reg
{
	FIELD_0(0x00088B00, 0x00088B00, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_0(0x00088B01, 0x00088B01, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_0(0x00088B02, 0x00088B02, unsigned char, TIORH)	// タイマI/OコントロールレジスタH
	FIELD_0(0x00088B03, 0x00088B03, unsigned char, TIORL)	// タイマI/OコントロールレジスタL
	FIELD_0(0x00088B04, 0x00088B04, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_0(0x00088B05, 0x00088B05, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_0(0x00088B06, 0x00088B07, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x00088B08, 0x00088B09, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_0(0x00088B0A, 0x00088B0B, unsigned short, TGRB)	// タイマジェネラルレジスタB
	FIELD_0(0x00088B0C, 0x00088B0D, unsigned short, TGRC)	// タイマジェネラルレジスタC
	FIELD_S(0x00088B0E, 0x00088B1F, unsigned short, TGRD)	// タイマジェネラルレジスタD
	FIELD_0(0x00088B20, 0x00088B21, unsigned short, TGRE)	// タイマジェネラルレジスタE
	FIELD_0(0x00088B22, 0x00088B23, unsigned short, TGRF)	// タイマジェネラルレジスタF
	FIELD_S(0x00088B24, 0x00088B25, unsigned char, TIER2)	// タイマインタラプトイネーブルレジスタ2
	FIELD_0(0x00088B26, 0x00088B26, unsigned char, TBTM)	// タイマバッファ動作転送モードレジスタ
};

struct TRX62NMTU7Reg
{
	FIELD_0(0x00088B80, 0x00088B80, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_0(0x00088B81, 0x00088B81, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_S(0x00088B82, 0x00088B83, unsigned char, TIOR)	// タイマI/Oコントロールレジスタ
	FIELD_0(0x00088B84, 0x00088B84, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_0(0x00088B85, 0x00088B85, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_0(0x00088B86, 0x00088B87, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x00088B88, 0x00088B89, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_S(0x00088B8A, 0x00088B8F, unsigned short, TGRB)	// タイマジェネラルレジスタB
	FIELD_0(0x00088B90, 0x00088B90, unsigned char, TICCR)	// タイマインプットキャプチャコントロールレジスタ
};

struct TRX62NMTU8Reg
{
	FIELD_0(0x00088C00, 0x00088C00, unsigned char, TCR)	// タイマコントロールレジスタ
	FIELD_0(0x00088C01, 0x00088C01, unsigned char, TMDR)	// タイマモードレジスタ
	FIELD_S(0x00088C02, 0x00088C03, unsigned char, TIOR)	// タイマI/Oコントロールレジスタ
	FIELD_0(0x00088C04, 0x00088C04, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_0(0x00088C05, 0x00088C05, unsigned char, TSR)	// タイマステータスレジスタ
	FIELD_0(0x00088C06, 0x00088C07, unsigned short, TCNT)	// タイマカウンタ
	FIELD_0(0x00088C08, 0x00088C09, unsigned short, TGRA)	// タイマジェネラルレジスタA
	FIELD_0(0x00088C0A, 0x00088C0B, unsigned short, TGRB)	// タイマジェネラルレジスタB
};

struct TRX62NMTU11Reg
{
	FIELD_0(0x00088C80, 0x00088C81, unsigned short, TCNTU)	// タイマカウンタU
	FIELD_0(0x00088C82, 0x00088C83, unsigned short, TGRU)	// タイマジェネラルレジスタU
	FIELD_S(0x00088C84, 0x00088C85, unsigned char, TCRU)	// タイマコントロールレジスタU
	FIELD_S(0x00088C86, 0x00088C8F, unsigned char, TIORU)	// タイマI/OコントロールレジスタU
	FIELD_0(0x00088C90, 0x00088C91, unsigned short, TCNTV)	// タイマカウンタV
	FIELD_0(0x00088C92, 0x00088C93, unsigned short, TGRV)	// タイマジェネラルレジスタV
	FIELD_S(0x00088C94, 0x00088C95, unsigned char, TCRV)	// タイマコントロールレジスタV
	FIELD_S(0x00088C96, 0x00088C9F, unsigned char, TIORV)	// タイマI/OコントロールレジスタV
	FIELD_0(0x00088CA0, 0x00088CA1, unsigned short, TCNTW)	// タイマカウンタW
	FIELD_0(0x00088CA2, 0x00088CA3, unsigned short, TGRW)	// タイマジェネラルレジスタW
	FIELD_S(0x00088CA4, 0x00088CA5, unsigned char, TCRW)	// タイマコントロールレジスタW
	FIELD_S(0x00088CA6, 0x00088CB1, unsigned char, TIORW)	// タイマI/OコントロールレジスタW
	FIELD_S(0x00088CB2, 0x00088CB3, unsigned char, TIER)	// タイマインタラプトイネーブルレジスタ
	FIELD_S(0x00088CB4, 0x00088CB5, unsigned char, TSTR)	// タイマスタートレジスタ
	FIELD_0(0x00088CB6, 0x00088CB6, unsigned char, TCNTCMPCLR)	// タイマコンペアマッチクリアレジスタ
};

struct TRX62NS12ADReg
{
	FIELD_S(0x00089000, 0x00089003, unsigned char, ADCSR)	// A/Dコントロールレジスタ
	FIELD_S(0x00089004, 0x00089007, unsigned short, ADANS)	// A/Dチャネル選択レジスタ
	FIELD_S(0x00089008, 0x0008900B, unsigned short, ADADS)	// A/D変換値加算モード選択レジスタ
	FIELD_S(0x0008900C, 0x0008900D, unsigned char, ADADC)	// A/D変換値加算回数選択レジスタ
	FIELD_0(0x0008900E, 0x0008900F, unsigned short, ADCER)	// A/Dコントロール拡張レジスタ
	FIELD_S(0x00089010, 0x0008901F, unsigned char, ADSTRGR)	// A/D開始トリガ選択レジスタ
	FIELD_0(0x00089020, 0x00089021, unsigned short, ADDR0)	// A/Dデータレジスタ0
	FIELD_0(0x00089022, 0x00089023, unsigned short, ADDR1)	// A/Dデータレジスタ1
	FIELD_0(0x00089024, 0x00089025, unsigned short, ADDR2)	// A/Dデータレジスタ2
	FIELD_0(0x00089026, 0x00089027, unsigned short, ADDR3)	// A/Dデータレジスタ3
	FIELD_0(0x00089028, 0x00089029, unsigned short, ADDR4)	// A/Dデータレジスタ4
	FIELD_0(0x0008902A, 0x0008902B, unsigned short, ADDR5)	// A/Dデータレジスタ5
	FIELD_0(0x0008902C, 0x0008902D, unsigned short, ADDR6)	// A/Dデータレジスタ6
	FIELD_0(0x0008902E, 0x0008902F, unsigned short, ADDR7)	// A/Dデータレジスタ7
};

struct TRX62NPORT0Reg
{
	FIELD_S(0x0008C000, 0x0008C01F, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C020, 0x0008C03F, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C040, 0x0008C05F, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C060, 0x0008C07F, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C080, 0x0008C080, unsigned char, ODR)	// オープンドレインコントロールレジスタ
};

struct TRX62NPORT1Reg
{
	FIELD_S(0x0008C001, 0x0008C020, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C021, 0x0008C040, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C041, 0x0008C060, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C061, 0x0008C080, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C081, 0x0008C081, unsigned char, ODR)	// オープンドレインコントロールレジスタ
};

struct TRX62NPORT2Reg
{
	FIELD_S(0x0008C002, 0x0008C021, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C022, 0x0008C041, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C042, 0x0008C061, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C062, 0x0008C081, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C082, 0x0008C082, unsigned char, ODR)	// オープンドレインコントロールレジスタ
};

struct TRX62NPORT3Reg
{
	FIELD_S(0x0008C003, 0x0008C022, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C023, 0x0008C042, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C043, 0x0008C062, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C063, 0x0008C082, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C083, 0x0008C083, unsigned char, ODR)	// オープンドレインコントロールレジスタ
};

struct TRX62NPORT4Reg
{
	FIELD_S(0x0008C004, 0x0008C023, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C024, 0x0008C043, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C044, 0x0008C063, unsigned char, PORT)	// ポートレジスタ
	FIELD_0(0x0008C064, 0x0008C064, unsigned char, ICR)	// 入力バッファコントロールレジスタ
};

struct TRX62NPORT5Reg
{
	FIELD_S(0x0008C005, 0x0008C024, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C025, 0x0008C044, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C045, 0x0008C064, unsigned char, PORT)	// ポートレジスタ
	FIELD_0(0x0008C065, 0x0008C065, unsigned char, ICR)	// 入力バッファコントロールレジスタ
};

struct TRX62NPORT6Reg
{
	FIELD_S(0x0008C006, 0x0008C025, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C026, 0x0008C045, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C046, 0x0008C065, unsigned char, PORT)	// ポートレジスタ
	FIELD_0(0x0008C066, 0x0008C066, unsigned char, ICR)	// 入力バッファコントロールレジスタ
};

struct TRX62NPORT7Reg
{
	FIELD_S(0x0008C007, 0x0008C026, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C027, 0x0008C046, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C047, 0x0008C066, unsigned char, PORT)	// ポートレジスタ
	FIELD_0(0x0008C067, 0x0008C067, unsigned char, ICR)	// 入力バッファコントロールレジスタ
};

struct TRX62NPORT8Reg
{
	FIELD_S(0x0008C008, 0x0008C027, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C028, 0x0008C047, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C048, 0x0008C067, unsigned char, PORT)	// ポートレジスタ
	FIELD_0(0x0008C068, 0x0008C068, unsigned char, ICR)	// 入力バッファコントロールレジスタ
};

struct TRX62NPORT9Reg
{
	FIELD_S(0x0008C009, 0x0008C028, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C029, 0x0008C048, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C049, 0x0008C068, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C069, 0x0008C0C8, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C0C9, 0x0008C0C9, unsigned char, PCR)	// プルアップ抵抗コントロールレジスタ
};

struct TRX62NPORTAReg
{
	FIELD_S(0x0008C00A, 0x0008C029, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C02A, 0x0008C049, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C04A, 0x0008C069, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C06A, 0x0008C0C9, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C0CA, 0x0008C0CA, unsigned char, PCR)	// プルアップ抵抗コントロールレジスタ
};

struct TRX62NPORTBReg
{
	FIELD_S(0x0008C00B, 0x0008C02A, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C02B, 0x0008C04A, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C04B, 0x0008C06A, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C06B, 0x0008C0CA, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C0CB, 0x0008C0CB, unsigned char, PCR)	// プルアップ抵抗コントロールレジスタ
};

struct TRX62NPORTCReg
{
	FIELD_S(0x0008C00C, 0x0008C02B, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C02C, 0x0008C04B, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C04C, 0x0008C06B, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C06C, 0x0008C08B, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_S(0x0008C08C, 0x0008C0CB, unsigned char, ODR)	// オープンドレインコントロールレジスタ
	FIELD_0(0x0008C0CC, 0x0008C0CC, unsigned char, PCR)	// プルアップ抵抗コントロールレジスタ
};

struct TRX62NPORTDReg
{
	FIELD_S(0x0008C00D, 0x0008C02C, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C02D, 0x0008C04C, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C04D, 0x0008C06C, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C06D, 0x0008C0CC, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C0CD, 0x0008C0CD, unsigned char, PCR)	// プルアップ抵抗コントロールレジスタ
};

struct TRX62NPORTEReg
{
	FIELD_S(0x0008C00E, 0x0008C02D, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C02E, 0x0008C04D, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C04E, 0x0008C06D, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C06E, 0x0008C0CD, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C0CE, 0x0008C0CE, unsigned char, PCR)	// プルアップ抵抗コントロールレジスタ
};

struct TRX62NPORTFReg
{
	FIELD_S(0x0008C00F, 0x0008C02E, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C02F, 0x0008C04E, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C04F, 0x0008C06E, unsigned char, PORT)	// ポートレジスタ
	FIELD_0(0x0008C06F, 0x0008C06F, unsigned char, ICR)	// 入力バッファコントロールレジスタ
};

struct TRX62NPORTGReg
{
	FIELD_S(0x0008C010, 0x0008C02F, unsigned char, DDR)	// データディレクションレジスタ
	FIELD_S(0x0008C030, 0x0008C04F, unsigned char, DR)	// データレジスタ
	FIELD_S(0x0008C050, 0x0008C06F, unsigned char, PORT)	// ポートレジスタ
	FIELD_S(0x0008C070, 0x0008C0CF, unsigned char, ICR)	// 入力バッファコントロールレジスタ
	FIELD_0(0x0008C0D0, 0x0008C0D0, unsigned char, PCR)	// プルアップ抵抗コントロールレジスタ
};

struct TRX62NIOPORTReg
{
	FIELD_0(0x0008C100, 0x0008C100, unsigned char, PF0CSE)	// ポートファンクションレジスタ0
	FIELD_0(0x0008C101, 0x0008C101, unsigned char, PF1CSS)	// ポートファンクションレジスタ1
	FIELD_0(0x0008C102, 0x0008C102, unsigned char, PF2CSS)	// ポートファンクションレジスタ2
	FIELD_0(0x0008C103, 0x0008C103, unsigned char, PF3BUS)	// ポートファンクションレジスタ3
	FIELD_0(0x0008C104, 0x0008C104, unsigned char, PF4BUS)	// ポートファンクションレジスタ4
	FIELD_0(0x0008C105, 0x0008C105, unsigned char, PF5BUS)	// ポートファンクションレジスタ5
	FIELD_0(0x0008C106, 0x0008C106, unsigned char, PF6BUS)	// ポートファンクションレジスタ6
	FIELD_0(0x0008C107, 0x0008C107, unsigned char, PF7DMA)	// ポートファンクションレジスタ7
	FIELD_0(0x0008C108, 0x0008C108, unsigned char, PF8IRQ)	// ポートファンクションレジスタ8
	FIELD_0(0x0008C109, 0x0008C109, unsigned char, PF9IRQ)	// ポートファンクションレジスタ9
	FIELD_0(0x0008C10A, 0x0008C10A, unsigned char, PFAADC)	// ポートファンクションレジスタA
	FIELD_0(0x0008C10B, 0x0008C10B, unsigned char, PFBTMR)	// ポートファンクションレジスタB
	FIELD_0(0x0008C10C, 0x0008C10C, unsigned char, PFCMTU)	// ポートファンクションレジスタC
	FIELD_0(0x0008C10D, 0x0008C10D, unsigned char, PFDMTU)	// ポートファンクションレジスタD
	FIELD_0(0x0008C10E, 0x0008C10E, unsigned char, PFENET)	// ポートファンクションレジスタE
	FIELD_0(0x0008C10F, 0x0008C10F, unsigned char, PFFSCI)	// ポートファンクションレジスタF
	FIELD_0(0x0008C110, 0x0008C110, unsigned char, PFGSPI)	// ポートファンクションレジスタG
	FIELD_S(0x0008C111, 0x0008C112, unsigned char, PFHSPI)	// ポートファンクションレジスタH
	FIELD_0(0x0008C113, 0x0008C113, unsigned char, PFJCAN)	// ポートファンクションレジスタJ
	FIELD_0(0x0008C114, 0x0008C114, unsigned char, PFKUSB)	// ポートファンクションレジスタK
	FIELD_0(0x0008C115, 0x0008C115, unsigned char, PFLUSB)	// ポートファンクションレジスタL
	FIELD_0(0x0008C116, 0x0008C116, unsigned char, PFMPOE)	// ポートファンクションレジスタM
	FIELD_0(0x0008C117, 0x0008C117, unsigned char, PFNPOE)	// ポートファンクションレジスタN
};

struct TRX62NRTCReg
{
	FIELD_S(0x0008C400, 0x0008C401, unsigned char, R64CNT)	// 64Hzカウンタ
	FIELD_S(0x0008C402, 0x0008C403, unsigned char, RSECCNT)	// 秒カウンタ
	FIELD_S(0x0008C404, 0x0008C405, unsigned char, RMINCNT)	// 分カウンタ
	FIELD_S(0x0008C406, 0x0008C407, unsigned char, RHRCNT)	// 時カウンタ
	FIELD_S(0x0008C408, 0x0008C409, unsigned char, RWKCNT)	// 曜日カウンタ
	FIELD_S(0x0008C40A, 0x0008C40B, unsigned char, RDAYCNT)	// 日カウンタ
	FIELD_S(0x0008C40C, 0x0008C40D, unsigned char, RMONCNT)	// 月カウンタ
	FIELD_0(0x0008C40E, 0x0008C40F, unsigned short, RYRCNT)	// 年カウンタ
	FIELD_S(0x0008C410, 0x0008C411, unsigned char, RSECAR)	// 秒アラームレジスタ
	FIELD_S(0x0008C412, 0x0008C413, unsigned char, RMINAR)	// 分アラームレジスタ
	FIELD_S(0x0008C414, 0x0008C415, unsigned char, RHRAR)	// 時アラームレジスタ
	FIELD_S(0x0008C416, 0x0008C417, unsigned char, RWKAR)	// 曜日アラームレジスタ
	FIELD_S(0x0008C418, 0x0008C419, unsigned char, RDAYAR)	// 日アラームレジスタ
	FIELD_S(0x0008C41A, 0x0008C41B, unsigned char, RMONAR)	// 月アラームレジスタ
	FIELD_0(0x0008C41C, 0x0008C41D, unsigned short, RYRAR)	// 年アラームレジスタ
	FIELD_S(0x0008C41E, 0x0008C421, unsigned char, RYRAREN)	// 年アラームイネーブルレジスタ
	FIELD_S(0x0008C422, 0x0008C423, unsigned char, RCR1)	// RTCコントロールレジスタ1
	FIELD_0(0x0008C424, 0x0008C424, unsigned char, RCR2)	// RTCコントロールレジスタ2
};

struct TMBReg
{
	unsigned char Temp[16];
};

struct TRX62NCAN0Reg
{
	ARRAY_0(0x00090200, 0x000903FF, TMBReg, MB, 32)	// メールボックスレジスタ0～31
	FIELD_0(0x00090400, 0x00090403, unsigned int, MKR0)	// マスクレジスタ0
	FIELD_0(0x00090404, 0x00090407, unsigned int, MKR1)	// マスクレジスタ1
	FIELD_0(0x00090408, 0x0009040B, unsigned int, MKR2)	// マスクレジスタ2
	FIELD_0(0x0009040C, 0x0009040F, unsigned int, MKR3)	// マスクレジスタ3
	FIELD_0(0x00090410, 0x00090413, unsigned int, MKR4)	// マスクレジスタ4
	FIELD_0(0x00090414, 0x00090417, unsigned int, MKR5)	// マスクレジスタ5
	FIELD_0(0x00090418, 0x0009041B, unsigned int, MKR6)	// マスクレジスタ6
	FIELD_0(0x0009041C, 0x0009041F, unsigned int, MKR7)	// マスクレジスタ7
	FIELD_0(0x00090420, 0x00090423, unsigned int, FIDCR0)	// FIFO受信ID比較レジスタ0
	FIELD_0(0x00090424, 0x00090427, unsigned int, FIDCR1)	// FIFO受信ID比較レジスタ1
	FIELD_0(0x00090428, 0x0009042B, unsigned int, MKIVLR)	// マスク無効レジスタ
	FIELD_S(0x0009042C, 0x0009083E, unsigned int, MIER)	// メールボックス割り込み許可レジスタ
	ARRAY_0(0x00090820, 0x0009083F, unsigned char, MCTL, 32)	// メッセージ制御レジスタ0～31
	FIELD_0(0x00090840, 0x00090841, unsigned short, CTLR)	// 制御レジスタ
	FIELD_0(0x00090842, 0x00090843, unsigned short, STR)	// ステータスレジスタ
	FIELD_0(0x00090844, 0x00090847, unsigned int, BCR)	// ビットコンフィグレーションレジスタ
	FIELD_0(0x00090848, 0x00090848, unsigned char, RFCR)	// 受信FIFO制御レジスタ
	FIELD_0(0x00090849, 0x00090849, unsigned char, RFPCR)	// 受信FIFOポインタ制御レジスタ
	FIELD_0(0x0009084A, 0x0009084A, unsigned char, TFCR)	// 送信FIFO制御レジスタ
	FIELD_0(0x0009084B, 0x0009084B, unsigned char, TFPCR)	// 送信FIFOポインタ制御レジスタ
	FIELD_0(0x0009084C, 0x0009084C, unsigned char, EIER)	// エラー割り込み許可レジスタ
	FIELD_0(0x0009084D, 0x0009084D, unsigned char, EIFR)	// エラー割り込み要因判定レジスタ
	FIELD_0(0x0009084E, 0x0009084E, unsigned char, RECR)	// 受信エラーカウントレジスタ
	FIELD_0(0x0009084F, 0x0009084F, unsigned char, TECR)	// 送信エラーカウントレジスタ
	FIELD_0(0x00090850, 0x00090850, unsigned char, ECSR)	// エラーコード格納レジスタ
	FIELD_0(0x00090851, 0x00090851, unsigned char, CSSR)	// チャネルサーチサポートレジスタ
	FIELD_0(0x00090852, 0x00090852, unsigned char, MSSR)	// メールボックスサーチステータスレジスタ
	FIELD_0(0x00090853, 0x00090853, unsigned char, MSMR)	// メールボックスサーチモードレジスタ
	FIELD_0(0x00090854, 0x00090855, unsigned short, TSR)	// タイムスタンプレジスタ
	FIELD_0(0x00090856, 0x00090857, unsigned short, AFSR)	// アクセプタンスフィルタサポートレジスタ
	FIELD_0(0x00090858, 0x00090858, unsigned char, TCR)	// テスト制御レジスタ
};

struct TRX62NCAN1Reg
{
	ARRAY_0(0x00091200, 0x000913FF, TMBReg, MB, 32)	// メールボックスレジスタ0～31
	ARRAY_0(0x00091400, 0x0009141F, unsigned int, MKR, 8)	// マスクレジスタ0～7
	FIELD_0(0x00091420, 0x00091423, unsigned int, FIDCR0)	// FIFO受信ID比較レジスタ0
	FIELD_0(0x00091424, 0x00091427, unsigned int, FIDCR1)	// FIFO受信ID比較レジスタ1
	FIELD_0(0x00091428, 0x0009142B, unsigned int, MKIVLR)	// マスク無効レジスタ
	FIELD_S(0x0009142C, 0x0009183E, unsigned int, MIER)	// メールボックス割り込み許可レジスタ
	ARRAY_0(0x00091820, 0x0009183F, unsigned char, MCTL, 32)	// メッセージ制御レジスタ0～31
	FIELD_0(0x00091840, 0x00091841, unsigned short, CTLR)	// 制御レジスタ
	FIELD_0(0x00091842, 0x00091843, unsigned short, STR)	// ステータスレジスタ
	FIELD_0(0x00091844, 0x00091847, unsigned int, BCR)	// ビットコンフィグレーションレジスタ
	FIELD_0(0x00091848, 0x00091848, unsigned char, RFCR)	// 受信FIFO制御レジスタ
	FIELD_0(0x00091849, 0x00091849, unsigned char, RFPCR)	// 受信FIFOポインタ制御レジスタ
	FIELD_0(0x0009184A, 0x0009184A, unsigned char, TFCR)	// 送信FIFO制御レジスタ
	FIELD_0(0x0009184B, 0x0009184B, unsigned char, TFPCR)	// 送信FIFOポインタ制御レジスタ
	FIELD_0(0x0009184C, 0x0009184C, unsigned char, EIER)	// エラー割り込み許可レジスタ
	FIELD_0(0x0009184D, 0x0009184D, unsigned char, EIFR)	// エラー割り込み要因判定レジスタ
	FIELD_0(0x0009184E, 0x0009184E, unsigned char, RECR)	// 受信エラーカウントレジスタ
	FIELD_0(0x0009184F, 0x0009184F, unsigned char, TECR)	// 送信エラーカウントレジスタ
	FIELD_0(0x00091850, 0x00091850, unsigned char, ECSR)	// エラーコード格納レジスタ
	FIELD_0(0x00091851, 0x00091851, unsigned char, CSSR)	// チャネルサーチサポートレジスタ
	FIELD_0(0x00091852, 0x00091852, unsigned char, MSSR)	// メールボックスサーチステータスレジスタ
	FIELD_0(0x00091853, 0x00091853, unsigned char, MSMR)	// メールボックスサーチモードレジスタ
	FIELD_0(0x00091854, 0x00091855, unsigned short, TSR)	// タイムスタンプレジスタ
	FIELD_0(0x00091856, 0x00091857, unsigned short, AFSR)	// アクセプタンスフィルタサポートレジスタ
	FIELD_0(0x00091858, 0x00091858, unsigned char, TCR)	// テスト制御レジスタ
};

struct TRX62NCAN2Reg
{
	ARRAY_0(0x00092200, 0x000923FF, TMBReg, MB, 32)	// メールボックスレジスタ0～31
	ARRAY_0(0x00092400, 0x0009241F, unsigned int, MKR, 8)	// マスクレジスタ0～7
	FIELD_0(0x00092420, 0x00092423, unsigned int, FIDCR0)	// FIFO受信ID比較レジスタ0
	FIELD_0(0x00092424, 0x00092427, unsigned int, FIDCR1)	// FIFO受信ID比較レジスタ1
	FIELD_0(0x00092428, 0x0009242B, unsigned int, MKIVLR)	// マスク無効レジスタ
	FIELD_S(0x0009242C, 0x0009283E, unsigned int, MIER)	// メールボックス割り込み許可レジスタ
	ARRAY_0(0x00092820, 0x0009283F, unsigned char, MCTL, 32)	// メッセージ制御レジスタ0～31
	FIELD_0(0x00092840, 0x00092841, unsigned short, CTLR)	// 制御レジスタ
	FIELD_0(0x00092842, 0x00092843, unsigned short, STR)	// ステータスレジスタ
	FIELD_0(0x00092844, 0x00092847, unsigned int, BCR)	// ビットコンフィグレーションレジスタ
	FIELD_0(0x00092848, 0x00092848, unsigned char, RFCR)	// 受信FIFO制御レジスタ
	FIELD_0(0x00092849, 0x00092849, unsigned char, RFPCR)	// 受信FIFOポインタ制御レジスタ
	FIELD_0(0x0009284A, 0x0009284A, unsigned char, TFCR)	// 送信FIFO制御レジスタ
	FIELD_0(0x0009284B, 0x0009284B, unsigned char, TFPCR)	// 送信FIFOポインタ制御レジスタ
	FIELD_0(0x0009284C, 0x0009284C, unsigned char, EIER)	// エラー割り込み許可レジスタ
	FIELD_0(0x0009284D, 0x0009284D, unsigned char, EIFR)	// エラー割り込み要因判定レジスタ
	FIELD_0(0x0009284E, 0x0009284E, unsigned char, RECR)	// 受信エラーカウントレジスタ
	FIELD_0(0x0009284F, 0x0009284F, unsigned char, TECR)	// 送信エラーカウントレジスタ
	FIELD_0(0x00092850, 0x00092850, unsigned char, ECSR)	// エラーコード格納レジスタ
	FIELD_0(0x00092851, 0x00092851, unsigned char, CSSR)	// チャネルサーチサポートレジスタ
	FIELD_0(0x00092852, 0x00092852, unsigned char, MSSR)	// メールボックスサーチステータスレジスタ
	FIELD_0(0x00092853, 0x00092853, unsigned char, MSMR)	// メールボックスサーチモードレジスタ
	FIELD_0(0x00092854, 0x00092855, unsigned short, TSR)	// タイムスタンプレジスタ
	FIELD_0(0x00092856, 0x00092857, unsigned short, AFSR)	// アクセプタンスフィルタサポートレジスタ
	FIELD_0(0x00092858, 0x00092858, unsigned char, TCR)	// テスト制御レジスタ
};

struct TUSBSYSSTS0
{
	unsigned short LNST : 2;			// USBデータラインステータスモニタビット
	unsigned short IDMON : 1;			// 外部ID0入力端子モニタビット
	unsigned short : 3;
	unsigned short HTACT : 1;			// USBホストシーケンサステータスモニタビット
	unsigned short : 7;
	unsigned short OCVMON : 2;			// 外部USB0_OVRCURA/USB0_OVRCURB入力端子モニタビット
};

struct TUSBDVSTCTR0
{
	unsigned short RHST : 3;			// USBバスリセットステータスビット
	unsigned short : 1;
	unsigned short UACT : 1;			// USBバス許可ビット
	unsigned short RESUME : 1;			// レジューム出力ビット
	unsigned short USBRST : 1;			// USBバスリセット出力ビット
	unsigned short RWUPE : 1;			// ウェイクアップ検出許可ビット
	unsigned short WKUP : 1;			// ウェイクアップ出力ビット
	unsigned short VBUSEN : 1;			// USB0_VBUSEN出力端子制御ビット
	unsigned short EXICEN : 1;			// USB0_EXICEN出力端子制御ビット
	unsigned short HNPBTOA : 1;			// ホストネゴシエーションプロトコル（HNP）制御ビット
	unsigned short : 4;
};

struct TUSBFIFOSEL
{
	unsigned short CURPIPE: 4;			// CFIFOポートアクセスパイプ指定ビット
	unsigned short : 1;
	unsigned short ISEL  : 1;			// DCP選択時のCFIFOポートアクセス方向ビット
	unsigned short : 2;
	unsigned short BIGEND : 1;			// CFIFOポートエンディアン制御ビット
	unsigned short : 1;
	unsigned short MBW : 1;				// CFIFOポートアクセスビット幅ビット
	unsigned short : 3;
	unsigned short REW : 1;				// バッファポインタリワインドビット
	unsigned short RCNT : 1;			// リードカウントモードビット
};

struct TUSBFIFOCTR
{
	unsigned short DTLN : 12;			// 受信データ長表示ビット
	unsigned short : 1;
	unsigned short FRDY : 1;			// FIFOポートレディビット
	unsigned short BCLR : 1;			// CPUバッファクリアビット
	unsigned short BVAL : 1;			// バッファメモリ有効フラグ
};

struct TUSBINTSTS0
{
	unsigned short CTSQ : 3;			// コントロール転送ステージビット
	unsigned short VALID : 1;			// USBリクエストビット
	unsigned short DVSQ : 3;			// デバイスステートビット
	unsigned short VBSTS : 1;			// VBUS入力ステータスビット
	unsigned short BRDY : 1;			// バッファレディ割り込みステータスビット
	unsigned short NRDY : 1;			// バッファノットレディ割り込みステータスビット
	unsigned short BEMP : 1;			// バッファエンプティ割り込みステータスビット
	unsigned short CTRT : 1;			// コントロール転送ステージ遷移割り込みステータスビット
	unsigned short DVST : 1;			// デバイスステート遷移割り込みステータスビット
	unsigned short SOFR : 1;			// フレーム番号更新割り込みステータスビット
	unsigned short RESM : 1;			// レジューム割り込みステータスビット
	unsigned short VBINT : 1;			// VBUS割り込みステータスビット
};

struct TUSBINTSTS1
{
	unsigned short : 4;
	unsigned short SACK : 1;			// セットアップトランザクション正常応答割り込みステータスビット
	unsigned short SIGN : 1;			// セットアップトランザクションエラー割り込みステータスビット
	unsigned short EOFERR : 1;			// EOFエラー検出割り込みステータスビット
	unsigned short : 4;
	unsigned short ATTCH : 1;			// ATTCH割り込みステータスビット
	unsigned short DTCH : 1;			// USB切断検出割り込みステータスビット
	unsigned short : 1;
	unsigned short BCHG : 1;			// USBバス変化割り込みステータスビット
	unsigned short OVRCR : 1;			// オーバカレント入力変化割り込みステータスビット
};

struct TUSBBRDYSTS
{
	unsigned short PIPE0BRDY : 1;		// パイプ0のBRDY割り込みステータスビット
	unsigned short PIPE1BRDY : 1;		// パイプ1のBRDY割り込みステータスビット
	unsigned short PIPE2BRDY : 1;		// パイプ2のBRDY割り込みステータスビット
	unsigned short PIPE3BRDY : 1;		// パイプ3のBRDY割り込みステータスビット
	unsigned short PIPE4BRDY : 1;		// パイプ4のBRDY割り込みステータスビット
	unsigned short PIPE5BRDY : 1;		// パイプ5のBRDY割り込みステータスビット
	unsigned short PIPE6BRDY : 1;		// パイプ6のBRDY割り込みステータスビット
	unsigned short PIPE7BRDY : 1;		// パイプ7のBRDY割り込みステータスビット
	unsigned short PIPE8BRDY : 1;		// パイプ8のBRDY割り込みステータスビット
	unsigned short PIPE9BRDY : 1;		// パイプ9のBRDY割り込みステータスビット
};

struct TUSBNRDYSTS
{
	unsigned short PIPE0NRDY : 1;		// パイプ0のNRDY割り込みステータスビット
	unsigned short PIPE1NRDY : 1;		// パイプ1のNRDY割り込みステータスビット
	unsigned short PIPE2NRDY : 1;		// パイプ2のNRDY割り込みステータスビット
	unsigned short PIPE3NRDY : 1;		// パイプ3のNRDY割り込みステータスビット
	unsigned short PIPE4NRDY : 1;		// パイプ4のNRDY割り込みステータスビット
	unsigned short PIPE5NRDY : 1;		// パイプ5のNRDY割り込みステータスビット
	unsigned short PIPE6NRDY : 1;		// パイプ6のNRDY割り込みステータスビット
	unsigned short PIPE7NRDY : 1;		// パイプ7のNRDY割り込みステータスビット
	unsigned short PIPE8NRDY : 1;		// パイプ8のNRDY割り込みステータスビット
	unsigned short PIPE9NRDY : 1;		// パイプ9のNRDY割り込みステータスビット
};

struct TUSBBEMPSTS
{
	unsigned short PIPE0BEMP : 1;		// パイプ0のBEMP割り込みステータスビット
	unsigned short PIPE1BEMP : 1;		// パイプ1のBEMP割り込みステータスビット
	unsigned short PIPE2BEMP : 1;		// パイプ2のBEMP割り込みステータスビット
	unsigned short PIPE3BEMP : 1;		// パイプ3のBEMP割り込みステータスビット
	unsigned short PIPE4BEMP : 1;		// パイプ4のBEMP割り込みステータスビット
	unsigned short PIPE5BEMP : 1;		// パイプ5のBEMP割り込みステータスビット
	unsigned short PIPE6BEMP : 1;		// パイプ6のBEMP割り込みステータスビット
	unsigned short PIPE7BEMP : 1;		// パイプ7のBEMP割り込みステータスビット
	unsigned short PIPE8BEMP : 1;		// パイプ8のBEMP割り込みステータスビット
	unsigned short PIPE9BEMP : 1;		// パイプ9のBEMP割り込みステータスビット
};

struct TUSBDCPCFG
{
	unsigned short : 4;
	unsigned short DIR : 1;				// 転送方向設定ビット
	unsigned short : 2;
	unsigned short SHTNAK : 1;			// トランスファ終了時のパイプ禁止ビット
	unsigned short : 8;
};

struct TUSBDCPMAXP
{
	unsigned short MXPS : 7;			// マックスパケットサイズビット
	unsigned short : 5;
	unsigned short DEVSEL : 4;			// デバイス選択ビット
};

struct TUSBDCPCTR
{
	unsigned short PID : 2;				// 応答PIDビット
	unsigned short CCPL : 1;			// コントロール転送終了許可ビット
	unsigned short : 2;
	unsigned short PBUSY : 1;			// パイプビジービット
	unsigned short SQMON : 1;			// シーケンストグルビットモニタビット
	unsigned short SQSET : 1;			// トグルビットセットビット
	unsigned short SQCLR : 1;			// トグルビットクリアビット
	unsigned short : 2;
	unsigned short SUREQCLR : 1;		// SUREQビットクリアビット
	unsigned short : 2;
	unsigned short SUREQ : 1;			// SETUPトークン送出ビット
	unsigned short BSTS : 1;			// バッファステータスビット
};

struct TUSBPIPESEL
{
	unsigned short PIPESEL : 4;			// パイプウィンドウ選択ビット
	unsigned short : 12;
};

struct TUSBPIPECFG
{
	unsigned short EPNUM : 4;			// エンドポイント番号ビット
	unsigned short DIR : 1;				// 転送方向指定ビット
	unsigned short : 2;
	unsigned short SHTNAK: 1;			// トランスファ終了時のパイプ禁止ビット
	unsigned short : 1;
	unsigned short DBLB : 1;			// ダブルバッファモードビット
	unsigned short BFRE : 1;			// BRDY割り込み動作指定ビット
	unsigned short : 3;
	unsigned short TYPE : 2;			// 転送タイプビット
};

struct TUSBPIPEMAXP
{
	unsigned short MXPS : 9;			// MAXパケットサイズビット
	unsigned short : 3;
	unsigned short DEVSEL : 4;			// デバイス選択ビット
};

struct TUSBPIPEPERI
{
	unsigned short IITV : 3;			// インターバルエラー検出間隔ビット
	unsigned short : 9;
	unsigned short IFIS : 1;			// アイソクロナスINバッファフラッシュビット
	unsigned short : 3;
};

struct TUSBPIPECTR
{
	unsigned short PID : 2;				// 応答PIDビット
	unsigned short : 3;
	unsigned short PBUSY : 1;			// パイプビジービット
	unsigned short SQMON : 1;			// トグルビット確認ビット
	unsigned short SQSET : 1;			// トグルビットセットビット
	unsigned short SQCLR : 1;			// トグルビットクリアビット
	unsigned short ACLRM : 1;			// 自動バッファクリアモード
	unsigned short ATREPM : 1;			// 自動応答モードビット
	unsigned short : 3;
	unsigned short INBUFM : 1;			// 送信バッファモニタビット
	unsigned short BSTS : 1;			// バッファステータスビット
};

struct TUSBPIPETRE
{
	unsigned short : 8;
	unsigned short TRCLR : 1;			// トランザクションカウンタクリアビット
	unsigned short TRENB : 1;			// トランザクションカウンタ許可ビット
	unsigned short : 6;
};

struct TUSBPIPETRN
{
	unsigned short TRNCNT;				// トランザクションカウンタビット
};

struct TRX62NUSB0Reg
{
	FIELD_S(0x000A0000, 0x000A0003, unsigned short, SYSCFG)	// システムコンフィギュレーションコントロールレジスタ
	FIELD_S(0x000A0004, 0x000A0007, TUSBSYSSTS0, SYSSTS0)		// システムコンフィギュレーションステータスレジスタ0
	FIELD_S(0x000A0008, 0x000A0013, TUSBDVSTCTR0, DVSTCTR0)		// デバイスステートコントロールレジスタ0
	FIELD_S(0x000A0014, 0x000A0017, unsigned short, CFIFO)	// CFIFOポートレジスタ
	FIELD_S(0x000A0018, 0x000A001B, unsigned short, D0FIFO)	// D0FIFOポートレジスタ
	FIELD_S(0x000A001C, 0x000A001F, unsigned short, D1FIFO)	// D1FIFOポートレジスタ
	FIELD_0(0x000A0020, 0x000A0021, TUSBFIFOSEL, CFIFOSEL)		// CFIFOポート選択レジスタ
	FIELD_S(0x000A0022, 0x000A0027, TUSBFIFOCTR, CFIFOCTR)		// CFIFOポートコントロールレジスタ
	FIELD_0(0x000A0028, 0x000A0029, TUSBFIFOSEL, D0FIFOSEL)		// D0FIFOポート選択レジスタ
	FIELD_0(0x000A002A, 0x000A002B, TUSBFIFOCTR, D0FIFOCTR)		// D0FIFOポートコントロールレジスタ
	FIELD_0(0x000A002C, 0x000A002D, TUSBFIFOSEL, D1FIFOSEL)		// D1FIFOポート選択レジスタ
	FIELD_0(0x000A002E, 0x000A002F, TUSBFIFOCTR, D1FIFOCTR)		// D1FIFOポートコントロールレジスタ
	FIELD_0(0x000A0030, 0x000A0031, unsigned short, INTENB0)	// 割り込み許可レジスタ0
	FIELD_S(0x000A0032, 0x000A0035, unsigned short, INTENB1)	// 割り込み許可レジスタ1
	FIELD_0(0x000A0036, 0x000A0037, unsigned short, BRDYENB)	// BRDY割り込み許可レジスタ
	FIELD_0(0x000A0038, 0x000A0039, unsigned short, NRDYENB)	// NRDY割り込み許可レジスタ
	FIELD_0(0x000A003A, 0x000A003B, unsigned short, BEMPENB)	// BEMP割り込み許可レジスタ
	FIELD_S(0x000A003C, 0x000A003F, unsigned short, SOFCFG)	// SOF出力コンフィグレーションレジスタ
	FIELD_0(0x000A0040, 0x000A0041, TUSBINTSTS0, INTSTS0)		// 割り込みステータスレジスタ0
	FIELD_S(0x000A0042, 0x000A0045, TUSBINTSTS1, INTSTS1)		// 割り込みステータスレジスタ1
	FIELD_0(0x000A0046, 0x000A0047, TUSBBRDYSTS, BRDYSTS)		// BRDY割り込みステータスレジスタ
	FIELD_0(0x000A0048, 0x000A0049, TUSBNRDYSTS, NRDYSTS)		// NRDY割り込みステータスレジスタ
	FIELD_0(0x000A004A, 0x000A004B, TUSBBEMPSTS, BEMPSTS)		// BEMP割り込みステータスレジスタ
	FIELD_0(0x000A004C, 0x000A004D, unsigned short, FRMNUM)	// フレームナンバーレジスタ
	FIELD_0(0x000A004E, 0x000A004F, unsigned short, DVCHGR)	// デバイスステート切り替えレジスタ
	FIELD_S(0x000A0050, 0x000A0053, unsigned short, USBADDR)	// USBアドレスレジスタ
	FIELD_0(0x000A0054, 0x000A0055, unsigned short, USBREQ)	// USBリクエストタイプレジスタ
	FIELD_0(0x000A0056, 0x000A0057, unsigned short, USBVAL)	// USBリクエストバリューレジスタ
	FIELD_0(0x000A0058, 0x000A0059, unsigned short, USBINDX)	// USBリクエストインデックスレジスタ
	FIELD_0(0x000A005A, 0x000A005B, unsigned short, USBLENG)	// USBリクエストレングスレジスタ
	FIELD_0(0x000A005C, 0x000A005D, TUSBDCPCFG, DCPCFG)			// DCPコンフィギュレーションレジスタ
	FIELD_0(0x000A005E, 0x000A005F, TUSBDCPMAXP, DCPMAXP)		// DCPマックスパケットサイズレジスタ
	FIELD_S(0x000A0060, 0x000A0063, TUSBDCPCTR, DCPCTR)			// DCPコントロールレジスタ
	FIELD_S(0x000A0064, 0x000A0067, TUSBPIPESEL, PIPESEL)		// パイプウィンドウ選択レジスタ
	FIELD_S(0x000A0068, 0x000A006B, TUSBPIPECFG, PIPECFG)		// パイプコンフィギュレーションレジスタ
	FIELD_0(0x000A006C, 0x000A006D, TUSBPIPEMAXP, PIPEMAXP)		// パイプマックスパケットサイズレジスタ
	FIELD_0(0x000A006E, 0x000A006F, TUSBPIPEPERI, PIPEPERI)		// パイプ周期制御レジスタ
	FIELD_0(0x000A0070, 0x000A0071, TUSBPIPECTR, PIPE1CTR)		// パイプ1コントロールレジスタ
	FIELD_0(0x000A0072, 0x000A0073, TUSBPIPECTR, PIPE2CTR)		// パイプ2コントロールレジスタ
	FIELD_0(0x000A0074, 0x000A0075, TUSBPIPECTR, PIPE3CTR)		// パイプ3コントロールレジスタ
	FIELD_0(0x000A0076, 0x000A0077, TUSBPIPECTR, PIPE4CTR)		// パイプ4コントロールレジスタ
	FIELD_0(0x000A0078, 0x000A0079, TUSBPIPECTR, PIPE5CTR)		// パイプ5コントロールレジスタ
	FIELD_0(0x000A007A, 0x000A007B, TUSBPIPECTR, PIPE6CTR)		// パイプ6コントロールレジスタ
	FIELD_0(0x000A007C, 0x000A007D, TUSBPIPECTR, PIPE7CTR)		// パイプ7コントロールレジスタ
	FIELD_0(0x000A007E, 0x000A007F, TUSBPIPECTR, PIPE8CTR)		// パイプ8コントロールレジスタ
	FIELD_S(0x000A0080, 0x000A008F, TUSBPIPECTR, PIPE9CTR)		// パイプ9コントロールレジスタ
	FIELD_0(0x000A0090, 0x000A0091, TUSBPIPETRE, PIPE1TRE)		// パイプ1トランザクションカウンタイネーブルレジスタ
	FIELD_0(0x000A0092, 0x000A0093, TUSBPIPETRN, PIPE1TRN)		// パイプ1トランザクションカウンタレジスタ
	FIELD_0(0x000A0094, 0x000A0095, TUSBPIPETRE, PIPE2TRE)		// パイプ2トランザクションカウンタイネーブルレジスタ
	FIELD_0(0x000A0096, 0x000A0097, TUSBPIPETRN, PIPE2TRN)		// パイプ2トランザクションカウンタレジスタ
	FIELD_0(0x000A0098, 0x000A0099, TUSBPIPETRE, PIPE3TRE)		// パイプ3トランザクションカウンタイネーブルレジスタ
	FIELD_0(0x000A009A, 0x000A009B, TUSBPIPETRN, PIPE3TRN)		// パイプ3トランザクションカウンタレジスタ
	FIELD_0(0x000A009C, 0x000A009D, TUSBPIPETRE, PIPE4TRE)		// パイプ4トランザクションカウンタイネーブルレジスタ
	FIELD_0(0x000A009E, 0x000A009F, TUSBPIPETRN, PIPE4TRN)		// パイプ4トランザクションカウンタレジスタ
	FIELD_0(0x000A00A0, 0x000A00A1, TUSBPIPETRE, PIPE5TRE)		// パイプ5トランザクションカウンタイネーブルレジスタ
	FIELD_S(0x000A00A2, 0x000A00CF, TUSBPIPETRN, PIPE5TRN)		// パイプ5トランザクションカウンタレジスタ
	FIELD_0(0x000A00D0, 0x000A00D1, unsigned short, DEVADD0)	// デバイスアドレス0コンフィグレーションレジスタ
	FIELD_0(0x000A00D2, 0x000A00D3, unsigned short, DEVADD1)	// デバイスアドレス1コンフィグレーションレジスタ
	FIELD_0(0x000A00D4, 0x000A00D5, unsigned short, DEVADD2)	// デバイスアドレス2コンフィグレーションレジスタ
	FIELD_0(0x000A00D6, 0x000A00D7, unsigned short, DEVADD3)	// デバイスアドレス3コンフィグレーションレジスタ
	FIELD_0(0x000A00D8, 0x000A00D9, unsigned short, DEVADD4)	// デバイスアドレス4コンフィグレーションレジスタ
	FIELD_0(0x000A00DA, 0x000A00DB, unsigned short, DEVADD5)	// デバイスアドレス5コンフィグレーションレジスタ
};

struct TRX62NUSB1Reg
{
	FIELD_S(0x000A0200, 0x000A0203, unsigned short, SYSCFG)	// システムコンフィギュレーションコントロールレジスタ
	FIELD_S(0x000A0204, 0x000A0207, unsigned short, SYSSTS0)	// システムコンフィギュレーションステータスレジスタ0
	FIELD_S(0x000A0208, 0x000A0213, unsigned short, DVSTCTR0)	// デバイスステートコントロールレジスタ0
	FIELD_S(0x000A0214, 0x000A0217, unsigned short, CFIFO)	// CFIFOポートレジスタ
	FIELD_S(0x000A0218, 0x000A021B, unsigned short, D0FIFO)	// D0FIFOポートレジスタ
	FIELD_S(0x000A021C, 0x000A021F, unsigned short, D1FIFO)	// D1FIFOポートレジスタ
	FIELD_0(0x000A0220, 0x000A0221, unsigned short, CFIFOSEL)	// CFIFOポート選択レジスタ
	FIELD_S(0x000A0222, 0x000A0227, unsigned short, CFIFOCTR)	// CFIFOポートコントロールレジスタ
	FIELD_0(0x000A0228, 0x000A0229, unsigned short, D0FIFOSEL)	// D0FIFOポート選択レジスタ
	FIELD_0(0x000A022A, 0x000A022B, unsigned short, D0FIFOCTR)	// D0FIFOポートコントロールレジスタ
	FIELD_0(0x000A022C, 0x000A022D, unsigned short, D1FIFOSEL)	// D1FIFOポート選択レジスタ
	FIELD_0(0x000A022E, 0x000A022F, unsigned short, D1FIFOCTR)	// D1FIFOポートコントロールレジスタ
	FIELD_0(0x000A0230, 0x000A0231, unsigned short, INTENB0)	// 割り込み許可レジスタ0
	FIELD_S(0x000A0232, 0x000A0235, unsigned short, INTENB1)	// 割り込み許可レジスタ1
	FIELD_0(0x000A0236, 0x000A0237, unsigned short, BRDYENB)	// BRDY割り込み許可レジスタ
	FIELD_0(0x000A0238, 0x000A0239, unsigned short, NRDYENB)	// NRDY割り込み許可レジスタ
	FIELD_0(0x000A023A, 0x000A023B, unsigned short, BEMPENB)	// BEMP割り込み許可レジスタ
	FIELD_S(0x000A023C, 0x000A023F, unsigned short, SOFCFG)	// SOF出力コンフィグレーションレジスタ
	FIELD_0(0x000A0240, 0x000A0241, unsigned short, INTSTS0)	// 割り込みステータスレジスタ0
	FIELD_S(0x000A0242, 0x000A0245, unsigned short, INTSTS1)	// 割り込みステータスレジスタ1
	FIELD_0(0x000A0246, 0x000A0247, unsigned short, BRDYSTS)	// BRDY割り込みステータスレジスタ
	FIELD_0(0x000A0248, 0x000A0249, unsigned short, NRDYSTS)	// NRDY割り込みステータスレジスタ
	FIELD_0(0x000A024A, 0x000A024B, unsigned short, BEMPSTS)	// BEMP割り込みステータスレジスタ
	FIELD_0(0x000A024C, 0x000A024D, unsigned short, FRMNUM)	// フレームナンバーレジスタ
	FIELD_0(0x000A024E, 0x000A024F, unsigned short, DVCHGR)	// デバイスステート切り替えレジスタ
	FIELD_S(0x000A0250, 0x000A0253, unsigned short, USBADDR)	// USBアドレスレジスタ
	FIELD_0(0x000A0254, 0x000A0255, unsigned short, USBREQ)	// USBリクエストタイプレジスタ
	FIELD_0(0x000A0256, 0x000A0257, unsigned short, USBVAL)	// USBリクエストバリューレジスタ
	FIELD_0(0x000A0258, 0x000A0259, unsigned short, USBINDX)	// USBリクエストインデックスレジスタ
	FIELD_0(0x000A025A, 0x000A025B, unsigned short, USBLENG)	// USBリクエストレングスレジスタ
	FIELD_0(0x000A025C, 0x000A025D, unsigned short, DCPCFG)	// DCPコンフィギュレーションレジスタ
	FIELD_0(0x000A025E, 0x000A025F, unsigned short, DCPMAXP)	// DCPマックスパケットサイズレジスタ
	FIELD_S(0x000A0260, 0x000A0263, unsigned short, DCPCTR)	// DCPコントロールレジスタ
	FIELD_S(0x000A0264, 0x000A0267, unsigned short, PIPESEL)	// パイプウィンドウ選択レジスタ
	FIELD_S(0x000A0268, 0x000A026B, unsigned short, PIPECFG)	// パイプコンフィギュレーションレジスタ
	FIELD_0(0x000A026C, 0x000A026D, unsigned short, PIPEMAXP)	// パイプマックスパケットサイズレジスタ
	FIELD_0(0x000A026E, 0x000A026F, unsigned short, PIPEPERI)	// パイプ周期制御レジスタ
	FIELD_0(0x000A0270, 0x000A0271, unsigned short, PIPE1CTR)	// パイプ1コントロールレジスタ
	FIELD_0(0x000A0272, 0x000A0273, unsigned short, PIPE2CTR)	// パイプ2コントロールレジスタ
	FIELD_0(0x000A0274, 0x000A0275, unsigned short, PIPE3CTR)	// パイプ3コントロールレジスタ
	FIELD_0(0x000A0276, 0x000A0277, unsigned short, PIPE4CTR)	// パイプ4コントロールレジスタ
	FIELD_0(0x000A0278, 0x000A0279, unsigned short, PIPE5CTR)	// パイプ5コントロールレジスタ
	FIELD_0(0x000A027A, 0x000A027B, unsigned short, PIPE6CTR)	// パイプ6コントロールレジスタ
	FIELD_0(0x000A027C, 0x000A027D, unsigned short, PIPE7CTR)	// パイプ7コントロールレジスタ
	FIELD_0(0x000A027E, 0x000A027F, unsigned short, PIPE8CTR)	// パイプ8コントロールレジスタ
	FIELD_S(0x000A0280, 0x000A028F, unsigned short, PIPE9CTR)	// パイプ9コントロールレジスタ
	FIELD_0(0x000A0290, 0x000A0291, unsigned short, PIPE1TRE)	// パイプ1トランザクションカウンタイネーブルレジスタ
	FIELD_0(0x000A0292, 0x000A0293, unsigned short, PIPE1TRN)	// パイプ1トランザクションカウンタレジスタ
	FIELD_0(0x000A0294, 0x000A0295, unsigned short, PIPE2TRE)	// パイプ2トランザクションカウンタイネーブルレジスタ
	FIELD_0(0x000A0296, 0x000A0297, unsigned short, PIPE2TRN)	// パイプ2トランザクションカウンタレジスタ
	FIELD_0(0x000A0298, 0x000A0299, unsigned short, PIPE3TRE)	// パイプ3トランザクションカウンタイネーブルレジスタ
	FIELD_0(0x000A029A, 0x000A029B, unsigned short, PIPE3TRN)	// パイプ3トランザクションカウンタレジスタ
	FIELD_0(0x000A029C, 0x000A029D, unsigned short, PIPE4TRE)	// パイプ4トランザクションカウンタイネーブルレジスタ
	FIELD_0(0x000A029E, 0x000A029F, unsigned short, PIPE4TRN)	// パイプ4トランザクションカウンタレジスタ
	FIELD_0(0x000A02A0, 0x000A02A1, unsigned short, PIPE5TRE)	// パイプ5トランザクションカウンタイネーブルレジスタ
	FIELD_S(0x000A02A2, 0x000A02CF, unsigned short, PIPE5TRN)	// パイプ5トランザクションカウンタレジスタ
	FIELD_0(0x000A02D0, 0x000A02D1, unsigned short, DEVADD0)	// デバイスアドレス0コンフィグレーションレジスタ
	FIELD_0(0x000A02D2, 0x000A02D3, unsigned short, DEVADD1)	// デバイスアドレス1コンフィグレーションレジスタ
	FIELD_0(0x000A02D4, 0x000A02D5, unsigned short, DEVADD2)	// デバイスアドレス2コンフィグレーションレジスタ
	FIELD_0(0x000A02D6, 0x000A02D7, unsigned short, DEVADD3)	// デバイスアドレス3コンフィグレーションレジスタ
	FIELD_0(0x000A02D8, 0x000A02D9, unsigned short, DEVADD4)	// デバイスアドレス4コンフィグレーションレジスタ
	FIELD_0(0x000A02DA, 0x000A02DB, unsigned short, DEVADD5)	// デバイスアドレス5コンフィグレーションレジスタ
};

struct TRX62NUSBReg
{
	FIELD_0(0x000A0400, 0x000A0403, unsigned int, DPUSR0R)	// ディープスタンバイUSBトランシーバ制御／端子モニタレジスタ
	FIELD_0(0x000A0404, 0x000A0407, unsigned int, DPUSR1R)	// ディープスタンバイUSBサスペンド／レジューム割り込みレジスタ
};

struct TEDMACEDTRR
{
	unsigned int TR : 1;	// 送信要求ビット
	unsigned int : 15;
};

struct TEDMACEDRRR
{
	unsigned int RR : 1;	// 受信要求ビット
	unsigned int : 15;
};

struct TEDMACEESR
{
	unsigned int CERF : 1;	// 受信フレームCRCエラービット
	unsigned int PRE : 1;	// PHY-LSI受信エラービット
	unsigned int RTSF : 1;	// ショートフレーム受信エラービット
	unsigned int RTLF : 1;	// ロングフレーム受信エラービット
	unsigned int RRF : 1;	// 端数ビットフレーム受信ビット
	unsigned int : 2;
	unsigned int RMAF : 1;	// マルチキャストアドレスフレーム受信ビット
	unsigned int TRO : 1;	// 送信リトライオーバビット
	unsigned int CD : 1;	// 遅延衝突検出ビット
	unsigned int DLC : 1;	// キャリア消失検出ビット
	unsigned int CND : 1;	// キャリア未検出ビット
	unsigned int : 4;
	unsigned int RFOF : 1;	// 受信FIFOオーバフロービット
	unsigned int RDE : 1;	// 受信ディスクリプタ枯渇ビット
	unsigned int FR : 1;	// フレーム受信ビット
	unsigned int TFUF : 1;	// 送信FIFOアンダフロービット
	unsigned int TDE : 1;	// 送信ディスクリプタ枯渇ビット
	unsigned int TC : 1;	// フレーム送信完了ビット
	unsigned int ECI : 1;	// ETHERCステータスレジスタ要因ビット
	unsigned int ADE : 1;	// アドレスエラービット
	unsigned int RFCOF : 1;	// 受信フレームカウンタオーバフロービット
	unsigned int RABT : 1;	// 受信中断検出ビット
	unsigned int TABT : 1;	// 送信中断検出ビット
	unsigned int : 3;
	unsigned int TWB : 1;	// ライトバック完了ビット
};

struct TRX62NEDMACReg
{
	FIELD_S(0x000C0000, 0x000C0007, unsigned int, EDMR)	// EDMACモードレジスタ
	FIELD_S(0x000C0008, 0x000C000F, TEDMACEDTRR, EDTRR)	// EDMAC送信要求レジスタ
	FIELD_S(0x000C0010, 0x000C0017, TEDMACEDRRR, EDRRR)	// EDMAC受信要求レジスタ
	FIELD_S(0x000C0018, 0x000C001F, unsigned int, TDLAR)	// 送信ディスクリプタリスト先頭アドレスレジスタ
	FIELD_S(0x000C0020, 0x000C0027, unsigned int, RDLAR)	// 受信ディスクリプタリスト先頭アドレスレジスタ
	FIELD_S(0x000C0028, 0x000C002F, TEDMACEESR, EESR)	// ETHERC/EDMACステータスレジスタ
	FIELD_S(0x000C0030, 0x000C0037, unsigned int, EESIPR)	// ETHERC/EDMACステータス割り込み許可レジスタ
	FIELD_S(0x000C0038, 0x000C003F, unsigned int, TRSCER)	// 送受信ステータスコピー指示レジスタ
	FIELD_S(0x000C0040, 0x000C0047, unsigned int, RMFCR)	// ミスドフレームカウンタレジスタ
	FIELD_S(0x000C0048, 0x000C004F, unsigned int, TFTR)	// 送信FIFOしきい値指定レジスタ
	FIELD_S(0x000C0050, 0x000C0057, unsigned int, FDR)	// FIFO容量指定レジスタ
	FIELD_S(0x000C0058, 0x000C0063, unsigned int, RMCR)	// 受信方式制御レジスタ
	FIELD_0(0x000C0064, 0x000C0067, unsigned int, TFUCR)	// 送信FIFOアンダランカウント
	FIELD_0(0x000C0068, 0x000C006B, unsigned int, RFOCR)	// 受信FIFOオーバフローカウント
	FIELD_0(0x000C006C, 0x000C006F, unsigned int, IOSR)	// 個別出力信号設定レジスタ
	FIELD_S(0x000C0070, 0x000C0077, unsigned int, FCFTR)	// フロー制御開始FIFOしきい値設定レジスタ
	FIELD_0(0x000C0078, 0x000C007B, unsigned int, RPADIR)	// 受信データパディング挿入設定レジスタ
	FIELD_S(0x000C007C, 0x000C00C7, unsigned int, TRIMD)	// 送信割り込み設定レジスタ
	FIELD_0(0x000C00C8, 0x000C00CB, unsigned int, RBWAR)	// 受信バッファライトアドレスレジスタ
	FIELD_S(0x000C00CC, 0x000C00D3, unsigned int, RDFAR)	// 受信ディスクリプタフェッチアドレスレジスタ
	FIELD_0(0x000C00D4, 0x000C00D7, unsigned int, TBRAR)	// 送信バッファリードアドレスレジスタ
	FIELD_0(0x000C00D8, 0x000C00DB, unsigned int, TDFAR)	// 送信ディスクリプタフェッチアドレスレジスタ
};

struct TETHERCECMR
{
	unsigned int PRM : 1;	// プロミスキャスモードビット
	unsigned int DM : 1;	// デュプレックスモードビット
	unsigned int RTM : 1;	// 送受信レートビット
	unsigned int ILB : 1;	// 内部ループバックモードビット
	unsigned int : 1;
	unsigned int TE : 1;	// 送信許可ビット
	unsigned int RE : 1;	// 受信許可ビット
	unsigned int : 2;
	unsigned int MPDE : 1;	// Magic PacketTM検出許可ビット
	unsigned int : 2;
	unsigned int PRCEF : 1;	// CRCエラーフレーム受信許可ビット
	unsigned int : 3;
	unsigned int TXF : 1;	// 送信系フロー制御動作モードビット
	unsigned int RXF : 1;	// 受信系フロー制御動作モードビット
	unsigned int PFR : 1;	// PAUSEフレーム受信モードビット
	unsigned int ZPF : 1;	// 0 time PAUSEフレーム使用許可ビット
	unsigned int TPC : 1;	// PAUSEフレーム送信ビット
	unsigned int : 11;
};

struct TRX62NETHERCReg
{
	FIELD_S(0x000C0100, 0x000C0107, TETHERCECMR, ECMR)	// ETHERCモードレジスタ
	FIELD_S(0x000C0108, 0x000C010F, unsigned int, RFLR)	// 受信フレーム長上限レジスタ
	FIELD_S(0x000C0110, 0x000C0117, unsigned int, ECSR)	// ETHERCステータスレジスタ
	FIELD_S(0x000C0118, 0x000C011F, unsigned int, ECSIPR)	// ETHERC割り込み許可レジスタ
	FIELD_S(0x000C0120, 0x000C0127, unsigned int, PIR)	// PHY部インタフェースレジスタ
	FIELD_S(0x000C0128, 0x000C013F, unsigned int, PSR)	// PHY部ステータスレジスタ
	FIELD_S(0x000C0140, 0x000C014F, unsigned int, RDMLR)	// 乱数生成カウンタ上限値設定レジスタ
	FIELD_0(0x000C0150, 0x000C0153, unsigned int, IPGR)	// IPG設定レジスタ
	FIELD_0(0x000C0154, 0x000C0157, unsigned int, APR)	// 自動PAUSEフレーム設定レジスタ
	FIELD_S(0x000C0158, 0x000C015F, unsigned int, MPR)	// 手動PAUSEフレーム設定レジスタ
	FIELD_0(0x000C0160, 0x000C0163, unsigned int, RFCF)	// 受信PAUSEフレームカウンタ
	FIELD_0(0x000C0164, 0x000C0167, unsigned int, TPAUSER)	// 自動PAUSEフレーム再送回数設定レジスタ
	FIELD_0(0x000C0168, 0x000C016B, unsigned int, TPAUSECR)	// PAUSEフレーム再送回数カウンタ
	FIELD_S(0x000C016C, 0x000C01BF, unsigned int, BCFRR)	// Broadcastフレーム受信回数設定レジスタ
	FIELD_S(0x000C01C0, 0x000C01C7, unsigned int, MAHR)	// MACアドレス上位設定レジスタ
	FIELD_S(0x000C01C8, 0x000C01CF, unsigned int, MALR)	// MACアドレス下位設定レジスタ
	FIELD_0(0x000C01D0, 0x000C01D3, unsigned int, TROCR)	// 送信リトライオーバカウンタレジスタ
	FIELD_0(0x000C01D4, 0x000C01D7, unsigned int, CDCR)	// 遅延衝突検出カウンタレジスタ
	FIELD_0(0x000C01D8, 0x000C01DB, unsigned int, LCCR)	// キャリア消失カウンタレジスタ
	FIELD_S(0x000C01DC, 0x000C01E3, unsigned int, CNDCR)	// キャリア未検出カウンタレジスタ
	FIELD_0(0x000C01E4, 0x000C01E7, unsigned int, CEFCR)	// CRCエラーフレーム受信カウンタレジスタ
	FIELD_0(0x000C01E8, 0x000C01EB, unsigned int, FRECR)	// フレーム受信エラーカウンタレジスタ
	FIELD_0(0x000C01EC, 0x000C01EF, unsigned int, TSFRCR)	// 64バイト未満フレーム受信カウンタレジスタ
	FIELD_0(0x000C01F0, 0x000C01F3, unsigned int, TLFRCR)	// 指定バイト超フレーム受信カウンタレジスタ
	FIELD_0(0x000C01F4, 0x000C01F7, unsigned int, RFCR)	// 端数ビットフレーム受信カウンタレジスタ
	FIELD_0(0x000C01F8, 0x000C01FB, unsigned int, MAFCR)	// マルチキャストアドレスフレーム受信カウンタレジスタ
};

struct TRX62NFLASHFSTATR0
{
	unsigned char PRGSPD : 1;			// 書き込みサスペンドステータスビット
	unsigned char ERSSPD : 1;			// 消去サスペンドステータスビット
	unsigned char : 1;
	unsigned char SUSRDY : 1;			// サスペンドレディビット
	unsigned char PRGERR : 1;			// 書き込みエラービット
	unsigned char ERSERR : 1;			// 消去エラービット
	unsigned char ILGLERR : 1;			// イリーガルコマンドエラービット
	unsigned char FRDY : 1;				// フラッシュレディビット
};

struct TRX62NFLASHFSTATR1
{
	unsigned char : 4;
	unsigned char FLOCKST : 1;			// ロックビットステータスビット
	unsigned char : 2;
	unsigned char FCUERR : 1;			// FCUエラービット
};

struct TRX62NFLASHReg
{
	FIELD_S(0x007FC400/*0x0008C289*/, 0x007FC401, unsigned char, FWEPROR)	// フラッシュライトイレースプロテクトレジスタ
	FIELD_S(0x007FC402, 0x007FC40F, unsigned char, FMODR)	// フラッシュモードレジスタ
	FIELD_0(0x007FC410, 0x007FC410, unsigned char, FASTAT)	// フラッシュアクセスステータスレジスタ
	FIELD_0(0x007FC411, 0x007FC411, unsigned char, FAEINT)	// フラッシュアクセスエラー割り込み許可レジスタ
	FIELD_S(0x007FC412, 0x007FC43F, unsigned char, FRDYIE)	// フラッシュレディ割り込み許可レジスタ
	FIELD_0(0x007FC440, 0x007FC441, unsigned short, DFLRE0)	// データフラッシュ読み出し許可レジスタ0
	FIELD_S(0x007FC442, 0x007FC44F, unsigned short, DFLRE1)	// データフラッシュ読み出し許可レジスタ1
	FIELD_0(0x007FC450, 0x007FC451, unsigned short, DFLWE0)	// データフラッシュ書き込み／消去許可レジスタ0
	FIELD_0(0x007FC452, 0x007FC453, unsigned short, DFLWE1)	// データフラッシュ書き込み／消去許可レジスタ1
	FIELD_S(0x007FC454, 0x007FFFAF, unsigned short, FCURAME)	// FCU RAMイネーブルレジスタ
	FIELD_0(0x007FFFB0, 0x007FFFB0, TRX62NFLASHFSTATR0, FSTATR0)	// フラッシュステータスレジスタ0
	FIELD_0(0x007FFFB1, 0x007FFFB1, TRX62NFLASHFSTATR1, FSTATR1)	// フラッシュステータスレジスタ1
	FIELD_0(0x007FFFB2, 0x007FFFB3, unsigned short, FENTRYR)	// フラッシュP/Eモードエントリレジスタ
	FIELD_0(0x007FFFB4, 0x007FFFB5, unsigned short, FPROTR)	// フラッシュプロテクトレジスタ
	FIELD_S(0x007FFFB6, 0x007FFFB9, unsigned short, FRESETR)	// フラッシュリセットレジスタ
	FIELD_S(0x007FFFBA, 0x007FFFC7, unsigned short, FCMDR)	// FCUコマンドレジスタ
	FIELD_0(0x007FFFC8, 0x007FFFC9, unsigned short, FCPSR)	// FCU処理切り替えレジスタ
	FIELD_0(0x007FFFCA, 0x007FFFCB, unsigned short, DFLBCCNT)	// データフラッシュブランクチェックレジスタ
	FIELD_0(0x007FFFCC, 0x007FFFCD, unsigned short, FPESTAT)	// フラッシュP/Eステータスレジスタ
	FIELD_S(0x007FFFCE, 0x007FFFE7, unsigned short, DFLBCSTAT)	// データフラッシュブランクチェック制御ステータスレジスタ
	FIELD_0(0x007FFFE8, 0x007FFFE9, unsigned short, PCKAR)	// 周辺クロック通知レジスタ
};

#pragma warning(disable:4201)

union TRX62NReg
{
	TRX62NSYSTEMReg SYSTEM;
	struct { uint8_t _ofs_BSC[0x00081300 - 0x00080000]; TRX62NBSCReg BSC; };
	struct { uint8_t _ofs_DMAC0[0x00082000 - 0x00080000]; TRX62NDMAC0Reg DMAC0; };
	struct { uint8_t _ofs_DMAC1[0x00082040 - 0x00080000]; TRX62NDMAC1Reg DMAC1; };
	struct { uint8_t _ofs_DMAC2[0x00082080 - 0x00080000]; TRX62NDMAC2Reg DMAC2; };
	struct { uint8_t _ofs_DMAC3[0x000820C0 - 0x00080000]; TRX62NDMAC3Reg DMAC3; };
	struct { uint8_t _ofs_DMAC[0x00082200 - 0x00080000]; TRX62NDMACReg DMAC; };
	struct { uint8_t _ofs_DTC[0x00082400 - 0x00080000]; TRX62NDTCReg DTC; };
	struct { uint8_t _ofs_EXDMAC0[0x00082800 - 0x00080000]; TRX62NEXDMAC0Reg EXDMAC0; };
	struct { uint8_t _ofs_EXDMAC1[0x00082840 - 0x00080000]; TRX62NEXDMAC1Reg EXDMAC1; };
	struct { uint8_t _ofs_EXDMAC[0x00082A00 - 0x00080000]; TRX62NEXDMACReg EXDMAC; };
	struct { uint8_t _ofs_MPU[0x00086400 - 0x00080000]; TRX62NMPUReg MPU; };
	struct { uint8_t _ofs_ICU[0x00087000 - 0x00080000]; TRX62NICUReg ICU; };
	struct { uint8_t _ofs_CMT[0x00088000 - 0x00080000]; TRX62NCMTReg CMT; };
	struct { uint8_t _ofs_CMT0[0x00088002 - 0x00080000]; TRX62NCMT0Reg CMT0; };
	struct { uint8_t _ofs_CMT1[0x00088008 - 0x00080000]; TRX62NCMT1Reg CMT1; };
	struct { uint8_t _ofs_CMT2[0x00088012 - 0x00080000]; TRX62NCMT2Reg CMT2; };
	struct { uint8_t _ofs_CMT3[0x00088018 - 0x00080000]; TRX62NCMT3Reg CMT3; };
	struct { uint8_t _ofs_WDT[0x00088028 - 0x00080000]; TRX62NWDTReg WDT; };
	struct { uint8_t _ofs_IWDT[0x00088030 - 0x00080000]; TRX62NIWDTReg IWDT; };
	struct { uint8_t _ofs_AD0[0x00088040 - 0x00080000]; TRX62NAD0Reg AD0; };
	struct { uint8_t _ofs_AD1[0x00088060 - 0x00080000]; TRX62NAD1Reg AD1; };
	struct { uint8_t _ofs_DA[0x000880C0 - 0x00080000]; TRX62NDAReg DA; };
	struct { uint8_t _ofs_PPG0[0x000881E6 - 0x00080000]; TRX62NPPG0Reg PPG0; };
	struct { uint8_t _ofs_PPG1[0x000881F0 - 0x00080000]; TRX62NPPG1Reg PPG1; };
	struct { uint8_t _ofs_TMR0[0x00088200 - 0x00080000]; TRX62NTMR0Reg TMR0; };
	struct { uint8_t _ofs_TMR1[0x00088201 - 0x00080000]; TRX62NTMR1Reg TMR1; };
	struct { uint8_t _ofs_TMR01[0x00088204 - 0x00080000]; TRX62NTMR01Reg TMR01; };
	struct { uint8_t _ofs_TMR2[0x00088210 - 0x00080000]; TRX62NTMR2Reg TMR2; };
	struct { uint8_t _ofs_TMR3[0x00088211 - 0x00080000]; TRX62NTMR3Reg TMR3; };
	struct { uint8_t _ofs_TMR23[0x00088214 - 0x00080000]; TRX62NTMR23Reg TMR23; };
	struct { uint8_t _ofs_SMCI0[0x00088240 - 0x00080000]; TRX62NSMCI0Reg SMCI0; };
	struct { uint8_t _ofs_SCI0[0x00088240 - 0x00080000]; TRX62NSCI0Reg SCI0; };
	struct { uint8_t _ofs_SMCI1[0x00088248 - 0x00080000]; TRX62NSMCI1Reg SMCI1; };
	struct { uint8_t _ofs_SCI1[0x00088248 - 0x00080000]; TRX62NSCI1Reg SCI1; };
	struct { uint8_t _ofs_SMCI2[0x00088250 - 0x00080000]; TRX62NSMCI2Reg SMCI2; };
	struct { uint8_t _ofs_SCI2[0x00088250 - 0x00080000]; TRX62NSCI2Reg SCI2; };
	struct { uint8_t _ofs_SMCI3[0x00088258 - 0x00080000]; TRX62NSMCI3Reg SMCI3; };
	struct { uint8_t _ofs_SCI3[0x00088258 - 0x00080000]; TRX62NSCI3Reg SCI3; };
	struct { uint8_t _ofs_SMCI5[0x00088268 - 0x00080000]; TRX62NSMCI5Reg SMCI5; };
	struct { uint8_t _ofs_SCI5[0x00088268 - 0x00080000]; TRX62NSCI5Reg SCI5; };
	struct { uint8_t _ofs_SMCI6[0x00088270 - 0x00080000]; TRX62NSMCI6Reg SMCI6; };
	struct { uint8_t _ofs_SCI6[0x00088270 - 0x00080000]; TRX62NSCI6Reg SCI6; };
	struct { uint8_t _ofs_CRC[0x00088280 - 0x00080000]; TRX62NCRCReg CRC; };
	struct { uint8_t _ofs_RIIC0[0x00088300 - 0x00080000]; TRX62NRIIC0Reg RIIC0; };
	struct { uint8_t _ofs_RIIC1[0x00088320 - 0x00080000]; TRX62NRIIC1Reg RIIC1; };
	struct { uint8_t _ofs_RSPI0[0x00088380 - 0x00080000]; TRX62NRSPI0Reg RSPI0; };
	struct { uint8_t _ofs_RSPI1[0x000883A0 - 0x00080000]; TRX62NRSPI1Reg RSPI1; };
	struct { uint8_t _ofs_MTU3[0x00088600 - 0x00080000]; TRX62NMTU3Reg MTU3; };
	struct { uint8_t _ofs_MTU4[0x00088601 - 0x00080000]; TRX62NMTU4Reg MTU4; };
	struct { uint8_t _ofs_MTUA[0x0008860A - 0x00080000]; TRX62NMTUAReg MTUA; };
	struct { uint8_t _ofs_MTU0[0x00088700 - 0x00080000]; TRX62NMTU0Reg MTU0; };
	struct { uint8_t _ofs_MTU1[0x00088780 - 0x00080000]; TRX62NMTU1Reg MTU1; };
	struct { uint8_t _ofs_MTU2[0x00088800 - 0x00080000]; TRX62NMTU2Reg MTU2; };
	struct { uint8_t _ofs_MTU5[0x00088880 - 0x00080000]; TRX62NMTU5Reg MTU5; };
	struct { uint8_t _ofs_POE[0x00088900 - 0x00080000]; TRX62NPOEReg POE; };
	struct { uint8_t _ofs_MTU9[0x00088A00 - 0x00080000]; TRX62NMTU9Reg MTU9; };
	struct { uint8_t _ofs_MTU10[0x00088A01 - 0x00080000]; TRX62NMTU10Reg MTU10; };
	struct { uint8_t _ofs_MTUB[0x00088A0A - 0x00080000]; TRX62NMTUBReg MTUB; };
	struct { uint8_t _ofs_MTU6[0x00088B00 - 0x00080000]; TRX62NMTU6Reg MTU6; };
	struct { uint8_t _ofs_MTU7[0x00088B80 - 0x00080000]; TRX62NMTU7Reg MTU7; };
	struct { uint8_t _ofs_MTU8[0x00088C00 - 0x00080000]; TRX62NMTU8Reg MTU8; };
	struct { uint8_t _ofs_MTU11[0x00088C80 - 0x00080000]; TRX62NMTU11Reg MTU11; };
	struct { uint8_t _ofs_S12AD[0x00089000 - 0x00080000]; TRX62NS12ADReg S12AD; };
	struct { uint8_t _ofs_PORT0[0x0008C000 - 0x00080000]; TRX62NPORT0Reg PORT0; };
	struct { uint8_t _ofs_PORT1[0x0008C001 - 0x00080000]; TRX62NPORT1Reg PORT1; };
	struct { uint8_t _ofs_PORT2[0x0008C002 - 0x00080000]; TRX62NPORT2Reg PORT2; };
	struct { uint8_t _ofs_PORT3[0x0008C003 - 0x00080000]; TRX62NPORT3Reg PORT3; };
	struct { uint8_t _ofs_PORT4[0x0008C004 - 0x00080000]; TRX62NPORT4Reg PORT4; };
	struct { uint8_t _ofs_PORT5[0x0008C005 - 0x00080000]; TRX62NPORT5Reg PORT5; };
	struct { uint8_t _ofs_PORT6[0x0008C006 - 0x00080000]; TRX62NPORT6Reg PORT6; };
	struct { uint8_t _ofs_PORT7[0x0008C007 - 0x00080000]; TRX62NPORT7Reg PORT7; };
	struct { uint8_t _ofs_PORT8[0x0008C008 - 0x00080000]; TRX62NPORT8Reg PORT8; };
	struct { uint8_t _ofs_PORT9[0x0008C009 - 0x00080000]; TRX62NPORT9Reg PORT9; };
	struct { uint8_t _ofs_PORTA[0x0008C00A - 0x00080000]; TRX62NPORTAReg PORTA; };
	struct { uint8_t _ofs_PORTB[0x0008C00B - 0x00080000]; TRX62NPORTBReg PORTB; };
	struct { uint8_t _ofs_PORTC[0x0008C00C - 0x00080000]; TRX62NPORTCReg PORTC; };
	struct { uint8_t _ofs_PORTD[0x0008C00D - 0x00080000]; TRX62NPORTDReg PORTD; };
	struct { uint8_t _ofs_PORTE[0x0008C00E - 0x00080000]; TRX62NPORTEReg PORTE; };
	struct { uint8_t _ofs_PORTF[0x0008C00F - 0x00080000]; TRX62NPORTFReg PORTF; };
	struct { uint8_t _ofs_PORTG[0x0008C010 - 0x00080000]; TRX62NPORTGReg PORTG; };
	struct { uint8_t _ofs_IOPORT[0x0008C100 - 0x00080000]; TRX62NIOPORTReg IOPORT; };
	struct { uint8_t _ofs_FLASH[0x0008C289 - 0x00080000]; TRX62NFLASHReg FLASH; };
	struct { uint8_t _ofs_RTC[0x0008C400 - 0x00080000]; TRX62NRTCReg RTC; };
};

#pragma warning(default:4201)

#pragma pack(pop)

#define	INT_NO_BUSERR	16
#define	INT_NO_FIFERR	21
#define	INT_NO_FRDYI	23
#define	INT_NO_SWINT	27
#define	INT_NO_CMI0	28
#define	INT_NO_CMI1	29
#define	INT_NO_CMI2	30
#define	INT_NO_CMI3	31
#define	INT_NO_EINT	32
#define	INT_NO_D0FIFO0	36
#define	INT_NO_D1FIFO0	37
#define	INT_NO_USBI0	38
#define	INT_NO_D0FIFO1	40
#define	INT_NO_D1FIFO1	41
#define	INT_NO_USBI1	42
#define	INT_NO_SPEI0	44
#define	INT_NO_SPRI0	45
#define	INT_NO_SPTI0	46
#define	INT_NO_SPII0	47
#define	INT_NO_SPEI1	48
#define	INT_NO_SPRI1	49
#define	INT_NO_SPTI1	50
#define	INT_NO_SPII1	51
#define	INT_NO_ERS0	56
#define	INT_NO_RXF0	57
#define	INT_NO_TXF0	58
#define	INT_NO_RXM0	59
#define	INT_NO_TXM0	60
#define	INT_NO_PRD	62
#define	INT_NO_CUP	63
#define	INT_NO_IRQ0	64
#define	INT_NO_IRQ1	65
#define	INT_NO_IRQ2	66
#define	INT_NO_IRQ3	67
#define	INT_NO_IRQ4	68
#define	INT_NO_IRQ5	69
#define	INT_NO_IRQ6	70
#define	INT_NO_IRQ7	71
#define	INT_NO_IRQ8	72
#define	INT_NO_IRQ9	73
#define	INT_NO_IRQ10	74
#define	INT_NO_IRQ11	75
#define	INT_NO_IRQ12	76
#define	INT_NO_IRQ13	77
#define	INT_NO_IRQ14	78
#define	INT_NO_IRQ15	79
#define	INT_NO_USBR0	90
#define	INT_NO_USBR1	91
#define	INT_NO_ALM	92
#define	INT_NO_WOVI	96
#define	INT_NO_ADI0	98
#define	INT_NO_ADI1	99
#define	INT_NO_S12ADI0	102
#define	INT_NO_TGIA0	114
#define	INT_NO_TGIB0	115
#define	INT_NO_TGIC0	116
#define	INT_NO_TGID0	117
#define	INT_NO_TCIV0	118
#define	INT_NO_TGIE0	119
#define	INT_NO_TGIF0	120
#define	INT_NO_TGIA1	121
#define	INT_NO_TGIB1	122
#define	INT_NO_TCIV1	123
#define	INT_NO_TCIU1	124
#define	INT_NO_TGIA2	125
#define	INT_NO_TGIB2	126
#define	INT_NO_TCIV2	127
#define	INT_NO_TCIU2	128
#define	INT_NO_TGIA3	129
#define	INT_NO_TGIB3	130
#define	INT_NO_TGIC3	131
#define	INT_NO_TGID3	132
#define	INT_NO_TCIV3	133
#define	INT_NO_TGIA4	134
#define	INT_NO_TGIB4	135
#define	INT_NO_TGIC4	136
#define	INT_NO_TGID4	137
#define	INT_NO_TCIV4	138
#define	INT_NO_TGIU5	139
#define	INT_NO_TGIV5	140
#define	INT_NO_TGIW5	141
#define	INT_NO_TGIA6	142
#define	INT_NO_TGIB6	143
#define	INT_NO_TGIC6	144
#define	INT_NO_TGID6	145
#define	INT_NO_TCIV6	146
#define	INT_NO_TGIE6	147
#define	INT_NO_TGIF6	148
#define	INT_NO_TGIA7	149
#define	INT_NO_TGIB7	150
#define	INT_NO_TCIV7	151
#define	INT_NO_TCIU7	152
#define	INT_NO_TGIA8	153
#define	INT_NO_TGIB8	154
#define	INT_NO_TCIV8	155
#define	INT_NO_TCIU8	156
#define	INT_NO_TGIA9	157
#define	INT_NO_TGIB9	158
#define	INT_NO_TGIC9	159
#define	INT_NO_TGID9	160
#define	INT_NO_TCIV9	161
#define	INT_NO_TGIA10	162
#define	INT_NO_TGIB10	163
#define	INT_NO_TGIC10	164
#define	INT_NO_TGID10	165
#define	INT_NO_TCIV10	166
#define	INT_NO_TGIU11	167
#define	INT_NO_TGIV11	168
#define	INT_NO_TGIW11	169
#define	INT_NO_OEI1	170
#define	INT_NO_OEI2	171
#define	INT_NO_OEI3	172
#define	INT_NO_OEI4	173
#define	INT_NO_CMIA0	174
#define	INT_NO_CMIB0	175
#define	INT_NO_OVI0	176
#define	INT_NO_CMIA1	177
#define	INT_NO_CMIB1	178
#define	INT_NO_OVI1	179
#define	INT_NO_CMIA2	180
#define	INT_NO_CMIB2	181
#define	INT_NO_OVI2	182
#define	INT_NO_CMIA3	183
#define	INT_NO_CMIB3	184
#define	INT_NO_OVI3	185
#define	INT_NO_DMACI0	198
#define	INT_NO_DMACI1	199
#define	INT_NO_DMACI2	200
#define	INT_NO_DMACI3	201
#define	INT_NO_EXDMACI0	202
#define	INT_NO_EXDMACI1	203
#define	INT_NO_ERI0	214
#define	INT_NO_RXI0	215
#define	INT_NO_TXI0	216
#define	INT_NO_TEI0	217
#define	INT_NO_ERI1	218
#define	INT_NO_RXI1	219
#define	INT_NO_TXI1	220
#define	INT_NO_TEI1	221
#define	INT_NO_ERI2	222
#define	INT_NO_RXI2	223
#define	INT_NO_TXI2	224
#define	INT_NO_TEI2	225
#define	INT_NO_ERI3	226
#define	INT_NO_RXI3	227
#define	INT_NO_TXI3	228
#define	INT_NO_TEI3	229
#define	INT_NO_ERI5	234
#define	INT_NO_RXI5	235
#define	INT_NO_TXI5	236
#define	INT_NO_TEI5	237
#define	INT_NO_ERI6	238
#define	INT_NO_RXI6	239
#define	INT_NO_TXI6	240
#define	INT_NO_TEI6	241
#define	INT_NO_ICEEI0	246
#define	INT_NO_ICRXI0	247
#define	INT_NO_ICTXI0	248
#define	INT_NO_ICTEI0	249
#define	INT_NO_ICEEI1	250
#define	INT_NO_ICRXI1	251
#define	INT_NO_ICTXI1	252
#define	INT_NO_ICTEI1	253

/* 送信ディスクリプタ */
struct TRX62NEtherCTxDescriper {
	uint32_t	Status : 26;
	uint32_t	TWBI : 1;
	uint32_t	TFE : 1;
	uint32_t	TFP : 2;
	uint32_t	TDLE : 1;
	uint32_t	TACT : 1;
	uint32_t	: 16;
	uint32_t	Size : 16;
	void		*Pointer;
	uint32_t	BINDING;
};

/* 受信ディスクリプタ */
struct TRX62NEtherCRxDescriper {
	uint32_t	Status : 27;
	uint32_t	RFE : 1;
	uint32_t	RFP : 2;
	uint32_t	RDLE : 1;
	uint32_t	RACT : 1;
	uint32_t	Size : 16;
	uint32_t	Capacity : 16;
	void		*Pointer;
	uint32_t	BINDING;
};

#ifdef __cplusplus
}
#endif

#endif // RX62NRegH
