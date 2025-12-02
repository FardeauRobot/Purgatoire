#!/bin/bash

# Script to quickly open PDF files from ~/Desktop/Purgatoire/Subjects
# Usage: fopen <filename>

SCRIPTS_PATH="$HOME/Desktop/Purgatoire/Subjects"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check if argument is provided
if [ -z "$1" ]; then
    echo -e "${RED}Usage: fopen <filename>${NC}"
    echo -e "${YELLOW}Example: fopen Libft${NC}"
    echo ""
    echo -e "${BLUE}Available PDF files:${NC}"
    if [ -d "$SCRIPTS_PATH" ]; then
        ls -1 "$SCRIPTS_PATH"/*.pdf 2>/dev/null | xargs -n 1 basename | sed 's/\.pdf$//' | sort
    else
        echo -e "${RED}Subjects directory not found: $SCRIPTS_PATH${NC}"
    fi
    exit 1
fi

FILENAME="$1"
PDF_FILE="$SCRIPTS_PATH/$FILENAME.pdf"

# Check if scripts directory exists
if [ ! -d "$SCRIPTS_PATH" ]; then
    echo -e "${RED}Error: Subjects directory not found: $SCRIPTS_PATH${NC}"
    exit 1
fi

# Check if PDF file exists
if [ ! -f "$PDF_FILE" ]; then
    echo -e "${RED}Error: File '$FILENAME.pdf' not found in $SCRIPTS_PATH${NC}"
    echo ""
    echo -e "${BLUE}Available PDF files:${NC}"
    ls -1 "$SCRIPTS_PATH"/*.pdf 2>/dev/null | xargs -n 1 basename | sed 's/\.pdf$//' | sort
    exit 1
fi

echo -e "${GREEN}Opening $FILENAME.pdf...${NC}"

# Try different PDF viewers in order of preference
if command -v evince &> /dev/null; then
    evince "$PDF_FILE" &>/dev/null &
elif command -v okular &> /dev/null; then
    okular "$PDF_FILE" &>/dev/null &
elif command -v xdg-open &> /dev/null; then
    xdg-open "$PDF_FILE" &>/dev/null &
elif command -v firefox &> /dev/null; then
    firefox "$PDF_FILE" &>/dev/null &
else
    echo -e "${YELLOW}No PDF viewer found. Please install evince, okular, or another PDF viewer.${NC}"
    echo -e "${BLUE}File location: $PDF_FILE${NC}"
    exit 1
fi

echo -e "${GREEN}Done!${NC}"