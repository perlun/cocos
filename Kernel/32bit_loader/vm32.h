/*
 * vm32.h - Virtual Memory prototypes
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 * Copyright: © 2008, 2013 Per Lundberg
*/

#ifndef __VM32_H__
#define __VM32_H__

#include "stdint.h"

/**
 * Set up the paging (VM) structures: PML4, PDP and PD tables.
 *
 * @param available_memory  The amount of available memory in the machine (in bytes).
 */
extern void vm_setup_paging_structures(uint64_t available_memory);

#endif // !__VM32_H__
