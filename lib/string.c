
void * memcpy(void * dest,const void *src,unsigned int count)
{
	char *tmp = (char *) dest, *s = (char *) src;
	while (count--)
		*tmp++ = *s++;
	return dest;
}

void hexdump(char *buf, unsigned int len)
{
	int i = 0, j = 0;
	char line[48];
	for(i = 0; i < len; i++) {
		line[j] = buf[i];
		j++;
		if((i+1)%16 == 0) {
			j = 0;
			for(j = 0; j < 16; j++) {
				printk("%x ", line[j]);
			}
			printk('\t');
			for(j = 0; j < 16; j++) {
				if(line[j] > 0 && line[j] < 128)
					printk("%c", line[j]);
				else 
					printk(".");
			}
			printk("\n");
			j = 0;
		}
	}
}