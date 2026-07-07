CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -Werror -fstack-protector-strong -g

.PHONY: all clean test

BUILDDIR = build
TEST_EXE = $(BUILDDIR)/test_executable

all: $(BUILDDIR)/cube

$(BUILDDIR)/cube: cube.c
	mkdir -p $(BUILDDIR)
	$(CC) -DN=5 $(CFLAGS) $< -o $@

$(TEST_EXE): cube.c
	mkdir -p $(BUILDDIR)
	$(CC) -DN=23 $(CFLAGS) $< -o $@

clean:
	if [ -f $(TEST_EXE) ]; then rm $(TEST_EXE); fi
	find tests -name "*.t" -delete

test: $(TEST_EXE) test.sh tests
	./test.sh
