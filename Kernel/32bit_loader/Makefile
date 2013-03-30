#
# This is the makefile for the 32-bit loader part of the cocOS kernel.
#
# Author: Per Lundberg <per@halleluja.nu>
# Copyright: © 2008-2009, 2013 Per Lundberg
#

TARGET_PATH = /var/lib/tftpboot
INSTALL = install
AS_FLAGS = -c -m32 -Wall -Werror -Wno-main -I..
CFLAGS = $(AS_FLAGS) --std=gnu99 -ggdb -I..
LDFLAGS = -m32 -nostdlib -Wl,-N -Wl,-Ttext -Wl,80000 

# No user-serviceable parts below this line. :-)

LINK = $(CC)
KERNEL = cocOS32.bin
KERNEL_OBJS = start.o io.o 64bit.o main.o vm.o

all: Makefile.dep $(KERNEL)

Makefile.dep:
	$(CC) $(CFLAGS) -M *.c > $(@)

$(KERNEL): $(KERNEL_OBJS)
	$(LINK) $(LDFLAGS) $(KERNEL_OBJS) -lgcc -o $(KERNEL)

clean:
	rm -f $(KERNEL) $(KERNEL_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $(@) $<

%.o: %.S
	$(CC) $(AS_FLAGS) -o $(@) $<

install: all
	$(INSTALL) $(KERNEL) $(TARGET_PATH)

include Makefile.dep
