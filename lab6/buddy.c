#include "mystd.h"
#include "buddy.h"
#include "uart.h"

buddy_head buddy[BUDDY_MAX+1];
buddy_allocated buddy_allocated_list;

void show_alloc_message(buddy_node node){
	char buf[16] = {0};

	uart_puts("[Buddy system] allocate memory PFN ( from ");
	ullToStr(node.start, buf);
	uart_puts(buf);
	uart_puts(" to ");
	ullToStr(node.end, buf);
	uart_puts(buf);
	uart_puts(" )\n");
}

void show_free_message(buddy_node node){
	char buf[16] = {0};

	uart_puts("[Buddy system] free memory PFN ( from ");
	ullToStr(node.start, buf);
	uart_puts(buf);
	uart_puts(" to ");
	ullToStr(node.end, buf);
	uart_puts(buf);
	uart_puts(" )\n");
}


void show_merge_message(buddy_node node1, buddy_node node2){
	char buf[16] = {0};

	uart_puts("[Buddy system] merge ( from ");
	ullToStr(node1.start, buf);
	uart_puts(buf);
	uart_puts(" to ");
	ullToStr(node1.end, buf);
	uart_puts(buf);
	uart_puts(" ) and ( from ");
	ullToStr(node2.start, buf);
	uart_puts(buf);
	uart_puts(" to ");
	ullToStr(node2.end, buf);
	uart_puts(buf);
	uart_puts(" )\n");
}

bool allocated_list_push(buddy_node node){
	if(buddy_allocated_list.num == BUDDY_ALLOCATED_NUM){
		uart_puts("allocated array full\n");
		return false;
	}

	node.allocated = 1;
	for(int i=0; i<BUDDY_ALLOCATED_NUM; i++){
		if(buddy_allocated_list.node_list[i].allocated == 0) {
			buddy_allocated_list.node_list[i] = node;
			show_alloc_message(node);
			break;
		}
	}

	buddy_allocated_list.num ++ ;
	return true;
}

buddy_node allocated_list_pop(int node_start){
	buddy_node ret_node;
	for(int i=0; i<BUDDY_ALLOCATED_NUM; i++){
		buddy_node *this = &buddy_allocated_list.node_list[i];
		if(this->allocated == 1 && this->start == node_start) {
			this->allocated = 0;
			ret_node = buddy_allocated_list.node_list[i];
			break;
		}
	}

	buddy_allocated_list.num -- ;
	return ret_node;
}

buddy_node buddy_list_pop(buddy_head *list){
	buddy_node ret_node = list->node_list[0];
	list->num -= 1;

	int step = list->num;
	int cur = 1;
	while(step != 0){
		list->node_list[cur-1] = list->node_list[cur];
		
		step--;
		cur++;
	}

	return ret_node;
}

// move node to next level
void buddy_list_push(buddy_head *list, buddy_node node){
	int size = (node.end - node.start + 1)/2;
	buddy_node temp;

	temp.start = node.start;
	temp.end = node.start + size - 1;
	list->node_list[ list->num ] = temp;
	list->num += 1;

	temp.start = node.start + size;
	temp.end = node.end;
	list->node_list[ list->num ] = temp;
	list->num += 1;
}

bool is_power_of_2(int index){
	return !(index & (index-1));
}

int next_power_of_2(int size){
	size -= 1;
	size |= (size>>1);
	size |= (size>>2);
	size |= (size>>4);
	size |= (size>>8);
	size |= (size>>16);
	return size + 1;
}

int log_2(int input){
	int num = 1;
	int power = 0;

	for(int i=0; i<BUDDY_MAX+1; i++){
		if(num == input) break;

		num = num << 1;
		power++;
	}

	return power;

}

void buddy_merge(int id, buddy_node node){
	int size = buddy[id].num;

	buddy_node next_node;
	int list_id;
	bool next_run = false;
	for(list_id=0; list_id<size; list_id++){
		if(buddy[id].node_list[list_id].end + 1 == node.start){
			next_node.start = buddy[id].node_list[list_id].start;
			next_node.end = node.end;
			next_run = true;
			break;
		}

		if(buddy[id].node_list[list_id].start == node.end + 1){
			next_node.start = node.start;
			next_node.end = buddy[id].node_list[list_id].end;
			next_run = true;
			break;
		}		
	}

	if(next_run){
		show_merge_message(buddy[id].node_list[list_id], node);

		if(size-1 == 0){
			buddy[id].num = 0;
		}else{
			buddy[id].node_list[list_id] = buddy[id].node_list[size];
			buddy[id].num -- ;
		}

		if(id != BUDDY_MAX){
			buddy_merge(id+1, next_node);
		}

	}else{
		buddy[id].node_list[size] = node;
		buddy[id].num++;
	}

}

void buddy_init(){
	for(int i=0; i<BUDDY_MAX+1; i++){
		buddy[i].num = 0;
	}

	buddy[BUDDY_MAX].num = 1;
	buddy_node first_node = {
		.start = 0,
		.end = PFN_NUM-1,
		.allocated = 1
	};
	buddy[BUDDY_MAX].node_list[0] = first_node;

	buddy_allocated_list.num = 0;
	for(int i=0; i<BUDDY_ALLOCATED_NUM; i++){
		(buddy_allocated_list.node_list[i]).allocated = 0;
	}

}

int buddy_alloc(int page_request_num){
	int alloc_num;
	if(is_power_of_2(page_request_num)) alloc_num = page_request_num;
	else alloc_num = next_power_of_2(page_request_num);

	int request_id = log_2(alloc_num);
	if(buddy[request_id].num > 0){
		buddy_node alloc_node = buddy_list_pop(&buddy[request_id]);
		allocated_list_push(alloc_node);
		return alloc_node.start;
	}

	int id;
	for(id = request_id+1; id < BUDDY_MAX+1; id++){
		if(buddy[id].num > 0) break;
	}

	if(id == BUDDY_MAX+1){
		uart_puts("fail to allocate memory\n");
		return -1;
	}

	buddy_node temp_node = buddy_list_pop(&buddy[id]);
	id--;
	while(id >= request_id){
		buddy_list_push(&buddy[id], temp_node);
		temp_node = buddy_list_pop(&buddy[id]);
		id--;
	}

	allocated_list_push(temp_node);
	return temp_node.start;
}

void buddy_free(int node_start){
	buddy_node node = allocated_list_pop(node_start);
	show_free_message(node);

	int size = node.end - node.start + 1;
	int id = log_2(size);

	node.allocated = 0;
	buddy_merge(id, node);
}