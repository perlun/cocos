/* start.S - entry point for the kernel. */
/* Copyright (C) 2008  Per Lundberg */

#include "multiboot.h"      
#include "kernel.h"
                        
        .text
        .globl start, _start

start:
_start:       
        jmp after_multiboot_header

	/* Align the header to the closest 32-bit boundary. */
	.align	4
        
multiboot_header:       
        /* The Multiboot header comes here. This header will let the Multiboot-compliant boot loader know that this is a
           Multiboot kernel and not Linux, OpenBSD or something else. */

        /* A "magic" value, the Multiboot signature. */
	.long	MULTIBOOT_HEADER_MAGIC

         /* Flags for how we want the boot loader to set up the environment for us. */
	.long	MULTIBOOT_HEADER_FLAGS

        /* Checksum -- this is a safety measure to decrease the risk that non-Multiboot kernels would be wrongly identified
           as such. */
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

        /* "Real" entry point. */
after_multiboot_header: 

        /* Initialize the stack pointer -- we cannot rely on any stack provided by the boot loader. */
	movl	$(stack + STACK_SIZE), %esp

	/* Reset the EFLAGS -- we cannot rely on the value provided by the boot loader. */
	pushl	$0
	popf

	/* Push the pointer to the Multiboot information structure. */
	pushl	%ebx
        
	/* Push the boot loader magic value. (This is not the same as the MULTIBOOT_HEADER_MAGIC) */
	pushl	%eax

	/* Call the kernel_main function. The rest of the kernel is now done in C, with some functions implemented in
           assembly language. */
	call	kernel_main

        /* We should never get back here, but... let's create an infinite loop "just in case"... */
halt:   jmp halt
        
	/* Our stack.  */
	.comm	stack, STACK_SIZE
       
