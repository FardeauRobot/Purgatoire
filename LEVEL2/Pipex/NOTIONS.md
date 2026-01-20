# PIPEX - Project Notions

## Project Overview

**Pipex** is a project that mimics the behavior of shell pipes (`|`). The goal is to execute two commands in sequence, where the output of the first command becomes the input of the second command.

### Example
```bash
./pipex infile "cmd1" "cmd2" outfile
# Equivalent to:
< infile cmd1 | cmd2 > outfile
```

---

## Mandatory Part

### What is expected

Your program must:

1. **Accept 4 arguments:**
   - `infile`: Input file to read from
   - `cmd1`: First command to execute
   - `cmd2`: Second command to execute
   - `outfile`: Output file to write to

2. **Behavior:**
   - Read from `infile`
   - Pass the output of `cmd1` as input to `cmd2`
   - Write the final output to `outfile`
   - Handle command arguments (e.g., `"cmd -flags args"`)
   - Find commands in `PATH` environment variable

3. **Error Handling:**
   - Handle file not found errors gracefully
   - Handle command not found errors
   - Exit with appropriate exit codes
   - Print meaningful error messages to stderr

4. **Example execution:**
   ```bash
   ./pipex input.txt "cat" "wc -l" output.txt
   # This reads input.txt, pipes it through cat, counts lines, writes to output.txt
   ```

---

## Bonus Part

### Additional features to implement

1. **Multiple Pipes:**
   - Support more than 2 commands
   - ```bash
     ./pipex infile "cmd1" "cmd2" "cmd3" "cmd4" outfile
     ```

2. **Bonus I/O Handling:**
   - Support `here_doc` syntax:
     ```bash
     ./pipex here_doc LIMITER cmd1 cmd2 outfile
     ```
   - Reads from stdin until `LIMITER` is encountered
   - Appends to `outfile` instead of truncating

---

## Key Notions to Understand

### 1. **Processes and Process Management**
- **fork()**: Creates child processes to run commands independently
- Parent and child processes have separate memory spaces
- Both share file descriptors (important for piping)
- Each command runs in its own process

**Why it matters:** Pipex requires running multiple commands concurrently

---

### 2. **Pipes (IPC - Inter-Process Communication)**
- **pipe()**: Creates a unidirectional channel between processes
- Returns two file descriptors: `fd[0]` (read) and `fd[1]` (write)
- Data written to write-end appears on read-end
- Essential for connecting command outputs to inputs

**Why it matters:** The core mechanism for implementing pipes

---

### 3. **File Descriptors and Redirection**
- **File descriptors**: Numbers representing open files (0=stdin, 1=stdout, 2=stderr)
- **dup2()**: Redirects one file descriptor to another
- Used to redirect stdin/stdout to files or pipes

**Example:**
```c
// Redirect stdout to a file
dup2(fd_file, STDOUT_FILENO);  // Now stdout writes to file
```

**Why it matters:** Required to connect pipes and files to commands

---

### 4. **Command Execution**
- **execve()**: Replaces current process with a new program
- Must search `PATH` to find executables
- Inherits file descriptors and environment
- Doesn't return on success (replaces process)

**Why it matters:** Running actual shell commands

---

### 5. **Environment Variables**
- **PATH**: Tells OS where to find executables
- Must parse PATH to locate commands
- Pass environment to child processes via `execve()`

**Why it matters:** Commands like `cat`, `ls` are found via PATH

---

### 6. **Process Waiting and Exit Codes**
- **wait() / waitpid()**: Parent waits for children to finish
- Exit codes indicate success (0) or failure (non-zero)
- Used to detect command errors

**Why it matters:** Proper process synchronization and error detection

---

### 7. **File I/O Operations**
- **open()**: Open files with appropriate flags
- **close()**: Properly manage file descriptors
- **read() / write()**: Manual I/O (handled by piping in pipex)

