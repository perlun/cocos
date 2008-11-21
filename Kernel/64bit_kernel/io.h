/* 
 * $Id$
 *
 * io.h - basic I/O support. (really only output, no input is supported. :-)
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008 Per Lundberg
 */

#ifndef __IO_H__
#define __IO_H__ 1

extern void io_init(void);
extern void io_leet_print(const char *string);
extern void io_print(const char *string);
extern void io_print_line(const char *string);
extern void io_print_formatted(const char *format_string, ...);
extern void io_move_cursor(int row, int column);

#endif /* ! __IO_H__ */
