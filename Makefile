CC = gcc
CFLAGS = -O3 -Wall -Wextra -pedantic -std=c11 -I./include
LDFLAGS = -lm

# Enable ARM NEON instructions
CFLAGS += -march=armv8-a+simd

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests
EXAMPLES_DIR = examples

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Examples
EXAMPLE_SRCS = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_BINS = $(patsubst $(EXAMPLES_DIR)/%.c,$(BIN_DIR)/%,$(EXAMPLE_SRCS))

# Tests
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(TEST_SRCS))

# Phony targets
.PHONY: all clean examples tests dirs

# Default target
all: dirs $(OBJS) examples tests

# Create directories
dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build examples
examples: dirs $(EXAMPLE_BINS)

$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(LDFLAGS)

# Build tests
tests: dirs $(TEST_BINS)

$(BIN_DIR)/%: $(TEST_DIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(LDFLAGS)

# Run a specific test
test: dirs
	@if [ -z "$(TEST_NAME)" ]; then \
		echo "Error: TEST_NAME not specified"; \
		echo "Usage: make test TEST_NAME=<test_name>"; \
		exit 1; \
	fi
	@if [ ! -f "$(BIN_DIR)/$(TEST_NAME)" ]; then \
		$(CC) $(CFLAGS) $(EXAMPLES_DIR)/$(TEST_NAME).c $(OBJS) -o $(BIN_DIR)/$(TEST_NAME) $(LDFLAGS); \
	fi
	@echo "Running test: $(TEST_NAME)"
	@$(BIN_DIR)/$(TEST_NAME)

# Format code with clang-format
format:
	@echo "Formatting source files..."
	@clang-format -i $(SRC_DIR)/*.c $(INCLUDE_DIR)/*.h $(EXAMPLES_DIR)/*.c $(TEST_DIR)/*.c

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)