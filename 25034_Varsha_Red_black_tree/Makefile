CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
TARGET = red_black_tree
SOURCES = src/main.c src/insert_search.c src/delete.c src/min_max.c src/display_validate.c src/rbtree_utils.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).exe

.PHONY: all clean
