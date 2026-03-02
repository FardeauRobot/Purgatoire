#!/bin/bash
# ============================================================
#  init.sh — Bootstrap a new C project from this template
#
#  Usage:  ./init.sh <project_name>
#
#  What it does:
#   1. Renames the binary target in the Makefile
#   2. Updates the header guard in project.h
#   3. Updates the .gitignore binary entry
#   4. Initialises a git repo (if not already inside one)
#   5. Self-destructs (removes itself) to keep the project clean
# ============================================================

set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

if [ $# -ne 1 ]; then
	printf "${RED}Usage: ./init.sh <project_name>${RESET}\n"
	exit 1
fi

NAME="$1"
UPPER_NAME=$(echo "$NAME" | tr '[:lower:]' '[:upper:]')

printf "${CYAN}${BOLD}Initialising project: ${NAME}${RESET}\n\n"

# --- Makefile: binary name ---
sed -i "s/^NAME\t\t= my_project/NAME\t\t= ${NAME}/" Makefile
printf "  ${GREEN}✓${RESET} Makefile binary → ${NAME}\n"

# --- includes/project.h: header guard + rename ---
sed -i "s/PROJECT_H/${UPPER_NAME}_H/g" includes/project.h
mv includes/project.h "includes/${NAME}.h"
printf "  ${GREEN}✓${RESET} project.h      → includes/${NAME}.h\n"

# --- includes/defines.h: update guard ---
sed -i "s/DEFINES_H/${UPPER_NAME}_DEFINES_H/g" includes/defines.h
printf "  ${GREEN}✓${RESET} defines.h      → ${UPPER_NAME}_DEFINES_H\n"

# --- includes/structures.h: update guard ---
sed -i "s/STRUCTURES_H/${UPPER_NAME}_STRUCTURES_H/g" includes/structures.h
printf "  ${GREEN}✓${RESET} structures.h   → ${UPPER_NAME}_STRUCTURES_H\n"

# --- includes/errors.h: update guard ---
sed -i "s/ERRORS_H/${UPPER_NAME}_ERRORS_H/g" includes/errors.h
printf "  ${GREEN}✓${RESET} errors.h       → ${UPPER_NAME}_ERRORS_H\n"

# --- Update #include in all .c files ---
find src -name '*.c' -exec sed -i "s/\"project.h\"/\"${NAME}.h\"/" {} +
printf "  ${GREEN}✓${RESET} #include       → \"${NAME}.h\"\n"

# --- Update error prefix in error.c ---
sed -i "s/\"project: \"/\"${NAME}: \"/" src/utils/error.c
printf "  ${GREEN}✓${RESET} Error prefix   → ${NAME}\n"

# --- .gitignore: binary name ---
sed -i "s/^my_project$/${NAME}/" .gitignore
sed -i '/^# TODO: replace/d' .gitignore
printf "  ${GREEN}✓${RESET} .gitignore     → ${NAME}\n"

# --- Git init ---
if ! git rev-parse --is-inside-work-tree &>/dev/null; then
	git init -q
	printf "  ${GREEN}✓${RESET} git init\n"
else
	printf "  ${CYAN}→${RESET} Already inside a git repo, skipping init\n"
fi

# --- Self-destruct ---
rm -- "$0"
printf "  ${GREEN}✓${RESET} Removed init.sh\n"

printf "\n${GREEN}${BOLD}Done! Your project '${NAME}' is ready.${RESET}\n"
printf "  Run ${CYAN}make${RESET} to build.\n\n"
