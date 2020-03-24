SRCS = $(wildcard *.c)
OBJS = $(SRCS: .c=.o)
all: final_compi.o

%.o : %.c
	gcc -std=c99 -c $< -o $@

final_compi.o : $(OBJS)
	gcc -std=c99 -o  final_compi.o $(OBJS)

clean:
	rm *.o

run:
	sudo ./final_compi.o