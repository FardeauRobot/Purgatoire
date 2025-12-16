#!/usr/bin/env bash
# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   open_subject.sh                                    :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/12/16 00:00:00 by tibras            #+#    #+#             #
#   Updated: 2025/12/16 00:00:00 by tibras           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #
#
# open_subject.sh - Open PDF subject files quickly
# Usage: open_subject.sh <filename>
# Read the sacred texts of thy trials

set -euo pipefail

SUBJECTS_PATH="$HOME/Desktop/Purgatoire/Subjects"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

if [ -z "${1:-}" ]; then
	echo -e "${RED}Usage: open_subject.sh <filename>${NC}"
	echo -e "${YELLOW}Example: open_subject.sh Libft${NC}"
	echo ""
	echo -e "${BLUE}📜 Available sacred texts:${NC}"
	if [ -d "$SUBJECTS_PATH" ]; then
		ls -1 "$SUBJECTS_PATH"/*.pdf 2>/dev/null | xargs -n 1 basename | sed 's/\.pdf$//' | sort
	else
		echo -e "${RED}❌ Subjects directory not found: $SUBJECTS_PATH${NC}"
	fi
	exit 1
fi

FILENAME="$1"
PDF_FILE="$SUBJECTS_PATH/$FILENAME.pdf"

if [ ! -d "$SUBJECTS_PATH" ]; then
	echo -e "${RED}❌ Subjects sanctuary not found: $SUBJECTS_PATH${NC}"
	exit 1
fi

if [ ! -f "$PDF_FILE" ]; then
	echo -e "${RED}❌ Sacred text '$FILENAME.pdf' not found${NC}"
	echo ""
	echo -e "${BLUE}📜 Available texts:${NC}"
	ls -1 "$SUBJECTS_PATH"/*.pdf 2>/dev/null | xargs -n 1 basename | sed 's/\.pdf$//' | sort
	exit 1
fi

echo -e "${GREEN}📖 Opening $FILENAME.pdf${NC}"

if command -v evince &> /dev/null; then
	evince "$PDF_FILE" &>/dev/null &
elif command -v okular &> /dev/null; then
	okular "$PDF_FILE" &>/dev/null &
elif command -v xdg-open &> /dev/null; then
	xdg-open "$PDF_FILE" &>/dev/null &
elif command -v firefox &> /dev/null; then
	firefox "$PDF_FILE" &>/dev/null &
else
	echo -e "${YELLOW}⚠️  No PDF viewer found${NC}"
	echo -e "${BLUE}File location: $PDF_FILE${NC}"
	exit 1
fi

echo -e "${GREEN}✅ Sacred text opened${NC}"
