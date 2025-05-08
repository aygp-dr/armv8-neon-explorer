#!/bin/bash
#!/bin/bash
# Helper script for container operations

set -e
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "$SCRIPT_DIR"

function help {
    echo "ARM v8 NEON Explorer Container Helper"
    echo ""
    echo "Usage: ./container.sh [command]"
    echo ""
    echo "Commands:"
    echo "  build         Build the development container"
    echo "  build-qemu    Build the QEMU ARM64 emulation container"
    echo "  shell         Start an interactive shell in the container"
    echo "  shell-qemu    Start an interactive shell in the QEMU emulation container"
    echo "  make [args]   Run make with the specified arguments in the container"
    echo "  test          Run tests in the container"
    echo "  clean         Clean up Docker resources"
    echo "  help          Show this help message"
}

case "$1" in
    build)
        docker-compose build arm-simd-dev
        ;;
    build-qemu)
        docker-compose build arm64-emulation
        ;;
    shell)
        docker-compose run --rm arm-simd-dev
        ;;
    shell-qemu)
        docker-compose run --rm arm64-emulation
        ;;
    make)
        shift
        docker-compose run --rm arm-simd-dev make "$@"
        ;;
    test)
        docker-compose run --rm arm-simd-dev make test
        ;;
    clean)
        docker-compose down
        docker system prune -f
        ;;
    help|*)
        help
        ;;
esac
