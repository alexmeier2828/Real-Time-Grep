CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lncurses
TARGET = rtgrep
SOURCES = rtgrep.c line_list.c arguments.c
OBJECTS = $(SOURCES:.c=.o)

TEST_TARGET = test_runner
TEST_SOURCES = test/test_root.c test/test_utils.c test/line_list_tests.c test/arguments_tests.c line_list.c arguments.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJECTS)

clean:
	rm -f $(TARGET) $(OBJECTS) $(TEST_TARGET) $(TEST_OBJECTS)

.PHONY: clean test
