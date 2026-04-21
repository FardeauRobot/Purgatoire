#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
TOTAL=0
PASSED=0
FAILED=0

# Cleanup function
cleanup() {
    rm -f test_input.txt test_output.txt expected_output.txt
    rm -f no_read_perm.txt no_write_dir/output.txt
    rm -rf no_write_dir
}

# Setup
setup() {
    echo "Creating test files..."
    echo -e "Hello World\nThis is a test\nPipex project\n42 School" > test_input.txt
    cleanup
}

# Test function
run_test() {
    local test_num=$1
    local description=$2
    local cmd=$3
    local expected_exit=$4
    
    TOTAL=$((TOTAL + 1))
    echo -e "\n${BLUE}Test $test_num: $description${NC}"
    echo "Command: $cmd"
    
    eval $cmd
    local exit_code=$?
    
    if [ "$expected_exit" = "any" ] || [ $exit_code -eq $expected_exit ]; then
        echo -e "${GREEN}✓ PASSED${NC} (exit code: $exit_code)"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ FAILED${NC} (expected: $expected_exit, got: $exit_code)"
        FAILED=$((FAILED + 1))
    fi
}

# Compare with shell behavior
run_compare_test() {
    local test_num=$1
    local description=$2
    local infile=$3
    local cmd1=$4
    local cmd2=$5
    local outfile=$6
    
    TOTAL=$((TOTAL + 1))
    echo -e "\n${BLUE}Test $test_num: $description${NC}"
    
    # Run with pipex
    ./pipex "$infile" "$cmd1" "$cmd2" test_output.txt 2>/dev/null
    local pipex_exit=$?
    
    # Run with shell
    if [ -f "$infile" ]; then
        < "$infile" $cmd1 | $cmd2 > expected_output.txt 2>/dev/null
        local shell_exit=$?
    else
        $cmd1 < "$infile" 2>/dev/null | $cmd2 > expected_output.txt 2>/dev/null
        local shell_exit=$?
    fi
    
    # Compare outputs
    if [ -f test_output.txt ] && [ -f expected_output.txt ]; then
        if diff -q test_output.txt expected_output.txt > /dev/null 2>&1; then
            echo -e "${GREEN}✓ PASSED${NC} (output matches shell)"
            PASSED=$((PASSED + 1))
        else
            echo -e "${YELLOW}⚠ OUTPUT DIFFERS${NC}"
            echo "Pipex output:"
            cat test_output.txt | head -3
            echo "Expected output:"
            cat expected_output.txt | head -3
            FAILED=$((FAILED + 1))
        fi
    elif [ ! -f test_output.txt ] && [ ! -f expected_output.txt ]; then
        echo -e "${GREEN}✓ PASSED${NC} (both failed as expected)"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ FAILED${NC} (behavior differs from shell)"
        FAILED=$((FAILED + 1))
    fi
    
    rm -f test_output.txt expected_output.txt
}

# Start tests
echo -e "${YELLOW}======================================${NC}"
echo -e "${YELLOW}      PIPEX COMPREHENSIVE TESTS      ${NC}"
echo -e "${YELLOW}======================================${NC}"

setup

# TEST 1: Basic valid case
echo "📌 What to check: Basic pipe execution, dup2 redirections, execve calls"
run_compare_test 1 "Basic two commands" "test_input.txt" "cat" "wc -l" "test_output.txt"

# TEST 2: grep and wc
echo "📌 What to check: Command argument parsing (grep with args), pipe data flow"
run_compare_test 2 "grep and wc combination" "test_input.txt" "grep test" "wc -w" "test_output.txt"

# TEST 3: Non-existent input file
echo -e "\n${BLUE}Test 3: Non-existent input file${NC}"
echo "📌 What to check: open() return value check, proper error handling with perror()"
./pipex "nonexistent.txt" "cat" "wc -l" test_output.txt 2>/dev/null
if [ $? -ne 0 ]; then
    echo -e "${GREEN}✓ PASSED${NC} (correctly handles missing input)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC} (should fail with missing input)"
    FAILED=$((FAILED + 1))
