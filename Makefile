INCLUDE := -I ./include
CROSS_COMPILE = /home/fear/work/gcc-3.4.5-glibc-2.3.6/bin/arm-linux-
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

CFLAGS := -Wall -O2 -g
CFLAGS += -I $(shell pwd)/include

LDFLAGS := -Tlink.lds -nostdlib

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

TARGET := fear.bin

obj-y += boot/
obj-y += init/
obj-y += lib/
obj-y += kernel/
obj-y += driver/
#obj-y += boot/


all : 
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(LD) $(LDFLAGS) -o fear.elf built-in.o
	$(OBJCOPY) -O binary -S fear.elf $(TARGET)
	$(OBJDUMP) -D -m arm fear.elf > fear.dis


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.elf")
	rm -f $(shell find -name "*.dis")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.elf")
	rm -f $(shell find -name "*.dis")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	
