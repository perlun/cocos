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
    io_print_line("cocOS32 version 2008 (build 0001) loading...");

    /* Alright, let's get moving. What we do now is set up basic data structures to be able to activate the ultra-cool amd64
       "long mode". :-) But first, detect that the CPU is actually a 64-bit CPU.

       Initially, I had thought about doing it in a bunch of C functions but it's actually more elegant to do it all in one
       single assembly function. 

       Because of its nature, this function will never return. If 64-bit initialization fails, it will halt the CPU. */
    _64bit_init();
}
