#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#define LOADIMG_TEMP_LOCATION 0x40000

void bootloader_shell(void);
void bootloader_shell_parser();
int bootloader_shell_help(char * string_buffer);
int bootloader_shell_loadimg(char * string_buffer);

#endif

