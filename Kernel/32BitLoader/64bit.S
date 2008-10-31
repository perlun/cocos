/*
 * $Id$
 *
 * 64bit.S - code for entering 64-bit mode.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 */

#include "64bit.h"
        
        .text
        .code32
        
        // This makes GNU assembler behave a bit more like MASM, TASM and NASM -- in other words, conventional x86
	// assemblers.
        .intel_syntax noprefix
        
        .globl _64bit_init

        // This code is more or less a rip-off of the sample code in the AMD64 Architecture Programmers Manual, Volume 2. 
        // (page 361 and onwards). What I've done is minor modifications.
        
_64bit_init:

        // First, detect a 64-bit CPU.
        mov     eax, 0x80000000         // Extended function 0x80000000
        cpuid                           // Get the largest extended function.
        cmp     eax, 0x80000000         // Do we have any function > 0x80000000?
        jbe     no_long_mode            // If not, long mode is not supported.
        mov     eax, 0x80000001         // Extended function 0x80000001
        cpuid                           // EDX = extended features flags.
        bt      edx, 29                 // Test for bit 29.
        jnc     no_long_mode            // If it's not set, long mode isn't supported
        jmp     has_long_mode

no_long_mode:
        push    dword ptr _non64bit_cpu_message
        call    io_print_line
halt:   jmp     halt

has_long_mode:  
        // OK, 64-bit (long mode) is supported. Let's get moving. We start off by setting enabling PAE (Page Address
        // Extensions) -- a requirement before entering long mode. This is done by setting CR4.PAE = 1.
        mov     eax, cr4
        bts     eax, 5
        mov     cr4, eax
       
        // Create the long-mode page tables, and initialize the 64-bit CR3 (page-table base address) to point to the
        // base of the PML4 page table. The PML4 page table must be located below 4 GiB because only 32 bits of CR3
        // are loaded when the processor is in 32-bit mode.

        // The page tables are placed at A000 (in other words, at 10 KiB) and they will look like this:

        // PML4:
        // dq 0x000000000000B00F		;00000000 00000000 00000000 00000000 00000000 00000000 10010000 00001111
        // times 511 dq 0x0000000000000000

        // Page Directory:
        // dq 0x000000000000C00F		;00000000 00000000 00000000 00000000 00000000 00000000 10100000 00001111
        // times 511 dq 0x0000000000000000

        // Page Tables: :
        // dq 0x000000000000018F		;00000000 00000000 00000000 00000000 00000000 00000000 00000001 10001111
        // times 511 dq 0x0000000000000000

        // This defines one 2MB identity-mapped page at the start of memory (so we can access the first 2 megs of
        // RAM just like we would if paging/long mode was not in place).

        // First the PML4.
        cld
        mov     edi, 0xA000

        mov     eax, 0xB00F
        stosd

        xor     eax, eax
        mov     ecx, 0x03FF
        rep     stosd

        // Then the page directory.
        mov     eax, 0xC00F
        stosd

        xor     eax, eax
        mov     ecx, 0x03FF
        rep     stosd

        // Then, finally, the page tables.
        mov     ax, 0x018F
        stosd

        xor     eax, eax
        mov     ecx, 0x03FF
        rep     stosd
                
        mov     eax, 0xA000
        mov     cr3, eax
       
        // Enable long mode (set EFER.LME = 1).
        mov     ecx, 0xC0000080         // EFER MSR number.
        rdmsr                
        bts     eax, 8                  //  Set LME = 1.
        wrmsr
       
        // Enable paging to activate long mode. This is done by setting CR0.PG = 1.
        mov     eax, cr0
        bts     eax, 31                 //  Set PE = 1
        mov     cr0, eax

        // We are now in 32-bit "compatibility mode". Let's do one more thing before jumping into 64-bit mode:  Set up
        // a new GDT, Global Descriptor Table. First, copy it over to its designated place.
        /* FIXME: This doesn't work, for some reason... Anyway, we leave it like this
           for now. This means the GDT will be in the 32-bit kernel data area, so we can't just wipe it later without
           pretty fatal consequences. */
/*      mov     esi, dword ptr gdt
        mov     edi, 0x1000
        mov     ecx, 8 * 3
        rep     movsb */
       
        // ...then, load the GDTR register.
        lgdt    gdt_pointer

        // Now, let's jump into the 64-bit code segment. This one is a bit tricky to have GAS assemble correctly so
        // we hardcode the opcodes instead. :)
        .byte   0xEA                    // jmp far
        .long   _64bit_entry_point
        .word   8                       // 64-bit code selector. Shift off the last 3 bits of this and you get the
                                        // index of the selector in the GDT.

        .code64
_64bit_entry_point:
        // Print a simple message indicating that the 64-bit initialization was successful.
        mov     edi, 0x00b8000 + (80 * 2)
        //mov     esi, _64bit_initialized_message
        jmp     _end_of_64print_loop
        
_64print_loop:
        lodsb
        jz        _end_of_64print_loop
        stosb
        jmp     _64print_loop

_end_of_64print_loop:   
        
        // Now, call the 64-bit kernel. When this method (_64bit_init) was called, we received an argument that
        // is a pointed to the Multiboot header. We want to pass this header on to the 64-bit kernel, so we do it
        // as you would expect -- by pushing it onto the stack.
        push    0xC0CAC01A //qword ptr [esp + 4]
        
        mov     rax, _64BIT_KERNEL_ENTRY_POINT          // We can't just call this address directly, there doesn't seem
        call    rax                                     // to be such an instruction... so we go via a register.
                    
        .data
_non64bit_cpu_message:      
        .asciz  "The CPU does not support 64-bit mode. Kernel halted."

_64bit_initialized_message:
        .asciz  "64-bit mode successfully initialized."
        
        // PML4 page tables are placed here.
pml4_base:
        .fill   512, 4, 0

        // Reference to the gdt , which we load so we can perform the jump into hyperspace... ehh, I mean, 64-bit code. :-)
        // We set the limit to 0xFFFF = 
gdt_pointer:
        .word   3 * 8                   // The size of the GDT, in bytes. We only need three descriptors à 8 bytes.
        .long   gdt                     // The location of the GDT. We place it at the second page in memory (4 KiB).
        //.long   0x1000               // This one doesn't work, as mentioned earlier.
        
        // FIXME: having these hardwired like this makes it extremely ugly and hard to see what this says... Perhaps we
	// should even create the GDT from a C function to make it more obvious what they look like.
gdt:    .quad   0x0000000000000000      // Null descriptor.
        .quad   0x0020980000000000      // Code descriptor..
        .quad   0x0000900000000000      // Data descriptor..
        
