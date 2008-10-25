/* 64bit.h - 64bit routines. */
/* Copyright (C) 2008  Per Lundberg */

#ifndef __INT64_H__
#define __INT64_H__ 1

/* A 64-bit GDT or IDT entry (they are identical so we use the same data type for both). */
typedef struct
{
    uint16_t limit __attribute__ ((packed));
    uint64_t base __attribute__ ((packed));
} descriptor_t;

extern void _64bit_detect();
extern void _64bit_init();

#endif /* ! __INT64_H__ */
