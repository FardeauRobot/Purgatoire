#!/bin/bash
cd /home/fardeau/new_pipex
echo "hello world" > /tmp/px_in
chmod 644 /tmp/px_in
touch /tmp/px_noperm && chmod 000 /tmp/px_noperm

echo "===== TEST 1: Non-existent infile ====="
timeout 3 ./pipex /tmp/doesnotexist "cat" "wc -l" /tmp/px_out 2>&1
echo "pipex exit: $?"
timeout 3 bash -c '< /tmp/doesnotexist cat | wc -l' 2>&1
echo "shell exit: $?"

echo ""
echo "===== TEST 2: No read permission on infile ====="
timeout 3 ./pipex /tmp/px_noperm "cat" "wc -l" /tmp/px_out 2>&1
echo "pipex exit: $?"
timeout 3 bash -c '< /tmp/px_noperm cat | wc -l' 2>&1
echo "shell exit: $?"
chmod 644 /tmp/px_noperm

echo ""
echo "===== TEST 3: Both commands not found ====="
timeout 3 ./pipex /tmp/px_in "fakecmd1" "fakecmd2" /tmp/px_out 2>&1
echo "pipex exit: $?"
timeout 3 bash -c '< /tmp/px_in fakecmd1 2>&1 | fakecmd2' 2>&1
echo "shell exit: $?"

echo ""
echo "===== TEST 4: Empty command string ====="
timeout 3 ./pipex /tmp/px_in "" "wc -l" /tmp/px_out 2>&1
echo "pipex exit: $?"

echo ""
echo "===== TEST 5: Command is just spaces ====="
timeout 3 ./pipex /tmp/px_in "   " "wc -l" /tmp/px_out 2>&1
echo "pipex exit: $?"

echo ""
echo "===== TEST 6: Outfile in non-existent directory ====="
timeout 3 ./pipex /tmp/px_in "cat" "wc -l" /tmp/no/dir/out 2>&1
echo "pipex exit: $?"
timeout 3 bash -c '< /tmp/px_in cat | wc -l > /tmp/no/dir/out' 2>&1
echo "shell exit: $?"

echo ""
echo "===== TEST 7: cmd1 fails (ls nonexistent path) ====="
timeout 3 ./pipex /tmp/px_in "ls /nonexistent_xyz" "wc -l" /tmp/px_out 2>&1
echo -n "pipex exit: $?  outfile: "
cat /tmp/px_out
timeout 3 bash -c '< /tmp/px_in ls /nonexistent_xyz 2>/dev/null | wc -l'
echo "shell exit: $?"

echo ""
echo "===== TEST 8: /dev/null as infile ====="
timeout 3 ./pipex /dev/null "cat" "wc -l" /tmp/px_out 2>&1
echo -n "pipex exit: $?  outfile: "
cat /tmp/px_out
timeout 3 bash -c '< /dev/null cat | wc -l'
echo "shell exit: $?"

echo ""
echo "===== TEST 9: Wrong number of arguments ====="
timeout 3 ./pipex /tmp/px_in "cat" 2>&1
echo "  (3 args) exit: $?"
timeout 3 ./pipex 2>&1
echo "  (0 args) exit: $?"
timeout 3 ./pipex a b c d e f 2>&1
echo "  (7 args) exit: $?"

echo ""
echo "===== TEST 10: Absolute path + bad binary ====="
timeout 3 ./pipex /tmp/px_in "/usr/bin/cat" "/usr/bin/wc" /tmp/px_out 2>&1
echo -n "pipex exit: $?  outfile: "
cat /tmp/px_out
echo ""
timeout 3 ./pipex /tmp/px_in "/no/such/binary" "wc -l" /tmp/px_out 2>&1
echo "bad binary exit: $?"

echo ""
echo "===== ALL 10 TESTS DONE ====="
