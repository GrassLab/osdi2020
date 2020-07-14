.PHONY: all 

all: test

test: tests/test.c buddy.c obj_allocator.c
	gcc tests/test.c buddy.c obj_allocator.c -o test

clean:
	rm test
