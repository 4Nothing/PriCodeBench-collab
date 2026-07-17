# Makefile for Task 104

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I../../include/perf -I.
LDFLAGS =

SRC = test_OsHwiHookDispatcher.c test_stubs.c notifier_test_stubs.c
TARGET = test_OsHwiHookDispatcher

all: $(TARGET)
	ln -sf $(TARGET) test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
