# ARM v8 NEON Explorer

A Socratic exploration of ARM v8 SIMD instructions using the NEON architecture.

## Overview

This project provides a structured approach to learning and experimenting with ARM NEON SIMD programming through a Socratic method. Instead of just presenting code examples, it guides you through discovering how SIMD works on ARM architectures by asking questions and exploring implementations.

## Features

- Cross-platform support (Raspberry Pi, Apple Silicon, FreeBSD, Android)
- Comprehensive testing framework for SIMD operations
- Performance comparison between scalar and SIMD implementations
- Interactive explorer tool for experimentation
- Docker-based development environment for consistent testing
- Detailed documentation with a Socratic learning approach

## Requirements

- ARMv8-A compatible processor (aarch64/ARM64)
- GCC or Clang compiler with ARM NEON support
- Make for building
- (Optional) Docker for containerized development

## Getting Started

### Building Directly

```bash
# Clone the repository
git clone https://github.com/aygp-dr/armv8-neon-explorer.git
cd armv8-neon-explorer

# Install prerequisites
./prerequisites.sh

# Build the project
make all

# Run tests
make test

# Run examples
./bin/vector_add_example
./bin/dot_product_example
```

### Using Docker

```bash
# Clone the repository
git clone https://github.com/aygp-dr/armv8-neon-explorer.git
cd armv8-neon-explorer

# Build the Docker container
./container.sh build

# Start a shell in the container
./container.sh shell

# Build and run within the container
make all
make test
```

## Project Structure

- `src/` - Core SIMD operation implementations
- `include/` - Header files and utility functions
- `examples/` - Example programs demonstrating SIMD usage
- `tests/` - Test suite for verification
- `tools/` - Development and helper tools
- `docs/` - Documentation and references

## Documentation

Detailed documentation is available in the `docs/` directory:

- [ARM NEON Quick Reference](docs/neon_cheatsheet.md)
- [Learning Approach](docs/learning_approach.md)
- [Performance Considerations](docs/performance.md)

## Interactive Explorer

The project includes an interactive tool for experimenting with NEON operations:

```bash
# Build the explorer
make -C tools

# Run the explorer
./tools/neon_explorer
```

Use `help` command within the explorer for available operations.

## Learning Path

This project follows a Socratic approach to learning:

1. Start with the basic vector operations in `examples/vector_add_example.c`
2. Progress to more complex operations like dot products
3. Explore advanced image processing techniques
4. Understand performance considerations and trade-offs
5. Experiment with your own SIMD implementations

## License

MIT License

## Contact

Aidan Pace - apace@defrecord.com
