/*
 * $Id$
 *
 * multiboot.h - Multiboot defines/constants and data structures.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 */

/* This is to make sure that the assembly files can still include multiboot.h without getting compilation errors. */
#ifndef __ASSEMBLER__
#include <stdint.h>
#endif /* ! __ASSEMBLER__ */

/* Constants. */
/* The magic number for the Multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* The flags for the Multiboot header.  */
#define MULTIBOOT_HEADER_FLAGS		0x00000003

/* The magic number passed by a Multiboot-compliant boot loader.  */
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002

/* Data structures. */
/* The following code should not be available to assembly code, only C. */
#ifndef __ASSEMBLER__

/* The Multiboot header.  */
typedef struct
{
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} multiboot_header_t;

/* The section header table for ELF.  */
typedef struct
{
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
} elf_section_header_table_t;

/* The Multiboot information -- this data structure is passed to the kernel by the boot loader.  */
typedef struct
{
    struct
    {
        uint32_t has_memory_info: 1;
        uint32_t has_boot_device: 1;
        uint32_t has_command_line: 1;
        uint32_t has_module_info: 1;
        uint32_t reserved: 28;  // We don't care about the rest of these, even though some can actually be used.
    } flags;
    uint32_t memory_lower;
    uint32_t memory_upper;
    uint32_t boot_device;
    uint32_t command_line;
    uint32_t modules_count;
    uint32_t modules_info;
    elf_section_header_table_t elf_section_header_table;
    uint32_t memory_map_length;
    uint32_t memory_map_address;
} multiboot_info_t;

/* The multiboot module info structure. If the module information is present, there is one of these for each module. */
typedef struct
{
    uint32_t start;
    uint32_t end;
    uint32_t command_line;
    uint32_t reserved;
} multiboot_module_info_t;

#endif /* !__ASSEMBLER__ */
