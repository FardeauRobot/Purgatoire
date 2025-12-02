#!/bin/bash

# Script to copy current directory to personal GitHub repository
# Usage: ./copy_to_personal_github.sh

set -e  # Exit on any error

# Configuration
PERSONAL_GITHUB_USERNAME="FardeauRobot"  # Replace with your GitHub username
PURGATOIRE_PATH="$HOME/Desktop/Purgatoire"
CURRENT_DIR=$(pwd)
PROJECT_NAME=$(basename "$CURRENT_DIR")

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== GitHub Repository Sync Script ===${NC}"
echo -e "${YELLOW}Current directory: $CURRENT_DIR${NC}"
echo -e "${YELLOW}Project name: $PROJECT_NAME${NC}"

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    echo -e "${RED}Error: Current directory is not a git repository${NC}"
    exit 1
fi

# Create Purgatoire directory if it doesn't exist
if [ ! -d "$PURGATOIRE_PATH" ]; then
    echo -e "${YELLOW}Creating Purgatoire directory at $PURGATOIRE_PATH${NC}"
    mkdir -p "$PURGATOIRE_PATH"
fi

# Path to the project in Purgatoire
PROJECT_PATH="$PURGATOIRE_PATH/$PROJECT_NAME"

# Function to copy files (excluding .git directory)
copy_project_files() {
    echo -e "${YELLOW}Copying project files...${NC}"
    
    # Create project directory if it doesn't exist
    mkdir -p "$PROJECT_PATH"
    
    # Copy all files except .git directory
    rsync -av --exclude='.git' --exclude='node_modules' --exclude='__pycache__' \
          --exclude='*.pyc' --exclude='.env' --exclude='venv' --exclude='env' \
          "$CURRENT_DIR/" "$PROJECT_PATH/"
    
    echo -e "${GREEN}Files copied successfully${NC}"
}

# Function to initialize git repository
init_git_repo() {
    cd "$PROJECT_PATH"
    
    if [ ! -d ".git" ]; then
        echo -e "${YELLOW}Initializing git repository...${NC}"
        git init
        git branch -M main
    fi
    
    # Add remote if it doesn't exist
    if ! git remote get-url origin &>/dev/null; then
        echo -e "${YELLOW}Adding remote origin...${NC}"
        git remote add origin "https://github.com/$PERSONAL_GITHUB_USERNAME/$PROJECT_NAME.git"
    fi
}

# Function to check if GitHub repository exists
check_github_repo() {
    echo -e "${YELLOW}Checking if GitHub repository exists...${NC}"
    
    # Check if repository exists on GitHub
    if curl -s -f -o /dev/null "https://api.github.com/repos/$PERSONAL_GITHUB_USERNAME/$PROJECT_NAME"; then
        echo -e "${GREEN}Repository exists on GitHub${NC}"
        return 0
    else
        echo -e "${YELLOW}Repository does not exist on GitHub${NC}"
        return 1
    fi
}

# Function to create GitHub repository automatically using GitHub API
create_github_repo() {
    echo -e "${YELLOW}Repository '$PROJECT_NAME' does not exist on GitHub.${NC}"
    if [ -z "$GITHUB_TOKEN" ]; then
        echo -e "${RED}GITHUB_TOKEN environment variable is not set.${NC}"
        echo -e "${YELLOW}Please create a Personal Access Token at: https://github.com/settings/tokens${NC}"
        echo -e "${YELLOW}Then run: export GITHUB_TOKEN=your_token_here${NC}"
        echo -e "${YELLOW}and re-run the script.${NC}"
        return 1
    fi
    echo -e "${YELLOW}Creating repository '$PROJECT_NAME' on GitHub...${NC}"
    RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" \
        -H "Authorization: token $GITHUB_TOKEN" \
        -H "Accept: application/vnd.github.v3+json" \
        https://api.github.com/user/repos \
        -d "{\"name\": \"$PROJECT_NAME\", \"private\": false}")
    if [ "$RESPONSE" = "201" ]; then
        echo -e "${GREEN}Repository created successfully!${NC}"
        return 0
    elif [ "$RESPONSE" = "422" ]; then
        echo -e "${YELLOW}Repository already exists or name conflict.${NC}"
        return 0
    else
        echo -e "${RED}Failed to create repository. HTTP status: $RESPONSE${NC}"
        return 1
    fi
}

# Function to commit and push changes
commit_and_push() {
    cd "$PROJECT_PATH"
    
    echo -e "${YELLOW}Adding files to git...${NC}"
    git add .
    
    # Check if there are changes to commit
    if git diff --staged --quiet; then
        echo -e "${YELLOW}No changes to commit${NC}"
    else
        echo -e "${YELLOW}Committing changes...${NC}"
        COMMIT_MSG="Update from $(date '+%Y-%m-%d %H:%M:%S')"
        git commit -m "$COMMIT_MSG"
    fi
    
    echo -e "${YELLOW}Pushing to GitHub...${NC}"
    
    # Try to push, handle authentication gracefully
    if git push -u origin main 2>/dev/null; then
        echo -e "${GREEN}Successfully pushed to GitHub!${NC}"
    else
        echo -e "${YELLOW}Push failed. This might be due to authentication.${NC}"
        echo -e "${BLUE}Please try one of these options:${NC}"
        echo -e "${GREEN}1. Use personal access token: git push -u origin main${NC}"
        echo -e "${GREEN}2. Configure Git credentials: git config --global credential.helper store${NC}"
        echo -e "${GREEN}3. Use SSH keys for authentication${NC}"
        echo ""
        echo -e "${YELLOW}To set up authentication:${NC}"
        echo -e "${GREEN}• Personal Access Token: https://github.com/settings/tokens${NC}"
        echo -e "${GREEN}• When prompted for password, use the token instead${NC}"
        echo ""
        echo -e "${YELLOW}Try pushing manually from: $PROJECT_PATH${NC}"
        echo -e "${BLUE}Command: git push -u origin main${NC}"
        return 1
    fi
}

# Main execution
main() {
    # Validate configuration
    if [ "$PERSONAL_GITHUB_USERNAME" = "your_github_username" ]; then
        echo -e "${RED}Error: Please update PERSONAL_GITHUB_USERNAME in the script${NC}"
        exit 1
    fi
    
    # Copy files
    copy_project_files
    
    # Initialize git repository
    init_git_repo
    
    # Check if repository exists on GitHub
    if ! check_github_repo; then
        # Create GitHub repository automatically
        create_github_repo
    fi
    
    # Commit and push changes
    commit_and_push
    
    echo -e "${GREEN}=== Sync completed successfully! ===${NC}"
    echo -e "${BLUE}Repository URL: https://github.com/$PERSONAL_GITHUB_USERNAME/$PROJECT_NAME${NC}"
    echo -e "${BLUE}Local copy: $PROJECT_PATH${NC}"
}

# Run main function
main "$@"