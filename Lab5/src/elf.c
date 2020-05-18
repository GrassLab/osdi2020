#include "include/uart.h"
#include "include/mm.h"
#include "include/elf.h"
#include "include/kernel.h"

unsigned long get_u64(char* start){
	unsigned long num=0;
	for(int i=0;i<8;i++){
		num |= ((unsigned long)start[i] << (8*i));
	}
	return num;
}

void* elf_parser(unsigned long elf_start, unsigned long size){
	
	printf("start at %x\r\n",elf_start);
	char *elf = (char *)elf_start;
	Elf64_Ehdr elf_header;
	memcpy((unsigned long)&elf_header,(unsigned long)elf,sizeof(Elf64_Ehdr));
		
	printf("Entry point 0x%x\r\n",elf_header.e_entry);
 	printf("There are %d program headers, starting at offset %d\r\n\r\n",elf_header.e_phnum, elf_header.e_phoff);
 	
	Elf64_Phdr prog_header;
	char *prog = (char *)(elf + elf_header.e_phoff);
	memcpy((unsigned long)&prog_header,(unsigned long)prog,sizeof(Elf64_Phdr));
	
	int count = 0;
	while(prog_header.p_type!=PT_LOAD){
		if(count >= elf_header.e_phnum)
			break;
		prog += elf_header.e_phentsize;
		count++;
	}

	if(count>=elf_header.e_phnum){
		printf("Can't find load segment\r\n");
		return NULL;
	}

	printf("Program Headers:\r\n");
	printf("Offset              VirtAddr                  PhysAddr\r\n");
	printf("0x%16x  0x%16x        0x%16x\r\n", \
		prog_header.p_offset, prog_header.p_vaddr, prog_header.p_paddr);
	printf("FileSiz             MemSiz               Flags    Align\r\n");
	printf("0x%16x  0x%16x   0x%x      0x%x\r\n",\
		prog_header.p_filesz, prog_header.p_memsz,prog_header.p_flags, prog_header.p_align);	
	//dump_mem(elf,size);
	printf("\r\n");

	//void *ptr = mmap((void *)prog_header.p_vaddr, prog_header.p_filesz, prog_header.p_flags, MAP_FIXED , (void *)elf_start, prog_header.p_offset);
	return NULL;
}
