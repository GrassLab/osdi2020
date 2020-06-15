#include "obj_alloc.h"
#include "mm.h"
#include "string.h"
#include "uart.h"
#define objsize 2000

int varied_size_alloc(int size)
{
	int alloc_obj = size / objsize;
	if (size % objsize != 0)
	{
		alloc_obj++;
	}
	int pa = (page_size / objsize);
	int create_page = alloc_obj / pa;
	if (alloc_obj % pa != 0)
	{
		create_page++;
	}
	printf("var_size = %d, alloc obj count = %d,one page have %d , shoud create "
		   "page = %d\n",
		   size, alloc_obj, pa, create_page);

	unsigned long addr = page_alloc(create_page * page_size);
	if (addr == -1)
	{
		printf("alloc error\n");
		return -1;
	}

	unsigned long create_size = pa * create_page;
	for (int i = fix_object_count; i < fix_object_count + create_size; i++)
	{
		fix_object_array[i].is_use = 0;
		fix_object_array[i].obj = (struct page_struct *)(addr + i * objsize);
	}

	int get = fix_object_count;
	for (int i = fix_object_count; i < fix_object_count + alloc_obj; i++)
	{
		fix_object_array[i].is_use = 1;
		fix_object_array[i].sequence = alloc_obj;
	}

	fix_object_count += create_size;
	return get;
}

int fix_obj_alloc()
{
	for (int i = 0; i < fix_object_count; i++)
	{
		if (fix_object_array[i].is_use == 0)
		{
			char *s = (char *)fix_object_array[i].obj;
			for (int j = 0; j < objsize; j++)
			{
				*(s + j) = 0;
			}
			fix_object_array[i].is_use = 1;
			fix_object_array[i].sequence = 1;
			return i;
		}
	}
	if (fix_object_count == 100)
	{
		printf("full\n");
		return -1;
	}
	unsigned long addr = page_alloc(1 * page_size);
	if (addr == -1)
	{
		printf("alloc error\n");
		return -1;
	}

	unsigned long create_size = page_size / objsize;
	printf("create_obj = %d\n", create_size);
	for (int i = fix_object_count; i < fix_object_count + create_size; i++)
	{
		fix_object_array[i].is_use = 0;
		fix_object_array[i].obj = (struct page_struct *)(addr + i * objsize);
	}
	int get = fix_object_count;
	fix_object_array[fix_object_count].is_use = 1;
	fix_object_array[fix_object_count].sequence = 1;
	fix_object_count += create_size;
	return get;
}

int obj_free(int free_index)
{
	if (fix_object_array[free_index].is_use == 1)
	{
		printf("free %d obj\n", fix_object_array[free_index].sequence);
		for (int i = free_index; i < fix_object_array[free_index].sequence; i++)
		{
			fix_object_array[i].is_use = 0;
			fix_object_array[i].sequence = 0;
		}
		return 1;
	}
	printf("free error ???????\n");
	return -1;
}