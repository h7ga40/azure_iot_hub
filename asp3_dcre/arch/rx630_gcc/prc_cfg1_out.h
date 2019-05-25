/*
 *		cfg1_out.cのリンクに必要なスタブの定義
 */

void sta_ker( void )
{
}

void hardware_init_hook( void )
{
}

void software_init_hook( void )
{
}

const size_t	_kernel_istksz = 0;

STK_T *const	_kernel_istk = NULL;

/*
 *  オフセットファイルを生成するための定義
 */
const uint8_t	MAGIC_1 = 0x12;
const uint16_t	MAGIC_2 = 0x1234;
const uint32_t	MAGIC_4 = 0x12345678;

const TCB	TCB_enatex = {
	{ NULL, NULL },			/* task_queue */
	NULL,					/* p_tinib */
	0U,						/* tstat */
	0U,						/* bpriority */
	0U,						/* priority */
	false,					/* acqeue */
	false,					/* wupque */
	true,					/* enatex */
#ifdef TOPPERS_SUPPORT_OVRHDR
	false,					/* staovr */
#endif /* TOPPERS_SUPPORT_OVRHDR */
	0U,						/* texptn */
	NULL,					/* p_winifo */
	NULL,			/* mutex_queue */
#ifdef TOPPERS_SUPPORT_OVRHDR
	0U,						/* leftotm */
#endif /* TOPPERS_SUPPORT_OVRHDR */
	{ NULL, NULL }			/* tskctxb */
};
