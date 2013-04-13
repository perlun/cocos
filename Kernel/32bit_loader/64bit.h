/*
 * 64bit.h - prototypes for 64bit.S, the 64-bit mode initialization code.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 * Copyright: © 2008-2009, 2013 Per Lundberg
 */

#ifndef __64BIT_H__
#define __64BIT_H__ 1

/* This is to make sure that the assembly files can still include 64bit.h without getting compilation errors. */
#ifndef __ASSEMBLER__
extern void _64bit_init(void *multiboot_header, uint64_t highest_address);
#endif

// The entry point of the 64-bit kernel.
#define _64BIT_KERNEL_ENTRY_POINT        0x200000

#endif // ! __64bIT_H__
