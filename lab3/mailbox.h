#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000
#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY   0x00010006
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

extern volatile unsigned int __attribute__((aligned(16))) mailbox[36];
int mailbox_call();

void get_board_revision();
void get_vc_memory();


