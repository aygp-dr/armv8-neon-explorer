# Raspberry Pi-specific Makefile configuration for ARM NEON

# Detect Raspberry Pi hardware
IS_RPI = $(shell grep -q "Raspberry Pi" /proc/cpuinfo && echo 1 || echo 0)

# Raspberry Pi-specific compiler flags
RPI_CFLAGS = -mcpu=cortex-a72 -D_RASPBERRYPI_

# Raspberry Pi 4 specific optimizations (you can add more for other models)
RPI_4_CFLAGS = -mcpu=cortex-a72 -DRPI_MODEL=4

# Raspberry Pi-specific libraries
RPI_LIBS = -lm -lpthread -lrt

# Raspberry Pi-specific defines
ifeq ($(IS_RPI),1)
    PLATFORM_CFLAGS += $(RPI_CFLAGS)
    PLATFORM_LIBS += $(RPI_LIBS)
    PLATFORM_DEFINES += -DPLATFORM_RPI=1
    
    # Detect Raspberry Pi model
    RPI_MODEL = $(shell cat /proc/cpuinfo | grep "Revision" | awk '{print $3}')
    
    # Set Raspberry Pi 4 specific flags
    ifeq ($(shell grep -q "Raspberry Pi 4" /proc/cpuinfo && echo 1 || echo 0),1)
        PLATFORM_CFLAGS += $(RPI_4_CFLAGS)
    endif
endif

# Raspberry Pi-specific performance tests
rpi-benchmark: all
	@echo "Running Raspberry Pi-specific benchmarks..."
	./examples/vector_add_example 1000000
	./examples/dot_product_example 1000000
	./examples/matrix_multiply 512 512 512

# Raspberry Pi-specific power-efficient build
rpi-power-efficient: CFLAGS += -O2 -fno-fast-math -fno-tree-vectorize
rpi-power-efficient: all

# Raspberry Pi-specific performance build
rpi-performance: CFLAGS += -O3 -ffast-math -ftree-vectorize
rpi-performance: all

.PHONY: rpi-benchmark rpi-power-efficient rpi-performance
