# Makefile for Task 145

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I.
LDFLAGS =

SRC = test_PRT_SplIrqLock.c notifier_test_stubs.c
TARGET = test_bin

all: $(TARGET)
	ln -sf $(TARGET) test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) test *.o

.PHONY: all run clean
