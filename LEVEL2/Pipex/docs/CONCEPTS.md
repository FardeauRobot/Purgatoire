# Key Concepts for Pipex

Understanding these concepts is essential for implementing pipex successfully.

## Table of Contents

1. [Processes and Process Management](#1-processes-and-process-management)
2. [Pipes (Inter-Process Communication)](#2-pipes-inter-process-communication)
3. [File Descriptors and Redirection](#3-file-descriptors-and-redirection)
4. [Command Execution](#4-command-execution)
5. [Environment Variables](#5-environment-variables)
6. [Process Waiting and Exit Codes](#6-process-waiting-and-exit-codes)
7. [File I/O Operations](#7-file-io-operations)
8. [Error Handling](#8-error-handling)
9. [Memory Management](#9-memory-management)

---

## 1. Processes and Process Management

### What is a Process?

A process is a running instance of a program. Each process has:
- Its own memory space
- Its own set of file descriptors
- A unique Process ID (PID)
- Parent-child relationships

### fork() - Creating Child Processes

```c
pid_t fork(void);
```

**What happens when you fork:**
1. Creates an exact copy of the current process
2. Returns `0` in the child process
3. Returns child's PID in the parent process
4. Child gets copy of all file descriptors

**Example:**
```c
pid_t pid = fork();

if (pid == 0)
{
	// This is the child process
	printf("I'm the child!\n");
}
else if (pid > 0)
{
	// This is the parent process
	printf("I'm the parent, my child's PID is %d\n", pid);
}
```

**Why it matters in pipex:** Each command runs in its own child process, allowing them to run concurrently.

---

## 2. Pipes (Inter-Process Communication)

### What is a Pipe?

A pipe is a unidirectional data channel that connects the output of one process to the input of another.

```c
int pipefd[2];
pipe(pipefd);
// pipefd[0] = read end
// pipefd[1] = write end
```

### How Pipes Work

```
Process 1                          Process 2
  stdout ──→ pipe[1] ──→ pipe[0] ──→ stdin
           (write end)  (read end)
```

**Key Rules:**
1. Data written to `pipefd[1]` can be read from `pipefd[0]`
2. Pipes are unidirectional (one-way only)
3. If all write ends are closed, reading gives EOF
4. If all read ends are closed, writing causes SIGPIPE

### Example

```c
int pipefd[2];
pipe(pipefd);

pid_t pid = fork();
if (pid == 0)  // Child writes
{
	close(pipefd[0]);  // Close read end
	write(pipefd[1], "hello", 5);
	close(pipefd[1]);
	exit(0);
}
else  // Parent reads
{
	close(pipefd[1]);  // Close write end
	char buf[10];
	read(pipefd[0], buf, 5);
	close(pipefd[0]);
	printf("Got: %s\n", buf);
}
```

**Why it matters in pipex:** The core mechanism for connecting command outputs to inputs.

---

## 3. File Descriptors and Redirection

### What are File Descriptors?

File descriptors are small integers that represent open files or communication channels.

**Standard descriptors:**
- `0` (STDIN_FILENO) = standard input
- `1` (STDOUT_FILENO) = standard output
- `2` (STDERR_FILENO) = standard error

### dup2() - Redirecting File Descriptors

```c
int dup2(int oldfd, int newfd);
```

Makes `newfd` point to whatever `oldfd` points to.

**Example: Redirecting stdout to a file**
```c
int fd = open("output.txt", O_WRONLY | O_CREAT, 0644);
dup2(fd, STDOUT_FILENO);  // Now stdout writes to output.txt
close(fd);  // Can close original fd

printf("This goes to output.txt\n");
```

**Example: Redirecting stdin from a pipe**
```c
int pipefd[2];
pipe(pipefd);

// In child process:
dup2(pipefd[0], STDIN_FILENO);  // stdin now reads from pipe
close(pipefd[0]);
close(pipefd[1]);

// Now any read from stdin actually reads from the pipe
```

**Why it matters in pipex:** Essential for connecting pipes to stdin/stdout before executing commands.

---

## 4. Command Execution

### execve() - Running Programs

```c
int execve(const char *path, char *argv[], char *envp[]);
```

**What execve does:**
1. Replaces the current process with a new program
2. Does NOT return on success (process is replaced)
3. Returns -1 only on error
4. Inherits file descriptors from parent

### Arguments Explained

**path:** Full path to executable (`"/bin/cat"`, not just `"cat"`)

**argv:** Array of argument strings, NULL-terminated
```c
char *args[] = {"cat", "-e", NULL};
execve("/bin/cat", args, envp);
```

**envp:** Array of environment variables, NULL-terminated
```c
int main(int argc, char *argv[], char *envp[])
{
	// Just pass envp through
	execve("/bin/cat", args, envp);
}
```

### Finding Commands in PATH

Commands like `cat` or `grep` need to be found in PATH:

```c
char *find_command(char *cmd, char *envp[])
{
	char *path_env = get_path_from_envp(envp);  // Get "PATH=/bin:/usr/bin"
	char **paths = split(path_env, ':');  // Split into ["/bin", "/usr/bin"]
	
	for (int i = 0; paths[i]; i++)
	{
		char *full_path = join(paths[i], cmd);  // "/bin/cat"
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
	}
	return (NULL);  // Command not found
}
```

**Why it matters in pipex:** You must find the full path to commands before calling execve.

---

## 5. Environment Variables

### What is envp?

Environment variables are key=value pairs that programs inherit from their parent.

**Common variables:**
- `PATH=/usr/bin:/bin` - where to find executables
- `HOME=/home/user` - user's home directory
- `USER=username` - current user

### Getting envp

**Method 1: main parameter**
```c
int main(int argc, char *argv[], char *envp[])
{
	// envp is available here
	execve(path, args, envp);
}
```

**Method 2: global variable**
```c
extern char **environ;

int main(int argc, char *argv[])
{
	execve(path, args, environ);
}
```

### Parsing PATH

```c
char *get_path(char *envp[])
{
	for (int i = 0; envp[i]; i++)
	{
		if (strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);  // Skip "PATH="
	}
	return (NULL);
}
```

**Why it matters in pipex:** Commands need PATH to find executables, and environment variables must be passed to children.

---

## 6. Process Waiting and Exit Codes

### wait() and waitpid()

Parent processes must wait for children to avoid zombie processes.

```c
int status;
pid_t pid = waitpid(child_pid, &status, 0);

if (WIFEXITED(status))
	printf("Child exited with code: %d\n", WEXITSTATUS(status));
```

### Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1-255 | Error (different meanings) |
| 127 | Command not found |
| 126 | Permission denied |

### Pipex Should Return

Return the exit code of the **last command** in the pipeline:

```c
waitpid(pid1, &status1, 0);
waitpid(pid2, &status2, 0);

// Return exit code of cmd2 (last command)
if (WIFEXITED(status2))
	return (WEXITSTATUS(status2));
return (1);
```

**Why it matters in pipex:** Proper exit codes help debug issues and match shell behavior.

---

## 7. File I/O Operations

### Opening Files

```c
// Input file - read only
int infd = open(argv[1], O_RDONLY);

// Output file - write, create if needed, truncate
int outfd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

### Closing Files

**Always close file descriptors you don't need!**

```c
// In child 1 (writes to pipe, doesn't read)
close(pipefd[0]);  // Close read end
dup2(pipefd[1], STDOUT_FILENO);
close(pipefd[1]);  // Close after redirecting
```

**Why close?**
- Prevents file descriptor leaks
- Allows EOF detection (pipe won't close until all write ends are closed)
- Avoids deadlocks

**Why it matters in pipex:** Proper file descriptor management prevents deadlocks and resource leaks.

---

## 8. Error Handling

### Check Every System Call

```c
int fd = open("file.txt", O_RDONLY);
if (fd < 0)
{
	perror("Error opening file");
	exit(1);
}
```

### Common Errors to Handle

1. **File not found**
```c
if (access(infile, F_OK) < 0)
	perror("Input file not found");
```

2. **Command not found**
```c
char *cmd_path = find_in_path(cmd, envp);
if (!cmd_path)
{
	fprintf(stderr, "%s: command not found\n", cmd);
	exit(127);
}
```

3. **Permission denied**
```c
if (access(cmd_path, X_OK) < 0)
{
	fprintf(stderr, "%s: permission denied\n", cmd);
	exit(126);
}
```

### Using errno

```c
#include <errno.h>

if (open("file", O_RDONLY) < 0)
{
	if (errno == ENOENT)
		printf("File doesn't exist\n");
	else if (errno == EACCES)
		printf("Permission denied\n");
	else
		printf("Error: %s\n", strerror(errno));
}
```

**Why it matters in pipex:** Robust error handling makes debugging easier and matches shell behavior.

---

## 9. Memory Management

### Allocate and Free

```c
char *str = malloc(100);
if (!str)
{
	perror("malloc failed");
	exit(1);
}

// Use str...

free(str);
str = NULL;  // Good practice
```

### Common Allocations in Pipex

1. **Parsing commands:**
```c
char **args = split(cmd, ' ');  // Allocate array
// Use args...
free_array(args);  // Free when done
```

2. **Building paths:**
```c
char *path = malloc(strlen(dir) + strlen(cmd) + 2);
sprintf(path, "%s/%s", dir, cmd);
// Use path...
free(path);
```

### When to Free

**Before execve:** Optional (process is replaced anyway)
```c
char **args = parse_command(cmd);
execve(path, args, envp);
// Never reaches here if execve succeeds
```

**In parent process:** Always free
```c
char *path = find_command(cmd, envp);
// ... fork and exec in child ...
free(path);  // Parent must free
```

**Why it matters in pipex:** Memory leaks are bad, but not critical if you exec immediately in children.

---

## Key Takeaways

1. **Processes:** Use fork() to create children, each runs a command
2. **Pipes:** Connect processes with unidirectional data channels
3. **Redirection:** Use dup2() to connect pipes to stdin/stdout
4. **Execution:** Use execve() with full paths and proper arguments
5. **Environment:** Pass envp through to children, parse PATH to find commands
6. **Waiting:** Use waitpid() to synchronize and get exit codes
7. **Files:** Open, close, and manage file descriptors carefully
8. **Errors:** Check every system call, report errors to stderr
9. **Memory:** Allocate what you need, free what you don't

**Master these concepts and pipex becomes straightforward!**
