CC = gcc
CFLAGS = -Wall -Iinclude

SRC = $(wildcard src/*.c)
TARGET = smart_editor

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)