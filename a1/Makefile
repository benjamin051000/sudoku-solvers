# Mostly from makefiletutorial.com
EXEC := sudoku
CFLAGS := -O3 -Wall -Wextra -Wunused -Wshadow -pedantic -Wwrite-strings -Wunused-value
ARGS := "sudoku1.txt"

BUILD_DIR := ./build
SRC_DIRS := .

# Find C files to compile
# Note the single quotes around the * expressions. Make will incorrectly expand these otherwise.
SRCS := $(shell find $(SRC_DIRS) -name '*.c')

# String substitution for every C file.
# For example, hello.c turns into ./build/hello.o
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)

# The final build step.
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Build step for C source
$(BUILD_DIR)/%.o: $(SRCS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean run
clean:
	rm -rf $(BUILD_DIR) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