**Why it matters:** Connecting initial input and final output

---

### 8. **Error Handling**
- **errno / strerror()**: System error codes and messages
- **perror()**: Print errors to stderr
- **access()**: Check file accessibility before operations

**Why it matters:** Robust error reporting like real shell

---

### 9. **Memory Management**
- **malloc() / free()**: Dynamic memory for command parsing
- Avoid memory leaks
- Properly clean up allocated strings/arrays

**Why it matters:** Parsing variable-length commands

---

### 10. **Process Flow in Pipex**

```
Main Process
    ├── fork() → Child 1
    │   ├── open(infile)
    │   ├── dup2(infile, stdin)
    │   ├── dup2(pipe[1], stdout)
    │   ├── execve(cmd1)
    │
    └── fork() → Child 2
        ├── dup2(pipe[0], stdin)
        ├── open(outfile)
        ├── dup2(outfile, stdout)
        ├── execve(cmd2)

Parent waits for both children
```

---

## Implementation Strategy

### Step 1: Argument Parsing
- Validate 4 arguments
- Parse command strings into program and args

### Step 2: File Handling
- Open input file for reading
- Open output file for writing
- Handle errors gracefully

### Step 3: Command Locating
- Search PATH for executables
- Build full paths to commands

### Step 4: Process and Pipe Setup
- Create pipe with `pipe()`
- Fork first child process
- Fork second child process
- Set up proper redirections with `dup2()`

### Step 5: Command Execution
- Use `execve()` to run commands
- Commands inherit proper stdin/stdout

### Step 6: Process Synchronization
- Wait for both child processes
- Retrieve exit codes
- Clean up resources

---

## Common Pitfalls to Avoid

1. **Forgetting to close unused pipe ends**
   - Can cause deadlocks
   - Child processes should close read end before writing, etc.

2. **Not handling PATH correctly**
   - Must search `PATH` environment variable
   - Or require absolute paths

3. **Improper file descriptor management**
   - Leaving files open
   - Not redirecting stdin/stdout properly

4. **Memory leaks**
   - Forgetting to free parsed commands
   - Not cleaning up dynamic allocations

5. **Wrong exit codes**
   - Should reflect command failures
   - Use `WEXITSTATUS()` to get child exit code

6. **Signal handling ignored**
   - May need to handle signals in bonus
   - Here_doc CTRL+D handling

---

## Testing Your Implementation

```bash
# Basic test
./pipex input.txt "cat" "grep word" output.txt

# Compare with shell
cat input.txt | grep word > expected.txt
diff output.txt expected.txt

# Test with various commands
./pipex input.txt "head -5" "wc -l" output.txt
./pipex input.txt "cat -e" "wc -w" output.txt
```

---

## Resources

- `man` pages for all functions (man 2 fork, man 2 execve, etc.)
- TUTO.md - Detailed function reference
- Test with simple commands first, then more complex ones

---

## Technical Concepts

### 1. File Descriptor Duplication

#### `dup(int oldfd)`
- **Function:** Duplicates `oldfd` to the **lowest-numbered unused** file descriptor.
- **Result:** Two FDs pointing to the same file table entry.

#### `dup2(int oldfd, int newfd)`
- **Function:** Duplicates `oldfd` specifically to `newfd`.
- **Behavior:** If `newfd` is already open, it is closed silently first.
- **Key Use Case:** Redirecting standard streams (e.g., `dup2(fd, STDOUT_FILENO)` makes `printf` write to `fd`).

### 2. Pipe Ends (`pipe(int fd[2])`)

A pipe is a unidirectional communication channel.
- **`fd[1]` (Write End):** The "Microphone". You write data *into* this.
- **`fd[0]` (Read End):** The "Speaker". You read data *out of* this.
- **Flow:** Data written to `fd[1]` becomes immediately available at `fd[0]`.

### 3. Pipe Internals (Kernel Buffer)

