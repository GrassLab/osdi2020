extern volatile unsigned int mailbox[36];

int mailbox_call(unsigned char);
void get_board_revision(void);
void get_vc_memory(void);
void set_clock_rate(void);
