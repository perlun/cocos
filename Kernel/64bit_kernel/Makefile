#
# This is the makefile for the 64-bit cocOS kernel.
#
# Author: Per Lundberg <per@halleluja.nu>
# Copyright: © 2008-2009, 2013, 2017 Per Lundberg
#

include ../Makefile.common

AS_FLAGS = -c -m64 -Wall -Werror -Wno-main

LDFLAGS = -m64 -nostdlib -Wl,--oformat -Wl,binary -Wl,-N -Wl,-Ttext -Wl,200000 -e main

# No user-serviceable parts below this line. :-)

LINK = $(CC)
KERNEL = cocOS64.bin
KERNEL_OBJS = main.o io.o vm.o

all: Makefile.dep $(KERNEL)

Makefile.dep: *.c *.h
	$(CC) $(CFLAGS) -M *.c > $(@)

$(KERNEL): $(KERNEL_OBJS)
	$(LINK) $(LDFLAGS) $(KERNEL_OBJS) -o $(KERNEL)

clean:
	rm -f $(KERNEL) $(KERNEL_OBJS) Makefile.dep

%.o: %.c Makefile
	$(CC) $(CFLAGS) -o $(@) $<

%.o: %.S Makefile
	$(CC) $(AS_FLAGS) -o $(@) $<

install: all
	mcopy -o $(KERNEL) $(TARGET_MTOOLS_VOLUME)

include Makefile.dep
