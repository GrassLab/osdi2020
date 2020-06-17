#include "mm.h"
#include "schedule.h"
#include "string.h"
#include "uart.h"

unsigned long PFN(unsigned long i)
{
	unsigned long in = i;
	in = in - (unsigned long)(&page);
	in = (in & 0x0000fffffffff000) >> (4 * 3);
	return in;
}

int getorder(unsigned long size)
{
	int sh = size >> 12;
	int order = 0;
	while (sh > 1)
	{
		sh = sh / 2;
		order++;
	}
	return order;
}

void init_buddy()
{
	for (int i = 0; i < 10; i++)
	{
		page_order[i].next = NULL;
	}
	for (int i = 0; i < 1024; i++)
	{
		page_use[i].order = -1; // set all order = -1
		page_use[i].next = NULL;
		page_use[i].pad = page[i];
	}
	page_order[10].next = &page_use[0];
	printf("start = %x\n", page_order[10].next);
}

void order_add(int order, struct page_struct *add_page)
{
	struct page_struct *now = page_order[order].next;
	if (now == NULL)
	{
		page_order[order].next = add_page;
	}
	else
	{
		struct page_struct *cr = now;
		while (cr != NULL)
		{
			now = cr;
			cr = cr->next;
		}
	}
	add_page->next = NULL;
}

unsigned long page_alloc(unsigned long size)
{
	int order = getorder(size);
	if (order > 10)
	{
		printf("out of range\n");
		return -1;
	}
	int pgframe = get_free_page(order);
	if (pgframe == -1)
	{
		return -1;
	}
	unsigned long page_addr = (unsigned long)&page[pgframe];
	return page_addr;
}

int get_free_page(int order)
{
	int or = -1;
	for (int i = order; i < 12; i++)
	{
		if (i == 12)
		{
			printf("dont have enough mem");
			return -1;
		}
		if (page_order[i].next != NULL)
		{
			or = i;
			break;
		}
	}

	int fcount = 1 << order;
	printf("want order = %d, get max order = %d, want frame count = %d\n", order,
		   or, fcount);

	struct page_struct *split_page = page_order[or].next;
	page_order[or].next = split_page->next; // remove linklist

	for (int i = or -1; i >= order; i--)
	{
		struct page_struct *add = split_page + (1 << i);
		printf("split %d to order %d\n", PFN((unsigned long)add->pad), i);
		order_add(i, add);
	}

	int index = PFN((unsigned long)split_page->pad);

	for (int i = index; i < index + fcount; i++)
	{
		page_use[i].used = 1;
		page_use[i].order = order;
	}
	printf("get page index = %d, frame use addr = %x , index order = %d\n", index,
		   &page_use[index], page_use[index].order);
	return index;
}

unsigned long page_free(unsigned long sp)
{
	unsigned long index = PFN(sp);
	int free_order = page_use[index].order;
	int find_buddy;
	printf("free pfn = %d, order = %d\n", index, free_order);

	struct page_struct *head = &page_use[index];

	for (int i = free_order; i < 11; i++)
	{
		printf("search order %d\n", i);
		int check = 0;

		find_buddy = PFN((unsigned long)head->pad) ^ (1 << free_order);
		printf("buddy = %d\n", find_buddy);

		struct page_struct *buddy_page = &page_use[find_buddy];

		if (page_order[i].next == NULL)
		{
			printf("empty\n");
			break;
		}
		else
		{
			struct page_struct *c = &page_order[i];
			for (struct page_struct *now = page_order[i].next; now != NULL;
				 now = now->next)
			{

				printf("search\n");
				printf("now = %x pfn = %d , buddy = %x pfn = %d\n", now,
					   PFN((unsigned long)now->pad), buddy_page,
					   PFN((unsigned long)buddy_page->pad));
				if (now == buddy_page)
				{
					printf("=====merge ======\n");
					c->next = now->next;
					if (PFN((unsigned long)now->pad) < PFN((unsigned long)head->next))
					{
						printf("change head\n");
						head = now;
						head->order++;
					}
					free_order++;
					check = 1;
					break;
				}
				c = now;
			}
			if (check == 1)
			{
				printf("end merge\n");
				break;
			}
		}
	}

	head->next = page_order[free_order].next;
	page_order[free_order].next = head;
	int fcount = 1 << head->order;
	printf("add f = %d index %d in  order %d, fcount = %d\n",
		   PFN((unsigned long)head->pad), index, free_order, fcount);
	for (int i = 0; i < fcount; i++)
	{
		page_use[i].used = 0;
	}

	return 0;
}