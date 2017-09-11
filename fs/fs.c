#include "string.h"
#include "fs.h"

#define MAX_SUPER_BLOCK 8
#define NULL (void *)0




int register_file_system(struct super_block *type,unsigned int id){
	if(fs_type[id]==NULL){
		fs_type[id]=type;
		return 0;
	}
	return -1;
}

void unregister_file_system(struct super_block *type,unsigned int id){
	fs_type[id]=NULL;
}