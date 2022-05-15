#include "common.h"
//#include "fs.c"
extern size_t get_ramdisk_size();
extern void ramdisk_read(void *buf,off_t offset,size_t len);
#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
	ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
  return (uintptr_t)DEFAULT_ENTRY;
}
