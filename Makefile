CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lvulkan

.PHONY: clean default

default: vkdevice

clean:
	rm src/*.o bin/*

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

VKDEVICE = src/vkdevice.o
vkdevice: $(VKDEVICE)
	$(CC) $(LDFLAGS) $(VKDEVICE) -o bin/vkdevice