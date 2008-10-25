/* 64bit.c - 64bit routines. */
/* Copyright (C) 2008  Per Lundberg */

#include "io.h"

/* Make sure the CPU is 64-bit capable. Halt otherwise. */
void _64bit_detect()
{
    int has_64bit_mode = 0;

    asm("movl $80000000, %%eax\n"       // Extended function 0x80000000
        "cpuid\n"                       // Get the largest extended function.
        "cmpl $80000000, %%eax\n"       // Do we have any function > 0x80000000?
        "jbe no_long_mode\n"            // If not, long mode is not supported.
        "movl $80000001, %%eax\n"       // Extended function 0x80000001
        "cpuid\n"                       // EDX = extended features flags.
        "bt %%edx, 29\n"               // Test for bit 29. (I don't know how to write that in decimal with GAS...)
        "jnc no_long_mode\n"            // If it's not set, long mode isn't supported
        "movl $1, %0\n"
        "no_long_mode:" :
        "=r" (has_64bit_mode));
    
    if (has_64bit_mode == 0)
    {
        io_print_line("Sorry, but your CPU is not 64-bit capable. System halted.");
    }        
}

/* Initialize 64-bit mode (also called "long mode" by AMD). */
void _64bit_init()
{
    /* Enable physical address extensions (PAE). */
    /* Set the appropriate value of CR3 (page directory base register). */
    /* Enable long mode. */
    /* Enable paging. */

    /* Alright, we are now in long mode -- yahoo! We now load the GDT, LDT, IDT and TR descriptor table registers with the
       values representing the 64-bit versions of these data structures. */
    
}
