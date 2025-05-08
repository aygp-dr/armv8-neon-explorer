# macOS-specific Makefile configuration for ARM NEON

# Check if running on Apple Silicon
ifneq ($(shell uname -p | grep -i 'arm'),)
    IS_APPLE_SILICON = 1
endif

# macOS-specific compiler flags
MACOS_CFLAGS = -D_DARWIN_C_SOURCE -Wno-deprecated-declarations

# Apple Silicon specific optimizations
ifdef IS_APPLE_SILICON
    MACOS_CFLAGS += -mcpu=apple-m1 -D_APPLE_SILICON_
endif

# macOS-specific libraries
MACOS_LIBS = -framework Accelerate

# macOS-specific defines
ifeq ($(shell uname -s),Darwin)
    PLATFORM_CFLAGS += $(MACOS_CFLAGS)
    PLATFORM_LIBS += $(MACOS_LIBS)
    PLATFORM_DEFINES += -DPLATFORM_MACOS=1
    ifdef IS_APPLE_SILICON
        PLATFORM_DEFINES += -DPLATFORM_APPLE_SILICON=1
    endif
endif

# macOS-specific test configuration
macos-test: all
	@echo "Running macOS-specific tests..."
	./tests/test_basic_ops
	./tests/test_advanced_ops

# macOS-specific optimized build with Apple-specific extensions
macos-optimized: CFLAGS += -O3 -ftree-vectorize -ffast-math
macos-optimized: all

.PHONY: macos-test macos-optimized
