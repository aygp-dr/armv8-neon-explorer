# ARM v8 NEON Explorer - Main Makefile

# Compiler settings
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O3 -g
ARCH_FLAGS = -march=armv8-a+simd
INCLUDE = -Iinclude
LIBS = -lm

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
EXAMPLES_DIR = examples
TESTS_DIR = tests
TOOLS_DIR = tools
DOCS_DIR = docs

# Files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
LIB = $(BUILD_DIR)/libneon_explorer.a

# Examples
EXAMPLE_SRCS = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_BINS = $(patsubst $(EXAMPLES_DIR)/%.c,$(BIN_DIR)/%,$(EXAMPLE_SRCS))

# Include platform-specific settings
include Makefile.platforms

# Default target
all: setup $(LIB) examples tests tools

# Setup directories
setup:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(ARCH_FLAGS) $(INCLUDE) $(PLATFORM_CFLAGS) -c $< -o $@

# Build static library
$(LIB): $(OBJS)
	ar rcs $@ $^

# Build examples
examples: $(EXAMPLE_BINS)

$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.c $(LIB)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(ARCH_FLAGS) $(INCLUDE) $(PLATFORM_CFLAGS) $< $(LIB) $(LIBS) $(PLATFORM_LIBS) -o $@

# Build tests
tests:
	@mkdir -p $(BUILD_DIR)/tests
	@$(MAKE) -C $(TESTS_DIR)

# Build tools
tools:
	@mkdir -p $(BUILD_DIR)/tools
	@$(MAKE) -C $(TOOLS_DIR)

# Run tests
test:
	@$(MAKE) -C $(TESTS_DIR) run

# Run examples
run-examples: examples
	@echo "Running vector addition example:"
	@$(BIN_DIR)/vector_add_example
	@echo "\nRunning dot product example:"
	@$(BIN_DIR)/dot_product_example

# Generate documentation
docs:
	@echo "Generating documentation..."
	@mkdir -p $(BUILD_DIR)/docs
	@cat $(DOCS_DIR)/*.md > $(BUILD_DIR)/docs/combined_docs.md
	@echo "Documentation generated in $(BUILD_DIR)/docs/combined_docs.md"

# Clean up
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
	@$(MAKE) -C $(TESTS_DIR) clean
	@$(MAKE) -C $(TOOLS_DIR) clean

# Help target
help:
	@echo "ARM v8 NEON Explorer - Available targets:"
	@echo "  all           Build everything (default)"
	@echo "  setup         Create necessary directories"
	@echo "  examples      Build example programs"
	@echo "  tests         Build test suite"
	@echo "  tools         Build tools"
	@echo "  test          Run test suite"
	@echo "  run-examples  Run example programs"
	@echo "  docs          Generate documentation"
	@echo "  clean         Remove build artifacts"
	@echo ""
	@echo "Platform-specific targets:"
	@echo "  platform-detect  Auto-detect platform and run appropriate tests"
	@echo "  platform-info    Show platform-specific build settings"
	@echo "  macos-test       Run macOS-specific tests"
	@echo "  macos-optimized  Build with macOS-specific optimizations"
	@echo "  freebsd-test     Run FreeBSD-specific tests"
	@echo "  freebsd-install  Install to FreeBSD system"
	@echo "  rpi-benchmark    Run Raspberry Pi-specific benchmarks"
	@echo "  rpi-performance  Build with Raspberry Pi performance optimizations"
	@echo "  android-all      Build for Android platform"

.PHONY: all setup examples tests tools test run-examples docs clean help