fi
TOTAL=$((TOTAL + 1))

# TEST 4: Invalid first command
echo -e "\n${BLUE}Test 4: Invalid first command${NC}"
echo "📌 What to check: Command path search in PATH, access() checks, execve error handling"
./pipex test_input.txt "invalidcmd123" "wc -l" test_output.txt 2>/dev/null
pipex_exit=$?
if [ $pipex_exit -ne 0 ]; then
    echo -e "${GREEN}✓ PASSED${NC} (correctly handles invalid command)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC} (should fail with invalid command)"
    FAILED=$((FAILED + 1))
fi
TOTAL=$((TOTAL + 1))

# TEST 5: Invalid second command
echo -e "\n${BLUE}Test 5: Invalid second command${NC}"
echo "📌 What to check: Second child process error handling, proper exit codes (127 for cmd not found)"
./pipex test_input.txt "cat" "invalidcmd456" test_output.txt 2>/dev/null
pipex_exit=$?
if [ $pipex_exit -ne 0 ]; then
    echo -e "${GREEN}✓ PASSED${NC} (correctly handles invalid command)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC} (should fail with invalid command)"
    FAILED=$((FAILED + 1))
fi
TOTAL=$((TOTAL + 1))

# TEST 6: Empty input file
echo -e "\n${BLUE}Test 6: Empty input file${NC}"
echo "📌 What to check: read() on empty file, EOF handling, pipe behavior with no data"
touch empty.txt
./pipex empty.txt "cat" "wc -l" test_output.txt 2>/dev/null
< empty.txt cat | wc -l > expected_output.txt 2>/dev/null
if diff -q test_output.txt expected_output.txt > /dev/null 2>&1; then
    echo -e "${GREEN}✓ PASSED${NC} (handles empty file correctly)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC}"
    FAILED=$((FAILED + 1))
fi
rm -f empty.txt test_output.txt expected_output.txt
TOTAL=$((TOTAL + 1))

# TEST 7: Command with arguments
echo "📌 What to check: ft_split on command args, proper argv array for execve"
run_compare_test 7 "Command with multiple arguments" "test_input.txt" "grep -i hello" "wc -l" "test_output.txt"

# TEST 8: Commands with absolute path
echo -e "\n${BLUE}Test 8: Commands with absolute paths${NC}"
echo "📌 What to check: Direct path execution (skip PATH search if / in command), access() before execve"
./pipex test_input.txt "/bin/cat" "/usr/bin/wc -l" test_output.txt 2>/dev/null
< test_input.txt /bin/cat | /usr/bin/wc -l > expected_output.txt 2>/dev/null
if diff -q test_output.txt expected_output.txt > /dev/null 2>&1; then
    echo -e "${GREEN}✓ PASSED${NC}"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC}"
    FAILED=$((FAILED + 1))
fi
rm -f test_output.txt expected_output.txt
TOTAL=$((TOTAL + 1))

# TEST 9: Output file creation
echo -e "\n${BLUE}Test 9: Output file creation${NC}"
echo "📌 What to check: open() with O_CREAT flag, file permissions (0644)"
rm -f new_output.txt
./pipex test_input.txt "cat" "wc -l" new_output.txt 2>/dev/null
if [ -f new_output.txt ]; then
    echo -e "${GREEN}✓ PASSED${NC} (output file created)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC} (output file not created)"
    FAILED=$((FAILED + 1))
fi
rm -f new_output.txt
TOTAL=$((TOTAL + 1))

# TEST 10: Overwrite existing output file
echo -e "\n${BLUE}Test 10: Overwrite existing output file${NC}"
echo "📌 What to check: open() with O_TRUNC flag (or O_TRUNC, not O_APPEND)"
echo "OLD CONTENT" > test_output.txt
./pipex test_input.txt "cat" "wc -l" test_output.txt 2>/dev/null
if grep -q "OLD CONTENT" test_output.txt 2>/dev/null; then
    echo -e "${RED}✗ FAILED${NC} (didn't overwrite)"
    FAILED=$((FAILED + 1))
