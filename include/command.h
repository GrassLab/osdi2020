#ifndef COMMAND_H
#define COMMAND_H

void input_buffer_overflow_message ( char[] );

void command_help ( );
void command_hello ( );
void command_timestamp ( );
void command_not_found ( char * );
void command_reboot ( );
void command_vc_base_addr ( );
void command_board_revision ( );

#endif