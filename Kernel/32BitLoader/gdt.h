/* gdt.h - basic GDT handling. */
/* Copyright (C) 2008  Per Lundberg */

#ifndef __GDT_H__
#define __GDT_H__ 1

#include <stdint.h>

extern void gdt_init();

/* A 64-bit GDT entry. */
typedef struct
{
    uint16_t limit __attribute__ ((packed));
    uint64_t base __attribute__ ((packed));
} gdt64_t;

#endif /* ! __GDT_H__ */
