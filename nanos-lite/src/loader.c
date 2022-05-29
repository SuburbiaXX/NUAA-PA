#include "common.h"
#include "fs.h"
extern size_t get_ramdisk_size();
extern void ramdisk_read(void *buf,off_t offset,size_t len);
#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
	//ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
	int file;
	size_t len;
	file=fs_open(filename,0,0);
	len=fs_filesz(file);
	fs_read(file,DEFAULT_ENTRY,len);
	fs_close(file);
 	return (uintptr_t)DEFAULT_ENTRY;
}
