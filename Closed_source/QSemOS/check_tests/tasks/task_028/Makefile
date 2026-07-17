# Makefile for task_028

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I.
LDFLAGS =

TESTS = test_OsSwtmrTargetCheck test_OsSwTmrCreateTimer_Extra

all: $(TESTS)
	ln -sf test_OsSwtmrTargetCheck test

test_OsSwtmrTargetCheck: test_OsSwtmrTargetCheck.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

test_OsSwTmrCreateTimer_Extra: test_OsSwTmrCreateTimer_Extra.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

run: all
	@for t in $(TESTS); do echo "=== Running $$t ==="; ./$$t; echo; done

clean:
	rm -f $(TESTS) *.o test

.PHONY: all run clean
