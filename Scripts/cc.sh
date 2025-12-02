#!/bin/bash
# cc - small wrapper to compile C files with 42 flags and configurable BUFFER_SIZE
# Usage: cc [-b BUFFER] [-o output] <files...>
# Examples:
#   cc -b 42 ex00/ft_*.c
#   cc -o myprog main.c other.c
# You can also export CC_BUFFER to set a default: export CC_BUFFER=64

set -euo pipefail

DEFAULT_BUF=42
BUFFER=${CC_BUFFER:-$DEFAULT_BUF}
OUT="a.out"

usage() {
  cat <<EOF
Usage: cc [-b BUFFER] [-o output] <files...>
  -b BUFFER   set BUFFER_SIZE macro (overrides CC_BUFFER)
  -o output   name of produced executable (default: a.out)
  -h          show this help

Examples:
  cc -b 100 main.c
  cc -o test prog.c utils.c
  CC_BUFFER=32 cc main.c
EOF
}

# parse options
while getopts ":b:o:h" opt; do
  case "$opt" in
    b) BUFFER="$OPTARG" ;;
    o) OUT="$OPTARG" ;;
    h) usage; exit 0 ;;
    :) echo "Error: -$OPTARG requires an argument" >&2; usage; exit 2 ;;
    \?) echo "Invalid option: -$OPTARG" >&2; usage; exit 2 ;;
  esac
done
shift $((OPTIND-1))

if [ $# -lt 1 ]; then
  echo "Error: no input files provided." >&2
  usage
  exit 2
fi

# build gcc command
# Use -DBUFFER_SIZE=<value> (no space)
GCC_CMD=(gcc -Wall -Wextra -Werror -DBUFFER_SIZE=${BUFFER})
# append all remaining args (input files)
for f in "$@"; do
  GCC_CMD+=("$f")
done
GCC_CMD+=(-o "$OUT")

# show and run
echo "Compiling with BUFFER_SIZE=${BUFFER} -> output: ${OUT}"
echo "Command: ${GCC_CMD[*]}"

# run and capture exit
if "${GCC_CMD[@]}"; then
  echo "\n✅ Build succeeded: ./$(basename "$OUT")"
else
  echo "\n❌ Build failed" >&2
  exit 1
fi

# Make executable permission
chmod +x "$OUT" 2>/dev/null || true

# Offer to run
read -r -p "Run executable now? [y/N] " runchoice
if [[ "$runchoice" =~ ^[Yy]$ ]]; then
  ./"$OUT"
fi
