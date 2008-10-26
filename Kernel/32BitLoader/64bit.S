/* 64bit.S - code for entering 64-bit mode. */
/* Copyright (C) 2008  Per Lundberg */

        .text

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
        push    dword ptr _32bit_cpu_message
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

        // Somewhere between here and the "stop" line, we crash...
               
        // We are now in 32-bit "compatibility mode". Let's do one more thing before jumping into 64-bit mode:  Set up
        // a new GDT, Global Descriptor Table. First, copy it over to its designated place.
        mov     edi, [gdt_pointer + 6]
        xor     ecx, ecx
        mov     cx, [gdt_pointer]
        rep     movsb

stop:   jmp     stop
        
        // ...then, load the GDTR register.
        lgdt    gdt_pointer
                        
        // Now, let's jump into the 64-bit code segment. The offset here must be
	// equal to the linear address of the 64-bit entry point, because 64-bit code is in an unsegmented address space.
        // The selector should point to a 32/64-bit code selector in the current GDT.
//      ljmpw   $0x0, $0x0
        .byte   0x66                    // Opcode prefix to let the CPU know that this is a 64-bit instruction. 
        .byte   0xEA
        .int    0                       // FIXME: 64 bit entry point here...
        .short  0                       // FIXME: 64-bit code selector here. 

        .data
_32bit_cpu_message:      
        .asciz  "The CPU does not support 64-bit mode. Kernel halted."

        // PML4 page tables are placed here.
pml4_base:
        .fill   512, 4, 0

        // Reference to the gdt , which we load so we can perform the jump into hyperspace... ehh, I mean, 64-bit code. :-)
        // We set the limit to 0xFFFF = 
.align  8
gdt_pointer:
        .word   3 * 8                   // The size of the GDT, in bytes. We only need three descriptors à 8 bytes.
        .quad   0x0000000000001000     // The location of the GDT. We place it at the second page in memory (4 KiB).

gdt:    .quad   0x0000000000000000      // Null selector.
        .quad   0x0020980000000000      // Code selector.
                                        // FIXME: having this hardwired like this makes it extremely ugly and hard to
                                        // see what this says... Perhaps we should even create the GDT from a C function
                                        // to make it more obvious what it does.
        .quad   0x0000900000000000      // Data selector.
        
