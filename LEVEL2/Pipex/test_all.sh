#!/bin/bash
cd /home/fardeau/new_pipex
echo "hello world" > /tmp/px_in

echo "=== TEST 1: Normal case ==="
timeout 5 ./pipex /tmp/px_in "cat" "wc -l" /tmp/px_out 2>&1; echo "exit: $?"; cat /tmp/px_out

echo ""
echo "=== TEST 2: Non-existent infile ==="
timeout 5 ./pipex /tmp/doesnotexist "cat" "wc -l" /tmp/px_out 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 3: Both commands not found ==="
timeout 5 ./pipex /tmp/px_in "fakecmd1" "fakecmd2" /tmp/px_out 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 4: Empty command ==="
timeout 5 ./pipex /tmp/px_in "" "wc -l" /tmp/px_out 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 5: Only spaces command ==="
timeout 5 ./pipex /tmp/px_in "   " "wc -l" /tmp/px_out 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 6: Outfile in bad directory ==="
timeout 5 ./pipex /tmp/px_in "cat" "wc -l" /tmp/no/dir/out 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 7: sleep 1 as cmd1 (should NOT hang) ==="
timeout 5 ./pipex /tmp/px_in "sleep 1" "wc -l" /tmp/px_out 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 8: sleep 1 as cmd2 (should NOT hang) ==="
timeout 5 ./pipex /tmp/px_in "cat" "sleep 1" /tmp/px_out 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 9: sleep 1 both (should NOT hang) ==="
timeout 5 ./pipex /tmp/px_in "sleep 1" "sleep 1" /tmp/px_out 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 10: sleep 100 (killed by 3s timeout) ==="
timeout 3 ./pipex /tmp/px_in "sleep 100" "sleep 100" /tmp/px_out 2>&1; echo "exit: $? (124=timeout)"

echo ""
echo "=== TEST 11: /dev/null infile ==="
timeout 5 ./pipex /dev/null "cat" "wc -l" /tmp/px_out 2>&1; echo "exit: $?"; cat /tmp/px_out

echo ""
echo "=== TEST 12: Wrong argc ==="
timeout 5 ./pipex /tmp/px_in "cat" 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 13: Unclosed quote ==="
timeout 5 ./pipex /tmp/px_in '"cat' "wc -l" /tmp/px_out 2>&1; echo "exit: $?"

echo ""
echo "=== TEST 14: Absolute path ==="
timeout 5 ./pipex /tmp/px_in "/usr/bin/cat" "/usr/bin/wc" /tmp/px_out 2>&1; echo "exit: $?"; cat /tmp/px_out

echo ""
echo "=== ALL 14 TESTS DONE ==="
