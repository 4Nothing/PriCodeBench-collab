# Makefile for Task 170

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O0 -I../../include -I.
LDFLAGS =

SRC = test_OsGetSrcPid.c notifier_test_stubs.c
TARGET = test_OsGetSrcPid
LINK = test

all: $(LINK)

$(TARGET): $(SRC) func_under_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)

$(LINK): $(TARGET)
	rm -f $(LINK); ln -sf $(TARGET) $(LINK)

run: $(LINK)
	./$(LINK)

clean:
	rm -f $(TARGET) $(LINK) *.o

.PHONY: all run clean
