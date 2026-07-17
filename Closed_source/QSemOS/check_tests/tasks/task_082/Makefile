# Makefile for Task 082

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I../../include/perf -I.
LDFLAGS =

SRC = test_OsSignalTimeOutSet.c stub_prt_signal_env.c
TARGET = test_OsSignalTimeOutSet

all: $(TARGET)
	ln -sf $(TARGET) test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) test *.o

.PHONY: all run clean
