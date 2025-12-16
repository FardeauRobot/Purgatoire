# Testing Guide

Comprehensive testing strategy for pipex project.

## Table of Contents

1. [Basic Testing](#basic-testing)
2. [Error Testing](#error-testing)
3. [Edge Cases](#edge-cases)
4. [Comparison with Shell](#comparison-with-shell)
5. [Bonus Testing](#bonus-testing)
6. [Memory Testing](#memory-testing)

---

## Basic Testing

### Setup Test Files

```bash
# Create test directory
mkdir -p test_files
cd test_files

# Create sample input file
cat > input.txt << EOF
apple
banana
cherry
date
elderberry
fig
grape
EOF

# Create empty file
touch empty.txt

# Create large file
seq 1 10000 > large.txt

# Create file with special characters
cat > special.txt << EOF
hello world
tabs	here
"quotes"
'single'
$variables
EOF
```

### Test 1: Basic Pipe

```bash
# Your pipex
./pipex input.txt "cat" "wc -l" output.txt

# Shell equivalent
< input.txt cat | wc -l > expected.txt

# Compare
diff output.txt expected.txt
echo $?  # Should be 0 (no difference)
```

### Test 2: grep and wc

```bash
./pipex input.txt "grep a" "wc -w" output.txt
< input.txt grep a | wc -w > expected.txt
diff output.txt expected.txt
```

### Test 3: sort and uniq

```bash
cat > fruits.txt << EOF
apple
banana
apple
cherry
banana
EOF

./pipex fruits.txt "sort" "uniq" output.txt
< fruits.txt sort | uniq > expected.txt
diff output.txt expected.txt
```

### Test 4: ls and grep

```bash
./pipex /dev/null "ls -l" "grep .c" output.txt
< /dev/null ls -l | grep .c > expected.txt
diff output.txt expected.txt
```

### Test 5: Commands with Multiple Arguments

```bash
./pipex input.txt "head -n 3" "tail -n 1" output.txt
< input.txt head -n 3 | tail -n 1 > expected.txt
diff output.txt expected.txt
```

---

## Error Testing

### Test 1: Invalid Input File

```bash
./pipex nonexistent.txt "cat" "wc -l" output.txt
echo "Exit status: $?"

# Expected behavior:
# - Error message printed to stderr
# - Program continues (second command still runs with empty input)
# - Output file created but may be empty
```

### Test 2: Invalid Command (First)

```bash
./pipex input.txt "invalid_cmd" "wc -l" output.txt
echo "Exit status: $?"

# Expected:
# - Error message about command not found
# - Second command still executes
# - Exit status 127 (command not found)
```

### Test 3: Invalid Command (Second)

```bash
./pipex input.txt "cat" "invalid_cmd" output.txt
echo "Exit status: $?"

# Expected:
# - First command executes
# - Error message about second command
# - Exit status 127
```

### Test 4: No Permission to Read Input

```bash
touch noperm.txt
chmod 000 noperm.txt
./pipex noperm.txt "cat" "wc -l" output.txt
echo "Exit status: $?"

# Expected: Permission denied error
chmod 644 noperm.txt  # Cleanup
```

### Test 5: No Permission to Write Output

```bash
touch readonly.txt
chmod 444 readonly.txt
./pipex input.txt "cat" "wc -l" readonly.txt
echo "Exit status: $?"

# Expected: Cannot write to output file error
chmod 644 readonly.txt  # Cleanup
```

### Test 6: Output Directory Doesn't Exist

```bash
./pipex input.txt "cat" "wc -l" nonexistent_dir/output.txt
echo "Exit status: $?"

# Expected: Error creating output file
```

### Test 7: Empty Command

```bash
./pipex input.txt "" "wc -l" output.txt
./pipex input.txt "cat" "" output.txt

# Expected: Handle gracefully
```

### Test 8: Wrong Number of Arguments

```bash
./pipex
./pipex input.txt
./pipex input.txt "cat"
./pipex input.txt "cat" "wc -l"
./pipex input.txt "cat" "wc -l" output.txt extra

# Expected: Usage message for each
```

---

## Edge Cases

### Test 1: Empty Input File

```bash
./pipex empty.txt "cat" "wc -l" output.txt
< empty.txt cat | wc -l > expected.txt
diff output.txt expected.txt
```

### Test 2: Large File

```bash
./pipex large.txt "head -n 100" "tail -n 10" output.txt
< large.txt head -n 100 | tail -n 10 > expected.txt
diff output.txt expected.txt
```

### Test 3: Binary File

```bash
cp /bin/ls binary_test
./pipex binary_test "cat" "wc -c" output.txt
< binary_test cat | wc -c > expected.txt
diff output.txt expected.txt
```

### Test 4: Special Characters in Filename

```bash
touch "file with spaces.txt"
echo "test" > "file with spaces.txt"
./pipex "file with spaces.txt" "cat" "wc -l" output.txt
< "file with spaces.txt" cat | wc -l > expected.txt
diff output.txt expected.txt
```

### Test 5: Commands with Quotes

```bash
./pipex input.txt "grep 'a'" "wc -l" output.txt
< input.txt grep 'a' | wc -l > expected.txt
diff output.txt expected.txt
```

### Test 6: Absolute Paths

```bash
./pipex /etc/passwd "/bin/cat" "/usr/bin/wc -l" output.txt
< /etc/passwd /bin/cat | /usr/bin/wc -l > expected.txt
diff output.txt expected.txt
```

### Test 7: Relative Paths

```bash
./pipex ./input.txt "cat" "wc -l" ./output.txt
< ./input.txt cat | wc -l > ./expected.txt
diff output.txt expected.txt
```

### Test 8: /dev/stdin and /dev/stdout

```bash
# These are special files
./pipex /dev/null "cat" "wc -l" output.txt
< /dev/null cat | wc -l > expected.txt
diff output.txt expected.txt
```

### Test 9: Overwriting Existing Output

```bash
echo "old content" > output.txt
./pipex input.txt "cat" "head -n 3" output.txt
cat output.txt
# Expected: Old content should be replaced
```

### Test 10: Commands that Exit with Non-zero

```bash
./pipex input.txt "grep NONEXISTENT" "wc -l" output.txt
echo "Exit status: $?"
# Compare with shell behavior
```

---

## Comparison with Shell

### Automated Test Script

```bash
#!/bin/bash

# test_pipex.sh
# Automated comparison with shell behavior

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

test_count=0
pass_count=0

test_case() {
	local infile=$1
	local cmd1=$2
	local cmd2=$3
	local outfile="test_output.txt"
	local expected="test_expected.txt"
	
	test_count=$((test_count + 1))
	
	# Run pipex
	./pipex "$infile" "$cmd1" "$cmd2" "$outfile" 2>/dev/null
	pipex_exit=$?
	
	# Run shell equivalent
	< "$infile" $cmd1 | $cmd2 > "$expected" 2>/dev/null
	shell_exit=$?
	
	# Compare outputs
	if diff -q "$outfile" "$expected" >/dev/null 2>&1; then
		echo -e "${GREEN}✓${NC} Test $test_count: PASS"
		echo "  Command: < $infile $cmd1 | $cmd2"
		pass_count=$((pass_count + 1))
	else
		echo -e "${RED}✗${NC} Test $test_count: FAIL"
		echo "  Command: < $infile $cmd1 | $cmd2"
		echo "  Pipex output:"
		cat "$outfile"
		echo "  Expected output:"
		cat "$expected"
	fi
	
	rm -f "$outfile" "$expected"
}

# Create test file
cat > test_input.txt << EOF
apple
banana
apple
cherry
date
EOF

echo "Running pipex tests..."
echo

# Run tests
test_case "test_input.txt" "cat" "wc -l"
test_case "test_input.txt" "grep a" "wc -w"
test_case "test_input.txt" "sort" "uniq"
test_case "test_input.txt" "head -n 3" "tail -n 1"
test_case "test_input.txt" "cat" "grep -v a"
test_case "test_input.txt" "tr a-z A-Z" "sort"
test_case "test_input.txt" "rev" "sort"
test_case "test_input.txt" "cat" "cat"

echo
echo "Results: $pass_count/$test_count tests passed"

# Cleanup
rm -f test_input.txt
```

### Usage

```bash
chmod +x test_pipex.sh
./test_pipex.sh
```

---

## Bonus Testing

### Multiple Pipes

```bash
# Test with 3 commands
./pipex input.txt "cat" "sort" "uniq" output.txt
< input.txt cat | sort | uniq > expected.txt
diff output.txt expected.txt

# Test with 4 commands
./pipex input.txt "cat" "grep a" "sort" "uniq" output.txt
< input.txt cat | grep a | sort | uniq > expected.txt
diff output.txt expected.txt

# Test with 5 commands
./pipex input.txt "cat" "tr a-z A-Z" "sort" "uniq" "wc -l" output.txt
< input.txt cat | tr a-z A-Z | sort | uniq | wc -l > expected.txt
diff output.txt expected.txt

# Test error in middle command
./pipex input.txt "cat" "invalid" "wc -l" output.txt
echo "Exit status: $?"
```

### Here_doc Testing

```bash
# Test basic here_doc
./pipex here_doc EOF "cat" "wc -l" output.txt << HEREDOC
line 1
line 2
line 3
EOF
HEREDOC
cat output.txt
# Expected: 3

# Test append mode (run twice)
echo "first" > output.txt
./pipex here_doc END "cat" "cat" output.txt << HEREDOC
second
END
HEREDOC
cat output.txt
# Expected: first\nsecond

# Test with grep
./pipex here_doc STOP "grep hello" "wc -w" output.txt << HEREDOC
hello world
goodbye world
hello again
STOP
HEREDOC
cat output.txt
# Expected: 3

# Test with Ctrl+D (manual test)
./pipex here_doc NEVER "cat" "wc -l" output.txt
# Type some lines, then press Ctrl+D

# Test limiter matching
./pipex here_doc END "cat" "cat" output.txt << HEREDOC
END is in this line
ENDNOPE
END
HEREDOC
cat output.txt
# Should include first two lines, stop at "END" alone
```

### Combining Multiple Pipes and Here_doc

```bash
./pipex here_doc EOF "cat" "grep a" "sort" "uniq" output.txt << HEREDOC
apple
banana
apple
cherry
EOF
HEREDOC

< <(cat << EOF
apple
banana
apple
cherry
EOF
) cat | grep a | sort | uniq > expected.txt

diff output.txt expected.txt
```

---

## Memory Testing

### Using Valgrind

```bash
# Basic memory check
valgrind --leak-check=full --show-leak-kinds=all ./pipex input.txt "cat" "wc -l" output.txt

# Check for file descriptor leaks
valgrind --track-fds=yes ./pipex input.txt "cat" "wc -l" output.txt

# Suppressions for system libraries (if needed)
valgrind --leak-check=full --suppressions=readline.supp ./pipex input.txt "cat" "wc -l" output.txt
```

### Expected Result

```
HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
  total heap usage: X allocs, X frees, Y bytes allocated

All heap blocks were freed -- no leaks are possible

FILE DESCRIPTORS: 3 open at exit.
Open file descriptor 0: /dev/pts/X
Open file descriptor 1: /dev/pts/X
Open file descriptor 2: /dev/pts/X
```

### Common Memory Issues

1. **Not freeing split arrays** from command parsing
2. **Not closing pipes** in parent or children
3. **Not closing files** after dup2
4. **Memory leaks in error paths** - check all exit points

### Test Error Path Cleanup

```bash
# Test that memory is freed even on errors
valgrind ./pipex nonexistent.txt "cat" "wc -l" output.txt
valgrind ./pipex input.txt "invalid_cmd" "wc -l" output.txt
valgrind ./pipex input.txt "cat" "invalid_cmd" output.txt
```

---

## Performance Testing

### Large Files

```bash
# Generate 1 million lines
seq 1 1000000 > huge.txt

# Time your pipex
time ./pipex huge.txt "cat" "wc -l" output.txt

# Time shell equivalent
time sh -c '< huge.txt cat | wc -l > expected.txt'

# Compare times (should be similar)
# Compare results
diff output.txt expected.txt

# Cleanup
rm huge.txt
```

### Many Pipes (Bonus)

```bash
# Test with 10 commands
time ./pipex input.txt "cat" "cat" "cat" "cat" "cat" "cat" "cat" "cat" "cat" "wc -l" output.txt

# Shell equivalent
time sh -c '< input.txt cat | cat | cat | cat | cat | cat | cat | cat | cat | wc -l > expected.txt'

diff output.txt expected.txt
```

---

## Stress Testing

### Maximum Pipes

```bash
# Test with maximum number of pipes your system allows
# Usually around 1024 open files, so test with 100+ pipes
```

### Rapid Execution

```bash
# Run many times in a row
for i in {1..100}; do
	./pipex input.txt "cat" "wc -l" output$i.txt
done

# Check for file descriptor exhaustion
ls output*.txt | wc -l  # Should be 100
rm output*.txt
```

### Parallel Execution

```bash
# Run multiple instances simultaneously
for i in {1..10}; do
	./pipex input.txt "cat" "wc -l" output$i.txt &
done
wait

# All should succeed
ls output*.txt | wc -l  # Should be 10
rm output*.txt
```

---

## Final Checklist

### Mandatory Part

- [ ] Handles 2 commands with 1 pipe
- [ ] Correctly opens input file
- [ ] Correctly creates/truncates output file (mode 0644)
- [ ] Both commands execute
- [ ] Data flows through pipe
- [ ] Matches shell behavior: `< infile cmd1 | cmd2 > outfile`
- [ ] Proper error messages for file errors
- [ ] Proper error messages for command errors
- [ ] No memory leaks
- [ ] No file descriptor leaks
- [ ] Proper exit status
- [ ] Handles errors gracefully

### Bonus Part (If Implemented)

- [ ] Handles multiple pipes (3+ commands)
- [ ] Handles here_doc mode
- [ ] Here_doc shows prompt
- [ ] Here_doc reads until limiter
- [ ] Here_doc handles Ctrl+D
- [ ] Here_doc uses O_APPEND
- [ ] Multiple pipes work with here_doc
- [ ] All bonuses tested with valgrind

### Edge Cases Covered

- [ ] Empty input file
- [ ] Large input file
- [ ] Non-existent input file
- [ ] No permission to read input
- [ ] No permission to write output
- [ ] Invalid commands
- [ ] Commands with multiple arguments
- [ ] Commands with quotes
- [ ] Special characters in filenames
- [ ] Absolute and relative paths

---

## Quick Test Command

```bash
# One-liner to test basic functionality
echo "hello world" > test.txt && ./pipex test.txt "cat" "wc -w" out.txt && cat out.txt && rm test.txt out.txt
```

Expected output: `2`

---

Good luck with testing! 🧪