- **Memory:** A pipe exists entirely in **Kernel RAM**, not on disk.
- **Structure:** It is a circular ring buffer (typically 64KB on Linux).
- **Behavior:**
    - **Producer (Writer):** Writes to the buffer. `write()` calls block if the buffer is full.
    - **Consumer (Reader):** Reads from the buffer. `read()` calls block if the buffer is empty.
- **Atomicity:** Writes smaller than `PIPE_BUF` (4KB) are atomic (not interleaved with other writers).

### 4. Process Synchronization (`waitpid`)

#### Why wait?
1.  **Synchronization:** The parent needs to know when children are done to avoid exiting too early.
2.  **Resource Cleanup:** When a child dies, it becomes a **"Zombie"** (defunct). It stays in the system process table until its parent reads its exit code. `waitpid` performs this "reaping".

#### Execution Flow Schema

```text
      TIME       PARENT PROCESS            CHILD 1 (Writer)          CHILD 2 (Reader)
       |               |                          |                         |
       |         pipe() created                   |                         |
       |         fork() -----------------------> START                      |
       |               |                          |                         |
       |         fork() -------------------------------------------------> START
       |               |                          |                         |
       |         close(pipe_fds)                  |                         |
       |         waitpid(pid1) ...waiting...    dup2(pipe, STDOUT)      dup2(pipe, STDIN)
       |               |                        execve(cmd1)            execve(cmd2)
       |               |                          |                         |
       |               |                      WRITES DATA ------------>  READS DATA
       |               |                          |                         |
       |               |                         DIES                       |
       v         [parent unblocks] <--------- (Exit Status)                 |
                 waitpid(pid2) ...waiting...                                |
                       |                                                   DIES
                 [parent unblocks] <--------------------------------- (Exit Status)
                 EXIT SUCCESS
```

### 5. Multi-Pipe Schema (N Processes)

For multiple commands (`cmd1 | cmd2 | cmd3 | ...`), the logic generalizes. The parent usually loops, creating a new pipe for each new child, passing the "read end" of the previous pipe to the next child.

#### The "Rolling Pipe" Strategy

```text
ITERATION   PARENT ACTION              CHILD PROCESS (i) CONFIGURATION
    |             |                                   |
   i=0      pipe(fd_A)                  stdin  <-- infile
            fork() ------------------>  stdout --> fd_A[1] (Write to A)
            close(fd_A[1])              
            prev_pipe = fd_A[0]           
    |             |
   i=1      pipe(fd_B)                  stdin  <-- prev_pipe (Read from A)
            fork() ------------------>  stdout --> fd_B[1] (Write to B)
            close(prev_pipe)              
            close(fd_B[1])
            prev_pipe = fd_B[0]
    |             |
   ...         .......                             .......
    |             |
   i=Last   (No new pipe needed)        stdin  <-- prev_pipe (Read from Prev)
            fork() ------------------>  stdout --> outfile
            close(prev_pipe)
    |             |
   Wait     waitpid(-1) loop            [All Children Run in Parallel]
```

**Crucial:** The Parent uses `close()` in every iteration. If the parent keeps any write-end open, the children reading from those pipes will never see EOF and will hang forever.

### 6. Summary of Key Discussions

#### A. Signals: SIGKILL vs SIGTERM
- **SIGKILL (9):** The "Nuclear Option". Immediate removal by kernel. Cannot be caught or ignored. Used only for frozen processes/emergencies.
- **SIGTERM (15):** The polite request to terminate. Can be handled (cleanup, save, etc.).

#### B. Importance of `close()`
Closing file descriptors before `execve` is mandatory for two reasons:
1.  **Preventing Hangs (EOF):** `read()` only returns 0 (EOF) when **ALL** write ends of a pipe are closed. If the parent or a sibling keeps a write-end open, the reader waits forever.
2.  **Cleanliness:** Prevents resource leaks and polluting the child process with useless FDs.

