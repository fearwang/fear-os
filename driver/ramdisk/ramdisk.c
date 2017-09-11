
#include "driver_core.h"
#include "string.h"


#define RAMDISK_SECTOR_SIZE		512
#define RAMDISK_SECTOR_MASK	(~(RAMDISK_SECTOR_SIZE-1))
#define RAMDISK_SECTOR_OFFSET	((RAMDISK_SECTOR_SIZE-1))




int ramdisk_read(struct storage_device *sd, void *dest, unsigned int addr, size_t size){

	memcpy(dest,(char *)(addr + sd->start_pos), size);

	return 0;
}

struct storage_device ramdisk_storage_device={
	.read = ramdisk_read,
	.sector_size = RAMDISK_SECTOR_SIZE,
	.storage_size = 2*1024*1024,
	.start_pos = 0x33000000,
};


int ramdisk_driver_init(void){
	int ret;
	//remap_l1(0x30800000,0x40800000,2*1024*1024);
	ret=register_storage_device(&ramdisk_storage_device,RAMDISK);
	return ret;
}

