/* kernel.c - the C part of the kernel */
/* Copyright (C) 1999  Free Software Foundation, Inc.
   
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
   
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
   
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <multiboot.h>
#include <io.h>
                             
/* Macros.  */
                             
/* Check if the bit BIT in FLAGS is set.  */
#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))

#define KERNEL_VERSION_MAJOR    0
#define KERNEL_VERSION_MINOR    1

/* Function prototypes. */
extern void kernel_main (unsigned long magic, unsigned long addr);

/* Make sure the magic number is valid and print the Multiboot
   information structure pointed to by 'addr'.  */
void kernel_main (unsigned long magic, unsigned long addr)
{
    multiboot_info_t *mbi;
    
    /* Initialize basic I/O functionality.  */
    io_init ();

    /* Are we being booted by a Multiboot-compliant boot loader?  */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        io_printf ("Invalid magic number: 0x%x\n", (unsigned) magic);
        return;
    }
  
    io_printf ("cocOS Kernel version %d.%d booting...\n", KERNEL_VERSION_MAJOR,
               KERNEL_VERSION_MINOR);

    /* Set MBI to the address of the Multiboot information structure.  */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags.  */
    io_printf ("flags = 0x%x\n", (unsigned) mbi->flags);

    /* Are the mem_* valid?  */
    if (CHECK_FLAG (mbi->flags, 0))
    {
        io_printf ("mem_lower = %uKB, mem_upper = %uKB\n",
                   (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);
    }
  
    /* Is the boot_device valid?  */
    if (CHECK_FLAG (mbi->flags, 1))
    {
        io_printf ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);
    }
  
    /* Is the command line passed?  */
    if (CHECK_FLAG (mbi->flags, 2))
    {
        io_printf ("cmdline = %s\n", (char *) mbi->cmdline);
    }

    /* Are mods_* valid?  */
    if (CHECK_FLAG (mbi->flags, 3))
    {
        module_t *mod;
        int i;
      
        io_printf ("mods_count = %d, mods_addr = 0x%x\n",
                   (int) mbi->mods_count, (int) mbi->mods_addr);
        for (i = 0, mod = (module_t *) mbi->mods_addr;
             i < mbi->mods_count;
             i++, mod++)
	{
            io_printf (" mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
                       (unsigned) mod->mod_start,
                       (unsigned) mod->mod_end,
                       (char *) mod->string);
	}
    }

    /* Bits 4 and 5 are mutually exclusive!  */
    if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
    {
        io_printf ("Both bits 4 and 5 are set -- this is a BUG in your boot loader. Please contact the supplier.\n");
        return;
    }

    /* Is the section header table of ELF valid?  */
    if (CHECK_FLAG (mbi->flags, 5))
    {
        elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

        io_printf ("elf_sec: num = %u, size = 0x%x,"
                   " addr = 0x%x, shndx = 0x%x\n",
                   (unsigned) elf_sec->num, (unsigned) elf_sec->size,
                   (unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
    }
  
    /* Are the mmap_* fields valid?  */
    if (CHECK_FLAG (mbi->flags, 6))
    {
        memory_map_t *mmap;
      
        io_printf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
                   (unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
        for (mmap = (memory_map_t *) mbi->mmap_addr;
             (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
             mmap = (memory_map_t *) ((unsigned long) mmap
                                      + mmap->size + sizeof (mmap->size)))
            io_printf (" size = 0x%x, base_addr = 0x%x%x,"
                       " length = 0x%x%x, type = 0x%x\n",
                       (unsigned) mmap->size,
                       (unsigned) mmap->base_addr_high,
                       (unsigned) mmap->base_addr_low,
                       (unsigned) mmap->length_high,
                       (unsigned) mmap->length_low,
                       (unsigned) mmap->type);
    }
}    

