#ifndef UTILITY_H
#define UTILITY_H

void printPowerOnMessage();
void printDebugPowerOnMessage();
void setRegister(unsigned int address, unsigned int value);
unsigned int getRegister(unsigned int address);
void reboot(int tick);
double getTimestamp();
void memset(void* mem, int value, int size);
int pow(int num, int p);
#endif