#### C. The "Pipe" Philosophy
- **Origin:** Douglas McIlroy (1964) - "Connect programs like a garden hose".
- **Concept:** Unidirectional flow of bytes in kernel memory (no disk I/O).
- **Usages:**
    - **Compression:** `tar | gzip` (Zero storage backup).
    - **Networking:** `cat file | nc server` (File transfer via netcat).
    - **Infinite Streams:** `cat /dev/urandom | aplay`.

#### D. The `execve` Mechanism ("Brain Transplant")
- **Behavior:** Replaces the current process's memory (code, stack, heap) with a new program.
- **Persistence:** **PID and File Descriptors survive.** This is the core magic of Pipex: you set up the FDs (`dup2`), then `execve` effectively "pours" the new program into your pre-configured FD container.
- **Return:** NEVER returns on success. If it returns, it failed.

#### E. Multiple Writers
- Multiple processes can write to the same pipe end.
- **Sequential:** `(cmd1; cmd2) | cmd3` (Clean).
- **Concurrent:** If writing < 4KB (PIPE_BUF), the kernel guarantees **atomicity** (data won't be scrambled/interleaved).

### 7. Deepening Understanding (Q&A)

Here are 5 advanced questions to solidify your mastery of the subject.

#### Q1: What is a "Zombie Process" and why is it dangerous?
**A:** A Zombie is a child process that has completed execution (called `exit()`), but its entry in the process table is still there because the parent hasn't read its exit status yet.
*   **Why it happens:** The parent forgot to call `wait()` or `waitpid()`.
*   **Danger:** Zombies don't consume CPU or RAM, but they consume **PIDs** (Process IDs). Since PIDs are finite (max 32768 on many systems), a "Zombie Apocalypse" can prevent the OS from starting any new processes, crashing the server.

#### Q2: Why doesn't `execve` find the command if I just verify `PATH` exists?
**A:** `execve` is "dumb". It does not search.
*   **Shell vs Kernel:** When you type `ls` in Bash, the **Shell** iterates through directories in `$PATH`, checks where `ls` lives (e.g., `/usr/bin/ls`), and *then* calls `execve("/usr/bin/ls", ...)`.
*   **Your Task:** You must manually replicate this behavior: split the `PATH` variable, join each path with `/cmd`, and use `access()` to check if the binary is executable before passing it to `execve`.

#### Q3: What is a "Pipe Deadlock" and how do I cause it?
**A:** A deadlock happens when two processes are stuck waiting for each other forever.
*   **Scenario:** A pipe buffer fills up (64KB).
*   **The Trap:** If the Parent tries to `waitpid()` for the Child to finish *before* reading the data the Child is writing, and the Child writes > 64KB, the Child will **BLOCK** (sleep) waiting for space to clear. The Parent is blocked waiting for the Child to exit. They wait for each other eternally.

#### Q4: Why can't I pass `*.c` to `execve` like in the shell?
**A:** Wildcard expansion (globbing) is a feature of the **Shell** program (Bash/Zsh), not the Kernel.
*   If you pass `*.c` to `execve`, the command receives the literal string `"*.c"`. It will try to open a file actually named `*.c`.
*   To support this, you would have to write your own code to read the directory and match filenames against the pattern *before* calling exec.

#### Q5: What is the difference between a "File Descriptor" and a "File Pointer" (`FILE *`)?
**A:**
*   **File Descriptor (`int`):** A raw integer used by the **Kernel** (syscalls: `read`, `write`, `open`). Buffering is handled by the kernel/disk.
*   **File Pointer (`FILE *`):** A C struct used by the **Standard Library** (libc functions: `fopen`, `fprintf`, `fscanf`). It adds a layer of user-space buffering for performance.
*   **Pipex:** Must use File Descriptors (`int`) because `pipe()` and `dup2()` are low-level system calls working directly with the kernel.



