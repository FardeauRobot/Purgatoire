# Bienvenue dans ce petit tuto sur PIPEX

## Table of Contents

1. [Authorized Headers](#authorized-headers)

2. [Essential Macros](#essential-macros)
   - [Standard File Descriptors](#standard-file-descriptors)
   - [File Control Flags](#file-control-flags)
   - [File Permissions](#file-permissions)
   - [Access Modes](#access-modes)

3. [I/O Functions](#io-functions)
   - [open()](#open)
   - [close()](#close)
   - [read()](#read)
   - [write()](#write)

4. [Memory Management](#memory-management)
   - [malloc()](#malloc)
   - [free()](#free)

5. [Error Handling](#error-handling)
   - [perror()](#perror)
   - [strerror()](#strerror)

6. [File Access & Duplication](#file-access--duplication)
   - [access()](#access)
   - [dup()](#dup)
   - [dup2()](#dup2)

7. [Process Management](#process-management)
   - [execve()](#execve)
   - [exit()](#exit)
   - [fork()](#fork)
   - [pipe()](#pipe)

8. [Process Termination](#process-termination)
   - [unlink()](#unlink)
   - [wait()](#wait)
   - [waitpid()](#waitpid)

9. [Status Parsing Macros](#status-parsing-macros)

10. [Advanced Macros](#advanced-macros)
    - [Wait Options](#wait-options)
    - [Signal Numbers](#signal-numbers)

11. [Getting Environment Variables](#getting-environment-variables-envp)

12. [Pipex Workflow Overview](#pipex-workflow-overview)

---

## Authorized Headers

These are the standard C library headers you can use in pipex:

| Header | Purpose | Functions Used |
|--------|---------|-----------------|
| `<unistd.h>` | POSIX API | fork, execve, pipe, dup, dup2, close, read, write, access, exit, waitpid |
| `<stdlib.h>` | Standard library | malloc, free, exit |
| `<stdio.h>` | Standard I/O | printf, fprintf, perror |
| `<string.h>` | String operations | strlen, strcpy, strdup, strcmp, strtok |
| `<sys/types.h>` | Type definitions | pid_t, mode_t, etc. |
| `<sys/wait.h>` | Process waiting | wait, waitpid, WIFEXITED, WEXITSTATUS, etc. |
| `<fcntl.h>` | File control | open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC |
| `<errno.h>` | Error handling | errno |
| `<signal.h>` | Signal handling | (needed for bonus) |

---

## Essential Macros

These macros are used constantly in pipex and should be understood first.

### Standard File Descriptors

From `<unistd.h>` - these represent stdin, stdout, and stderr:

```c
STDIN_FILENO    // Standard input (0)
STDOUT_FILENO   // Standard output (1)
STDERR_FILENO   // Standard error (2)
```

**Why use them:** Makes code more readable than using `0`, `1`, `2` directly.

**Example:**
```c
dup2(fd, STDOUT_FILENO);  // Redirect stdout to a file
dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin from pipe
```

---

### File Control Flags

From `<fcntl.h>` - used with `open()` function:

#### Access Modes (pick exactly one)

```c
O_RDONLY    // Read only
O_WRONLY    // Write only
O_RDWR      // Read and write
```

#### Creation/Modification Flags (combine with |)

```c
O_CREAT     // Create file if it doesn't exist
O_TRUNC     // Truncate (empty) file if it exists
O_APPEND    // Append to end of file
```

**Common combinations for pipex:**
```c
// Open input file for reading
int infd = open(infile, O_RDONLY);

// Open output file for writing (create if needed, truncate if exists)
int outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

---

### File Permissions

Octal notation for file permissions with `open()`:

```c
0644    // Owner: read+write, Group: read, Others: read
0755    // Owner: read+write+execute, Others: read+execute
0600    // Owner: read+write only
```

**Example:**
```c
int fd = open("file.txt", O_WRONLY | O_CREAT, 0644);
```

---

### Access Modes

From `<unistd.h>` - used with `access()`:

```c
F_OK    // Check if file exists
R_OK    // Check if readable
W_OK    // Check if writable
X_OK    // Check if executable
```

**Example:**
```c
if (access("file.txt", F_OK) == 0)
	printf("File exists\n");

if (access("file.txt", R_OK) == 0)
	printf("File is readable\n");
```

---

## Fonctions autorisées

### I/O Functions

#### `open()`
```c
int open(const char *pathname, int flags, mode_t mode);
```
**Purpose:** Opens a file and returns a file descriptor.

**Return Value:** 
- Positive integer (file descriptor) on success
- `-1` on error (check `errno`)

**Parameters:**
- `pathname`: Path to the file
- `flags`: `O_RDONLY`, `O_WRONLY`, `O_RDWR`, `O_CREAT`, `O_TRUNC`, `O_APPEND`
- `mode`: Permissions (e.g., `0644`) - only used with `O_CREAT`

**Things to take care of:**
- Always check return value for errors
- Close file descriptors when done
- Remember permissions matter when creating files

---

#### `close()`
```c
int close(int fd);
```
**Purpose:** Closes a file descriptor.

**Return Value:**
- `0` on success
- `-1` on error

**Things to take care of:**
- Always close file descriptors to avoid leaks
- Don't close already closed descriptors
- In pipex, close unnecessary pipes in parent/child processes

---

#### `read()`
```c
ssize_t read(int fd, void *buf, size_t count);
```
**Purpose:** Reads data from a file descriptor into a buffer.

**Return Value:**
- Number of bytes read (>0)
- `0` if EOF reached
- `-1` on error

**Parameters:**
- `fd`: File descriptor
- `buf`: Buffer to store data
- `count`: Maximum number of bytes to read

**Things to take care of:**
- Buffer must be large enough
- Check for errors (return -1)
- Handle EOF (return 0)

---

#### `write()`
```c
ssize_t write(int fd, const void *buf, size_t count);
```
**Purpose:** Writes data from a buffer to a file descriptor.

**Return Value:**
- Number of bytes written
- `-1` on error

**Parameters:**
- `fd`: File descriptor
- `buf`: Data to write
- `count`: Number of bytes to write

**Things to take care of:**
- Check return value (might write less than requested)
- Handle write errors

---

### Memory Management

#### `malloc()`
```c
void *malloc(size_t size);
```
**Purpose:** Allocates dynamic memory on the heap.

**Return Value:**
- Pointer to allocated memory on success
- `NULL` on failure

**Things to take care of:**
- Always check for NULL return
- Free allocated memory when done
- Track allocated pointers to avoid leaks

---

#### `free()`
```c
void free(void *ptr);
```
**Purpose:** Deallocates previously allocated memory.

**Things to take care of:**
- Only free memory allocated with `malloc`, `calloc`, `realloc`
- Don't free NULL (it's safe but unnecessary)
- Don't double-free the same pointer
- After free, set pointer to NULL if you might use it

---

### Error Handling

#### `perror()`
```c
void perror(const char *s);
```
**Purpose:** Prints error message from the current `errno` to stderr.

**Parameters:**
- `s`: Custom prefix message (can be NULL or "")

**Output Example:**
```
./pipex: No such file or directory
```

**Things to take care of:**
- Prints to stderr automatically
- Error message depends on `errno` value
- Useful for debugging

---

#### `strerror()`
```c
char *strerror(int errnum);
```
**Purpose:** Returns a string describing the error number.

**Return Value:** Pointer to error message string

**Example Usage:**
```c
fprintf(stderr, "Error: %s\n", strerror(errno));
```

**Things to take care of:**
- Returns pointer to static buffer (don't free)
- Buffer may be overwritten on next call
- Pass `errno` as argument

---

### File Access & Duplication

#### `access()`
```c
int access(const char *pathname, int mode);
```
**Purpose:** Checks if a file exists and if you have specific permissions.

**Return Value:**
- `0` if access is allowed
- `-1` if not allowed or file doesn't exist

**Parameters:**
- `pathname`: Path to file
- `mode`: `F_OK` (exists), `R_OK` (readable), `W_OK` (writable), `X_OK` (executable)

**Things to take care of:**
- Useful for validation before opening
- Race condition possible between check and use
- Prefer error handling on actual operations

---

#### `dup()`
```c
int dup(int oldfd);
```
**Purpose:** Duplicates a file descriptor (creates a new descriptor pointing to same file).

**Return Value:**
- New file descriptor on success
- `-1` on error

**Things to take care of:**
- Both descriptors must be closed separately
- They share the same file offset
- Useful in pipex before `execve`

---

#### `dup2()`
```c
int dup2(int oldfd, int newfd);
```
**Purpose:** Duplicates `oldfd` to `newfd`, replacing it if it exists.

**Return Value:**
- `newfd` on success
- `-1` on error

**Things to take care of:**
- If `newfd` is open, it's automatically closed first
- Essential for redirecting stdin/stdout in child processes
- In pipex: redirect pipe to stdin/stdout before `execve`

---

### Process Management

#### `execve()`
```c
int execve(const char *pathname, char *const argv[], char *const envp[]);
```
**Purpose:** Replaces current process image with a new program.

**Return Value:**
- Returns `-1` only on error (on success, process is replaced)

**Parameters:**
- `pathname`: Full path to executable
- `argv`: Arguments array (must end with NULL)
- `envp`: Environment variables array (must end with NULL)

**Things to take care of:**
- Must provide full path or search PATH manually
- `argv[0]` should be program name
- `argv` must be NULL-terminated
- Closes file descriptors only if they have `FD_CLOEXEC` flag
- In pipex: use after redirecting stdin/stdout

---

#### `exit()`
```c
void exit(int status);
```
**Purpose:** Terminates the program with an exit code.

**Parameters:**
- `status`: Exit code (0 = success, non-zero = error)

**Things to take care of:**
- Flushes open file streams before exit
- Called automatically at program end
- In pipex: use in child processes to terminate

---

#### `fork()`
```c
pid_t fork(void);
```
**Purpose:** Creates a new child process (duplicate of parent).

**Return Value:**
- `0` in child process
- Child's PID in parent process
- `-1` on error

**Things to take care of:**
- Child gets copy of all file descriptors
- Both parent and child continue execution
- Unidirectional communication requires pipes
- Close unused pipe ends in child and parent

---

#### `pipe()`
```c
int pipe(int pipefd[2]);
```
**Purpose:** Creates an unnamed pipe for inter-process communication.

**Return Value:**
- `0` on success
- `-1` on error

**Parameters:**
- `pipefd[2]`: Array where `pipefd[0]` = read end, `pipefd[1]` = write end

**Things to take care of:**
- `pipefd[0]` is for reading, `pipefd[1]` is for writing
- Close unused ends in child/parent to avoid deadlocks
- EOF detected when all write ends are closed
- In pipex: create pipe before fork, redirect in children

---

### Process Termination

#### `unlink()`
```c
int unlink(const char *pathname);
```
**Purpose:** Removes (deletes) a file.

**Return Value:**
- `0` on success
- `-1` on error

**Things to take care of:**
- Only deletes if you have write permission on directory
- Useful for cleanup in pipex temporary files

---

#### `wait()`
```c
pid_t wait(int *wstatus);
```
**Purpose:** Waits for a child process to terminate and retrieves its status.

**Return Value:**
- PID of terminated child on success
- `-1` on error

**Parameters:**
- `wstatus`: Pointer to status variable (can be NULL)

**Things to take care of:**
- Blocks until child terminates
- Use `WIFEXITED()`, `WEXITSTATUS()` to parse status
- If multiple children, blocks for any child

---

#### `waitpid()`
```c
pid_t waitpid(pid_t pid, int *wstatus, int options);
```
**Purpose:** Waits for a specific child process (more control than `wait`).

**Return Value:**
- PID of child that changed state
- `0` if `WNOHANG` used and no child ready
- `-1` on error

**Parameters:**
- `pid`: Specific PID to wait for (or `-1` for any child)
- `wstatus`: Pointer to status variable
- `options`: `0` for blocking, `WNOHANG` for non-blocking

**Things to take care of:**
- Allows waiting for specific children
- `WNOHANG` useful for non-blocking wait
- Better than `wait()` for multiple children

---

## Status Parsing Macros

### Where Do They Come From?

These macros are defined in the `<sys/wait.h>` header file. When you include this header, you get access to these macros for interpreting process exit status.

```c
#include <sys/wait.h>  // Required for these macros
```

### How Status Works

When a child process terminates, the operating system stores information about how it exited in a single integer called the **status code**. This status code is packed with multiple pieces of information in different bits. The macros extract this information for you.

**Status code layout (simplified):**
```
┌──────────────────────────────────────────┐
│ Bit 7  │ Bit 6  │ ... │ Bit 1  │ Bit 0  │
├──────────────────────────────────────────┤
│ Exit   │ Signal │ ... │Signal? │Exit?   │
│ Code   │ Number │ ... │killed? │normal? │
└──────────────────────────────────────────┘
```

You don't need to understand the exact bit layout - the macros handle it for you!

### The Four Main Macros

#### 1. `WIFEXITED(status)` - "Was It Exited?"

```c
int WIFEXITED(int status);
```

**What it does:** Returns true (non-zero) if the child process exited normally.

**Returns:**
- Non-zero (true) if process called `exit()` normally
- `0` (false) if process was killed by a signal

**Why use it:** You must check this BEFORE calling `WEXITSTATUS()`, otherwise you'll get garbage.

**Example:**
```c
int status;
waitpid(pid, &status, 0);

if (WIFEXITED(status))
	printf("Process exited normally\n");
else
	printf("Process was killed by a signal\n");
```

---

#### 2. `WEXITSTATUS(status)` - "What Was The Exit Status?"

```c
int WEXITSTATUS(int status);
```

**What it does:** Extracts the exit code (0-255) from the status.

**Returns:** The exit code passed to `exit()` by the child process

**Important:** Only valid if `WIFEXITED(status)` is true!

**Example:**
```c
int status;
waitpid(pid, &status, 0);

if (WIFEXITED(status))
{
	int exit_code = WEXITSTATUS(status);
	printf("Child exited with code: %d\n", exit_code);
	
	// Check the actual exit code
	if (exit_code == 0)
		printf("Success!\n");
	else if (exit_code == 127)
		printf("Command not found\n");
	else
		printf("Error occurred\n");
}
```

**Common exit codes:**
- `0` = Success
- `1` = General error
- `127` = Command not found
- `126` = Permission denied / cannot execute

---

#### 3. `WIFSIGNALED(status)` - "Was It Signaled?"

```c
int WIFSIGNALED(int status);
```

**What it does:** Returns true if the child was killed by a signal (not a normal exit).

**Returns:**
- Non-zero (true) if process was terminated by a signal
- `0` (false) if process exited normally

**Why use it:** To detect if a child was killed abnormally (e.g., segmentation fault, killed by SIGKILL).

**Example:**
```c
int status;
waitpid(pid, &status, 0);

if (WIFSIGNALED(status))
{
	int signal = WTERMSIG(status);
	printf("Process was killed by signal: %d\n", signal);
}
```

**Common signals that kill processes:**
- `11` (SIGSEGV) = Segmentation fault
- `9` (SIGKILL) = Killed (cannot be caught)
- `15` (SIGTERM) = Termination signal
- `6` (SIGABRT) = Abort signal

---

#### 4. `WTERMSIG(status)` - "What Was The Terminating Signal?"

```c
int WTERMSIG(int status);
```

**What it does:** Extracts which signal killed the process.

**Returns:** The signal number that terminated the process

**Important:** Only valid if `WIFSIGNALED(status)` is true!

**Example:**
```c
int status;
waitpid(pid, &status, 0);

if (WIFSIGNALED(status))
{
	int signal = WTERMSIG(status);
	printf("Killed by signal: %d\n", signal);
	
	if (signal == 11)
		printf("Segmentation fault!\n");
	else if (signal == 9)
		printf("Force killed\n");
}
```

---

### Common Exit Codes

| Code | Meaning | Context |
|------|---------|---------|
| `0` | Success | Normal completion |
| `1` | General error | Command failed |
| `2` | Misuse of command | Wrong arguments |
| `126` | Cannot execute | Permission denied |
| `127` | Command not found | Not in PATH |
| `128 + N` | Killed by signal N | Signal terminated process |

---

### Complete Decision Tree

```
waitpid(pid, &status, 0);

    ├─ WIFEXITED(status) → true
    │  ├─ Get exit code: WEXITSTATUS(status)
    │  └─ Check if 0 (success) or non-zero (failure)
    │
    └─ WIFSIGNALED(status) → true
       ├─ Get signal: WTERMSIG(status)
       └─ Determine what killed the process
```

---

### Example in Pipex

```c
int	main(int argc, char *argv[], char *envp[])
{
	int		status;
	pid_t	pid1, pid2;
	
	// ... fork and execute commands ...
	
	// Wait for first child
	waitpid(pid1, &status, 0);
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) != 0)
			fprintf(stderr, "Command 1 failed with code %d\n", WEXITSTATUS(status));
	}
	else if (WIFSIGNALED(status))
	{
		fprintf(stderr, "Command 1 killed by signal %d\n", WTERMSIG(status));
		return (128 + WTERMSIG(status));
	}
	
	// Wait for second child
	waitpid(pid2, &status, 0);
	if (WIFEXITED(status))
	{
		// Return the exit code of the last command
		return (WEXITSTATUS(status));
	}
	else if (WIFSIGNALED(status))
	{
		fprintf(stderr, "Command 2 killed by signal %d\n", WTERMSIG(status));
		return (128 + WTERMSIG(status));
	}
	
	return (0);
}
```

**Things to take care of:**
- Always check `WIFEXITED()` before calling `WEXITSTATUS()`
- Always check `WIFSIGNALED()` before calling `WTERMSIG()`
- Never mix them up - calling wrong macro gives garbage
- In pipex, return the exit code of the last command
- Signal numbers: SIGTERM=15, SIGKILL=9, SIGSEGV=11, SIGABRT=6

---

## Advanced Macros

These macros are less commonly needed but useful for advanced scenarios.

### Wait Options

From `<sys/wait.h>` - additional options for `waitpid()`:

```c
WNOHANG     // Don't block, return immediately
WUNTRACED   // Also return for stopped processes
WCONTINUED  // Also return for continued processes
```

**Example:**
```c
int status;

// Non-blocking wait - returns immediately
pid_t result = waitpid(pid, &status, WNOHANG);
if (result == 0)
	printf("Child still running\n");
else if (result > 0)
	printf("Child has terminated\n");
```

---

### Signal Numbers

From `<signal.h>` - common signal numbers you might encounter:

```c
SIGTERM     // Termination signal (15)
SIGKILL     // Kill signal (9) - cannot be caught
SIGSEGV     // Segmentation violation (11)
SIGABRT     // Abort signal (6)
SIGCHLD     // Child process signal (17)
SIGPIPE     // Broken pipe (13)
```

**Example in pipex:**
```c
int status;
waitpid(pid, &status, 0);

if (WIFSIGNALED(status))
{
	int sig = WTERMSIG(status);
	if (sig == SIGSEGV)
		fprintf(stderr, "Segmentation fault\n");
	else if (sig == SIGKILL)
		fprintf(stderr, "Process killed\n");
}
```

---

## Getting Environment Variables (envp)

### Method 1: Using `main()` with `envp` parameter

```c
int main(int argc, char *argv[], char *envp[])
{
	// envp is already provided
	// It's an array of strings like: ["PATH=/usr/bin:/bin", "HOME=/home/user", ..., NULL]
	
	// Access environment variables
	for (int i = 0; envp[i]; i++)
		printf("%s\n", envp[i]);
	
	// Pass to execve
	execve(path, args, envp);
	
	return (0);
}
```

**Things to take care of:**
- `envp` is NULL-terminated array
- Just pass it directly to `execve()`
- Commands need the environment to find libraries and tools

---

### Method 2: Using global `extern char **environ`

```c
#include <unistd.h>

extern char **environ;

int main(int argc, char *argv[])
{
	// environ points to environment variables
	
	// Pass to execve
	execve(path, args, environ);
	
	return (0);
}
```

**Things to take care of:**
- Must include `<unistd.h>`
- `environ` is global and always available
- Alternative if your main doesn't have `envp` parameter

---

### Why You Need envp

When you call `execve()`, the child process needs access to environment variables like:
- `PATH` - to find executables
- `HOME` - user home directory
- `USER` - username
- And many others your commands might need

**Without envp:**
```c
execve("/bin/cat", args, NULL);  // ❌ Child has NO environment - might fail
```

**With envp:**
```c
execve("/bin/cat", args, envp);  // ✅ Child inherits parent's environment
```

---

### Complete Example for Pipex

```c
int main(int argc, char *argv[], char *envp[])
{
	pid_t	pid;
	int		pipefd[2];
	
	if (argc != 5)
		return (1);
	
	pipe(pipefd);
	pid = fork();
	
	if (pid == 0)  // Child process 1
	{
		// Redirect stdout to pipe write end
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
		
		// Execute first command with environment
		execve("/bin/cat", args1, envp);
		exit(1);
	}
	else  // Parent
	{
		pid = fork();
		if (pid == 0)  // Child process 2
		{
			// Redirect stdin from pipe read end
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			close(pipefd[1]);
			
			// Execute second command with environment
			execve("/bin/grep", args2, envp);
			exit(1);
		}
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(pid, NULL, 0);
	}
	return (0);
}
```

**Key points:**
- Simply accept `envp` as third parameter in `main()`
- Pass it unchanged to `execve()` in child processes
- No modification needed - just pass it through

---

## Pipex Workflow Overview

This section provides a complete overview of how pipex works from start to finish.

### Command to Replicate

```bash
./pipex infile "cmd1" "cmd2" outfile
# Behaves like:
< infile cmd1 | cmd2 > outfile
```

### The Complete Flow

```
┌─────────────────────────────────────────────────────────────┐
│                    PARENT PROCESS                           │
│                                                             │
│  1. Validate arguments (argc == 5)                         │
│  2. Open infile (O_RDONLY)                                 │
│  3. Open outfile (O_WRONLY | O_CREAT | O_TRUNC, 0644)     │
│  4. Create pipe: pipe(pipefd)                              │
│                                                             │
│     pipefd[0] = read end                                   │
│     pipefd[1] = write end                                  │
│                                                             │
│  5. Fork first child (pid1)                                │
│                                                             │
│     ┌──────────────────────────────────────┐              │
│     │     CHILD PROCESS 1 (cmd1)           │              │
│     │                                       │              │
│     │  • Redirect stdin from infile:       │              │
│     │    dup2(infd, STDIN_FILENO)         │              │
│     │                                       │              │
│     │  • Redirect stdout to pipe:          │              │
│     │    dup2(pipefd[1], STDOUT_FILENO)   │              │
│     │                                       │              │
│     │  • Close all file descriptors:       │              │
│     │    close(infd)                       │              │
│     │    close(pipefd[0])                  │              │
│     │    close(pipefd[1])                  │              │
│     │                                       │              │
│     │  • Execute cmd1:                     │              │
│     │    execve(cmd1_path, args1, envp)   │              │
│     │                                       │              │
│     └──────────────────────────────────────┘              │
│                                                             │
│  6. Fork second child (pid2)                               │
│                                                             │
│     ┌──────────────────────────────────────┐              │
│     │     CHILD PROCESS 2 (cmd2)           │              │
│     │                                       │              │
│     │  • Redirect stdin from pipe:         │              │
│     │    dup2(pipefd[0], STDIN_FILENO)    │              │
│     │                                       │              │
│     │  • Redirect stdout to outfile:       │              │
│     │    dup2(outfd, STDOUT_FILENO)       │              │
│     │                                       │              │
│     │  • Close all file descriptors:       │              │
│     │    close(outfd)                      │              │
│     │    close(pipefd[0])                  │              │
│     │    close(pipefd[1])                  │              │
│     │                                       │              │
│     │  • Execute cmd2:                     │              │
│     │    execve(cmd2_path, args2, envp)   │              │
│     │                                       │              │
│     └──────────────────────────────────────┘              │
│                                                             │
│  7. Close pipe in parent:                                  │
│     close(pipefd[0])                                       │
│     close(pipefd[1])                                       │
│                                                             │
│  8. Wait for children:                                     │
│     waitpid(pid1, &status1, 0)                            │
│     waitpid(pid2, &status2, 0)                            │
│                                                             │
│  9. Return exit code of last command:                      │
│     return (WEXITSTATUS(status2))                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Key Steps Explained

#### 1. **Open Files**
```c
int infd = open(argv[1], O_RDONLY);
if (infd < 0)
	error_exit("Cannot open input file");

int outfd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (outfd < 0)
	error_exit("Cannot open output file");
```

#### 2. **Create Pipe**
```c
int pipefd[2];
if (pipe(pipefd) < 0)
	error_exit("Pipe failed");
```

#### 3. **Fork and Execute First Command**
```c
pid_t pid1 = fork();
if (pid1 == 0)  // Child 1
{
	dup2(infd, STDIN_FILENO);       // Read from infile
	dup2(pipefd[1], STDOUT_FILENO); // Write to pipe
	close(infd);
	close(pipefd[0]);
	close(pipefd[1]);
	
	// Parse and execute cmd1
	char **args = parse_command(argv[2]);
	char *path = find_in_path(args[0], envp);
	execve(path, args, envp);
	error_exit("Command not found");
}
```

#### 4. **Fork and Execute Second Command**
```c
pid_t pid2 = fork();
if (pid2 == 0)  // Child 2
{
	dup2(pipefd[0], STDIN_FILENO);  // Read from pipe
	dup2(outfd, STDOUT_FILENO);     // Write to outfile
	close(outfd);
	close(pipefd[0]);
	close(pipefd[1]);
	
	// Parse and execute cmd2
	char **args = parse_command(argv[3]);
	char *path = find_in_path(args[0], envp);
	execve(path, args, envp);
	error_exit("Command not found");
}
```

#### 5. **Clean Up and Wait**
```c
// Parent must close pipe ends
close(pipefd[0]);
close(pipefd[1]);
close(infd);
close(outfd);

// Wait for both children
int status1, status2;
waitpid(pid1, &status1, 0);
waitpid(pid2, &status2, 0);

// Return exit code of last command
if (WIFEXITED(status2))
	return (WEXITSTATUS(status2));
return (1);
```

### Critical Points to Remember

1. **Close All Unnecessary File Descriptors**
   - Each child must close the pipe ends it doesn't use
   - Forgetting to close causes deadlocks (pipe never receives EOF)

2. **Order Matters**
   - Create pipe BEFORE forking
   - Redirect BEFORE calling execve
   - Close AFTER redirecting

3. **Finding Commands in PATH**
   - Parse PATH environment variable
   - Try each directory until command found
   - Handle absolute paths (`/bin/cat`) and relative paths

4. **Error Handling**
   - Check every system call return value
   - Print meaningful error messages to stderr
   - Use appropriate exit codes

5. **Memory Management**
   - Free all allocated memory before execve
   - Or accept that execve replaces the process anyway
   - Parent must free any allocations

### Data Flow Visualization

```
Input File (infile)
       │
       │ read by fork/dup2
       ▼
   ┌─────────┐
   │  cmd1   │
   └─────────┘
       │
       │ write to pipe[1]
       ▼
   ┌─────────┐
   │  PIPE   │
   └─────────┘
       │
       │ read from pipe[0]
       ▼
   ┌─────────┐
   │  cmd2   │
   └─────────┘
       │
       │ write by fork/dup2
       ▼
Output File (outfile)
```

### Example: Complete Minimal Implementation

```c
int main(int argc, char *argv[], char *envp[])
{
	int		pipefd[2];
	pid_t	pid1, pid2;
	int		infd, outfd;
	
	if (argc != 5)
		return (write(2, "Usage: ./pipex infile cmd1 cmd2 outfile\n", 41), 1);
	
	infd = open(argv[1], O_RDONLY);
	outfd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	pipe(pipefd);
	
	pid1 = fork();
	if (pid1 == 0)
	{
		dup2(infd, STDIN_FILENO);
		dup2(pipefd[1], STDOUT_FILENO);
		close(infd);
		close(outfd);
		close(pipefd[0]);
		close(pipefd[1]);
		execute_command(argv[2], envp);  // Your function to parse & execve
	}
	
	pid2 = fork();
	if (pid2 == 0)
	{
		dup2(pipefd[0], STDIN_FILENO);
		dup2(outfd, STDOUT_FILENO);
		close(infd);
		close(outfd);
		close(pipefd[0]);
		close(pipefd[1]);
		execute_command(argv[3], envp);  // Your function to parse & execve
	}
	
	close(infd);
	close(outfd);
	close(pipefd[0]);
	close(pipefd[1]);
	
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
	
	return (0);
}
```

### Testing Your Implementation

```bash
# Create test file
echo "hello world" > input.txt

# Test basic functionality
./pipex input.txt "cat" "wc -l" output.txt
cat output.txt  # Should print: 1

# Compare with shell
< input.txt cat | wc -l > expected.txt
diff output.txt expected.txt  # Should be identical

# Test with grep
echo -e "apple\nbanana\napricot" > fruits.txt
./pipex fruits.txt "cat" "grep ap" output.txt
cat output.txt  # Should show: apple, apricot

# Test error handling
./pipex nonexistent.txt "cat" "wc" output.txt  # Should handle gracefully
./pipex input.txt "invalid_cmd" "wc" output.txt  # Should report error
```

---

## Bonus: Multiple Pipes

For the bonus part, you need to handle more than 2 commands.

### Command Format

```bash
./pipex infile "cmd1" "cmd2" "cmd3" "cmd4" outfile
# Behaves like:
< infile cmd1 | cmd2 | cmd3 | cmd4 > outfile
```

### Data Flow with 2 Commands (Mandatory)

```
Input File
    │
    ▼
┌─────────┐
│  cmd1   │  Process 1
└─────────┘
    │
    │ pipe[0/1]
    ▼
┌─────────┐
│  cmd2   │  Process 2
└─────────┘
    │
    ▼
Output File
```

### Data Flow with 3 Commands

```
Input File
    │
    ▼
┌─────────┐
│  cmd1   │  Process 1
└─────────┘
    │
    │ pipe1[0/1]
    ▼
┌─────────┐
│  cmd2   │  Process 2
└─────────┘
    │
    │ pipe2[0/1]
    ▼
┌─────────┐
│  cmd3   │  Process 3
└─────────┘
    │
    ▼
Output File
```

### Data Flow with 4 Commands

```
Input File (argv[1])
    │
    ▼
┌─────────┐
│  cmd1   │  Process 1: reads from infile, writes to pipe1[1]
└─────────┘
    │
    │ pipe1[0/1]
    ▼
┌─────────┐
│  cmd2   │  Process 2: reads from pipe1[0], writes to pipe2[1]
└─────────┘
    │
    │ pipe2[0/1]
    ▼
┌─────────┐
│  cmd3   │  Process 3: reads from pipe2[0], writes to pipe3[1]
└─────────┘
    │
    │ pipe3[0/1]
    ▼
┌─────────┐
│  cmd4   │  Process 4: reads from pipe3[0], writes to outfile
└─────────┘
    │
    ▼
Output File (argv[argc-1])
```

### Detailed Flow with N Commands

```
For N commands, you need N-1 pipes

Example: 5 commands = 4 pipes

infile → [cmd1] → pipe1 → [cmd2] → pipe2 → [cmd3] → pipe3 → [cmd4] → pipe4 → [cmd5] → outfile
```

### Pattern Recognition

```
Command Index:        0      1      2      3      4
                      │      │      │      │      │
                   ┌──┴──┐┌──┴──┐┌──┴──┐┌──┴──┐┌──┴──┐
                   │cmd1 ││cmd2 ││cmd3 ││cmd4 ││cmd5 │
                   └──┬──┘└──┬──┘└──┬──┘└──┬──┘└──┬──┘
                      │      │      │      │      │
Pipe Used:        none   pipe0  pipe1  pipe2  pipe3   none
(for reading)

Pipe Used:        pipe0  pipe1  pipe2  pipe3  none    none
(for writing)

First command:    Reads from infile, writes to first pipe
Middle commands:  Read from previous pipe, write to next pipe
Last command:     Reads from last pipe, writes to outfile
```

### Implementation Strategy

```c
int main(int argc, char *argv[], char *envp[])
{
	int		num_cmds = argc - 3;  // Total commands
	int		num_pipes = num_cmds - 1;  // Pipes needed
	int		pipes[num_pipes][2];  // Array of pipes
	pid_t	pids[num_cmds];  // Array of process IDs
	int		i;
	
	// Create all pipes
	for (i = 0; i < num_pipes; i++)
		pipe(pipes[i]);
	
	// Fork and execute each command
	for (i = 0; i < num_cmds; i++)
	{
		pids[i] = fork();
		if (pids[i] == 0)
		{
			// First command: read from infile
			if (i == 0)
			{
				int infd = open(argv[1], O_RDONLY);
				dup2(infd, STDIN_FILENO);
				close(infd);
			}
			// Middle commands: read from previous pipe
			else
			{
				dup2(pipes[i - 1][0], STDIN_FILENO);
			}
			
			// Last command: write to outfile
			if (i == num_cmds - 1)
			{
				int outfd = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
				dup2(outfd, STDOUT_FILENO);
				close(outfd);
			}
			// Middle commands: write to next pipe
			else
			{
				dup2(pipes[i][1], STDOUT_FILENO);
			}
			
			// Close ALL pipe file descriptors in child
			for (int j = 0; j < num_pipes; j++)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			
			// Execute command
			execute_command(argv[2 + i], envp);
			exit(1);
		}
	}
	
	// Parent: close all pipes
	for (i = 0; i < num_pipes; i++)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
	}
	
	// Wait for all children
	for (i = 0; i < num_cmds; i++)
		waitpid(pids[i], NULL, 0);
	
	return (0);
}
```

### Visual Example: 4 Commands with Pipe Management

```
./pipex infile "cat" "grep a" "sort" "uniq" outfile

Step 1: Create 3 pipes (4 commands - 1)
    pipe[0] = {pipe[0][0], pipe[0][1]}
    pipe[1] = {pipe[1][0], pipe[1][1]}
    pipe[2] = {pipe[2][0], pipe[2][1]}

Step 2: Fork command 1 (cat)
    Read:  infile
    Write: pipe[0][1]
    Close: pipe[0][0], pipe[0][1], pipe[1][0], pipe[1][1], pipe[2][0], pipe[2][1]

Step 3: Fork command 2 (grep a)
    Read:  pipe[0][0]
    Write: pipe[1][1]
    Close: pipe[0][0], pipe[0][1], pipe[1][0], pipe[1][1], pipe[2][0], pipe[2][1]

Step 4: Fork command 3 (sort)
    Read:  pipe[1][0]
    Write: pipe[2][1]
    Close: pipe[0][0], pipe[0][1], pipe[1][0], pipe[1][1], pipe[2][0], pipe[2][1]

Step 5: Fork command 4 (uniq)
    Read:  pipe[2][0]
    Write: outfile
    Close: pipe[0][0], pipe[0][1], pipe[1][0], pipe[1][1], pipe[2][0], pipe[2][1]

Step 6: Parent closes all pipes and waits
```

### Key Points for Multiple Pipes

1. **Number of pipes = Number of commands - 1**
   - 2 commands → 1 pipe
   - 3 commands → 2 pipes
   - 4 commands → 3 pipes
   - N commands → N-1 pipes

2. **Each child must close ALL pipe ends**
   - Even the ones it doesn't use
   - This prevents deadlocks

3. **Pattern for command i:**
   - If first (i == 0): read from infile
   - If last (i == num_cmds - 1): write to outfile
   - Otherwise: read from pipe[i-1][0], write to pipe[i][1]

4. **Parent must close all pipes**
   - After all forks
   - Before waiting for children

### Testing Multiple Pipes

```bash
# Test with 3 commands
echo -e "apple\nbanana\napple\napricot" > fruits.txt
./pipex fruits.txt "cat" "sort" "uniq" output.txt
cat output.txt  # Should show: apple, apricot, banana (sorted, unique)

# Test with 4 commands
./pipex fruits.txt "cat" "grep a" "sort" "uniq" output.txt

# Compare with shell
< fruits.txt cat | grep a | sort | uniq > expected.txt
diff output.txt expected.txt  # Should match
```

---

## Bonus: Here_doc Support

### What is Here_doc?

**Here_doc** (heredoc) is a special input method in shell where you type input directly into the terminal until you type a specific delimiter word.

### Shell Syntax

In bash, here_doc looks like this:

```bash
cat << LIMITER
line 1
line 2
line 3
LIMITER
```

The shell reads input until it sees the word `LIMITER` on its own line, then passes all that input to the command.

### Pipex Here_doc Syntax

```bash
./pipex here_doc LIMITER cmd1 cmd2 outfile
```

**What this means:**
- Instead of reading from an input file, your program reads from **stdin** (user types input)
- Keep reading lines until the user types the `LIMITER` word
- Pass all that input to `cmd1`
- The output of `cmd1 | cmd2` is **appended** to `outfile` (not truncated)

### Shell Equivalent

```bash
./pipex here_doc EOF "grep hello" "wc -l" output.txt
# Is equivalent to:
grep hello | wc -l >> output.txt
# Where you type input and end with "EOF"
```

### Example Session

```bash
$ ./pipex here_doc END "grep a" "wc -l" output.txt
> apple          # You type this
> banana         # You type this
> cherry         # You type this
> apricot        # You type this
> END            # You type this - program stops reading
$
$ cat output.txt
3                # 3 lines contained 'a'
```

### Key Differences from Normal Mode

| Feature | Normal Mode | Here_doc Mode |
|---------|-------------|---------------|
| Input source | File (argv[1]) | stdin (terminal) |
| Output mode | Truncate (`O_TRUNC`) | Append (`O_APPEND`) |
| Arguments | 5 args (infile cmd1 cmd2 outfile) | 5 args (here_doc LIMITER cmd1 cmd2 outfile) |
| First arg check | Open as file | Check if == "here_doc" |

### Implementation Strategy

```c
int main(int argc, char *argv[], char *envp[])
{
	if (argc != 5)
		return (error_message());
	
	// Check if first argument is "here_doc"
	if (strcmp(argv[1], "here_doc") == 0)
	{
		// HERE_DOC MODE
		char	*limiter = argv[2];  // Delimiter word
		char	*cmd1 = argv[3];
		char	*cmd2 = argv[4];
		char	*outfile = argv[5];  // Wait... only 4 args after program name!
		
		handle_heredoc(limiter, cmd1, cmd2, outfile, envp);
	}
	else
	{
		// NORMAL MODE
		char	*infile = argv[1];
		char	*cmd1 = argv[2];
		char	*cmd2 = argv[3];
		char	*outfile = argv[4];
		
		handle_normal(infile, cmd1, cmd2, outfile, envp);
	}
	
	return (0);
}
```

**Wait!** The argument count is still 5 total:
- `argv[0]` = "./pipex"
- `argv[1]` = "here_doc"
- `argv[2]` = LIMITER
- `argv[3]` = cmd1
- `argv[4]` = cmd2
- `argv[5]` = outfile

So actually you need **6 arguments** for here_doc mode!

### Correct Argument Handling

```c
int main(int argc, char *argv[], char *envp[])
{
	// Check for here_doc mode
	if (argc > 1 && strcmp(argv[1], "here_doc") == 0)
	{
		if (argc != 6)  // here_doc needs 6 total arguments
		{
			write(2, "Usage: ./pipex here_doc LIMITER cmd1 cmd2 outfile\n", 51);
			return (1);
		}
		handle_heredoc(argv[2], argv[3], argv[4], argv[5], envp);
	}
	else
	{
		if (argc != 5)  // normal mode needs 5 total arguments
		{
			write(2, "Usage: ./pipex infile cmd1 cmd2 outfile\n", 41);
			return (1);
		}
		handle_normal(argv[1], argv[2], argv[3], argv[4], envp);
	}
	
	return (0);
}
```

### Here_doc Implementation Details

#### Step 1: Read Input Until Limiter

```c
void	read_heredoc(char *limiter, int write_fd)
{
	char	*line;
	size_t	limiter_len;
	
	limiter_len = strlen(limiter);
	
	while (1)
	{
		write(STDOUT_FILENO, "> ", 2);  // Show prompt
		line = get_next_line(STDIN_FILENO);  // Or read line by line
		
		if (!line)  // EOF (Ctrl+D)
			break;
		
		// Check if line matches limiter
		if (strncmp(line, limiter, limiter_len) == 0 && 
		    line[limiter_len] == '\n')
		{
			free(line);
			break;  // Stop reading
		}
		
		// Write line to pipe
		write(write_fd, line, strlen(line));
		free(line);
	}
}
```

#### Step 2: Create Pipe and Fork

```c
void	handle_heredoc(char *limiter, char *cmd1, char *cmd2, 
                       char *outfile, char *envp[])
{
	int		pipefd[2];
	pid_t	pid1, pid2;
	
	pipe(pipefd);
	
	// Read heredoc input into pipe
	read_heredoc(limiter, pipefd[1]);
	close(pipefd[1]);  // Done writing to pipe
	
	// Fork first command
	pid1 = fork();
	if (pid1 == 0)
	{
		dup2(pipefd[0], STDIN_FILENO);  // Read from pipe
		close(pipefd[0]);
		// ... execute cmd1
	}
	
	// Fork second command
	pid2 = fork();
	if (pid2 == 0)
	{
		// Open output file in APPEND mode
		int outfd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
		dup2(outfd, STDOUT_FILENO);
		close(outfd);
		// ... execute cmd2
	}
	
	close(pipefd[0]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
}
```

### Key Implementation Points

1. **Prompt for input:** Show `> ` or `pipe heredoc> ` to indicate waiting for input

2. **Read line by line:** Use `get_next_line()` or read character by character

3. **Compare with limiter:** Stop when input line exactly matches the limiter

4. **Handle Ctrl+D:** Treat EOF as end of input (like the limiter)

5. **Use O_APPEND:** Open output file with `O_APPEND` flag instead of `O_TRUNC`

6. **Temporary storage:** Write heredoc input to a pipe (or temporary file)

### Alternative: Using Temporary File

Some implementations create a temporary file:

```c
void	handle_heredoc(char *limiter, char *cmd1, char *cmd2, 
                       char *outfile, char *envp[])
{
	int		tmpfd;
	char	*line;
	
	// Create temporary file
	tmpfd = open(".heredoc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	
	// Read input and write to temp file
	while (1)
	{
		write(1, "> ", 2);
		line = get_next_line(0);
		if (!line || strcmp(line, limiter) == 0)
			break;
		write(tmpfd, line, strlen(line));
		free(line);
	}
	close(tmpfd);
	
	// Now use .heredoc_tmp as input file
	handle_normal(".heredoc_tmp", cmd1, cmd2, outfile, envp);
	
	// Clean up
	unlink(".heredoc_tmp");
}
```

### Complete Here_doc Example

```c
// Simplified here_doc handling
void	heredoc_mode(char *lim, char *cmd1, char *cmd2, char *out, char **envp)
{
	int		pipefd[2];
	char	*line;
	
	pipe(pipefd);
	
	// Read until limiter
	write(1, "heredoc> ", 9);
	while ((line = get_next_line(0)))
	{
		if (strcmp(line, lim) == 0)
		{
			free(line);
			break;
		}
		write(pipefd[1], line, strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
		write(1, "heredoc> ", 9);
	}
	close(pipefd[1]);
	
	// Fork and execute commands (read from pipefd[0])
	// Output to file with O_APPEND
	execute_pipeline(pipefd[0], cmd1, cmd2, out, O_APPEND, envp);
	
	close(pipefd[0]);
}
```

### Testing Here_doc

```bash
# Test basic here_doc
$ ./pipex here_doc EOF "cat" "wc -l" output.txt
heredoc> hello
heredoc> world
heredoc> test
heredoc> EOF
$ cat output.txt
3

# Test with grep
$ ./pipex here_doc END "grep a" "wc -w" output.txt
heredoc> apple banana
heredoc> cherry apricot
heredoc> grape orange
heredoc> END
$ cat output.txt
4

# Test append mode (run twice)
$ ./pipex here_doc STOP "cat" "cat" output.txt
heredoc> first run
heredoc> STOP
$ ./pipex here_doc STOP "cat" "cat" output.txt
heredoc> second run
heredoc> STOP
$ cat output.txt
first run
second run
```

### Summary

**"here_doc" mode means:**
- First argument is literally the string `"here_doc"`
- Second argument is the delimiter (LIMITER)
- Read from stdin until LIMITER is typed
- Output is **appended** (not truncated) using `O_APPEND`
- Need 6 total arguments instead of 5

```bash
./pipex here_doc LIMITER cmd1 cmd2 outfile
# Equivalent to:
cmd1 | cmd2 >> outfile
# With input from stdin until LIMITER
```

---

**Congratulations!** You now have a complete understanding of pipex. Good luck with your implementation! 🚀