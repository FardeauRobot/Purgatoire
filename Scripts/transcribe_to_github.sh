#!/usr/bin/env bash
# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   transcribe_to_github.sh                            :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/12/16 00:00:00 by tibras            #+#    #+#             #
#   Updated: 2025/12/16 00:00:00 by tibras           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #
#
# transcribe_to_github.sh - Transcribe project to personal GitHub
# Usage: transcribe_to_github.sh
# Copy thy works to the eternal repository

set -e

PERSONAL_GITHUB_USERNAME="FardeauRobot"
PURGATOIRE_PATH="$HOME/Desktop/Purgatoire"
CURRENT_DIR=$(pwd)
PROJECT_NAME=$(basename "$CURRENT_DIR")

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

info() { echo -e "${BLUE}📜 $1${NC}"; }
warn() { echo -e "${YELLOW}⚠️  $1${NC}"; }
err() { echo -e "${RED}❌ $1${NC}"; }
success() { echo -e "${GREEN}✅ $1${NC}"; }

info "Transcribing to the eternal repository"
echo -e "${YELLOW}Current directory: $CURRENT_DIR${NC}"
echo -e "${YELLOW}Project name: $PROJECT_NAME${NC}"

if [ ! -d ".git" ]; then
	err "This directory is not under divine version control"
	exit 1
fi

if [ ! -d "$PURGATOIRE_PATH" ]; then
	info "Creating Purgatoire sanctuary"
	mkdir -p "$PURGATOIRE_PATH"
fi

PROJECT_PATH="$PURGATOIRE_PATH/$PROJECT_NAME"

copy_sacred_texts() {
	info "Transcribing project files"
	mkdir -p "$PROJECT_PATH"
	rsync -av --exclude='.git' --exclude='node_modules' --exclude='__pycache__' \
		--exclude='*.pyc' --exclude='.env' --exclude='venv' --exclude='env' \
		"$CURRENT_DIR/" "$PROJECT_PATH/"
	success "Sacred texts transcribed"
}

initialize_repository() {
	cd "$PROJECT_PATH"
	if [ ! -d ".git" ]; then
		info "Initializing divine repository"
		git init
		git branch -M main
	fi
	if ! git remote get-url origin &>/dev/null; then
		info "Connecting to eternal storage"
		git remote add origin "https://github.com/$PERSONAL_GITHUB_USERNAME/$PROJECT_NAME.git"
	fi
}

verify_repository() {
	info "Verifying repository existence"
	if curl -s -f -o /dev/null "https://api.github.com/repos/$PERSONAL_GITHUB_USERNAME/$PROJECT_NAME"; then
		success "Repository exists in the eternal realm"
		return 0
	else
		warn "Repository not found in eternal realm"
		return 1
	fi
}

create_repository() {
	warn "Repository '$PROJECT_NAME' does not exist"
	if [ -z "$GITHUB_TOKEN" ]; then
		err "GITHUB_TOKEN divine key is not set"
		warn "Create thy token at: https://github.com/settings/tokens"
		warn "Then: export GITHUB_TOKEN=your_sacred_token"
		return 1
	fi
	info "Creating repository in eternal realm"
	RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" \
		-H "Authorization: token $GITHUB_TOKEN" \
		-H "Accept: application/vnd.github.v3+json" \
		https://api.github.com/user/repos \
		-d "{\"name\": \"$PROJECT_NAME\", \"private\": false}")
	if [ "$RESPONSE" = "201" ]; then
		success "Repository created in eternal realm"
		return 0
	elif [ "$RESPONSE" = "422" ]; then
		warn "Repository already exists"
		return 0
	else
		err "Failed to create repository. Divine code: $RESPONSE"
		return 1
	fi
}

commit_and_ascend() {
	cd "$PROJECT_PATH"
	info "Preparing sacred commit"
	git add .
	if git diff --staged --quiet; then
		warn "No changes to inscribe"
	else
		info "Inscribing changes"
		COMMIT_MSG="Transcribed on $(date '+%Y-%m-%d %H:%M:%S')"
		git commit -m "$COMMIT_MSG"
	fi
	info "Ascending to eternal storage"
	if git push -u origin main 2>/dev/null; then
		success "Successfully ascended to eternal realm"
	else
		warn "Ascension failed - authentication required"
		echo -e "${BLUE}Divine authentication options:${NC}"
		echo -e "${GREEN}1. Personal Access Token${NC}"
		echo -e "${GREEN}2. SSH Keys${NC}"
		warn "Manual ascension from: $PROJECT_PATH"
		return 1
	fi
}

main() {
	if [ "$PERSONAL_GITHUB_USERNAME" = "your_github_username" ]; then
		err "Update PERSONAL_GITHUB_USERNAME in the sacred script"
		exit 1
	fi
	copy_sacred_texts
	initialize_repository
	if ! verify_repository; then
		create_repository
	fi
	commit_and_ascend
	success "Transcription completed"
	echo -e "${BLUE}🔗 Repository: https://github.com/$PERSONAL_GITHUB_USERNAME/$PROJECT_NAME${NC}"
	echo -e "${BLUE}📁 Local: $PROJECT_PATH${NC}"
}

main "$@"
