# Makefile for task_204

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I. -include log_decl.h
LDFLAGS =

SRC = test_OsLogLockOn.c log_lock_stub.c
TARGET = test

all: $(TARGET)

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
