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

void elf_parser(unsigned long elf_start, unsigned long size){
	Elf64_Ehdr elf_header;
	Elf64_Phdr prog_header;
	
	printf("start at %x\r\n",elf_start);
	char *elf = (char *)elf_start;
	memcpy((void *)&elf_header,(void *)elf,sizeof(Elf64_Ehdr));
		
	printf("Entry point 0x%x\r\n",elf_header.e_entry);
 	printf("There are %d program headers, starting at offset %d\r\n\r\n",elf_header.e_phnum, elf_header.e_phoff);
 	
	char *prog = (char *)(elf + elf_header.e_phoff);
	memcpy((void *)&prog_header,(void *)prog,sizeof(Elf64_Phdr));
	
	int count = 0;
	while(prog_header.p_type!=PT_LOAD){
		if(count >= elf_header.e_phnum)
			break;
		prog += elf_header.e_phentsize;
		count++;
	}

	if(count>=elf_header.e_phnum){
		printf("Can't find load segment\r\n");
		return;
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
	
	/*char *f = (char * )(elf_start+prog_header.p_offset);
	
	for(int i=0;i<prog_header.p_filesz;i++){
		if(i%16==0)
			printf("\r\n");
		printf("%2x ",f[i]);
	}
	printf("\r\n");*/

	return ;
}
