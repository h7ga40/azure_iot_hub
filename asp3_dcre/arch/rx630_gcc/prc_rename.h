/* This file is generated from prc_rename.def by genrename. */

#ifndef TOPPERS_PRC_RENAME_H
#define TOPPERS_PRC_RENAME_H

/*
 *  prc_kernel_impl.c
 */
#define intnest						_kernel_intnest
#define lock_flag					_kernel_lock_flag
#define saved_ipl					_kernel_saved_ipl
#define prc_initialize				_kernel_prc_initialize
#define prc_terminate				_kernel_prc_terminate
#define config_int					_kernel_config_int
#define default_int_handler			_kernel_default_int_handler
#define default_exc_handler			_kernel_default_exc_handler

/*
 *  prc_support.src
 */
#define dispatch					_kernel_dispatch
#define start_dispatch				_kernel_start_dispatch
#define exit_and_dispatch			_kernel_exit_and_dispatch
#define call_exit_kernel			_kernel_call_exit_kernel
#define start_r						_kernel_start_r
#define interrupt					_kernel_interrupt
#define exception					_kernel_exception

/*
 *  rx630_kernel_impl.c
 */
#define ipr_reg_addr				_kernel_ipr_reg_addr
#define ier_reg_addr				_kernel_ier_reg_addr
#define ir_reg_addr					_kernel_ir_reg_addr
#define irqcr_reg_addr				_kernel_irqcr_reg_addr

/*
 *  kernel_cfg.c
 */
#define cfg_int_table				_kernel_cfg_int_table

#ifdef TOPPERS_LABEL_ASM

/*
 *  prc_kernel_impl.c
 */
#define _intnest					__kernel_intnest
#define _lock_flag					__kernel_lock_flag
#define _saved_ipl					__kernel_saved_ipl
#define _prc_initialize				__kernel_prc_initialize
#define _prc_terminate				__kernel_prc_terminate
#define _config_int					__kernel_config_int
#define _default_int_handler		__kernel_default_int_handler
#define _default_exc_handler		__kernel_default_exc_handler

/*
 *  prc_support.src
 */
#define _dispatch					__kernel_dispatch
#define _start_dispatch				__kernel_start_dispatch
#define _exit_and_dispatch			__kernel_exit_and_dispatch
#define _call_exit_kernel			__kernel_call_exit_kernel
#define _start_r					__kernel_start_r
#define _interrupt					__kernel_interrupt
#define _exception					__kernel_exception


/*
 *  rx630_kernel_impl.c
 */
#define _ipr_reg_addr				__kernel_ipr_reg_addr
#define _ier_reg_addr				__kernel_ier_reg_addr
#define _ir_reg_addr				__kernel_ir_reg_addr
#define _irqcr_reg_addr				__kernel_irqcr_reg_addr

/*
 *  kernel_cfg.c
 */
#define _cfg_int_table				__kernel_cfg_int_table

#endif /* TOPPERS_LABEL_ASM */


#endif /* TOPPERS_PRC_RENAME_H */
