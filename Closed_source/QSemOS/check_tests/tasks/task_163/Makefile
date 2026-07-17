# Makefile for Task 163

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I. -I../../include
LDFLAGS =

SRC = test_OsSemPostErrorCheck.c test_stubs.c notifier_test_stubs.c
TARGET = test_OsSemPostErrorCheck
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