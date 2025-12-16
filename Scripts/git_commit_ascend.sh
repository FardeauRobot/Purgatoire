#!/usr/bin/env bash
# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   git_commit_ascend.sh                               :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/12/16 00:00:00 by tibras            #+#    #+#             #
#   Updated: 2025/12/16 00:00:00 by tibras           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #
#
# git_commit_ascend.sh - Add, commit and push in one divine command
# Usage: git_commit_ascend.sh "commit message"
# Ascend thy works to the eternal repository

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

show_usage() {
	echo -e "${BLUE}=== Git Commit & Ascend ===${NC}"
	echo -e "${YELLOW}Usage: git_commit_ascend.sh \"commit message\"${NC}"
	echo -e "${CYAN}Examples:${NC}"
	echo -e "  git_commit_ascend.sh \"Fixed the sorting algorithm\""
	echo -e "  git_commit_ascend.sh \"Added new divine feature\""
	echo -e "  git_commit_ascend.sh \"Updated sacred documentation\""
}

if [ ! -d .git ]; then
	echo -e "${RED}❌ Not a git repository${NC}"
	echo -e "${YELLOW}⚠️  Run 'git init' to begin divine versioning${NC}"
	exit 1
fi

if [ -z "${1:-}" ]; then
	show_usage
	exit 1
fi

COMMIT_MSG="$1"
CURRENT_DIR=$(basename "$(pwd)")

echo -e "${BLUE}🔥 Ascending works from '$CURRENT_DIR'${NC}"
echo -e "${CYAN}Current status:${NC}"
git status --short

if [ -z "$(git status --porcelain)" ]; then
	echo -e "${YELLOW}⚠️  No changes to ascend. Repository is pure.${NC}"
	exit 0
fi

echo -e "${CYAN}Files to be inscribed:${NC}"
git diff --name-only 2>/dev/null || true
git diff --cached --name-only 2>/dev/null || true

echo -e "${YELLOW}📝 Adding all files to divine record${NC}"
git add .

echo -e "${CYAN}💬 Message: ${NC}\"$COMMIT_MSG\""
echo -e "${YELLOW}✍️  Inscribing changes${NC}"
git commit -m "$COMMIT_MSG"

CURRENT_BRANCH=$(git branch --show-current)
echo -e "${CYAN}🌿 Branch: $CURRENT_BRANCH${NC}"

if git remote get-url origin &>/dev/null; then
	echo -e "${YELLOW}🚀 Ascending to origin/$CURRENT_BRANCH${NC}"
	
	if git push origin "$CURRENT_BRANCH" 2>/dev/null; then
		echo -e "${GREEN}✅ Successfully ascended to eternal realm${NC}"
		REMOTE_URL=$(git remote get-url origin 2>/dev/null | sed 's/\.git$//')
		if [[ $REMOTE_URL == *"github.com"* ]]; then
			echo -e "${BLUE}🔗 Repository: $REMOTE_URL${NC}"
		fi
	else
		echo -e "${RED}❌ Ascension failed. Trying with upstream${NC}"
		if git push -u origin "$CURRENT_BRANCH"; then
			echo -e "${GREEN}✅ Successfully ascended with upstream${NC}"
		else
			echo -e "${RED}❌ Ascension failed. Check divine authentication${NC}"
			echo -e "${YELLOW}💡 Try: git push -u origin $CURRENT_BRANCH${NC}"
			exit 1
		fi
	fi
else
	echo -e "${YELLOW}⚠️  No eternal repository configured${NC}"
	echo -e "${YELLOW}💡 Connect to eternal realm: git remote add origin <url>${NC}"
fi

echo -e "${GREEN}=== Ascension completed ===${NC}"
echo -e "${CYAN}📁 Directory: $CURRENT_DIR${NC}"
echo -e "${CYAN}💬 Message: \"$COMMIT_MSG\"${NC}"
echo -e "${CYAN}🌿 Branch: $CURRENT_BRANCH${NC}"
