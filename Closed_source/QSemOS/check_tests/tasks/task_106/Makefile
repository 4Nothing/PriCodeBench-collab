# Makefile for Task 106

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I../../include/perf -I.
LDFLAGS =

SRC = test_PRT_HwiDelete.c test_stubs.c notifier_test_stubs.c
TARGET = test_PRT_HwiDelete

all: $(TARGET)
	ln -sf test_PRT_HwiDelete test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f test $(TARGET) *.o

.PHONY: all run clean
