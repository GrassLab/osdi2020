#include "mbox.h"
#include "utils.h"
#include "uart.h"

#define MBOX_CH_PROP    8

#define GET_BOARD_REVISION  0x00010002
#define GET_VC_CORE_BASE	0x00010006

#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000


unsigned int __attribute__((aligned(16))) mailbox[8];
char buff[10];

void get_board_revision(){
	mailbox[0] = 7 * 4; // buffer size in bytes
	mailbox[1] = REQUEST_CODE;
	// tags begin
	mailbox[2] = GET_BOARD_REVISION; // tag identifier
	mailbox[3] = 4; // maximum of request and response value buffer's length.
	mailbox[4] = TAG_REQUEST_CODE;
	mailbox[5] = 0; // value buffer
	// tags end
	mailbox[6] = END_TAG;
	mailbox_call(mailbox, MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
	print("board revision: ");
	unsigned int num=mailbox[5];
	int i;
	for(i=0;num!=0;i++){
		buff[i]=(num%16<10)?(num%16+'0'):(num%16-10+'A');
		num/=16;
	}
	while(i>0){
		--i;
		uart_write(buff[i]);
	}
	print("\n\r");

}

void get_VCCore_base_address(){
	mailbox[0] = 8 * 4; // buffer size in bytes
	mailbox[1] = REQUEST_CODE;
	// tags begin
	mailbox[2] = GET_VC_CORE_BASE; // tag identifier
	mailbox[3] = 8; // maximum of request and response value buffer's length.
	mailbox[4] = TAG_REQUEST_CODE;
	mailbox[5] = 0; // value buffer
	// tags end
	mailbox[6] = 0;
	mailbox[7] = END_TAG;
	mailbox_call(mailbox, MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
	print("VC core base address: ");
	unsigned int num=mailbox[5];
	int i;
	for(i=0;num!=0;i++){
		buff[i]=(num%16<10)?(num%16+'0'):(num%16-10+'A');
		num/=16;
	}
	while(i>0){
		--i;
		uart_write(buff[i]);
	}
	print("\n\r");
	
}

void hard_info(){
	get_board_revision();
	get_VCCore_base_address();
}
