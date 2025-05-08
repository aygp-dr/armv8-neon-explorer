# FreeBSD-specific Makefile configuration for ARM NEON

# FreeBSD-specific compiler flags
FREEBSD_CFLAGS = -I/usr/local/include -D_FREEBSD_ -Wno-unknown-pragmas

# FreeBSD-specific libraries
FREEBSD_LIBS = -L/usr/local/lib -lm

# FreeBSD-specific defines
ifeq ($(shell uname -s),FreeBSD)
    PLATFORM_CFLAGS += $(FREEBSD_CFLAGS)
    PLATFORM_LIBS += $(FREEBSD_LIBS)
    PLATFORM_DEFINES += -DPLATFORM_FREEBSD=1
endif

# FreeBSD-specific installation directories
FREEBSD_INSTALL_DIR = /usr/local/lib/armv8-neon-explorer

# FreeBSD-specific test configuration
freebsd-test: all
	@echo "Running FreeBSD-specific tests..."
	./tests/test_basic_ops
	./tests/test_advanced_ops

# FreeBSD-specific install target
freebsd-install: all
	@echo "Installing to $(FREEBSD_INSTALL_DIR)..."
	mkdir -p $(FREEBSD_INSTALL_DIR)/bin
	mkdir -p $(FREEBSD_INSTALL_DIR)/examples
	mkdir -p $(FREEBSD_INSTALL_DIR)/tests
	cp bin/* $(FREEBSD_INSTALL_DIR)/bin/
	cp examples/* $(FREEBSD_INSTALL_DIR)/examples/
	cp tests/* $(FREEBSD_INSTALL_DIR)/tests/

.PHONY: freebsd-test freebsd-install
