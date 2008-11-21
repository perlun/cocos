/*
 * $Id$
 *
 * cpu.h - Functions for operating directly with CPU registers and similar.
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008 Per Lundberg
 */

#ifndef __CPU_H__
#define __CPU_H__ 1

#include <stdint.h>

/*
 * Get the value of the RSP register.
 *
 * @returns the value of the RSP register.
 */
static inline uint64_t cpu_get_rsp(void)
{
    uint64_t rsp;
    asm("movq %%rsp, %0"
        : "=r"(rsp));
    return rsp;
}

/*
 * Get the value of the RBP register.
 *
 * @returns the value of the RBP register.
 */
static inline uint64_t cpu_get_rbp(void)
{
    uint64_t rsp;
    asm("movq %%rbp, %0"
        : "=r"(rsp));
    return rsp;
}

#endif /* !__CPU_H__ */
