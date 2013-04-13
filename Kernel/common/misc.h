/*
 * misc.h - Miscellaneous defines.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 * Copyright: © 2008, 2013 Per Lundberg
 */

#ifndef __MISC_H__
#define __MISC_H__

//// Defines.
// Standard sizes. We define those as "unsigned long long" = 64-bit numeric constants, to be able to use them in 64-bit
// expressions.
#define KiB     1024ULL
#define MiB     (KiB * 1024)
#define GiB     (MiB * 1024)

#define HALT()    while (1 == 1)

// TODO: Horribly hardwired for small-memory configurations at the moment. Will overwrite the VM structures if we go above 1024
// MiB of RAM... vm32.c should be patched to calculate this and return it. I did an attempt but managed to screw up the code
// completely, making the CPU reset every time you go to long mode... :)
#define KERNEL_COMMAND_LINE (char *) 0x6000

#endif // !__MISC_H__
