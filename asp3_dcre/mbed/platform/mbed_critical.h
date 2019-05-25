#ifndef MBED_CRITICAL_H
#define MBED_CRITICAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void core_util_critical_section_enter(void);
void core_util_critical_section_exit(void);

#ifdef __cplusplus
}
#endif

#endif
