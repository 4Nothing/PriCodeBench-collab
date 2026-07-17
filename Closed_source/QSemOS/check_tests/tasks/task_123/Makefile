# Makefile for Task 123

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I../../common -I.
LDFLAGS =

SRC = test_OsIdleTaskExe.c test_stubs.c notifier_test_stubs.c
TARGET = test_OsIdleTaskExe

all: $(TARGET)
	ln -sf $(TARGET) test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f test $(TARGET) *.o

.PHONY: all run clean