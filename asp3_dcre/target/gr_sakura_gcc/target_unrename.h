/* This file is generated from target_rename.def by genrename. */

#ifndef TOPPERS_TARGET_UNRENAME_H
#define TOPPERS_TARGET_UNRENAME_H

/*
 *  target_kernel_impl.c
 */
#undef target_initialize
#undef target_exit

/*
 *  trace_config.c
 */
#undef log_dsp_enter
#undef log_dsp_leave
#undef log_inh_enter
#undef log_inh_leave
#undef log_exc_enter
#undef log_exc_leave


#ifdef TOPPERS_LABEL_ASM

/*
 *  target_kernel_impl.c
 */
#undef _target_initialize
#undef _target_exit

/*
 *  trace_config.c
 */
#undef _log_dsp_enter
#undef _log_dsp_leave
#undef _log_inh_enter
#undef _log_inh_leave
#undef _log_exc_enter
#undef _log_exc_leave


#endif /* TOPPERS_LABEL_ASM */

#include "prc_unrename.h"

#endif /* TOPPERS_TARGET_UNRENAME_H */
