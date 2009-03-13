#
# $Id$
#
# This is the makefile for the 32-bit loader part of the cocOS kernel.
# Author: Per Lundberg <per@halleluja.nu>
#

TARGET_PATH = /var/lib/tftpboot
CC = gcc-4.1
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

# FIXME: having a hardwired reference to a particular version of libgcc.a here is ugly... I couldn't get it to work with just
# -lgcc though. Someone else should look into this at some time.
$(KERNEL): $(KERNEL_OBJS)
	$(LINK) $(LDFLAGS) $(KERNEL_OBJS) /usr/lib/gcc/x86_64-linux-gnu/4.1.2/32/libgcc.a -o $(KERNEL)

clean:
	rm -f $(KERNEL) $(KERNEL_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $(@) $<

%.o: %.S
	$(CC) $(AS_FLAGS) -o $(@) $<

install: all
	$(INSTALL) $(KERNEL) $(TARGET_PATH)

include Makefile.dep
