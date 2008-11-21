/*
 * $Id$
 *
 * math.h - miscellaneous math-related functions.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 */

#ifndef __MATH_H__
#define __MATH_H__

// This one is sooo beautiful... :-) What about the case when a == b? Well, b will be returned, but it doesn't matter since
// they are equal... Math can be beautiful sometimes, can it not?
#define MAX(a,b) (a > b ? a : b)

#endif // !__MATH_H__
