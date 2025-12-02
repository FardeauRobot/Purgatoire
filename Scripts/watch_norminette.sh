#!/usr/bin/env bash
# watch_norminette.sh - Rainbominette: watch-like wrapper to run norminette repeatedly with rainbow output
# Usage: Rainbominette [-n interval] [-L] [path]
# Examples:
#   Rainbominette            # run norminette on current directory every 2s
#   Rainbominette -n 5 src   # run norminette on 'src' every 5s
#   Rainbominette -L .       # force using lolcat (if available)

set -euo pipefail
INTERVAL=15
TARGET="."
FORCE_LOLCAT=false

usage(){
  cat <<EOF
Rainbominette - run norminette repeatedly and colorize output

Usage: Rainbominette [-n interval] [-L] [path]
  -n interval   seconds between runs (default: 2)
  -L             force using lolcat (if available)
  -r             strip norminette colors and apply rainbow (lolcat if available)
  -h            show this help

Examples:
  Rainbominette
  Rainbominette -n 5 ~/Desktop/42/PiscineReloaded
  Rainbominette -L src
EOF
}

STRIP_AND_RAINBOW=false

while getopts ":n:hLr" opt; do
  case "$opt" in
    n) INTERVAL="$OPTARG" ;;
    L) FORCE_LOLCAT=true ;;
    r) STRIP_AND_RAINBOW=true ;;
    h) usage; exit 0 ;;
    :) echo "Error: -$OPTARG requires an argument" >&2; usage; exit 2 ;;
    \?) echo "Invalid option: -$OPTARG" >&2; usage; exit 2 ;;
  esac
done
shift $((OPTIND-1))

if [ $# -ge 1 ]; then
  TARGET="$1"
fi

# check norminette
if ! command -v norminette &>/dev/null; then
  echo "Error: 'norminette' command not found. Please install or add it to PATH." >&2
  echo "If you have a local ruby gem, you can run: gem install norminette or use the provided binary." >&2
  exit 1
fi

# Detect lolcat
USE_LOLCAT=false
if command -v lolcat &>/dev/null; then
  USE_LOLCAT=true
fi

# honor force flag
if [ "$FORCE_LOLCAT" = true ]; then
  if command -v lolcat &>/dev/null; then
    USE_LOLCAT=true
  else
    echo "Warning: -L requested but 'lolcat' not found in PATH. Continuing with fallback." >&2
    USE_LOLCAT=false
  fi
fi

# simple color array for fallback rainbow (use $'...' so escape sequences are real)
COLORS=( $'\e[38;5;196m' $'\e[38;5;202m' $'\e[38;5;226m' $'\e[38;5;082m' $'\e[38;5;045m' $'\e[38;5;093m' $'\e[38;5;201m' )
NC=$'\e[0m'

# function to rainbowize using bash (fallback if lolcat not present)
rainbow_pipe(){
  local i=0
  local n=${#COLORS[@]}
  # read line by line and print with cycling colors
  while IFS= read -r line || [ -n "$line" ]; do
    color="${COLORS[$((i % n))]}"
    # print line with color
    printf "%b%s%b\n" "$color" "$line" "$NC"
    i=$((i+1))
  done
}

clear
trap 'echo; echo "Exiting Rainbominette."; exit 0' SIGINT SIGTERM

while true; do
  clear
  echo "------------------------------------------------------------"
  echo " Rainbominette: running norminette on: $TARGET    (every ${INTERVAL}s)"
  echo " $(date)"
  echo "------------------------------------------------------------"
  echo

  if [ "$STRIP_AND_RAINBOW" = true ]; then
    # Strip existing ANSI sequences then rainbowize
    if [ "$USE_LOLCAT" = true ]; then
      norminette "$TARGET" 2>&1 | perl -pe 's/\e\[[\d;]*[A-Za-z]//g' | lolcat -a -S 0.5 || true
    else
      norminette "$TARGET" 2>&1 | perl -pe 's/\e\[[\d;]*[A-Za-z]//g' | rainbow_pipe || true
    fi
  else
    # Keep norminette's own coloring intact and print directly
    norminette "$TARGET" 2>&1 || true
    # If user forced lolcat (-L) but didn't request stripping, warn about/avoid mixing
    # (mixing lolcat with pre-colored output can look messy)
  fi

  echo
  echo "Next run in ${INTERVAL}s. Press Ctrl-C to stop."
  sleep "$INTERVAL"
done
