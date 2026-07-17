# Makefile for Task 080

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../../include -I.
LDFLAGS =

SRC = test_OsSigWaitProcPendingSignal.c stub_prt_signal_env.c
TARGET = test_OsSigWaitProcPendingSignal

all: $(TARGET)
	ln -sf $(TARGET) test

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) test *.o

.PHONY: all run clean
