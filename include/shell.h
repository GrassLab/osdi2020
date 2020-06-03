struct shell_command {
    char name[0x10];
    void (*func); 
};

struct shell_command shell_command_list[0x10];
void shell();