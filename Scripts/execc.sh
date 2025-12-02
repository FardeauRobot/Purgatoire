#!/bin/bash
# execc - Compile C file with ft_putchar.c from Labo directory
# Usage: execc <filename.c>

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Path to ft_putchar.c
FT_PUTCHAR_PATH="$HOME/Desktop/Labo/ft_putchar.c"

# Function to show usage
show_usage() {
    echo -e "${BLUE}=== ExecC - C Compiler with ft_putchar ===${NC}"
    echo -e "${YELLOW}Usage: execc <filename.c> [output_name]${NC}"
    echo -e "${CYAN}Examples:${NC}"
    echo -e "  execc main.c"
    echo -e "  execc test.c my_program"
    echo -e "  execc ex00/ft_print_alphabet.c"
}

# Check if filename is provided
if [ -z "$1" ]; then
    show_usage
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_NAME="$2"

# Check if input file exists
if [ ! -f "$INPUT_FILE" ]; then
    echo -e "${RED}Error: File '$INPUT_FILE' not found.${NC}"
    exit 1
fi

# Check if ft_putchar.c exists
if [ ! -f "$FT_PUTCHAR_PATH" ]; then
    echo -e "${RED}Error: ft_putchar.c not found at $FT_PUTCHAR_PATH${NC}"
    echo -e "${YELLOW}Please make sure ft_putchar.c exists in ~/Desktop/Labo/${NC}"
    exit 1
fi

# Generate output name if not provided
if [ -z "$OUTPUT_NAME" ]; then
    # Remove path and extension from input file to create output name
    OUTPUT_NAME=$(basename "$INPUT_FILE" .c)
fi

echo -e "${BLUE}=== Compiling with ExecC ===${NC}"
echo -e "${CYAN}Input file: $INPUT_FILE${NC}"
echo -e "${CYAN}ft_putchar: $FT_PUTCHAR_PATH${NC}"
echo -e "${CYAN}Output: $OUTPUT_NAME${NC}"

# Compile the files
echo -e "${YELLOW}Compiling...${NC}"
if gcc -Wall -Wextra -Werror "$INPUT_FILE" "$FT_PUTCHAR_PATH" -o "$OUTPUT_NAME" 2>/dev/null; then
    echo -e "${GREEN}✅ Compilation successful!${NC}"
    echo -e "${CYAN}Executable created: $OUTPUT_NAME${NC}"
    
    # Make it executable (just in case)
    chmod +x "$OUTPUT_NAME"
    
    # Show file info
    echo -e "${BLUE}File size: $(ls -lh "$OUTPUT_NAME" | awk '{print $5}')${NC}"
    
    # Ask if user wants to run it
    echo -e "${YELLOW}Run the program? (y/n)${NC}"
    read -r choice
    if [[ $choice =~ ^[Yy]$ ]]; then
        echo -e "${CYAN}Running ./$OUTPUT_NAME:${NC}"
        echo -e "${GREEN}--- Output ---${NC}"
        ./"$OUTPUT_NAME"
        echo -e "${GREEN}--- End ---${NC}"
    fi
else
    echo -e "${RED}❌ Compilation failed!${NC}"
    echo -e "${YELLOW}Trying with detailed error output:${NC}"
    gcc -Wall -Wextra -Werror "$INPUT_FILE" "$FT_PUTCHAR_PATH" -o "$OUTPUT_NAME"
    exit 1
fi