obj-y := main.o

all: main.bin
	cp main.elf ./rootfs
	genromfs -d rootfs/ -f rootfs.img
	cp rootfs.img ..

main.bin: $(obj-y)
		$(LD) -Tapp.lds -o main.elf $^
		$(OBJCOPY) -O binary -S main.elf $@
		$(OBJDUMP) -D -m arm main.elf > main.dis
					
%.o:%.c
		$(CC) $(CFLAGS) -Wall -O2 -c -o $@ $<

%.o:%.S
		$(CC) $(CFLAGS) -Wall -O2 -c -o $@ $<

clean:
		rm -f main.bin main.elf main.dis *.o rootfs.img	

