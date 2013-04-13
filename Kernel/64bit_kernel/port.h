/* 
 * port.h - basic I/O support. (really only output, no input is supported. :-)
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: © 2013 Per Lundberg
 */

#ifndef __PORT_H__
#define __PORT_H__ 1

static inline uint8_t inb(uint16_t port)
{
    uint8_t result;
    asm volatile("inb %1, %0"
                 : "=a"(result) 
                 : "Nd"(port));
    return result;
}

static inline void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(value),
                    "Nd"(port));
}

#endif /* !__PORT_H__ */