# Makefile for Task 089

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O0 -fno-inline -fkeep-inline-functions -I../../include -I../../include/perf -I.
LDFLAGS =

SRC = test_PRT_SignalWait.c notifier_test_stubs.c stub_prt_signal_env.c test_stubs.c
TARGET = test_PRT_SignalWait

all: $(TARGET)
	ln -sf $(TARGET) test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) test *.o

.PHONY: all run clean
