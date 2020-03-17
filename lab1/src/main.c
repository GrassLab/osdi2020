#include "uart.h"
#include "shell.h"

int main(){
    char buf[256];
    char check;
    int i = 0;

    uart_init();
    uart_puts("-----------------------------------------\n");
    uart_puts("|               welcome!!!              |\n");
    uart_puts("-----------------------------------------\n");
    uart_puts("\n");
	uart_puts("⣿⣿⣟⣽⣿⣿⣿⣿⣟⣵⣿⣿⣿⡿⣳⣫⣾⣿⣿⠟⠻⣿⣿⣿⢻⣿⣿⣿⣿⣷⡽⣿\n");
	uart_puts("⣿⣟⣾⡿⣿⣿⢟⣽⣿⣿⣿⣿⣫⡾⣵⣿⣿⣿⠃⠄⠄⠘⢿⣿⣾⣿⣿⣿⢿⣿⣿⡜\n");
	uart_puts("⡿⣼⡟⣾⣿⢫⣿⣿⣿⣿⡿⣳⣿⣱⣿⣿⣿⡋⠄⠄⠄⠄⠄⠛⠛⠋⠁⠄⠄⣿⢸⣿\n");
	uart_puts("⢳⣟⣼⡿⣳⣿⣿⣿⣿⡿⣹⡿⣃⣿⣿⣿⢳⠁⠄⠄⠄⢀⣀⠄⠄⠄⠄⠄⢀⣿⢿⣿\n");
	uart_puts("⡟⣼⣿⣱⣿⡿⣿⣿⣿⢡⣫⣾⢸⢿⣿⡟⣿⣶⡶⢰⣿⣿⣿⢷⠄⠄⠄⠄⢼⣿⣸⣿\n");
	uart_puts("⣽⣿⢣⣿⡟⣽⣿⣿⠃⣲⣿⣿⣸⣷⡻⡇⣿⣿⢇⣿⣿⣿⣏⣎⣸⣦⣠⡞⣾⢧⣿⣿\n");
	uart_puts("⣿⡏⣿⡿⢰⣿⣿⡏⣼⣿⣿⡏⠙⣿⣿⣤⡿⣿⢸⣿⣿⢟⡞⣰⣿⣿⡟⣹⢯⣿⣿⣿\n");
	uart_puts("⡿⢹⣿⠇⣿⣿⣿⣸⣿⣿⣿⣿⣦⡈⠻⣿⣿⣮⣿⣿⣯⣏⣼⣿⠿⠏⣰⡅⢸⣿⣿⣿\n");
	uart_puts("⡀⣼⣿⢰⣿⣿⣇⣿⣿⡿⠛⠛⠛⠛⠄⣘⣿⣿⣿⣿⣿⣿⣶⣿⠿⠛⢾⡇⢸⣿⣿⣿\n");
    uart_puts("⠄⣿⡟⢸⣿⣿⢻⣿⣿⣷⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⡋⠉⣠⣴⣾⣿⡇⣸⣿⣿⡏\n");
	uart_puts("⠄⣿⡇⢸⣿⣿⢸⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠘⢿⣿⠏⠄⣿⣿⣿⣹\n");
	uart_puts("⠄⢻⡇⢸⣿⣿⠸⣿⣿⣿⣿⣿⣿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣦⣼⠃⠄⢰⣿⣿⢯⣿\n");
	uart_puts("⠄⢸⣿⢸⣿⣿⡄⠙⢿⣿⣿⡿⠁⠄⠄⠄⠄⠉⣿⣿⣿⣿⣿⣿⡏⠄⢀⣾⣿⢯⣿⣿\n");
	uart_puts("⣾⣸⣿⠄⣿⣿⡇⠄⠄⠙⢿⣀⠄⠄⠄⠄⠄⣰⣿⣿⣿⣿⣿⠟⠄⠄⣼⡿⢫⣻⣿⣿\n");
	uart_puts("⣿⣿⣿⠄⢸⣿⣿⠄⠄⠄⠄⠙⠿⣷⣶⣤⣴⣿⠿⠿⠛⠉⠄⠄ ⢸⣿⣿⣿⣿⠃⠄\n");
	uart_puts("\n");
    uart_puts("# ");

    // check whether the first char is displayable
    check = uart_getc();
    if (check > 31 && check < 127){
        buf[0] = check;
        uart_send(buf[0]);
        i++;
    }
    else if (check == '\n'){
        uart_send('\r');
        uart_send('\n');
        uart_puts("# ");
    }

    while(1){
        buf[i] = uart_getc();
        uart_send(buf[i]);
        if (buf[i] == '\n' || buf[i] == '\r'){
            uart_send('\r');
            if (i != 0) run(buf);
            uart_puts("# ");

            i = 0;
            continue;
        }

        i++;
    }
}
