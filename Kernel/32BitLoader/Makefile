TARGET_PATH = /var/lib/tftpboot/cocOS32.bin
CC = gcc-4.1
INSTALL = install
AS_FLAGS = -c -m32 -Wall -Werror
CFLAGS = $(AS_FLAGS) --std=gnu99
LDFLAGS = -m32 -nostdlib -Wl,-N -Wl,-Ttext -Wl,100000

# No user-serviceable parts below this line. :-)

LINK = $(CC)
KERNEL = cocOS32.bin
KERNEL_OBJS = start.o io.o 64bit.o interrupt.o gdt.o tss.o paging.o main.o

all: $(KERNEL)

$(KERNEL): $(KERNEL_OBJS)
	$(LINK) $(LDFLAGS) $(KERNEL_OBJS) -o $(KERNEL)

clean:
	rm -f $(KERNEL) $(KERNEL_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $(@) $<

%.o: %.S
	$(CC) $(AS_FLAGS) -o $(@) $<

install: all
	$(INSTALL) $(KERNEL) $(TARGET_PATH)
