# Makefile for Task 101

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I.
LDFLAGS =

SRC = test_OsHwiDescFieldInit.c test_stubs.c notifier_test_stubs.c
TARGET = test_OsHwiDescFieldInit

all: $(TARGET)
	ln -sf test_OsHwiDescFieldInit test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f test $(TARGET) *.o

.PHONY: all run clean
