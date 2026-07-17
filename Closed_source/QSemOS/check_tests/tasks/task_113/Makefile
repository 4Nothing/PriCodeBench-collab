# Makefile for task_113
# Mock paths first, then src paths for src code dependencies

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I../../common -I.
LDFLAGS =

TESTS = test_PRT_Perf_Integration test_PRT_PerfHooks test_OsPerfStop test_OsPerfSetIrqRegs

all: $(TESTS)
	ln -sf test_OsPerfStop test

test_PRT_Perf_Integration: test_PRT_Perf_Integration.c test_stubs.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(shell cat test_PRT_Perf_Integration.cflags 2>/dev/null) -o $@ test_PRT_Perf_Integration.c test_stubs.c

test_PRT_PerfHooks: test_PRT_PerfHooks.c test_stubs.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(shell cat test_PRT_PerfHooks.cflags 2>/dev/null) -o $@ test_PRT_PerfHooks.c test_stubs.c

test_OsPerfStop: test_OsPerfStop.c test_stubs.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(shell cat test_OsPerfStop.cflags 2>/dev/null) -o $@ test_OsPerfStop.c test_stubs.c

test_OsPerfSetIrqRegs: test_OsPerfSetIrqRegs.c test_stubs.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(shell cat test_OsPerfSetIrqRegs.cflags 2>/dev/null) -o $@ test_OsPerfSetIrqRegs.c test_stubs.c

run: all
	@for t in $(TESTS); do echo "=== Running $$t ==="; ./$$t; echo; done

clean:
	rm -f $(TESTS) *.o

.PHONY: all run clean