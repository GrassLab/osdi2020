.PHONY: all 

all: test

test: test.c vfs.c tmpfs.c
	gcc test.c vfs.c tmpfs.c -o test

clean:
	rm test
