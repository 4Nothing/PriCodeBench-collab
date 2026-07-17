# Makefile for Task 150

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I.
LDFLAGS =

TARGET = test

all: $(TARGET)

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
SRC = test_PRT_SplIrqWriteUnlock.c notifier_test_stubs.c
