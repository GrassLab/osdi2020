#include "buildin.h"
#include "kernel.h"
#include "string.h"
#include "uart.h"
void help();
struct command {
	char *name;
	char *descript;
	void (*func)(char *argv);
};
struct command buildin_cmds[] = {
	{ .name = "help", .descript = "get help", .func = help },
	{ .name = "hello", .descript = "print hello world", .func = hello },
	{ .name = "reset", .descript = "reset raspberry pi3", .func = reset },
	{ .name = "cancel_reset",
	  .descript = "cancel reset",
	  .func = cancel_reset },
	{ .name = "info", .descript = "list System info", .func = print_info },
	{ .name = "timestamp",
	  .descript = "print timestamp",
	  .func = getTimestamp },
	{ .name = NULL, .descript = NULL, .func = NULL }
};

void tty(int argc, char **argv)
{
}

void cmd(char *line)
{
	struct command *tmp = buildin_cmds;
	while (tmp->name != NULL) {
		if (strcmp(line, tmp->name) == 0) {
			tmp->func(line);
			return;
		}
		tmp++;
	}
	printk("%s: command not found\n", line);
	return;
}
void help()
{
	struct command *tmp = buildin_cmds;
	printk("usage:\n");
	while (tmp->name != NULL) {
		printk("\t%s\r\t\t\t%s\n", tmp->name, tmp->descript);
		tmp++;
	}
}

struct pos {
	int x;
	int y;
};

struct input {
	char *buf; // current buffer size
	struct pos current_pos; // current cursor pos
	int len; // length of input
};

char *delete (struct input line, int index)
{
	char *buf = (line.buf + index);
	while (*buf != '\0') {
		*buf = *(buf + 1);
		buf++;
	}
	return buf;
}

char *readline(char *buf)
{
	struct input line = { .buf = buf,
			      .current_pos = { .x = 0, .y = 0 },
			      .len = 0 }; // initial line buf
	char ctrl[3] = { 0x1B, 0x5B, 0x41 };
	char tmp;
	while ((tmp = uart_getc()) != '\n') {
		// printk("recv: %d", tmp);
		switch (tmp) {
		case 0x1B:
			if (uart_getc() != 0x5B) // not our targeted control
				continue;
			switch ((ctrl[2] = uart_getc())) {
			case 0x41: // up
				break; //  do nothing for now
			case 0x42: // down
				break; // do nothing for now
			case 0x43: // right
				if (line.len == line.current_pos.x) {
					break;
				}
				line.current_pos.x++;
				printk(ctrl);
				break;
			case 0x44: // left
				if (line.current_pos.x == 0) {
					break;
				}
				line.current_pos.x--;
				printk(ctrl);
				break;
			}
			break;
		case 0x7F: //  backspace
			// break; //  pass this function for now
			if (line.current_pos.x > 0) {
				buf = delete (line, --line.current_pos.x);
				line.len--;
				printk("\b%s ", line.buf + line.current_pos.x);
				for (int cursor = line.len;
				     cursor >= line.current_pos.x; cursor--) {
					printk("\b");
				}
			}
			break;
		case 0x09:
			uart_puts("tabed");
			break;
		default:
			line.len++;
			line.current_pos.x++;
			*(buf++) = tmp;
			printk("%c", tmp);
		}
	}
	*buf = '\0';
	return line.buf;
}