/* 
 * $Id$
 *
 * memory.h - simple functions for operating on memory ranges. In the future, those will be optimized using inline
 * assembly. For now, doing such optimizations is not something we prioritize.
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008 Per Lundberg
 */

#ifndef __MEMORY_H__
#define __MEMORY_H__ 1

#include <stdint.h>

/**
 * Zero a given memory region.
 *
 * @memory  The memory to zero.
 * @length  The number of bytes to zero.
 */
static inline void memory_zero(void *memory, unsigned int length)
{
    uint8_t *memory_uint8 = (uint8_t *)memory;

    for (unsigned int i = 0; i < length; i++)
    {
        memory_uint8[i] = 0;
    }
}

/**
 * Copy an area of memory.
 *
 * @target  The target of the copying.
 * @source  The source of the copying.
 * @length  The number of bytes that should be copied.
 */
static inline void memory_copy(void *target, void *source, unsigned int length)
{
    // Of course, doing it like this (copying one single byte at a time) is extremely inefficient, but it works and is
    // fool-proof.
    uint8_t *target_uint8 = (uint8_t *) target;
    uint8_t *source_uint8 = (uint8_t *) source;

    for (unsigned int i = 0; i < length; i++)
    {
        target_uint8[i] = source_uint8[i];
    }
}

#endif /* !__MEMORY_H__ */
