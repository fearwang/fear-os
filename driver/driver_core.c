#include <driver_core.h>
#include <debug.h>



int register_storage_device(struct storage_device *sd,unsigned int num){
	if(num>MAX_STORAGE_DEVICE)
		return -1;

	if (storage[num]) {
		return -1;
	} else {
		storage[num] = sd;
	}
	
	return 0;
};



typedef int (*driver_init_func)(void);

extern int ramdisk_driver_init(void);

static driver_init_func init[] = {
    ramdisk_driver_init,
	0,
};



void driver_init()
{
    int i = 0;
	int ret = 0;
    for(i = 0; init[i]; i++) {
        ret = init[i]();
		if(ret != 0) {
			pr_error("init[%d] ret = %d\n", i, ret);
		}
    }
}
