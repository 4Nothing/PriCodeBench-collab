# riot-claude sandbox image
# Provides: RIOT build toolchain + Claude Code CLI in an isolated environment.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Build dependencies for RIOT native64
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc-13-multilib \
    g++-13-multilib \
    linux-libc-dev \
    make \
    cmake \
    python3 \
    python3-pexpect \
    curl \
    ca-certificates \
    unzip \
    git \
    && rm -rf /var/lib/apt/lists/*

# Latest Node.js 22.x for Claude Code CLI
RUN curl -fsSL https://deb.nodesource.com/setup_22.x | bash - \
    && apt-get install -y --no-install-recommends nodejs \
    && rm -rf /var/lib/apt/lists/*

# Install Claude Code CLI (pinned version for reproducibility)
# Bump this when re-building the benchmark image.
RUN npm install -g @anthropic-ai/claude-code@2.1.146

# Non-root agent user
RUN useradd -m -s /bin/bash agent && chmod a+x /home/agent
USER agent
WORKDIR /workspace

# Entrypoint: Claude will read /home/agent/.claude/settings.json (mounted volume)
ENTRYPOINT ["claude", "--dangerously-skip-permissions"]
