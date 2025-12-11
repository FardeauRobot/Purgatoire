#!/usr/bin/env bash
# AddLibft.sh - build libft.a from ~/Desktop/Purgatoire/LIBFT and copy entire folder to includes directory
# Usage: AddLibft [make-target]
# By default runs `make -C LIBFT_DIR all`. Pass an optional make target (e.g., re) to override.

set -euo pipefail

LIBFT_DIR="$HOME/Desktop/Purgatoire/libft"
LIBFT_LIB_NAME="libft.a"
DEFAULT_TARGET="all"
MAKE_TARGET="${1:-$DEFAULT_TARGET}"
DEST_DIR="$(pwd)/includes"

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

# Create includes directory if it doesn't exist
if [ ! -d "$DEST_DIR" ]; then
  info "Creating includes directory at $DEST_DIR"
  mkdir -p "$DEST_DIR"
fi

# Copy only .c, .h, .md and Makefile files from libft folder
info "Copying .c, .h, .md and Makefile files to $DEST_DIR"
find "$LIBFT_DIR" -type f \( -name "*.c" -o -name "*.h" -o -name "*.md" -o -name "[Mm]akefile" \) -exec cp --parents {} "$DEST_DIR/" \; 2>/dev/null || {
  # Fallback for systems without --parents
  cd "$LIBFT_DIR"
  find . -type f \( -name "*.c" -o -name "*.h" -o -name "*.md" -o -name "[Mm]akefile" \) | while read -r file; do
    mkdir -p "$DEST_DIR/$(dirname "$file")"
    cp "$file" "$DEST_DIR/$file"
  done
  cd - > /dev/null
}

info "Contents copied to includes directory:"
ls -la "$DEST_DIR" 2>/dev/null || true

info "AddLibft completed successfully."
