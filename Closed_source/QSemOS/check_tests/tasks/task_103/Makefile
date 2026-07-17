# Makefile for Task 103

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I.
LDFLAGS =

SRC = test_OsHwiDisableAll.c test_stubs.c notifier_test_stubs.c
EXEC = test_run
SYMLINK = test

all: $(SYMLINK)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(EXEC) $(SRC)

$(SYMLINK): $(EXEC)
	rm -f $(SYMLINK) && ln -s $(EXEC) $(SYMLINK)

run: $(SYMLINK)
	./$(SYMLINK)

clean:
	rm -f $(EXEC) $(SYMLINK) *.o

.PHONY: all run clean
