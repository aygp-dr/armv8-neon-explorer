FROM debian:bullseye-slim as base

# Install basic dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    gcc \
    g++ \
    make \
    curl \
    wget \
    pkg-config \
    python3 \
    python3-pip \
    gdb \
    && rm -rf /var/lib/apt/lists/*

# Development stage with all tools
FROM base as dev

# Install additional development tools
RUN apt-get update && apt-get install -y \
    clang \
    clang-format \
    clang-tidy \
    valgrind \
    htop \
    emacs-nox \
    vim \
    tmux \
    && rm -rf /var/lib/apt/lists/*

# Set up non-root user (optional but recommended)
ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Set the default user
USER $USERNAME

# Set working directory
WORKDIR /workspace

# Command to run when container starts
CMD ["/bin/bash"]
