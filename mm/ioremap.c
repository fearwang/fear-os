
void *ioremap(unsigned long phys_addr, unsigned long size, unsigned long flags)
{
    return (void *)(phys_addr + 0x00000000);
}
