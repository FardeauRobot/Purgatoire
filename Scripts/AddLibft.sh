#!/usr/bin/env bash
# AddLibft.sh - build libft.a from ~/Desktop/Purgatoire/LIBFT and copy artifacts to current directory
# Usage: AddLibft [make-target]
# By default runs `make -C LIBFT_DIR all`. Pass an optional make target (e.g., re) to override.

set -euo pipefail

LIBFT_DIR="$HOME/Desktop/Purgatoire/LIBFT"
LIBFT_LIB_NAME="libft.a"
DEFAULT_TARGET="all"
MAKE_TARGET="${1:-$DEFAULT_TARGET}"
DEST_DIR="$(pwd)"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

info(){ echo -e "${BLUE}$1${NC}"; }
warn(){ echo -e "${YELLOW}$1${NC}"; }
err(){ echo -e "${RED}$1${NC}"; }

# Ensure source directory exists
if [ ! -d "$LIBFT_DIR" ]; then
  err "LIBFT directory not found at $LIBFT_DIR"
  exit 1
fi

info "Building libft in $LIBFT_DIR (target: $MAKE_TARGET)"
if ! make -C "$LIBFT_DIR" "$MAKE_TARGET"; then
  err "make failed. Please fix build errors before running AddLibft."
  exit 1
fi

LIBFT_LIB_PATH="$LIBFT_DIR/$LIBFT_LIB_NAME"
if [ ! -f "$LIBFT_LIB_PATH" ]; then
  err "Compilation succeeded but $LIBFT_LIB_NAME was not found in $LIBFT_DIR"
  exit 1
fi

# Locate libft header
find_header(){
  if [ -n "${LIBFT_HEADER:-}" ] && [ -f "$LIBFT_HEADER" ]; then
    echo "$LIBFT_HEADER"
    return
  fi
  find "$LIBFT_DIR" -maxdepth 2 -type f -name "libft.h" | head -n 1
}

HEADER_PATH=$(find_header)
if [ -z "$HEADER_PATH" ]; then
  warn "libft.h not found within $LIBFT_DIR (max depth 2). Only copying $LIBFT_LIB_NAME."
else
  info "Found header: $HEADER_PATH"
fi

# Locate README
README_PATH=""
for readme_name in README.md README README.txt readme.md; do
  if [ -f "$LIBFT_DIR/$readme_name" ]; then
    README_PATH="$LIBFT_DIR/$readme_name"
    info "Found README: $README_PATH"
    break
  fi
done
if [ -z "$README_PATH" ]; then
  warn "README not found in $LIBFT_DIR"
fi

# Copy artifacts to destination
info "Copying artifacts to $DEST_DIR"
cp "$LIBFT_LIB_PATH" "$DEST_DIR/"
if [ -n "$HEADER_PATH" ]; then
  cp "$HEADER_PATH" "$DEST_DIR/"
fi
if [ -n "$README_PATH" ]; then
  cp "$README_PATH" "$DEST_DIR/"
fi

# Clean up object files in LIBFT directory
info "Cleaning object files from $LIBFT_DIR"
find "$LIBFT_DIR" -type f -name "*.o" -delete 2>/dev/null || true

info "Artifacts in destination:"
ls -l "$DEST_DIR/$LIBFT_LIB_NAME" 2>/dev/null || true
if [ -n "$HEADER_PATH" ]; then
  ls -l "$DEST_DIR/$(basename "$HEADER_PATH")" 2>/dev/null || true
fi
if [ -n "$README_PATH" ]; then
  ls -l "$DEST_DIR/$(basename "$README_PATH")" 2>/dev/null || true
fi

info "AddLibft completed successfully."
