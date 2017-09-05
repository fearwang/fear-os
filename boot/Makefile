obj-y := head.o init.o nand.o serial.o

init.bin: $(obj-y)
		$(LD) -Tboot.lds -o init_elf $^
		$(OBJCOPY) -O binary -S init_elf $@
		$(OBJDUMP) -D -m arm init_elf > init.dis
		cp init.bin ..
					
%.o:%.c
		$(CC) -Wall -O2 -c -o $@ $<

%.o:%.S
		$(CC) -Wall -O2 -c -o $@ $<

clean:
		rm -f init.bin init_elf init.dis *.o		

