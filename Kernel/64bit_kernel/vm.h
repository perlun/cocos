/* 
 * $Id$
 *
 * vm.h - Virtual Memory prototypes and data structures.
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008 Per Lundberg
 */

#ifndef __VM_H__
#define __VM_H__

#include "stdint.h"

// The size of a "small" page.
#define VM_SMALL_PAGE_SIZE      4096

// The size of a "large" page.
#define VM_LARGE_PAGE_SIZE      (2 * 1024 * 1024)

/**
 * Initialize the virtual memory subsystem.
 *
 * @param upper_memory_limit  The upper memory limit. The highest accessible memory address is upper_memory_limit - 1.
 */
extern void vm_init(uint64_t upper_memory_limit);

#endif // !__VM_H__
