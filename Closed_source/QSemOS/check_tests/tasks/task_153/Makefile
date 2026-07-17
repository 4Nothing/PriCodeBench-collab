# Makefile for Task 153

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I.
LDFLAGS =

SRC = test_PRT_SplUnlock.c stubs.c notifier_test_stubs.c
TARGET = test_PRT_SplUnlock
SYMLINK = test

all: $(SYMLINK)

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

$(SYMLINK): $(TARGET)
	ln -sf $(TARGET) $(SYMLINK)

run: $(SYMLINK)
	./$(SYMLINK)

clean:
	rm -f $(TARGET) $(SYMLINK) *.o

.PHONY: all run clean