else
    echo -e "${GREEN}✓ PASSED${NC} (correctly overwrote)"
    PASSED=$((PASSED + 1))
fi
rm -f test_output.txt
TOTAL=$((TOTAL + 1))

# TEST 11: Large input file
echo -e "\n${BLUE}Test 11: Large input file${NC}"
echo "📌 What to check: Buffer handling, pipe capacity (64KB), no hardcoded buffer limits"
seq 1 10000 > large.txt
./pipex large.txt "cat" "wc -l" test_output.txt 2>/dev/null
< large.txt cat | wc -l > expected_output.txt 2>/dev/null
if diff -q test_output.txt expected_output.txt > /dev/null 2>&1; then
    echo -e "${GREEN}✓ PASSED${NC} (handles large files)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC}"
    FAILED=$((FAILED + 1))
fi
rm -f large.txt test_output.txt expected_output.txt
TOTAL=$((TOTAL + 1))

# TEST 12: Multiple spaces in command arguments
echo "📌 What to check: Quote handling in split, proper tokenization of arguments"
run_compare_test 12 "Command with multiple spaces" "test_input.txt" "cat" "tr -s ' '" "test_output.txt"

# TEST 13: Commands that fail
echo -e "\n${BLUE}Test 13: Command that fails (grep with no match)${NC}"
echo "📌 What to check: Child exit codes (grep returns 1 when no match), waitpid() handling"
echo "no match here" > nomatch.txt
./pipex nomatch.txt "grep ZZZZZ" "wc -l" test_output.txt 2>/dev/null
pipex_exit=$?
< nomatch.txt grep ZZZZZ | wc -l > expected_output.txt 2>/dev/null
shell_exit=$?
if diff -q test_output.txt expected_output.txt > /dev/null 2>&1; then
    echo -e "${GREEN}✓ PASSED${NC} (handles failed grep correctly)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC}"
    FAILED=$((FAILED + 1))
fi
rm -f nomatch.txt test_output.txt expected_output.txt
TOTAL=$((TOTAL + 1))

# TEST 14: Special characters in input
echo -e "\n${BLUE}Test 14: Special characters in input${NC}"
echo "📌 What to check: Binary-safe read/write, no string manipulation on data flow"
echo -e "Hello\tWorld\n\$PATH\n*.*" > special.txt
./pipex special.txt "cat" "wc -l" test_output.txt 2>/dev/null
< special.txt cat | wc -l > expected_output.txt 2>/dev/null
if diff -q test_output.txt expected_output.txt > /dev/null 2>&1; then
    echo -e "${GREEN}✓ PASSED${NC}"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC}"
    FAILED=$((FAILED + 1))
fi
rm -f special.txt test_output.txt expected_output.txt
TOTAL=$((TOTAL + 1))

# TEST 15: Binary input (should handle or fail gracefully)
echo -e "\n${BLUE}Test 15: Binary input handling${NC}"
echo "📌 What to check: No strlen/string functions on piped data, use read/write syscalls only"
echo -e "\x00\x01\x02\xFF\xFE" > binary.txt
./pipex binary.txt "cat" "wc -c" test_output.txt 2>/dev/null
pipex_exit=$?
if [ $pipex_exit -eq 0 ] || [ $pipex_exit -ne 139 ]; then
    echo -e "${GREEN}✓ PASSED${NC} (handles binary without crashing)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC} (segfault or crash)"
    FAILED=$((FAILED + 1))
fi
rm -f binary.txt test_output.txt
TOTAL=$((TOTAL + 1))

# TEST 16: No arguments
echo -e "\n${BLUE}Test 16: No arguments${NC}"
echo "📌 What to check: argc validation at start of main, proper error message"
./pipex 2>/dev/null
if [ $? -ne 0 ]; then
    echo -e "${GREEN}✓ PASSED${NC} (rejects no arguments)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC} (should reject no arguments)"
    FAILED=$((FAILED + 1))
fi
TOTAL=$((TOTAL + 1))

