CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc
LIBS = -lncurses
VPATH = src
TARGET = rtgrep
SOURCES = rtgrep.c line_list.c arguments.c
OBJECTS = $(addprefix src/,$(SOURCES:.c=.o))

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

TEST_TARGET = test_runner
TEST_SOURCES = test/test_root.c test/test_utils.c test/line_list_tests.c test/arguments_tests.c src/line_list.c src/arguments.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJECTS)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)
	install -d $(MANDIR)
	install -m 644 man/rtgrep.1 $(MANDIR)

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -f $(MANDIR)/rtgrep.1

clean:
	rm -f $(TARGET) $(OBJECTS) $(TEST_TARGET) $(TEST_OBJECTS)

.PHONY: clean test install uninstall
