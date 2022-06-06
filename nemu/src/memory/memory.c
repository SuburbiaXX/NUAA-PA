#include "nemu.h"
#include "device/mmio.h"
#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int mmio_id=is_mmio(addr);
	if(mmio_id==-1){
		return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
	}else{
		return mmio_read(addr,len,mmio_id);
	}
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int mmio_id=is_mmio(addr);
	if(mmio_id==-1){
		memcpy(guest_to_host(addr), &data, len);
	}else{
		mmio_write(addr,len,data,mmio_id);
	}
}


paddr_t page_translate(vaddr_t vaddr,bool is_write){
	PDE pde;
	PTE pte;
	
	//Log("vaddr:%#x",vaddr);
	//Log("CR3:%#x",cpu.cr3.page_directory_base);
	
	//vaddr右移22位取出高10位的DIR
	uint32_t DIR = (vaddr>>22);
	
	//Log("DIR:%#x",DIR);
	
	//DIR乘4与cr3寄存器的高20位相加
	uint32_t PDE_addr=(cpu.cr3.page_directory_base<<12)+(DIR<<2);
	//访问存储找到页表地址
	pde.val=paddr_read(PDE_addr,4);
	assert(pde.present);
	//vaddr取中间10位PAGE
	uint32_t PAGE=((vaddr>>12)&0x3ff);
	//PAGE乘4与PDE访存得到的值的高20位相加
	uint32_t PTE_addr=(pde.val & 0xfffff000)+(PAGE<<2);
	//访问存储
	pte.val=paddr_read(PTE_addr,4);
	assert(pte.present);
	//用PTE访存得到的值的高20位和vaddr的低12位相加得到最终物理地址
	uint32_t phaddr=(pte.val & 0xfffff000)+(vaddr & 0xfff);
	//设置accessed位
	pde.accessed=1;
	//写回页目录所在的地址	
	paddr_write(PDE_addr,4,pde.val);
	if(pte.accessed==0 || (pte.dirty==0 && is_write)){
		//检测accessed位和dirty位，is_write的状态，更新两个位
		pte.accessed=1;
		pte.dirty=1;
	}
	//写回页表项所在地址
	paddr_write(PTE_addr,4,pte.val);
	return phaddr;
}


uint32_t vaddr_read(vaddr_t addr, int len) {
  //return paddr_read(addr, len);
	if(cpu.cr0.paging){
		if((((addr<<20)>>20)+len)>0x1000){
			//addr在这一页的相对位置加上len大于一页的容量时
			int len1,len2;
			//分别计算俩页面上的数据长度
			len1=0x1000 - (addr & 0xfff);
			len2=len - len1;
			uint32_t addr1,mem1,addr2,mem2;
			//分别得到两个页面的地址和数据
			addr1=page_translate(addr,false);
			addr2=page_translate(addr+len1,false);
			mem1=paddr_read(addr1,len1);
			mem2=paddr_read(addr2,len2);
			//最后将俩部分数据缝合起来，注意是小端方式，第二个页面的数据应该放在高位
			return mem1+(mem2<<(len1<<3));
			//assert(0);
		}else{
			paddr_t paddr = page_translate(addr,false);
			return paddr_read(paddr,len);
		}
	}else{
		return paddr_read(addr,len);
	}
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  //paddr_write(addr, len, data);
	if(cpu.cr0.paging){
		if((((addr<<20)>>20)+len)>0x1000){
			//addr在这一页的相对位置加上len大于一页的容量时
      int len1,len2;
      //分别计算俩页面上的数据长度
      len1=0x1000 - (addr & 0xfff);
      len2=len - len1;
      uint32_t addr1,addr2;
      //分别得到两个页面的地址
      addr1=page_translate(addr,true);
      addr2=page_translate(addr+len1,true);
      paddr_write(addr1,len1,data);
			data=data>>(len1<<3);
			//第二个页面写入的是高位数据
      paddr_write(addr2,len2,data);
			//assert(0);
		}else{
			paddr_t paddr = page_translate(addr,true);
			return paddr_write(paddr,len,data);
		}
	}else{
		return paddr_write(addr,len,data);
	}
}


