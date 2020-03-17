#ifndef CMD_H
#define CMD_H

#include "type.h"

bool strcmp(const char* s1, const char* s2);
command_t checkCmdType(const char *input);
void helpCmd();
void helloCmd();
void noneCmd(const char* input);
void processCmd(const char* input);

#endif