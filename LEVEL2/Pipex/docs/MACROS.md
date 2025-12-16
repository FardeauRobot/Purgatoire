# Macros and Constants Reference

Complete reference for all macros and constants used in pipex.

## Table of Contents

1. [Authorized Headers](#authorized-headers)
2. [Standard File Descriptors](#standard-file-descriptors)
3. [File Control Flags](#file-control-flags)
4. [File Permissions](#file-permissions)
5. [Access Modes](#access-modes)
6. [Status Parsing Macros](#status-parsing-macros)
7. [Wait Options](#wait-options)
8. [Signal Numbers](#signal-numbers)

---

## Authorized Headers

| Header | Purpose | Functions/Macros Used |
|--------|---------|-----------------|
| `<unistd.h>` | POSIX API | fork, execve, pipe, dup, dup2, close, read, write, access, exit, waitpid, STDIN_FILENO, STDOUT_FILENO |
| `<stdlib.h>` | Standard library | malloc, free, exit |
| `<stdio.h>` | Standard I/O | printf, fprintf, perror |
| `<string.h>` | String operations | strlen, strcpy, strdup, strcmp, strtok |
| `<sys/types.h>` | Type definitions | pid_t, mode_t, ssize_t, etc. |
| `<sys/wait.h>` | Process waiting | wait, waitpid, WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG |
| `<fcntl.h>` | File control | open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_APPEND |
| `<errno.h>` | Error handling | errno |
| `<signal.h>` | Signal handling | SIGTERM, SIGKILL, SIGSEGV, etc. |

---

## Standard File Descriptors

From `<unistd.h>` - represent stdin, stdout, and stderr:

```c
STDIN_FILENO    // Standard input (0)
STDOUT_FILENO   // Standard output (1)
STDERR_FILENO   // Standard error (2)
```

**Why use them:** Makes code more readable than using `0`, `1`, `2` directly.

**Example:**
```c
dup2(fd, STDOUT_FILENO);        // Redirect stdout to a file
dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin from pipe
```

---

## File Control Flags

From `<fcntl.h>` - used with `open()` function:

### Access Modes (pick exactly one)

```c
O_RDONLY    // Read only
O_WRONLY    // Write only
O_RDWR      // Read and write
```

### Creation/Modification Flags (combine with |)

```c
O_CREAT     // Create file if it doesn't exist
O_TRUNC     // Truncate (empty) file if it exists
O_APPEND    // Append to end of file
O_EXCL      // Fail if file exists (with O_CREAT)
```

**Common combinations for pipex:**
```c
// Open input file for reading
int infd = open(infile, O_RDONLY);

// Open output file for writing (create if needed, truncate if exists)
int outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

// Open output file in append mode (for here_doc)
int outfd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
```

---

## File Permissions

Octal notation for file permissions with `open()`:

```c
0644    // Owner: read+write, Group: read, Others: read
0755    // Owner: read+write+execute, Others: read+execute
0600    // Owner: read+write only
0777    // Everyone: read+write+execute
```

**Example:**
```c
int fd = open("file.txt", O_WRONLY | O_CREAT, 0644);
```

**Breakdown of 0644:**
- `0` = octal prefix
- `6` = owner (4 read + 2 write = 6)
- `4` = group (4 read = 4)
- `4` = others (4 read = 4)

---

## Access Modes

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

if (access("/bin/cat", X_OK) == 0)
	printf("Command is executable\n");
```

---

## Status Parsing Macros

From `<sys/wait.h>` - used to interpret child process exit status.

### Where They Come From

These macros extract information from the status integer returned by `wait()` or `waitpid()`.

### The Four Main Macros

#### `WIFEXITED(status)` - "Was It Exited?"

```c
int WIFEXITED(int status);
```

**Returns:** Non-zero (true) if process exited normally, `0` (false) if killed by signal.

**Example:**
```c
int status;
waitpid(pid, &status, 0);

if (WIFEXITED(status))
	printf("Process exited normally\n");
```

---

#### `WEXITSTATUS(status)` - "What Was The Exit Status?"

```c
int WEXITSTATUS(int status);
```

**Returns:** The exit code (0-255) from the child process.

**Important:** Only valid if `WIFEXITED(status)` is true!

**Example:**
```c
if (WIFEXITED(status))
{
	int exit_code = WEXITSTATUS(status);
	printf("Child exited with code: %d\n", exit_code);
}
```

---

#### `WIFSIGNALED(status)` - "Was It Signaled?"

```c
int WIFSIGNALED(int status);
```

**Returns:** Non-zero (true) if process was killed by a signal.

**Example:**
```c
if (WIFSIGNALED(status))
	printf("Process was killed by a signal\n");
```

---

#### `WTERMSIG(status)` - "What Was The Terminating Signal?"

```c
int WTERMSIG(int status);
```

**Returns:** The signal number that terminated the process.

**Important:** Only valid if `WIFSIGNALED(status)` is true!

**Example:**
```c
if (WIFSIGNALED(status))
{
	int signal = WTERMSIG(status);
	printf("Killed by signal: %d\n", signal);
}
```

---

### Complete Status Parsing Example

```c
int status;
waitpid(pid, &status, 0);

if (WIFEXITED(status))
{
	int exit_code = WEXITSTATUS(status);
	if (exit_code == 0)
		printf("Success!\n");
	else
		printf("Failed with code: %d\n", exit_code);
}
else if (WIFSIGNALED(status))
{
	int signal = WTERMSIG(status);
	printf("Killed by signal: %d\n", signal);
}
```

### Common Exit Codes

| Code | Meaning |
|------|---------|
| `0` | Success |
| `1` | General error |
| `2` | Misuse of shell command |
| `126` | Command cannot execute |
| `127` | Command not found |
| `128 + N` | Process killed by signal N |

---

## Wait Options

From `<sys/wait.h>` - additional options for `waitpid()`:

```c
WNOHANG     // Don't block, return immediately
WUNTRACED   // Also return for stopped processes
WCONTINUED  // Also return for continued processes
```

**Example:**
```c
// Non-blocking wait
pid_t result = waitpid(pid, &status, WNOHANG);
if (result == 0)
	printf("Child still running\n");
else if (result > 0)
	printf("Child has terminated\n");
```

---

## Signal Numbers

From `<signal.h>` - common signals that can kill processes:

```c
SIGTERM     // Termination signal (15)
SIGKILL     // Kill signal (9) - cannot be caught
SIGSEGV     // Segmentation violation (11)
SIGABRT     // Abort signal (6)
SIGCHLD     // Child process signal (17)
SIGPIPE     // Broken pipe (13)
SIGINT      // Interrupt (Ctrl+C) (2)
```

**Example:**
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
	else if (sig == SIGPIPE)
		fprintf(stderr, "Broken pipe\n");
}
```

---

## Quick Reference Table

### Pipex Essential Macros

| Macro | Value | Usage |
|-------|-------|-------|
| `STDIN_FILENO` | 0 | Input redirection |
| `STDOUT_FILENO` | 1 | Output redirection |
| `STDERR_FILENO` | 2 | Error messages |
| `O_RDONLY` | 0 | Open for reading |
| `O_WRONLY` | 1 | Open for writing |
| `O_CREAT` | 64 | Create if doesn't exist |
| `O_TRUNC` | 512 | Truncate to zero |
| `O_APPEND` | 1024 | Append to end |
| `F_OK` | 0 | File exists check |
| `X_OK` | 1 | Executable check |

---

## Practical Examples

### Opening Files in Pipex

```c
// Input file (mandatory part)
int infd = open(argv[1], O_RDONLY);
if (infd < 0)
{
	perror("Error opening input file");
	exit(1);
}

// Output file (mandatory part) - truncate
int outfd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (outfd < 0)
{
	perror("Error opening output file");
	exit(1);
}

// Output file (here_doc bonus) - append
int outfd = open(argv[5], O_WRONLY | O_CREAT | O_APPEND, 0644);
```

### Redirecting in Child Processes

```c
// Child 1: Read from infile, write to pipe
dup2(infd, STDIN_FILENO);       // stdin = infile
dup2(pipefd[1], STDOUT_FILENO); // stdout = pipe write end
close(infd);
close(pipefd[0]);
close(pipefd[1]);

// Child 2: Read from pipe, write to outfile
dup2(pipefd[0], STDIN_FILENO);  // stdin = pipe read end
dup2(outfd, STDOUT_FILENO);     // stdout = outfile
close(outfd);
close(pipefd[0]);
close(pipefd[1]);
```

### Checking Exit Status

```c
int status;
waitpid(pid, &status, 0);

// Check if exited normally
if (WIFEXITED(status))
	return (WEXITSTATUS(status));  // Return child's exit code

// Check if killed by signal
if (WIFSIGNALED(status))
	return (128 + WTERMSIG(status));  // Shell convention

return (1);  // Generic error
```
