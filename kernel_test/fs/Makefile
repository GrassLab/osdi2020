.PHONY: all 

all: test

test: tests/test.c vfs.c tmpfs.c
	gcc tests/test.c vfs.c tmpfs.c -o test

clean:
	rm test
