# PIPEX - Project Documentation

Welcome to the Pipex project! This project teaches you about Unix pipes, processes, and inter-process communication.

## What is Pipex?

**Pipex** mimics the behavior of shell pipes (`|`). It executes two commands in sequence, where the output of the first command becomes the input of the second.

### Basic Usage

```bash
./pipex infile "cmd1" "cmd2" outfile
```

**Equivalent shell command:**
```bash
< infile cmd1 | cmd2 > outfile
```

### Example

```bash
./pipex input.txt "grep hello" "wc -l" output.txt
# Counts lines containing "hello" from input.txt, writes count to output.txt
```

---

## Documentation Structure

📚 **Quick Navigation:**

- **[FUNCTIONS.md](FUNCTIONS.md)** - Complete reference for all allowed functions
- **[MACROS.md](MACROS.md)** - All macros and constants you'll need
- **[CONCEPTS.md](CONCEPTS.md)** - Key concepts to master for pipex
- **[WORKFLOW.md](WORKFLOW.md)** - Step-by-step implementation guide
- **[BONUS.md](BONUS.md)** - Bonus features (multiple pipes, here_doc)
- **[TESTING.md](TESTING.md)** - How to test your implementation

---

## Quick Start

### 1. Understand the Requirements

**Mandatory part:**
- Accept 4 arguments: `infile`, `cmd1`, `cmd2`, `outfile`
- Execute: `< infile cmd1 | cmd2 > outfile`
- Handle errors gracefully
- Find commands in PATH

### 2. Key Functions You'll Use

```c
pipe()     // Create communication channel
fork()     // Create child processes
dup2()     // Redirect stdin/stdout
execve()   // Execute commands
waitpid()  // Wait for children
```

### 3. Implementation Steps

1. Parse and validate arguments
2. Open input and output files
3. Create a pipe
4. Fork first child → execute cmd1 (read from infile, write to pipe)
5. Fork second child → execute cmd2 (read from pipe, write to outfile)
6. Parent closes pipes and waits for children

### 4. Study Order Recommendation

1. Read **[CONCEPTS.md](CONCEPTS.md)** - Understand the theory
2. Read **[FUNCTIONS.md](FUNCTIONS.md)** - Learn the tools
3. Read **[MACROS.md](MACROS.md)** - Know the constants
4. Read **[WORKFLOW.md](WORKFLOW.md)** - Follow the implementation guide
5. Implement your pipex
6. Use **[TESTING.md](TESTING.md)** to verify
7. Tackle **[BONUS.md](BONUS.md)** for extra features

---

## Authorized Functions

```c
// I/O
open, close, read, write

// Memory
malloc, free

// Error handling
perror, strerror

// File operations
access, dup, dup2, unlink

// Process management
execve, exit, fork, pipe

// Process waiting
wait, waitpid
```

## Authorized Headers

```c
#include <unistd.h>      // fork, execve, pipe, dup, dup2, etc.
#include <stdlib.h>      // malloc, free, exit
#include <stdio.h>       // perror
#include <string.h>      // String operations
#include <sys/types.h>   // Type definitions
#include <sys/wait.h>    // wait, waitpid, WIFEXITED, etc.
#include <fcntl.h>       // open, O_RDONLY, O_WRONLY, etc.
#include <errno.h>       // errno
```

---

## Visual Overview

```
┌─────────────────────────────────────────────────┐
│                PARENT PROCESS                   │
│                                                 │
│  1. Open infile and outfile                    │
│  2. Create pipe                                 │
│  3. Fork child 1 (cmd1)                        │
│  4. Fork child 2 (cmd2)                        │
│  5. Close pipes                                 │
│  6. Wait for children                           │
│                                                 │
│     ┌──────────────┐      ┌──────────────┐    │
│     │   CHILD 1    │      │   CHILD 2    │    │
│     │              │      │              │    │
│     │  stdin  ←────┼──infile              │    │
│     │  stdout ─────┼──→ pipe[1]          │    │
│     │              │      │              │    │
│     │  execve(cmd1)│      │  stdin  ←────┼──pipe[0]
│     └──────────────┘      │  stdout ─────┼──→ outfile
│                           │              │    │
│                           │  execve(cmd2)│    │
│                           └──────────────┘    │
└─────────────────────────────────────────────────┘
```

---

## Common Pitfalls to Avoid

❌ **Don't:**
- Forget to close unused pipe ends (causes deadlock)
- Skip error checking on system calls
- Leave file descriptors open (memory leaks)
- Use relative paths without searching PATH

✅ **Do:**
- Close all unused file descriptors in children
- Check every function return value
- Handle "command not found" errors
- Free all allocated memory
- Test with various commands and edge cases

---

## Getting Help

If you're stuck:
1. Check the relevant documentation file
2. Use `man` pages: `man 2 fork`, `man 2 pipe`, etc.
3. Draw diagrams of process flow
4. Test with simple commands first
5. Add debugging output to trace execution

---

## Project Goals

By completing pipex, you will learn:
- How Unix pipes work internally
- Process creation and management
- File descriptor manipulation
- Inter-process communication
- Command execution and PATH resolution
- Error handling in system programming

Good luck! 🚀