# TEST 17: Only 3 arguments
echo -e "\n${BLUE}Test 17: Wrong number of arguments (3)${NC}"
echo "📌 What to check: argc != 5 check (for mandatory), usage message"
./pipex test_input.txt "cat" "wc" 2>/dev/null
if [ $? -ne 0 ]; then
    echo -e "${GREEN}✓ PASSED${NC} (rejects wrong arg count)"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC} (should reject wrong arg count)"
    FAILED=$((FAILED + 1))
fi
TOTAL=$((TOTAL + 1))

# TEST 18: Multiple pipes (BONUS - 3 commands)
echo -e "\n${BLUE}Test 18: BONUS - Three commands (multiple pipes)${NC}"
echo "📌 What to check: Dynamic pipe array allocation, proper dup2 for middle processes"
./pipex test_input.txt "cat" "grep test" "wc -l" test_output.txt 2>/dev/null
pipex_exit=$?
if [ $pipex_exit -eq 0 ]; then
    < test_input.txt cat | grep test | wc -l > expected_output.txt 2>/dev/null
    if diff -q test_output.txt expected_output.txt > /dev/null 2>&1; then
        echo -e "${GREEN}✓ PASSED${NC} (bonus working!)"
        PASSED=$((PASSED + 1))
    else
        echo -e "${YELLOW}⚠ BONUS ATTEMPTED but output differs${NC}"
        FAILED=$((FAILED + 1))
    fi
else
    echo -e "${YELLOW}⚠ BONUS not implemented (OK for mandatory)${NC}"
    FAILED=$((FAILED + 1))
fi
rm -f test_output.txt expected_output.txt
TOTAL=$((TOTAL + 1))

# TEST 19: Four commands (BONUS)
echo -e "\n${BLUE}Test 19: BONUS - Four commands${NC}"
echo "📌 What to check: Loop for n-1 pipes, each child reads from prev pipe, writes to next"
./pipex test_input.txt "cat" "grep i" "sort" "uniq" test_output.txt 2>/dev/null
pipex_exit=$?
if [ $pipex_exit -eq 0 ]; then
    < test_input.txt cat | grep i | sort | uniq > expected_output.txt 2>/dev/null
    if diff -q test_output.txt expected_output.txt > /dev/null 2>&1; then
        echo -e "${GREEN}✓ PASSED${NC} (bonus working!)"
        PASSED=$((PASSED + 1))
    else
        echo -e "${YELLOW}⚠ BONUS ATTEMPTED but output differs${NC}"
        FAILED=$((FAILED + 1))
    fi
else
    echo -e "${YELLOW}⚠ BONUS not implemented (OK for mandatory)${NC}"
    FAILED=$((FAILED + 1))
fi
rm -f test_output.txt expected_output.txt
TOTAL=$((TOTAL + 1))

# TEST 20: Memory leak test with valgrind (if available)
echo -e "\n${BLUE}Test 20: Memory leak check (if valgrind available)${NC}"
echo "📌 What to check: All malloc/calloc freed, close all fds, ft_gc_free_all() called"
if command -v valgrind &> /dev/null; then
    valgrind --leak-check=full --errors-for-leak-kinds=all --error-exitcode=42 \
        ./pipex test_input.txt "cat" "wc -l" test_output.txt 2>&1 | grep -q "ERROR SUMMARY: 0 errors"
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ PASSED${NC} (no memory leaks detected)"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ FAILED${NC} (memory leaks detected)"
        FAILED=$((FAILED + 1))
    fi
else
    echo -e "${YELLOW}⚠ SKIPPED${NC} (valgrind not installed)"
    TOTAL=$((TOTAL - 1))
fi
rm -f test_output.txt

# Summary
echo -e "\n${YELLOW}======================================${NC}"
echo -e "${YELLOW}           TEST SUMMARY               ${NC}"
echo -e "${YELLOW}======================================${NC}"
echo -e "Total tests: $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "\n${GREEN}🎉 ALL TESTS PASSED! 🎉${NC}"
else
    echo -e "\n${RED}Some tests failed. Check the output above.${NC}"
fi

# Cleanup
cleanup

exit $FAILED
