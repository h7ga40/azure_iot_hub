/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2010 by Witz Corporation, JAPAN
 *  Copyright (C) 2013 by Mitsuhiro Matsuura
 * 
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  @(#) $Id$
 */

/*
 *		プロセッサ依存モジュール（RX630用）
 */

#include "kernel_impl.h"


/*
 *  割込み要因プライオリティレジスタアドレステーブル
 */
volatile uint8_t __evenaccess * const ipr_reg_addr[ INHNO_MAX ] = {
	NULL,							/* No.0  予約 */
	NULL,							/* No.1  予約 */
	NULL,							/* No.2  予約 */
	NULL,							/* No.3  予約 */
	NULL,							/* No.4  予約 */
	NULL,							/* No.5  予約 */
	NULL,							/* No.6  予約 */
	NULL,							/* No.7  予約 */
	NULL,							/* No.8  予約 */
	NULL,							/* No.9  予約 */
	NULL,							/* No.10  予約 */
	NULL,							/* No.11  予約 */
	NULL,							/* No.12  予約 */
	NULL,							/* No.13  予約 */
	NULL,							/* No.14  予約 */
	NULL,							/* No.15  予約 */
	ICU_IPR000_ADDR,				/* No.16  バスエラー BUSERR */
	NULL,							/* No.17  予約 */
	NULL,							/* No.18  予約 */
	NULL,							/* No.19  予約 */
	NULL,							/* No.20  予約 */
	ICU_IPR001_ADDR,				/* No.21  FCU FIFERR */
	NULL,							/* No.22  予約 */
	ICU_IPR002_ADDR,				/* No.23  FCU FRDYI */
	NULL,							/* No.24  予約 */
	NULL,							/* No.25  予約 */
	NULL,							/* No.26  予約 */
	ICU_IPR003_ADDR,				/* No.27  SWINT */
	ICU_IPR004_ADDR,				/* No.28  CMTユニット0 CMT0 */
	ICU_IPR005_ADDR,				/* No.29  CMTユニット0 CMT1 */
	ICU_IPR006_ADDR,				/* No.30  CMTユニット1 CMT2 */
	ICU_IPR007_ADDR,				/* No.31  CMTユニット1 CMT3 */
	ICU_IPR032_ADDR,				/* No.32  ETHER */
	ICU_IPR033_ADDR,				/* No.33  USB0 D0FIFO0 */
	ICU_IPR034_ADDR,				/* No.34  USB0 D1FIFO0 */
	ICU_IPR035_ADDR,				/* No.35  USB0 USBI0 */
	ICU_IPR036_ADDR,				/* No.36  USB1 D0FIFO1 */
	ICU_IPR037_ADDR,				/* No.37  USB1 D1FIFO1 */
	ICU_IPR038_ADDR,				/* No.38  USB1 USBI1 */
	ICU_IPR039_ADDR,				/* No.39  SPRI0 */
	ICU_IPR039_ADDR,				/* No.40  SPTI0 */
	ICU_IPR039_ADDR,				/* No.41  SPII0 */
	ICU_IPR042_ADDR,				/* No.42  SPRI1 */
	ICU_IPR042_ADDR,				/* No.43  SPTI1 */
	ICU_IPR042_ADDR,				/* No.44  SPII1 */
	ICU_IPR045_ADDR,				/* No.45  SPRI2 */
	ICU_IPR045_ADDR,				/* No.46  SPTI2 */
	ICU_IPR045_ADDR,				/* No.47  SPII2 */
	ICU_IPR048_ADDR, 				/* No.48  CAN RXF0 */
	ICU_IPR048_ADDR, 				/* No.49  CAN TXF0 */
	ICU_IPR048_ADDR, 				/* No.50  CAN RXM0 */
	ICU_IPR048_ADDR, 				/* No.51  CAN TXM0 */
	ICU_IPR052_ADDR, 				/* No.52  CAN RXF1 */
	ICU_IPR052_ADDR, 				/* No.53  CAN TXF1 */
	ICU_IPR052_ADDR, 				/* No.54  CAN RXM1 */
	ICU_IPR052_ADDR, 				/* No.55  CAN TXM1 */
	ICU_IPR056_ADDR, 				/* No.56  CAN RXF2 */
	ICU_IPR056_ADDR, 				/* No.57  CAN TXF2 */
	ICU_IPR056_ADDR, 				/* No.58  CAN RXM2 */
	ICU_IPR056_ADDR, 				/* No.59  CAN TXM2 */
	NULL,							/* No.60  予約 */
	NULL,							/* No.61  予約 */
	ICU_IPR062_ADDR,			 	/* No.62  RTC PRD */
	NULL,							/* No.63  予約 */
	ICU_IPR064_ADDR,				/* No.64  外部端子 IRQ0 */
	ICU_IPR065_ADDR,				/* No.65  外部端子 IRQ1 */
	ICU_IPR066_ADDR,				/* No.66  外部端子 IRQ2 */
	ICU_IPR067_ADDR,				/* No.67  外部端子 IRQ3 */
	ICU_IPR068_ADDR,				/* No.68  外部端子 IRQ4 */
	ICU_IPR069_ADDR,				/* No.69  外部端子 IRQ5 */
	ICU_IPR070_ADDR,				/* No.70  外部端子 IRQ6 */
	ICU_IPR071_ADDR,				/* No.71  外部端子 IRQ7 */
	ICU_IPR072_ADDR,				/* No.72  外部端子 IRQ8 */
	ICU_IPR073_ADDR,				/* No.73  外部端子 IRQ9 */
	ICU_IPR074_ADDR,				/* No.74  外部端子 IRQ10 */
	ICU_IPR075_ADDR,				/* No.75  外部端子 IRQ11 */
	ICU_IPR076_ADDR,				/* No.76  外部端子 IRQ12 */
	ICU_IPR077_ADDR,				/* No.77  外部端子 IRQ13 */
	ICU_IPR078_ADDR,				/* No.78  外部端子 IRQ14 */
	ICU_IPR079_ADDR,				/* No.79  外部端子 IRQ15 */
	NULL,							/* No.80  予約 */
	NULL,							/* No.81  予約 */
	NULL,							/* No.82  予約 */
	NULL,							/* No.83  予約 */
	NULL,							/* No.84  予約 */
	NULL,							/* No.85  予約 */
	NULL,							/* No.86  予約 */
	NULL,							/* No.87  予約 */
	NULL,							/* No.88  予約 */
	NULL,							/* No.89  予約 */
	ICU_IPR090_ADDR, 				/* No.90  USBR0 */
	ICU_IPR091_ADDR, 				/* No.91  USBR1 */
	ICU_IPR092_ADDR,				/* No.92  RTC ALM */
	ICU_IPR093_ADDR,				/* No.93  RTC PRD */
	NULL,							/* No.94  予約 */
	NULL,							/* No.95  予約 */
	NULL,							/* No.96  予約 */
	NULL,							/* No.97  予約 */
	ICU_IPR098_ADDR,				/* No.98  AD ADI0 */
	NULL,							/* No.99  予約 */
	NULL,							/* No.100  予約 */
	NULL,							/* No.101  予約 */
	ICU_IPR102_ADDR,				/* No.102 S12AD S12ADIO */
	NULL,							/* No.103  予約 */
	NULL,							/* No.104  予約 */
	NULL,							/* No.105  予約 */
	ICU_IPR106_ADDR,				/* No.106 ICU GROUP0 */
	ICU_IPR107_ADDR,				/* No.107 ICU GROUP1 */
	ICU_IPR108_ADDR,				/* No.108 ICU GROUP2 */
	ICU_IPR109_ADDR,				/* No.109 ICU GROUP3 */
	ICU_IPR110_ADDR,				/* No.110 ICU GROUP4 */
	ICU_IPR111_ADDR,				/* No.111 ICU GROUP5 */
	ICU_IPR112_ADDR,				/* No.112 ICU GROUP6 */
	NULL,							/* No.113  予約 */
	ICU_IPR114_ADDR,				/* No.114 ICU GROUP12 */
	NULL,							/* No.115  予約 */
	NULL,							/* No.116  予約 */
	NULL,							/* No.117  予約 */
	NULL,							/* No.118  予約 */
	NULL,							/* No.119  予約 */
	NULL,							/* No.120  予約 */
	NULL,							/* No.121  予約 */
	ICU_IPR122_ADDR,				/* No.122 SCI12 SCIX0 */
	ICU_IPR123_ADDR,				/* No.123 SCI12 SCIX1 */
	ICU_IPR124_ADDR,				/* No.124 SCI12 SCIX2 */
	ICU_IPR125_ADDR,				/* No.125 SCI12 SCIX3 */
	ICU_IPR126_ADDR,				/* No.126 TPU0 TGI0A */
	ICU_IPR126_ADDR,				/* No.127 TPU0 TGI0B */
	ICU_IPR126_ADDR,				/* No.128 TPU0 TGI0C */
	ICU_IPR126_ADDR,				/* No.129 TPU0 TGI0D */
	ICU_IPR130_ADDR,				/* No.130 TPU1 TGI1A */
	ICU_IPR130_ADDR,				/* No.131 TPU1 TGI1B */
	ICU_IPR132_ADDR,				/* No.132 TPU2 TGI2A */
	ICU_IPR132_ADDR,				/* No.133 TPU2 TGI2B */
	ICU_IPR134_ADDR,				/* No.134 TPU3 TGI3A */
	ICU_IPR134_ADDR,				/* No.135 TPU3 TGI3B */
	ICU_IPR134_ADDR,				/* No.136 TPU3 TGI3C */
	ICU_IPR134_ADDR,				/* No.137 TPU3 TGI3D */
	ICU_IPR138_ADDR,				/* No.138 TPU4 TGI4A */
	ICU_IPR138_ADDR,				/* No.139 TPU4 TGI4B */
	ICU_IPR140_ADDR,				/* No.140 TPU5 TGI5A */
	ICU_IPR140_ADDR,				/* No.141 TPU5 TGI5B */
	ICU_IPR142_ADDR,				/* No.142 TPU6 TGI6A */
	ICU_IPR142_ADDR,				/* No.143 TPU6 TGI6B */
	ICU_IPR142_ADDR,				/* No.144 TPU6 TGI6C */
	ICU_IPR142_ADDR,				/* No.145 TPU6 TGI6D */
	ICU_IPR146_ADDR,				/* No.146 TGIE0 */
	ICU_IPR146_ADDR,				/* No.147 TGIF0 */
	ICU_IPR148_ADDR,				/* No.148 TPU7 TGI7A */
	ICU_IPR148_ADDR,				/* No.149 TPU7 TGI7B */
	ICU_IPR150_ADDR,				/* No.150 TPU8 TGI8A */
	ICU_IPR150_ADDR,				/* No.151 TPU8 TGI8B */
	ICU_IPR152_ADDR,				/* No.152 TPU9 TGI9A */
	ICU_IPR152_ADDR,				/* No.153 TPU9 TGI9B */
	ICU_IPR152_ADDR,				/* No.154 TPU9 TGI9C */
	ICU_IPR152_ADDR,				/* No.155 TPU9 TGI9D */
	ICU_IPR156_ADDR,				/* No.156 TPU10 TGI10A */
	ICU_IPR156_ADDR,				/* No.157 TPU10 TGI10B */
	ICU_IPR156_ADDR,				/* No.158 TGIC4 */
	ICU_IPR156_ADDR,				/* No.159 TGID4 */
	ICU_IPR160_ADDR,				/* No.160 TGIV4 */
	ICU_IPR161_ADDR,				/* No.161 TGIU5 */
	ICU_IPR162_ADDR,				/* No.162 TGIV5 */
	ICU_IPR163_ADDR,				/* No.163 TGIW5 */
	ICU_IPR164_ADDR,				/* No.164 TCI11A */
	ICU_IPR165_ADDR,				/* No.165 TCI11B */
	ICU_IPR167_ADDR,				/* No.166 POE OEI1 */
	ICU_IPR167_ADDR,				/* No.167 POE OEI2 */
	NULL,							/* No.168  予約 */
	NULL,							/* No.169  予約 */
	ICU_IPR170_ADDR,				/* No.170 TMR0 CMIA0 */
	ICU_IPR170_ADDR,				/* No.171 TMR0 CMIB0 */
	ICU_IPR170_ADDR,				/* No.172 TMR0 OVI0 */
	ICU_IPR173_ADDR,				/* No.173 TMR1 CMIA1 */
	ICU_IPR173_ADDR,				/* No.174 TMR1 CMIB1 */
	ICU_IPR173_ADDR,				/* No.175 TMR1 OVI1 */
	ICU_IPR176_ADDR,				/* No.176 TMR2 CMIA2 */
	ICU_IPR176_ADDR,				/* No.177 TMR2 CMIB2 */
	ICU_IPR176_ADDR,				/* No.178 TMR2 OVI2 */
	ICU_IPR179_ADDR,				/* No.179 TMR3 CMIA3 */
	ICU_IPR179_ADDR,				/* No.180 TMR3 CMIB3 */
	ICU_IPR179_ADDR,				/* No.181 TMR3 OVI3 */
	ICU_IPR182_ADDR,				/* No.182 RIIC0 EEI0 */
	ICU_IPR183_ADDR,				/* No.183 RIIC0 RXI0 */
	ICU_IPR184_ADDR,				/* No.184 RIIC0 TXI0 */
	ICU_IPR185_ADDR,				/* No.185 RIIC0 TEI0 */
	ICU_IPR186_ADDR,				/* No.186 RIIC0 EEI1 */
	ICU_IPR187_ADDR,				/* No.187 RIIC0 RXI1 */
	ICU_IPR188_ADDR,				/* No.188 RIIC0 TXI1 */
	ICU_IPR189_ADDR,				/* No.189 RIIC0 TEI1 */
	ICU_IPR190_ADDR,				/* No.190 RIIC0 EEI2 */
	ICU_IPR191_ADDR,				/* No.191 RIIC0 RXI2 */
	ICU_IPR192_ADDR,				/* No.192 RIIC0 TXI2 */
	ICU_IPR193_ADDR,				/* No.193 RIIC0 TEI2 */
	ICU_IPR194_ADDR,				/* No.194 RIIC0 EEI3 */
	ICU_IPR195_ADDR,				/* No.195 RIIC0 RXI3 */
	ICU_IPR196_ADDR,				/* No.196 RIIC0 TXI3 */
	ICU_IPR197_ADDR,				/* No.197 RIIC0 TEI3 */
	ICU_IPR198_ADDR,				/* No.198 DMAC DMTEND0 */
	ICU_IPR199_ADDR,				/* No.199 DMAC DMTEND1 */
	ICU_IPR200_ADDR,				/* No.200 DMAC DMTEND2 */
	ICU_IPR201_ADDR,				/* No.201 DMAC DMTEND3 */
	ICU_IPR202_ADDR,				/* No.202 EXDMAC EXDMACI0 */
	ICU_IPR203_ADDR,				/* No.203 EXDMAC EXDMACI1 */
	NULL,							/* No.204  予約 */
	NULL,							/* No.205  予約 */
	NULL,							/* No.206  予約 */
	NULL,							/* No.207  予約 */
	NULL,							/* No.208  予約 */
	NULL,							/* No.209  予約 */
	NULL,							/* No.210  予約 */
	NULL,							/* No.211  予約 */
	NULL,							/* No.212  予約 */
	NULL,							/* No.213  予約 */
	ICU_IPR214_ADDR,				/* No.214 SCI0 RXI0 */
	ICU_IPR214_ADDR,				/* No.215 SCI0 TXI0 */
	ICU_IPR214_ADDR,				/* No.216 SCI0 TEI0 */
	ICU_IPR217_ADDR,				/* No.217 SCI1 RXI1 */
	ICU_IPR217_ADDR,				/* No.218 SCI1 TXI1 */
	ICU_IPR217_ADDR,				/* No.219 SCI1 TEI1 */
	ICU_IPR220_ADDR,				/* No.220 SCI2 RXI2 */
	ICU_IPR220_ADDR,				/* No.221 SCI2 TXI2 */
	ICU_IPR220_ADDR,				/* No.222 SCI2 TEI2 */
	ICU_IPR223_ADDR,				/* No.223 SCI3 RXI3 */
	ICU_IPR223_ADDR,				/* No.224 SCI3 TXI3 */
	ICU_IPR223_ADDR,				/* No.225 SCI3 TEI3 */
	ICU_IPR226_ADDR,				/* No.226 SCI4 RXI4 */
	ICU_IPR226_ADDR,				/* No.227 SCI4 TXI4 */
	ICU_IPR226_ADDR,				/* No.228 SCI4 TEI4 */
	ICU_IPR229_ADDR,				/* No.229 SCI5 RXI5 */
	ICU_IPR229_ADDR,				/* No.230 SCI5 TXI5 */
	ICU_IPR229_ADDR,				/* No.231 SCI5 TEI5 */
	ICU_IPR232_ADDR,				/* No.232 SCI6 RXI6 */
	ICU_IPR232_ADDR,				/* No.233 SCI6 TXI6 */
	ICU_IPR232_ADDR,				/* No.234 SCI6 TEI6 */
	ICU_IPR235_ADDR,				/* No.235 SCI7 RXI7 */
	ICU_IPR235_ADDR,				/* No.236 SCI7 TXI7 */
	ICU_IPR235_ADDR,				/* No.237 SCI7 TEI7 */
	ICU_IPR238_ADDR,				/* No.238 SCI8 RXI8 */
	ICU_IPR238_ADDR,				/* No.239 SCI8 TXI8 */
	ICU_IPR238_ADDR,				/* No.240 SCI8 TEI8 */
	ICU_IPR241_ADDR,				/* No.241 SCI9 RXI9 */
	ICU_IPR241_ADDR,				/* No.242 SCI9 TXI9 */
	ICU_IPR241_ADDR,				/* No.243 SCI9 TEI9 */
	ICU_IPR244_ADDR,				/* No.244 SCI10 RXI10 */
	ICU_IPR244_ADDR,				/* No.245 SCI10 TXI10 */
	ICU_IPR244_ADDR,				/* No.246 SCI10 TEI10 */
	ICU_IPR247_ADDR,				/* No.247 SCI11 RXI11 */
	ICU_IPR247_ADDR,				/* No.248 SCI11 TXI11 */
	ICU_IPR247_ADDR,				/* No.249 SCI11 TEI11 */
	ICU_IPR250_ADDR,				/* No.250 SCI12 RXI12 */
	ICU_IPR250_ADDR,				/* No.251 SCI12 TXI12 */
	ICU_IPR250_ADDR,				/* No.252 SCI12 TEI12 */
	ICU_IPR253_ADDR,				/* No.253 IEB IEBINT */
	NULL,							/* No.254  予約 */
	NULL,							/* No.255  予約 */
};


/*
 *  割込み要求許可レジスタアドレステーブル
 */
const IER_INFO ier_reg_addr[ INHNO_MAX ] = {
	{ NULL, INVALID_OFFSET },			/* No.0   予約 */
	{ NULL, INVALID_OFFSET },			/* No.1   予約 */
	{ NULL, INVALID_OFFSET },			/* No.2   予約 */
	{ NULL, INVALID_OFFSET },			/* No.3   予約 */
	{ NULL, INVALID_OFFSET },			/* No.4   予約 */
	{ NULL, INVALID_OFFSET },			/* No.5   予約 */
	{ NULL, INVALID_OFFSET },			/* No.6   予約 */
	{ NULL, INVALID_OFFSET },			/* No.7   予約 */
	{ NULL, INVALID_OFFSET },			/* No.8   予約 */
	{ NULL, INVALID_OFFSET },			/* No.9   予約 */
	{ NULL, INVALID_OFFSET },			/* No.10  予約 */
	{ NULL, INVALID_OFFSET },			/* No.11  予約 */
	{ NULL, INVALID_OFFSET },			/* No.12  予約 */
	{ NULL, INVALID_OFFSET },			/* No.13  予約 */
	{ NULL, INVALID_OFFSET },			/* No.14  予約 */
	{ NULL, INVALID_OFFSET },			/* No.15  予約 */
	{ ICU_IER02_ADDR, ICU_IEN0_BIT },	/* No.16  バスエラー BUSERR */
	{ NULL, INVALID_OFFSET },			/* No.17  予約 */
	{ NULL, INVALID_OFFSET },			/* No.18  予約 */
	{ NULL, INVALID_OFFSET },			/* No.19  予約 */
	{ NULL, INVALID_OFFSET },			/* No.20  予約 */
	{ ICU_IER02_ADDR, ICU_IEN5_BIT },	/* No.21  FCU FIFERR */
	{ NULL, INVALID_OFFSET },			/* No.22  予約 */
	{ ICU_IER02_ADDR, ICU_IEN7_BIT },	/* No.23  FCU FRDYI */
	{ NULL, INVALID_OFFSET },			/* No.24  予約 */
	{ NULL, INVALID_OFFSET },			/* No.25  予約 */
	{ NULL, INVALID_OFFSET },			/* No.26  予約 */
	{ ICU_IER03_ADDR, ICU_IEN3_BIT },	/* No.27  SWINT */
	{ ICU_IER03_ADDR, ICU_IEN4_BIT },	/* No.28  CMTユニット0 CMT0 */
	{ ICU_IER03_ADDR, ICU_IEN5_BIT },	/* No.29  CMTユニット0 CMT1 */
	{ ICU_IER03_ADDR, ICU_IEN6_BIT },	/* No.30  CMTユニット1 CMT2 */
	{ ICU_IER03_ADDR, ICU_IEN7_BIT },	/* No.31  CMTユニット1 CMT3 */
	{ ICU_IER04_ADDR, ICU_IEN0_BIT },	/* No.32  ETHER */
	{ ICU_IER04_ADDR, ICU_IEN1_BIT },	/* No.33  USB0 D0FIFO0 */
	{ ICU_IER04_ADDR, ICU_IEN2_BIT },	/* No.34  USB0 D1FIFO0 */
	{ ICU_IER04_ADDR, ICU_IEN3_BIT },	/* No.35  USB0 USBI0 */
	{ ICU_IER04_ADDR, ICU_IEN4_BIT },	/* No.36  USB1 D0FIFO1 */
	{ ICU_IER04_ADDR, ICU_IEN5_BIT },	/* No.37  USB1 D1FIFO1 */
	{ ICU_IER04_ADDR, ICU_IEN6_BIT },	/* No.38  USB1 USBI1 */
	{ ICU_IER04_ADDR, ICU_IEN7_BIT },	/* No.39  RSPI0 SPRI0 */
	{ ICU_IER05_ADDR, ICU_IEN0_BIT },	/* No.40  RSPI0 SPTI0 */
	{ ICU_IER05_ADDR, ICU_IEN1_BIT },	/* No.41  RSPI0 SPII0 */
	{ ICU_IER05_ADDR, ICU_IEN2_BIT },	/* No.42  RSPI1 SPRI1 */
	{ ICU_IER05_ADDR, ICU_IEN3_BIT },	/* No.43  RSPI1 SPTI1 */
	{ ICU_IER05_ADDR, ICU_IEN4_BIT },	/* No.44  RSPI1 SPII1 */
	{ ICU_IER05_ADDR, ICU_IEN5_BIT },	/* No.45  RSPI2 SPRI2 */
	{ ICU_IER05_ADDR, ICU_IEN6_BIT },	/* No.46  RSPI2 SPTI2 */
	{ ICU_IER05_ADDR, ICU_IEN7_BIT },	/* No.47  RSPI2 SPII2 */
	{ ICU_IER06_ADDR, ICU_IEN0_BIT },	/* No.48  CAN0 RXF0 */
	{ ICU_IER06_ADDR, ICU_IEN1_BIT },	/* No.49  CAN0 TXF0 */
	{ ICU_IER06_ADDR, ICU_IEN2_BIT },	/* No.50  CAN0 RXM0 */
	{ ICU_IER06_ADDR, ICU_IEN3_BIT },	/* No.51  CAN0 TXM0 */
	{ ICU_IER06_ADDR, ICU_IEN4_BIT },	/* No.52  CAN1 RXF1 */
	{ ICU_IER06_ADDR, ICU_IEN5_BIT },	/* No.53  CAN1 TXF1 */
	{ ICU_IER06_ADDR, ICU_IEN6_BIT },	/* No.54  CAN1 RXM1 */
	{ ICU_IER06_ADDR, ICU_IEN7_BIT },	/* No.55  CAN1 TXM1 */
	{ ICU_IER07_ADDR, ICU_IEN0_BIT },	/* No.56  CAN2 RXF2 */
	{ ICU_IER07_ADDR, ICU_IEN1_BIT },	/* No.57  CAN2 TXF2 */
	{ ICU_IER07_ADDR, ICU_IEN2_BIT },	/* No.58  CAN2 RXM2 */
	{ ICU_IER07_ADDR, ICU_IEN3_BIT },	/* No.59  CAN2 TXM2 */
	{ NULL, INVALID_OFFSET },			/* No.60  予約 */
	{ NULL, INVALID_OFFSET },			/* No.61  予約 */
	{ ICU_IER07_ADDR, ICU_IEN6_BIT },	/* No.62  RTC PRD */
	{ NULL, INVALID_OFFSET },			/* No.63  予約 */
	{ ICU_IER08_ADDR, ICU_IEN0_BIT },	/* No.64  外部端子 IRQ0 */
	{ ICU_IER08_ADDR, ICU_IEN1_BIT },	/* No.65  外部端子 IRQ1 */
	{ ICU_IER08_ADDR, ICU_IEN2_BIT },	/* No.66  外部端子 IRQ2 */
	{ ICU_IER08_ADDR, ICU_IEN3_BIT },	/* No.67  外部端子 IRQ3 */
	{ ICU_IER08_ADDR, ICU_IEN4_BIT },	/* No.68  外部端子 IRQ4 */
	{ ICU_IER08_ADDR, ICU_IEN5_BIT },	/* No.69  外部端子 IRQ5 */
	{ ICU_IER08_ADDR, ICU_IEN6_BIT },	/* No.70  外部端子 IRQ6 */
	{ ICU_IER08_ADDR, ICU_IEN7_BIT },	/* No.71  外部端子 IRQ7 */
	{ ICU_IER09_ADDR, ICU_IEN0_BIT },	/* No.72  外部端子 IRQ8 */
	{ ICU_IER09_ADDR, ICU_IEN1_BIT },	/* No.73  外部端子 IRQ9 */
	{ ICU_IER09_ADDR, ICU_IEN2_BIT },	/* No.74  外部端子 IRQ10 */
	{ ICU_IER09_ADDR, ICU_IEN3_BIT },	/* No.75  外部端子 IRQ11 */
	{ ICU_IER09_ADDR, ICU_IEN4_BIT },	/* No.76  外部端子 IRQ12 */
	{ ICU_IER09_ADDR, ICU_IEN5_BIT },	/* No.77  外部端子 IRQ13 */
	{ ICU_IER09_ADDR, ICU_IEN6_BIT },	/* No.78  外部端子 IRQ14 */
	{ ICU_IER09_ADDR, ICU_IEN7_BIT },	/* No.79  外部端子 IRQ15 */
	{ NULL, INVALID_OFFSET },			/* No.80  予約 */
	{ NULL, INVALID_OFFSET },			/* No.81  予約 */
	{ NULL, INVALID_OFFSET },			/* No.82  予約 */
	{ NULL, INVALID_OFFSET },			/* No.83  予約 */
	{ NULL, INVALID_OFFSET },			/* No.84  予約 */
	{ NULL, INVALID_OFFSET },			/* No.85  予約 */
	{ NULL, INVALID_OFFSET },			/* No.86  予約 */
	{ NULL, INVALID_OFFSET },			/* No.87  予約 */
	{ NULL, INVALID_OFFSET },			/* No.88  予約 */
	{ NULL, INVALID_OFFSET },			/* No.89  予約 */
	{ ICU_IER0B_ADDR, ICU_IEN2_BIT },	/* No.90  USBR0 */
	{ ICU_IER0B_ADDR, ICU_IEN3_BIT },	/* No.91  USBR1 */
	{ ICU_IER0B_ADDR, ICU_IEN4_BIT },	/* No.92  RTC ALM */
	{ ICU_IER0B_ADDR, ICU_IEN5_BIT },	/* No.92  RTC PRD */
	{ NULL, INVALID_OFFSET },			/* No.94  予約 */
	{ NULL, INVALID_OFFSET },			/* No.95  予約 */
	{ NULL, INVALID_OFFSET },			/* No.96  予約 */
	{ NULL, INVALID_OFFSET },			/* No.97  予約 */
	{ ICU_IER0C_ADDR, ICU_IEN2_BIT },	/* No.98  AD0 ADI0 */
	{ NULL, INVALID_OFFSET },			/* No.99  予約 */
	{ NULL, INVALID_OFFSET },			/* No.100 予約 */
	{ NULL, INVALID_OFFSET },			/* No.101 予約 */
	{ ICU_IER0C_ADDR, ICU_IEN6_BIT },	/* No.102 S12AD S12ADI00 */
	{ NULL, INVALID_OFFSET },			/* No.103 予約 */
	{ NULL, INVALID_OFFSET },			/* No.104 予約 */
	{ NULL, INVALID_OFFSET },			/* No.105 予約 */
	{ ICU_IER0D_ADDR, ICU_IEN2_BIT },	/* No.106 ICU GROUP0 */
	{ ICU_IER0D_ADDR, ICU_IEN3_BIT },	/* No.107 ICU GROUP1 */
	{ ICU_IER0D_ADDR, ICU_IEN4_BIT },	/* No.108 ICU GROUP2 */
	{ ICU_IER0D_ADDR, ICU_IEN5_BIT },	/* No.109 ICU GROUP3 */
	{ ICU_IER0D_ADDR, ICU_IEN6_BIT },	/* No.110 ICU GROUP4 */
	{ ICU_IER0D_ADDR, ICU_IEN7_BIT },	/* No.111 ICU GROUP5 */
	{ ICU_IER0E_ADDR, ICU_IEN0_BIT },	/* No.112 ICU GROUP6 */
	{ NULL, INVALID_OFFSET },			/* No.113 予約 */
	{ ICU_IER0E_ADDR, ICU_IEN1_BIT },	/* No.114 ICU GROUP12 */
	{ NULL, INVALID_OFFSET },			/* No.115 予約 */
	{ NULL, INVALID_OFFSET },			/* No.116 予約 */
	{ NULL, INVALID_OFFSET },			/* No.117 予約 */
	{ NULL, INVALID_OFFSET },			/* No.118 予約 */
	{ NULL, INVALID_OFFSET },			/* No.119 予約 */
	{ NULL, INVALID_OFFSET },			/* No.120 予約 */
	{ NULL, INVALID_OFFSET },			/* No.121 予約 */
	{ ICU_IER0F_ADDR, ICU_IEN2_BIT },	/* No.122 SCI12 SCIX0 */
	{ ICU_IER0F_ADDR, ICU_IEN3_BIT },	/* No.123 SCI12 SCIX1 */
	{ ICU_IER0F_ADDR, ICU_IEN4_BIT },	/* No.124 SCI12 SCIX2 */
	{ ICU_IER0F_ADDR, ICU_IEN5_BIT },	/* No.125 SCI12 SCIX3 */
	{ ICU_IER0F_ADDR, ICU_IEN6_BIT },	/* No.126 TPU0 TGI0A */
	{ ICU_IER0F_ADDR, ICU_IEN7_BIT },	/* No.127 TPU0 TGI0B */
	{ ICU_IER10_ADDR, ICU_IEN0_BIT },	/* No.128 TPU0 TGI0C */
	{ ICU_IER10_ADDR, ICU_IEN1_BIT },	/* No.129 TPU0 TGI0D */
	{ ICU_IER10_ADDR, ICU_IEN2_BIT },	/* No.130 TPU1 TGI1A */
	{ ICU_IER10_ADDR, ICU_IEN3_BIT },	/* No.131 TPU1 TGI1B */
	{ ICU_IER10_ADDR, ICU_IEN4_BIT },	/* No.132 TPU2 TGI2A */
	{ ICU_IER10_ADDR, ICU_IEN5_BIT },	/* No.133 TPU2 TGI2B */
	{ ICU_IER10_ADDR, ICU_IEN6_BIT },	/* No.134 TPU3 TGI3A */
	{ ICU_IER10_ADDR, ICU_IEN7_BIT },	/* No.135 TPU3 TGI3B */
	{ ICU_IER11_ADDR, ICU_IEN0_BIT },	/* No.136 TPU3 TGI3C */
	{ ICU_IER11_ADDR, ICU_IEN1_BIT },	/* No.137 TPU3 TGI3D */
	{ ICU_IER11_ADDR, ICU_IEN2_BIT },	/* No.138 TPU4 TGI4A */
	{ ICU_IER11_ADDR, ICU_IEN3_BIT },	/* No.139 TPU4 TGI4B */
	{ ICU_IER11_ADDR, ICU_IEN4_BIT },	/* No.140 TPU5 TGI5A */
	{ ICU_IER11_ADDR, ICU_IEN5_BIT },	/* No.141 TPU5 TGI5B */
	{ ICU_IER11_ADDR, ICU_IEN6_BIT },	/* No.142 TPU6 TGI6A */
	{ ICU_IER11_ADDR, ICU_IEN7_BIT },	/* No.143 TPU6 TGI6B */
	{ ICU_IER12_ADDR, ICU_IEN0_BIT },	/* No.144 TPU6 TGI6C */
	{ ICU_IER12_ADDR, ICU_IEN1_BIT },	/* No.145 TPU6 TGI6D */
	{ ICU_IER12_ADDR, ICU_IEN2_BIT },	/* No.146 TPU6 TGIE0 */
	{ ICU_IER12_ADDR, ICU_IEN3_BIT },	/* No.147 TPU6 TGIF0 */
	{ ICU_IER12_ADDR, ICU_IEN4_BIT },	/* No.148 TPU7 TGI7A */
	{ ICU_IER12_ADDR, ICU_IEN5_BIT },	/* No.149 TPU7 TGI7B */
	{ ICU_IER12_ADDR, ICU_IEN6_BIT },	/* No.150 TPU8 TGI8A */
	{ ICU_IER12_ADDR, ICU_IEN7_BIT },	/* No.151 TPU8 TGI8B */
	{ ICU_IER13_ADDR, ICU_IEN0_BIT },	/* No.152 TPU9 TGI9A */
	{ ICU_IER13_ADDR, ICU_IEN1_BIT },	/* No.153 TPU9 TGI9B */
	{ ICU_IER13_ADDR, ICU_IEN2_BIT },	/* No.154 TPU9 TGI9C */
	{ ICU_IER13_ADDR, ICU_IEN3_BIT },	/* No.155 TPU9 TGI9D */
	{ ICU_IER13_ADDR, ICU_IEN4_BIT },	/* No.156 TPU10 TGI10A */
	{ ICU_IER13_ADDR, ICU_IEN5_BIT },	/* No.157 TPU10 TGI10B */
	{ ICU_IER13_ADDR, ICU_IEN6_BIT },	/* No.158 TPU10 TGIC4 */
	{ ICU_IER13_ADDR, ICU_IEN7_BIT },	/* No.159 TPU10 TGID4 */
	{ ICU_IER14_ADDR, ICU_IEN0_BIT },	/* No.160 TPU10 TGIV4 */
	{ ICU_IER14_ADDR, ICU_IEN1_BIT },	/* No.161 TPU11 TGIU5 */
	{ ICU_IER14_ADDR, ICU_IEN2_BIT },	/* No.162 TPU11 TGIV5 */
	{ ICU_IER14_ADDR, ICU_IEN3_BIT },	/* No.163 TPU11 TGIW5 */
	{ ICU_IER14_ADDR, ICU_IEN4_BIT },	/* No.164 TPU11 TCI11A */
	{ ICU_IER14_ADDR, ICU_IEN5_BIT },	/* No.165 TPU11 TCI11B */
	{ ICU_IER14_ADDR, ICU_IEN6_BIT },	/* No.166 POE OEI1 */
	{ ICU_IER14_ADDR, ICU_IEN7_BIT },	/* No.167 POE OEI2 */
	{ NULL, INVALID_OFFSET },			/* No.168 予約 */
	{ NULL, INVALID_OFFSET },			/* No.169 予約 */
	{ ICU_IER15_ADDR, ICU_IEN2_BIT },	/* No.170 TMR0 CMIA0 */
	{ ICU_IER15_ADDR, ICU_IEN3_BIT },	/* No.171 TMR0 CMIB0 */
	{ ICU_IER15_ADDR, ICU_IEN4_BIT },	/* No.172 TMR0 OVI0 */
	{ ICU_IER15_ADDR, ICU_IEN5_BIT },	/* No.173 TMR1 CMIA1 */
	{ ICU_IER15_ADDR, ICU_IEN6_BIT },	/* No.174 TMR1 CMIB1 */
	{ ICU_IER15_ADDR, ICU_IEN7_BIT },	/* No.175 TMR1 OVI1 */
	{ ICU_IER16_ADDR, ICU_IEN0_BIT },	/* No.176 TMR2 CMIA2 */
	{ ICU_IER16_ADDR, ICU_IEN1_BIT },	/* No.177 TMR2 CMIB2 */
	{ ICU_IER16_ADDR, ICU_IEN2_BIT },	/* No.178 TMR2 OVI2 */
	{ ICU_IER16_ADDR, ICU_IEN3_BIT },	/* No.179 TMR3 CMIA3 */
	{ ICU_IER16_ADDR, ICU_IEN4_BIT },	/* No.180 TMR3 CMIB3 */
	{ ICU_IER16_ADDR, ICU_IEN5_BIT },	/* No.181 TMR3 OVI3 */
	{ ICU_IER16_ADDR, ICU_IEN6_BIT },	/* No.182 RIIC0 EEI0 */
	{ ICU_IER16_ADDR, ICU_IEN7_BIT },	/* No.183 RIIC0 RXI0 */
	{ ICU_IER17_ADDR, ICU_IEN0_BIT },	/* No.184 RIIC0 TXI0 */
	{ ICU_IER17_ADDR, ICU_IEN1_BIT },	/* No.185 RIIC0 TEI0 */
	{ ICU_IER17_ADDR, ICU_IEN2_BIT },	/* No.186 RIIC0 EEI1 */
	{ ICU_IER17_ADDR, ICU_IEN3_BIT },	/* No.187 RIIC0 RXI1 */
	{ ICU_IER17_ADDR, ICU_IEN4_BIT },	/* No.188 RIIC0 TXI1 */
	{ ICU_IER17_ADDR, ICU_IEN5_BIT },	/* No.189 RIIC0 TEI1 */
	{ ICU_IER17_ADDR, ICU_IEN6_BIT },	/* No.190 RIIC0 EEI2 */
	{ ICU_IER17_ADDR, ICU_IEN7_BIT },	/* No.191 RIIC0 RXI2 */
	{ ICU_IER18_ADDR, ICU_IEN0_BIT },	/* No.192 RIIC0 TXI2 */
	{ ICU_IER18_ADDR, ICU_IEN1_BIT },	/* No.193 RIIC0 TEI2 */
	{ ICU_IER18_ADDR, ICU_IEN2_BIT },	/* No.194 RIIC0 EEI3 */
	{ ICU_IER18_ADDR, ICU_IEN3_BIT },	/* No.195 RIIC0 RXI3 */
	{ ICU_IER18_ADDR, ICU_IEN4_BIT },	/* No.196 RIIC0 TXI3 */
	{ ICU_IER18_ADDR, ICU_IEN5_BIT },	/* No.197 RIIC0 TEI3 */
	{ ICU_IER18_ADDR, ICU_IEN6_BIT },	/* No.198 DMAC DMAC0I */
	{ ICU_IER18_ADDR, ICU_IEN7_BIT },	/* No.199 DMAC DMAC1I */
	{ ICU_IER19_ADDR, ICU_IEN0_BIT },	/* No.200 DMAC DMAC2I */
	{ ICU_IER19_ADDR, ICU_IEN1_BIT },	/* No.201 DMAC DMAC3I */
	{ ICU_IER19_ADDR, ICU_IEN2_BIT },	/* No.202 EXDMAC EXDMAC0I */
	{ ICU_IER19_ADDR, ICU_IEN3_BIT },	/* No.203 EXDMAC EXDMAC1I */
	{ NULL, INVALID_OFFSET },			/* No.204 予約 */
	{ NULL, INVALID_OFFSET },			/* No.205 予約 */
	{ NULL, INVALID_OFFSET },			/* No.206 予約 */
	{ NULL, INVALID_OFFSET },			/* No.207 予約 */
	{ NULL, INVALID_OFFSET },			/* No.208 予約 */
	{ NULL, INVALID_OFFSET },			/* No.209 予約 */
	{ NULL, INVALID_OFFSET },			/* No.210 予約 */
	{ NULL, INVALID_OFFSET },			/* No.211 予約 */
	{ NULL, INVALID_OFFSET },			/* No.212 予約 */
	{ NULL, INVALID_OFFSET },			/* No.213 予約 */
	{ ICU_IER1A_ADDR, ICU_IEN6_BIT },	/* No.214 SCI0 RXI0 */
	{ ICU_IER1A_ADDR, ICU_IEN7_BIT },	/* No.215 SCI0 TXI0 */
	{ ICU_IER1B_ADDR, ICU_IEN0_BIT },	/* No.216 SCI0 TEI0 */
	{ ICU_IER1B_ADDR, ICU_IEN1_BIT },	/* No.217 SCI1 RXI1 */
	{ ICU_IER1B_ADDR, ICU_IEN2_BIT },	/* No.228 SCI1 TXI1 */
	{ ICU_IER1B_ADDR, ICU_IEN3_BIT },	/* No.229 SCI1 TEI1 */
	{ ICU_IER1B_ADDR, ICU_IEN4_BIT },	/* No.220 SCI2 RXI2 */
	{ ICU_IER1B_ADDR, ICU_IEN5_BIT },	/* No.221 SCI2 TXI2 */
	{ ICU_IER1B_ADDR, ICU_IEN6_BIT },	/* No.222 SCI2 TEI2 */
	{ ICU_IER1B_ADDR, ICU_IEN7_BIT },	/* No.223 SCI3 RXI3 */
	{ ICU_IER1C_ADDR, ICU_IEN0_BIT },	/* No.224 SCI3 TXI3 */
	{ ICU_IER1C_ADDR, ICU_IEN1_BIT },	/* No.225 SCI3 TEI3 */
	{ ICU_IER1C_ADDR, ICU_IEN2_BIT },	/* No.226 SCI4 RXI4 */
	{ ICU_IER1C_ADDR, ICU_IEN3_BIT },	/* No.227 SCI4 TXI4 */
	{ ICU_IER1C_ADDR, ICU_IEN4_BIT },	/* No.228 SCI4 TEI4 */
	{ ICU_IER1C_ADDR, ICU_IEN5_BIT },	/* No.229 SCI5 RXI5 */
	{ ICU_IER1C_ADDR, ICU_IEN6_BIT },	/* No.230 SCI5 TXI5 */
	{ ICU_IER1C_ADDR, ICU_IEN7_BIT },	/* No.231 SCI5 TEI5 */
	{ ICU_IER1D_ADDR, ICU_IEN0_BIT },	/* No.232 SCI6 RXI6 */
	{ ICU_IER1D_ADDR, ICU_IEN1_BIT },	/* No.233 SCI6 TXI6 */
	{ ICU_IER1D_ADDR, ICU_IEN2_BIT },	/* No.234 SCI6 TEI6 */
	{ ICU_IER1D_ADDR, ICU_IEN3_BIT },	/* No.235 SCI7 RXI7 */
	{ ICU_IER1D_ADDR, ICU_IEN4_BIT },	/* No.236 SCI7 TXI7 */
	{ ICU_IER1D_ADDR, ICU_IEN5_BIT },	/* No.237 SCI7 TEI7 */
	{ ICU_IER1D_ADDR, ICU_IEN6_BIT },	/* No.238 SCI8 RXI8 */
	{ ICU_IER1D_ADDR, ICU_IEN7_BIT },	/* No.239 SCI8 TXI8 */
	{ ICU_IER1E_ADDR, ICU_IEN0_BIT },	/* No.240 SCI8 TEI8 */
	{ ICU_IER1E_ADDR, ICU_IEN1_BIT },	/* No.241 SCI9 RXI9 */
	{ ICU_IER1E_ADDR, ICU_IEN2_BIT },	/* No.242 SCI9 TXI9 */
	{ ICU_IER1E_ADDR, ICU_IEN3_BIT },	/* No.243 SCI9 TEI9 */
	{ ICU_IER1E_ADDR, ICU_IEN4_BIT },	/* No.244 SCI10 RXI10 */
	{ ICU_IER1E_ADDR, ICU_IEN5_BIT },	/* No.245 SCI10 TXI10 */
	{ ICU_IER1E_ADDR, ICU_IEN6_BIT },	/* No.246 SCI10 TEI10 */
	{ ICU_IER1E_ADDR, ICU_IEN7_BIT },	/* No.247 SCI11 RXI11 */
	{ ICU_IER1F_ADDR, ICU_IEN0_BIT },	/* No.248 SCI11 TXI11 */
	{ ICU_IER1F_ADDR, ICU_IEN1_BIT },	/* No.249 SCI11 TEI11 */
	{ ICU_IER1F_ADDR, ICU_IEN2_BIT },	/* No.250 SCI12 RXI12 */
	{ ICU_IER1F_ADDR, ICU_IEN3_BIT },	/* No.251 SCI12 TXI12 */
	{ ICU_IER1F_ADDR, ICU_IEN4_BIT },	/* No.252 SCI12 TEI12 */
	{ ICU_IER1F_ADDR, ICU_IEN5_BIT },	/* No.253 IEB IEBINT */
	{ NULL, INVALID_OFFSET },			/* No.254 予約 */
	{ NULL, INVALID_OFFSET },			/* No.255 予約 */
};


/*
 *  IRQコントロールレジスタアドレステーブル
 */
volatile uint8_t __evenaccess * const irqcr_reg_addr[ IRQ_MAX ] = {
	ICU_IRQ0_ADDR,
	ICU_IRQ1_ADDR,
	ICU_IRQ2_ADDR,
	ICU_IRQ3_ADDR,
	ICU_IRQ4_ADDR,
	ICU_IRQ5_ADDR,
	ICU_IRQ6_ADDR,
	ICU_IRQ7_ADDR,
	ICU_IRQ8_ADDR,
	ICU_IRQ9_ADDR,
	ICU_IRQ10_ADDR,
	ICU_IRQ11_ADDR,
	ICU_IRQ12_ADDR,
	ICU_IRQ13_ADDR,
	ICU_IRQ14_ADDR,
	ICU_IRQ15_ADDR,
};

