#include "common.h"
#include "fs.h"
#include "memory.h"
extern size_t get_ramdisk_size();
extern void ramdisk_read(void *buf,off_t offset,size_t len);
//#define DEFAULT_ENTRY ((void *)0x4000000)
#define DEFAULT_ENTRY ((void *)0x8048000)


uintptr_t loader(_Protect *as, const char *filename) {
	//ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
	int file;
	size_t len;
	file=fs_open(filename,0,0);
	len=fs_filesz(file);
	void *pa;
	void *va=DEFAULT_ENTRY;
	int count=len/PGSIZE;
	for(int i=0;i<count+1;i++){
		pa=new_page();
		Log("Map va to pa: 0x%08x to 0x%08x",va,pa);
		_map(as,va,pa);
		fs_read(file,pa,PGSIZE);
		va+=PGSIZE;
	}
//	fs_read(file,DEFAULT_ENTRY,len);
	fs_close(file);
 	return (uintptr_t)DEFAULT_ENTRY;
}
