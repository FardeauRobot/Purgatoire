#!/usr/bin/env bash
# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   compile_with_putchar.sh                            :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/12/16 00:00:00 by tibras            #+#    #+#             #
#   Updated: 2025/12/16 00:00:00 by tibras           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #
#
# compile_with_putchar.sh - Compile with ft_putchar from Labo
# Usage: compile_with_putchar.sh <filename.c> [output_name]
# For the Piscine exercises that require ft_putchar

set -euo pipefail

FT_PUTCHAR_PATH="$HOME/Desktop/Labo/ft_putchar.c"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

show_usage() {
	echo -e "${BLUE}=== Compile with ft_putchar ===${NC}"
	echo -e "${YELLOW}Usage: compile_with_putchar.sh <filename.c> [output_name]${NC}"
	echo -e "${CYAN}Examples:${NC}"
	echo -e "  compile_with_putchar.sh main.c"
	echo -e "  compile_with_putchar.sh test.c my_program"
	echo -e "  compile_with_putchar.sh ex00/ft_print_alphabet.c"
}

if [ -z "${1:-}" ]; then
	show_usage
	exit 1
fi

INPUT_FILE="$1"
OUTPUT_NAME="${2:-$(basename "$INPUT_FILE" .c)}"

if [ ! -f "$INPUT_FILE" ]; then
	echo -e "${RED}❌ File '$INPUT_FILE' not found${NC}"
	exit 1
fi

if [ ! -f "$FT_PUTCHAR_PATH" ]; then
	echo -e "${RED}❌ ft_putchar.c not found at $FT_PUTCHAR_PATH${NC}"
	echo -e "${YELLOW}⚠️  Ensure ft_putchar.c exists in ~/Desktop/Labo/${NC}"
	exit 1
fi

echo -e "${BLUE}⚒️  Compiling with ft_putchar${NC}"
echo -e "${CYAN}Input: $INPUT_FILE${NC}"
echo -e "${CYAN}Output: $OUTPUT_NAME${NC}"

if gcc -Wall -Wextra -Werror "$INPUT_FILE" "$FT_PUTCHAR_PATH" -o "$OUTPUT_NAME" 2>/dev/null; then
	echo -e "${GREEN}✅ Compilation successful${NC}"
	chmod +x "$OUTPUT_NAME"
	echo -e "${BLUE}File size: $(ls -lh "$OUTPUT_NAME" | awk '{print $5}')${NC}"
	
	read -r -p "Execute? [y/N] " choice
	if [[ $choice =~ ^[Yy]$ ]]; then
		echo -e "${CYAN}--- Output ---${NC}"
		./"$OUTPUT_NAME"
		echo -e "${CYAN}--- End ---${NC}"
	fi
else
	echo -e "${RED}❌ Compilation failed${NC}"
	gcc -Wall -Wextra -Werror "$INPUT_FILE" "$FT_PUTCHAR_PATH" -o "$OUTPUT_NAME"
	exit 1
fi
