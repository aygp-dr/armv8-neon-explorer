# Makefile for ARM NEON tests

CC = gcc
CFLAGS = -Wall -Wextra -O3 -g
ARCH_FLAGS = -march=armv8-a+simd
INCLUDE = -I../include
LIBS = -lm

TESTS = test_basic_ops test_advanced_ops

.PHONY: all clean run

all: $(TESTS)

test_basic_ops: test_basic_ops.c
	$(CC) $(CFLAGS) $(ARCH_FLAGS) $(INCLUDE) -o $@ $< ../src/simd_ops.c $(LIBS)

test_advanced_ops: test_advanced_ops.c
	$(CC) $(CFLAGS) $(ARCH_FLAGS) $(INCLUDE) -o $@ $< ../src/simd_ops.c $(LIBS)

run: all
	@echo "Running all tests..."
	@for test in $(TESTS); do \
		echo "\n--- Running $$test ---"; \
		./$$test; \
		if [ $$? -ne 0 ]; then \
			echo "\n$$test FAILED!\n"; \
			exit 1; \
		fi; \
	done
	@echo "\nAll tests passed!"

clean:
	rm -f $(TESTS)

# Makefile for ARM NEON tests

CC = gcc
CFLAGS = -Wall -Wextra -O3 -g
ARCH_FLAGS = -march=armv8-a+simd
INCLUDE = -I../include
LIBS = -lm

TESTS = test_basic_ops test_advanced_ops

.PHONY: all clean run

all: $(TESTS)

test_basic_ops: test_basic_ops.c
	$(CC) $(CFLAGS) $(ARCH_FLAGS) $(INCLUDE) -o $@ $< ../src/simd_ops.c $(LIBS)

test_advanced_ops: test_advanced_ops.c
	$(CC) $(CFLAGS) $(ARCH_FLAGS) $(INCLUDE) -o $@ $< ../src/simd_ops.c $(LIBS)

run: all
	@echo "Running all tests..."
	@for test in $(TESTS); do \
		echo "\n--- Running $$test ---"; \
		./$$test; \
		if [ $$? -ne 0 ]; then \
			echo "\n$$test FAILED!\n"; \
			exit 1; \
		fi; \
	done
	@echo "\nAll tests passed!"

clean:
	rm -f $(TESTS)
