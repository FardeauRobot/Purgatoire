#!/usr/bin/env bash
# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   forge_libft.sh                                     :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/12/16 00:00:00 by tibras            #+#    #+#             #
#   Updated: 2025/12/16 00:00:00 by tibras           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #
#
# forge_libft.sh - Forge the sacred libft library and copy to includes
# Usage: forge_libft.sh [make-target]
# The Foundation Stone must be built before thy trials begin

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

info() { echo -e "${BLUE}⚒️  $1${NC}"; }
warn() { echo -e "${YELLOW}⚠️  $1${NC}"; }
err() { echo -e "${RED}❌ $1${NC}"; }
success() { echo -e "${GREEN}✅ $1${NC}"; }

if [ ! -d "$LIBFT_DIR" ]; then
	err "The Foundation Stone was not found at $LIBFT_DIR"
	exit 1
fi

info "Forging libft in the sacred fires (target: $MAKE_TARGET)"
if ! make -C "$LIBFT_DIR" "$MAKE_TARGET"; then
	err "The forge has failed. Repair thy code before proceeding."
	exit 1
fi

LIBFT_LIB_PATH="$LIBFT_DIR/$LIBFT_LIB_NAME"
if [ ! -f "$LIBFT_LIB_PATH" ]; then
	err "The forge succeeded but $LIBFT_LIB_NAME was not created"
	exit 1
fi

if [ ! -d "$DEST_DIR" ]; then
	info "Creating the sacred includes directory"
	mkdir -p "$DEST_DIR"
fi

info "Inscribing the sacred texts to includes"
cd "$LIBFT_DIR"
find . -type f \( -name "*.c" -o -name "*.h" -o -name "*.md" -o -name "[Mm]akefile" \) | while read -r file; do
	mkdir -p "$DEST_DIR/$(dirname "$file")"
	cp "$file" "$DEST_DIR/$file"
done
cd - > /dev/null

success "The Foundation Stone has been forged and inscribed"
