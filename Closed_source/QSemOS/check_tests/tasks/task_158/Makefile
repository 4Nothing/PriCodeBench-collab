# Makefile for Task 158

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -finline-functions -I. -I../../include
LDFLAGS =

SRC = test_OsResortSemList.c test_stubs.c notifier_test_stubs.c
TARGET = test_OsResortSemList

all: $(TARGET)
	rm -f test; ln -sf $(TARGET) test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f test_OsResortSemList test *.o

.PHONY: all run clean
