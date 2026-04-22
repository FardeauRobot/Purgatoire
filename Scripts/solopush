#!/bin/bash
# SoloPush - Enhanced script to add, commit, and push all files in the current git repository
# Usage: solopush "commit message"

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to show usage
show_usage() {
    echo -e "${BLUE}=== SoloPush - Quick Git Automation ===${NC}"
    echo -e "${YELLOW}Usage: solopush \"commit message\"${NC}"
    echo -e "${CYAN}Examples:${NC}"
    echo -e "  solopush \"Fix bug in login function\""
    echo -e "  solopush \"Add new feature\""
    echo -e "  solopush \"Update documentation\""
}

# Check if we're in a git repository
if [ ! -d .git ]; then
    echo -e "${RED}Error: Current directory is not a git repository.${NC}"
    echo -e "${YELLOW}Run 'git init' to initialize a repository first.${NC}"
    exit 1
fi

# Check if commit message is provided
if [ -z "$1" ]; then
    show_usage
    exit 1
fi

COMMIT_MSG="$1"
CURRENT_DIR=$(basename "$(pwd)")

echo -e "${BLUE}=== SoloPush for '$CURRENT_DIR' ===${NC}"

# Show current git status
echo -e "${CYAN}Current status:${NC}"
git status --short

# Check if there are any changes
if [ -z "$(git status --porcelain)" ]; then
    echo -e "${YELLOW}No changes to commit. Repository is clean.${NC}"
    exit 0
fi

# Show what will be added
echo -e "${CYAN}Files to be added:${NC}"
git diff --name-only 2>/dev/null || true
git diff --cached --name-only 2>/dev/null || true

# Add all files
echo -e "${YELLOW}Adding all files...${NC}"
git add .

# Show commit info
echo -e "${CYAN}Commit message: ${NC}\"$COMMIT_MSG\""

# Commit changes
echo -e "${YELLOW}Committing changes...${NC}"
git commit -m "$COMMIT_MSG"

# Get current branch
CURRENT_BRANCH=$(git branch --show-current)
echo -e "${CYAN}Current branch: $CURRENT_BRANCH${NC}"

# Check if remote exists
if git remote get-url origin &>/dev/null; then
    # Push to remote
    echo -e "${YELLOW}Pushing to origin/$CURRENT_BRANCH...${NC}"
    
    # Try to push
    if git push origin "$CURRENT_BRANCH" 2>/dev/null; then
        echo -e "${GREEN}✅ Successfully pushed to GitHub!${NC}"
        
        # Show remote URL if available
        REMOTE_URL=$(git remote get-url origin 2>/dev/null | sed 's/\.git$//')
        if [[ $REMOTE_URL == *"github.com"* ]]; then
            echo -e "${BLUE}🔗 Repository: $REMOTE_URL${NC}"
        fi
    else
        echo -e "${RED}❌ Push failed. Trying with upstream...${NC}"
        if git push -u origin "$CURRENT_BRANCH"; then
            echo -e "${GREEN}✅ Successfully pushed with upstream set!${NC}"
        else
            echo -e "${RED}❌ Push failed. Check your authentication or network.${NC}"
            echo -e "${YELLOW}💡 Try: git push -u origin $CURRENT_BRANCH${NC}"
            exit 1
        fi
    fi
else
    echo -e "${YELLOW}⚠️  No remote repository configured.${NC}"
    echo -e "${YELLOW}💡 Add remote with: git remote add origin <url>${NC}"
fi

# Show final status
echo -e "${GREEN}=== SoloPush completed! ===${NC}"
echo -e "${CYAN}📁 Directory: $CURRENT_DIR${NC}"
echo -e "${CYAN}💬 Message: \"$COMMIT_MSG\"${NC}"
echo -e "${CYAN}🌿 Branch: $CURRENT_BRANCH${NC}"