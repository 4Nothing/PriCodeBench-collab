# Makefile for Task 107

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I../../include/perf -I.
LDFLAGS =

SRC = test_PRT_HwiSetAttr.c test_stubs.c notifier_test_stubs.c
TARGET = test_PRT_HwiSetAttr

all: $(TARGET)
	ln -sf $(TARGET) test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) test *.o

.PHONY: all run clean
