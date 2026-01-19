#!/bin/bash

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check if so_long executable exists
if [ ! -f "./so_long" ]; then
    echo -e "${RED}Error: ./so_long executable not found.${NC}"
    echo "Please compile your project first (make) and run this script from the project root."
    exit 1
fi

MAP_DIR="maps"

# Iterate over all .ber files in maps directory
for map in "$MAP_DIR"/*.ber; do
    echo -e "${BLUE}===================================================${NC}"
    echo -e "${BLUE}Testing map: ${GREEN}$map${NC}"
    echo -e "${BLUE}===================================================${NC}"
    
    # Run funcheck
    funcheck ./so_long "$map"
    
    # Capture exit code
    EXIT_CODE=$?
    
    if [ $EXIT_CODE -eq 0 ]; then
        echo -e "${GREEN}>> Game exited purely.${NC}"
    else
        echo -e "${RED}>> Game exited with error code $EXIT_CODE.${NC}"
    fi
     
    echo ""
    echo -e "${BLUE}Press [ENTER] to run verify next map (or Ctrl+C to stop)...${NC}"
    read
done
