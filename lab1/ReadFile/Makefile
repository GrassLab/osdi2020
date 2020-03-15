SRCS = $(wildcard *.c)
OBJS = $(SRCS: .c=.o)
all: readf.o

%.o : %.c
	gcc -std=c99 -c $< -o $@

readf.o : $(OBJS)
	gcc -std=c99 -o  readf.o $(OBJS)

clean:
	rm *.o

run:
	sudo ./readf.o
