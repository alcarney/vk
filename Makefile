CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lvulkan

SOURCES=$(shell find ./src -name "*.c")
OBJECTS=$(SOURCES:.c=.o)

default: vkdevice

.PHONY: clean
clean:
	rm src/*.o bin/*

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@

vkdevice: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o bin/vkdevice