#ifndef __DRIVER_CORE_H__
#define __DRIVER_CORE_H__

#define MAX_STORAGE_DEVICE (2)
#define RAMDISK 0
#define NAND 1

struct storage_device *storage[MAX_STORAGE_DEVICE];

typedef unsigned int size_t;
struct storage_device{
    unsigned int start_pos;
    size_t sector_size;
    size_t storage_size;
    int (*read)(struct storage_device *sd,void *dest,unsigned int bias,size_t size);
    int (*write)(struct storage_device *sd,void *dest,unsigned int  bias,size_t size);
};

void driver_init();
struct storage_device *storage[MAX_STORAGE_DEVICE];
int register_storage_device(struct storage_device *sd,unsigned int num);

#endif
