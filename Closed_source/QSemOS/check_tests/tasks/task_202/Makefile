# Makefile for task_202

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I. -D_GNU_SOURCE
LDFLAGS =

SRC = test_OsLog.c
TARGET = test

all: $(TARGET)

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
