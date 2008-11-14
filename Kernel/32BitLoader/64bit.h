/*
 * $Id$
 *
 * 64bit.h - prototypes for 64bit.S, the 64-bit mode initialization code.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 */

#ifndef __INT64_H__
#define __INT64_H__ 1

/* This is to make sure that the assembly files can still include multiboot.h without getting compilation errors. */
#ifndef __ASSEMBLER__
extern void _64bit_init(void *multiboot_header, uint64_t highest_address);
#endif

// The entry point of the 64-bit kernel.
#define _64BIT_KERNEL_ENTRY_POINT        0x100000

#endif /* ! __INT64_H__ */
