#include "nlibc.h"
#include "buildin.h"

int main()
{
	char buf[1024];
	while (1) {
		printf("njt@osdi2020# ");
		readline(buf);
		if (*buf) {
			printf("\n");
			cmd(buf);
		} else {
			printf("\n");
		}
	}
}

void exit()
{
}
