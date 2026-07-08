#!/bin/bash
set -e

# Resolve project root (directory containing this script)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# NXDK_DIR must be set or default to ../nxdk relative to this project
if [ -z "$NXDK_DIR" ]; then
    if [ -d "$SCRIPT_DIR/../nxdk" ]; then
        export NXDK_DIR="$(cd "$SCRIPT_DIR/../nxdk" && pwd)"
    else
        echo "ERROR: NXDK_DIR not set and ../nxdk not found." >&2
        echo "Set NXDK_DIR to your nxdk checkout path." >&2
        exit 1
    fi
fi

# Ensure nxdk/bin is on PATH (contains nxdk-cc, nxdk-cxx, etc.)
export PATH="$NXDK_DIR/bin:$PATH"

# Number of parallel jobs (default: number of processors or 4)
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

cd "$SCRIPT_DIR"

# Clean stale objects (optional, skip with NOCLEAN=1)
if [ "${NOCLEAN:-0}" != "1" ]; then
    make clean 2>/dev/null || true
fi

exec make -j"$JOBS" "$@"
