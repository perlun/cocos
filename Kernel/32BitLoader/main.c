/* main.c - C entry point for the kernel. */
/* Copyright (C) 2008  Per Lundberg */

#include <stdint.h>

#include "64bit.h"
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "multiboot.h"
#include "paging.h"
#include "tss.h"

/* This is where execution starts when the entry point code in start.S has finished setting the most fundamental parts up. 

   @param magic The magic number provided to us from the boot loader. 
   @param addr */
void kernel_main (uint32_t magic, multiboot_header_t *multiboot_header)
{
    io_init();
    io_print_line("cocOS32 version 0.1 loading...");

    /* Alright, let's get moving. What we do now is set up basic data structures to be able to activate the ultra-cool amd64
       "long mode". :-) */
    /* But first, detect that the CPU is actually a 64-bit CPU and not some stupid 32-bit one. ;-) */
    _64bit_detect();

    /* Prepare for 64-bit interrupt handling. */
    interrupt_init();

    /* Setup the 64-bit GDT (global descriptor table) and TSS (task switch segment). */
    gdt_init();
    tss_init();

    /* Setup 64-bit paging. */
    paging_init();

    /* All set -- initialize long mode! This function will never return. */
    _64bit_init();
}
