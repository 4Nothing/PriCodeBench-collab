# Makefile for Task 161

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I. -I../../include
LDFLAGS =

SRC = test_OsSemPendNotNeedSche.c test_stubs.c notifier_test_stubs.c
TARGET = test

all: $(TARGET)

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
