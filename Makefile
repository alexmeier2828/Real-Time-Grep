CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lncurses
TARGET = rtgrep
SOURCES = rtgrep.c line_list.c
OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: clean
