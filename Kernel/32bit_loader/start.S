/*
 * start.S - assembly language entry point for the kernel.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 * Copyright © 2008, 2013 Per Lundberg
 */

#include "multiboot32.h"      

        /* The size of our kernel stack (16 KiB, 4 x 4 KiB pages). */
        #define STACK_SIZE                      0x4000
                         
        .text
        .globl start, _start

        .intel_syntax noprefix

start:
_start:       
        jmp after_multiboot_header

        .align  4
        
multiboot_header:       
        // The Multiboot header marks to the boot loader that this is a Multiboot kernel and not Linux, OpenBSD or something else.

        // A "magic" value, the Multiboot signature.
        .long   MULTIBOOT_HEADER_MAGIC

         // Flags for how we want the boot loader to set up the environment for us.
        .long   MULTIBOOT_HEADER_FLAGS

        // The checksum acts as a safety measure to decrease the risk that a non-Multiboot kernels would be mistakes as Multiboot.
        .long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

after_multiboot_header: 

        // Initialize the stack pointer -- we cannot rely on any stack provided by the boot loader.
        mov     esp, offset stack + STACK_SIZE

        // Reset the EFLAGS -- we cannot rely on the value provided by the boot loader.
        push    0
        popf

        // Push the pointer to the Multiboot information structure.
        push    ebx
        
        // Push the boot loader magic value. (This is not the same as the MULTIBOOT_HEADER_MAGIC)
        push    eax

        call    main

        // Should never get back here, but we set up an infinite loop just in case.
halt:   jmp halt
        
        // Our stack.
        .comm   stack, STACK_SIZE
