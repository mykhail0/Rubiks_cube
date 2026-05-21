CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -Werror -fstack-protector-strong -g

.PHONY: all clean test

all: bin/cube

bin/cube: cube.c bin
	$(CC) -DN=5 $(CFLAGS) $< -o $@

bin:
	mkdir -p bin

clean:
	if [ -d "bin" ]; then rm bin/*; fi
	find tests -name "*.t" -delete

test: bin/test_executable test.sh tests
	./test.sh

bin/test_executable: cube.c
	$(CC) -DN=23 $(CFLAGS) $< -o $@
