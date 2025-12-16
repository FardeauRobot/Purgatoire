#!/usr/bin/env bash
# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   divine_compile.sh                                  :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/12/16 00:00:00 by tibras            #+#    #+#             #
#   Updated: 2025/12/16 00:00:00 by tibras           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #
#
# divine_compile.sh - Compile C files with sacred flags
# Usage: divine_compile.sh [-b BUFFER] [-o output] <files...>
# The divine compiler speaks with -Wall -Wextra -Werror

set -euo pipefail

DEFAULT_BUF=42
BUFFER=${CC_BUFFER:-$DEFAULT_BUF}
OUT="a.out"

usage() {
	cat <<EOF
Divine Compile - Compile thy C files with holy flags

Usage: divine_compile.sh [-b BUFFER] [-o output] <files...>
	-b BUFFER   set BUFFER_SIZE macro (overrides CC_BUFFER)
	-o output   name of produced executable (default: a.out)
	-h          show this divine wisdom

Examples:
	divine_compile.sh -b 100 main.c
	divine_compile.sh -o sacred_test prog.c utils.c
	CC_BUFFER=32 divine_compile.sh main.c
EOF
}

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
	echo "Error: no sacred texts provided." >&2
	usage
	exit 2
fi

GCC_CMD=(gcc -Wall -Wextra -Werror -DBUFFER_SIZE=${BUFFER})
for f in "$@"; do
	GCC_CMD+=("$f")
done
GCC_CMD+=(-o "$OUT")

echo "🔥 Compiling with BUFFER_SIZE=${BUFFER} -> output: ${OUT}"
echo "⚒️  Command: ${GCC_CMD[*]}"

if "${GCC_CMD[@]}"; then
	echo "✅ The divine compilation succeeded: ./$(basename "$OUT")"
	chmod +x "$OUT" 2>/dev/null || true
	
	read -r -p "Execute the sacred binary? [y/N] " runchoice
	if [[ "$runchoice" =~ ^[Yy]$ ]]; then
		./"$OUT"
	fi
else
	echo "❌ The divine compilation has failed" >&2
	exit 1
fi
