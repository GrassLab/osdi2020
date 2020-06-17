TARGET = buddy

all : $(TARGET)

buddy: buddy.o list.o
	gcc -lm $^ -o $@

$%.o: %.c
	gcc -lm -c $< -o $@

clean :
	rm *.o $(TARGET)
