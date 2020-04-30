#ifndef UTILITY_H
#define UTILITY_H
#define GET_TIMESTAMP 0x105
void printPowerOnMessage();
void printDebugPowerOnMessage();
void setRegister(unsigned int address, unsigned int value);
unsigned int getRegister(unsigned int address);
void reboot(int tick);
void getTimestamp();
void memset(void* mem, int value, int size);
int pow(int num, int p);
#endif