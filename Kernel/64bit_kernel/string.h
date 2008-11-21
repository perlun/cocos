/* 
 * $Id$
 *
 * string.h - simple string functions. (really only output, no input is supported. :-)
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008 Per Lundberg
 */

#ifndef __STRING_H__
#define __STRING_H__ 1

/**
 * Copy the source string to the target. The source string is presumed to be zero-terminated; the target string will
 * also be zero-terminated.
 *
 * @param target the target string.
 * @param source the source string.
 */
static inline void string_copy(char *target, const char *source)
{
    int i;
    for (i = 0; source[i] != '\0'; i++)
    {
        target[i] = source[i];
    }

    target[i] = '\0';
}

#endif /* !__STRING_H__ */
