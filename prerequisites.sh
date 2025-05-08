#!/bin/bash
#!/bin/bash
# Check and install prerequisites for ARM v8 NEON development

set -e
echo "Checking prerequisites for ARM v8 NEON Explorer..."

# Check if running on ARM64 platform
if [[ $(uname -m) != "aarch64" ]]; then
    echo "WARNING: Not running on aarch64 architecture. Some functionality may not work."
    echo "Current architecture: $(uname -m)"
fi

# Check for GCC
if ! command -v gcc &> /dev/null; then
    echo "Installing GCC..."
    sudo apt-get update
    sudo apt-get install -y gcc
else
    echo "GCC is installed: $(gcc --version | head -n1)"
fi

# Check for Make
if ! command -v make &> /dev/null; then
    echo "Installing Make..."
    sudo apt-get update
    sudo apt-get install -y make
else
    echo "Make is installed: $(make --version | head -n1)"
fi

# Install additional development tools
echo "Installing development tools..."
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    git \
    cmake \
    gdb \
    valgrind \
    binutils \
    python3 \
    python3-pip \
    clang \
    clang-format

# Check for ARM NEON support
echo "Checking for ARM NEON support..."
if gcc -dM -E - < /dev/null | grep -q "__ARM_NEON"; then
    echo "ARM NEON is supported by the compiler."
else
    echo "WARNING: ARM NEON support not detected in compiler!"
    echo "Make sure you're using GCC with ARM NEON support."
fi

# Install tools for benchmarking and performance testing
echo "Installing performance measurement tools..."
sudo apt-get install -y \
    linux-tools-common \
    linux-tools-generic \
    time \
    htop

echo "Prerequisites check complete!"
