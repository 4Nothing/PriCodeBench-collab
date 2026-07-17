# Makefile for task_026

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I.
LDFLAGS =

TESTS = test_OsSwTmrResInit test_OsSwTmrCreateTimer_Extra

all: test

test: test_OsSwTmrResInit
	ln -sf test_OsSwTmrResInit test

test_OsSwTmrResInit: test_OsSwTmrResInit.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

test_OsSwTmrCreateTimer_Extra: test_OsSwTmrCreateTimer_Extra.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

run: all
	@for t in $(TESTS); do echo "=== Running $$t ==="; ./$$t; echo; done

clean:
	rm -f $(TESTS) *.o test

.PHONY: all run clean
