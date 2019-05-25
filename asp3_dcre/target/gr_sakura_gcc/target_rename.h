/* This file is generated from target_rename.def by genrename. */

#ifndef TOPPERS_TARGET_RENAME_H
#define TOPPERS_TARGET_RENAME_H

/*
 *  target_kernel_impl.c
 */
#define target_initialize			_kernel_target_initialize
#define target_exit					_kernel_target_exit

/*
 *  trace_config.c
 */
#define log_dsp_enter				_kernel_log_dsp_enter
#define log_dsp_leave				_kernel_log_dsp_leave
#define log_inh_enter				_kernel_log_inh_enter
#define log_inh_leave				_kernel_log_inh_leave
#define log_exc_enter				_kernel_log_exc_enter
#define log_exc_leave				_kernel_log_exc_leave


#ifdef TOPPERS_LABEL_ASM

/*
 *  target_kernel_impl.c
 */
#define _board_id					__kernel_board_id
#define _board_addr					__kernel_board_addr
#define _target_initialize			__kernel_target_initialize
#define _target_exit				__kernel_target_exit
#define _config_int					__kernel_config_int

/*
 *  trace_config.c
 */
#define _log_dsp_enter				__kernel_log_dsp_enter
#define _log_dsp_leave				__kernel_log_dsp_leave
#define _log_inh_enter				__kernel_log_inh_enter
#define _log_inh_leave				__kernel_log_inh_leave
#define _log_exc_enter				__kernel_log_exc_enter
#define _log_exc_leave				__kernel_log_exc_leave


#endif /* TOPPERS_LABEL_ASM */

#include "prc_rename.h"

#endif /* TOPPERS_TARGET_RENAME_H */
