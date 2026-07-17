# Makefile for task_110
# Mock paths first, then src paths for src code dependencies

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I../../common -I.
LDFLAGS =

TESTS = test_PRT_Perf_Integration test_PRT_PerfHooks test_OsPerfParamValid test_OsPerfSetIrqRegs

all: $(TESTS)

# Helper to read cflags from .cflags files
define get_cflags
	$(shell [ -f $1.cflags ] && cat $1.cflags || true)
endef

# Compile stubs with test-specific cflags
test_PRT_Perf_Integration_stubs.o: test_stubs.c
	$(CC) $(CFLAGS) $(call get_cflags,test_PRT_Perf_Integration) -c -o $@ $<

test_OsPerfParamValid_stubs.o: test_stubs.c
	$(CC) $(CFLAGS) $(call get_cflags,test_OsPerfParamValid) -c -o $@ $<

test_PRT_PerfHooks_stubs.o: test_stubs.c
	$(CC) $(CFLAGS) -c -o $@ $<

test_OsPerfSetIrqRegs_stubs.o: test_stubs.c
	$(CC) $(CFLAGS) -c -o $@ $<

test_PRT_Perf_Integration: test_PRT_Perf_Integration.c test_PRT_Perf_Integration_stubs.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test_PRT_Perf_Integration.c test_PRT_Perf_Integration_stubs.o

test_PRT_PerfHooks: test_PRT_PerfHooks.c test_PRT_PerfHooks_stubs.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test_PRT_PerfHooks.c test_PRT_PerfHooks_stubs.o

test_OsPerfParamValid: test_OsPerfParamValid.c test_OsPerfParamValid_stubs.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test_OsPerfParamValid.c test_OsPerfParamValid_stubs.o

test_OsPerfSetIrqRegs: test_OsPerfSetIrqRegs.c test_OsPerfSetIrqRegs_stubs.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test_OsPerfSetIrqRegs.c test_OsPerfSetIrqRegs_stubs.o

all: $(TESTS) test

test: test_PRT_Perf_Integration test_PRT_PerfHooks test_OsPerfParamValid test_OsPerfSetIrqRegs
	ln -sf test_OsPerfParamValid test

run: all
	@for t in $(TESTS); do echo "=== Running $$t ==="; ./$$t; echo; done

clean:
	rm -f $(TESTS) test *_stubs.o *.o

.PHONY: all run clean test
