/* This file is generated from prc_rename.def by genrename. */

#ifndef TOPPERS_PRC_RENAME_H
#define TOPPERS_PRC_RENAME_H

/*
 *  prc_kernel_impl.c
 */
#undef intnest
#undef lock_flag
#undef saved_ipl
#undef prc_initialize
#undef prc_terminate
#undef config_int
#undef default_int_handler
#undef default_exc_handler

/*
 *  prc_support.src
 */
#undef dispatch
#undef start_dispatch
#undef exit_and_dispatch
#undef call_exit_kernel
#undef start_r
#undef interrupt
#undef exception

/*
 *  rx630_kernel_impl.c
 */
#undef ipr_reg_addr
#undef ier_reg_addr
#undef ir_reg_addr
#undef irqcr_reg_addr

/*
 *  kernel_cfg.c
 */
#undef cfg_int_table

#ifdef TOPPERS_LABEL_ASM

/*
 *  prc_kernel_impl.c
 */
#undef _intnest
#undef _lock_flag
#undef _saved_ipl
#undef _prc_initialize
#undef _prc_terminate
#undef _x_config_int
#undef _default_int_handler
#undef _default_exc_handler

/*
 *  prc_support.src
 */
#undef _dispatch
#undef _start_dispatch
#undef _exit_and_dispatch
#undef _call_exit_kernel
#undef _start_r
#undef _interrupt
#undef _exception

/*
 *  rx630_kernel_impl.c
 */
#undef _ipr_reg_addr
#undef _ier_reg_addr
#undef _ir_reg_addr
#undef _irqcr_reg_addr

/*
 *  kernel_cfg.c
 */
#undef _cfg_int_table

#endif /* TOPPERS_LABEL_ASM */


#endif /* TOPPERS_PRC_RENAME_H */
