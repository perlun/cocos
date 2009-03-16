/*
 * $Id$
 *
 * misc.h - Miscellaneous defines.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 * Copyright: (C) 2008 Per Lundberg
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

#endif // !__MISC_H__
