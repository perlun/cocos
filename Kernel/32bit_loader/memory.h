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

#endif /* !__MEMORY_H__ */
