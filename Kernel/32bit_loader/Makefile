#
# This is the makefile for the 32-bit loader part of the cocOS kernel.
#
# Author: Per Lundberg <per@halleluja.nu>
# Copyright: © 2008-2009, 2013 Per Lundberg
#

INSTALL = install
AS_FLAGS = -c -m32 -Wall -Werror -Wno-main -I..
LDFLAGS = -m32 -nostdlib -Wl,-N -Wl,-Ttext -Wl,100000

# No user-serviceable parts below this line. :-)

LINK = $(CC)
KERNEL = cocOS32.bin
KERNEL_OBJS = start.o io32.o 64bit.o main32.o vm32.o compiler_rt/udivdi3.o compiler_rt/umoddi3.o

all: Makefile.dep $(KERNEL)

Makefile.dep:
	$(CC) $(CFLAGS) -M *.c > $(@)

$(KERNEL): $(KERNEL_OBJS)
	$(LINK) $(LDFLAGS) $(KERNEL_OBJS) -o $(KERNEL)

clean:
	rm -f $(KERNEL) $(KERNEL_OBJS) Makefile.dep

%.o: %.c
	$(CC) $(CFLAGS) -o $(@) $<

%.o: %.S
	$(CC) $(AS_FLAGS) -o $(@) $<

install: all
	$(INSTALL) $(KERNEL) $(TARGET_PATH)

include ../Makefile.common
include Makefile.dep
