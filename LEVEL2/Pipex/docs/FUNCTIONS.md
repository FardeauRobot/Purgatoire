# Function Reference Guide

Complete reference for all authorized functions in pipex.

## Table of Contents

1. [I/O Functions](#io-functions)
2. [Memory Management](#memory-management)
3. [Error Handling](#error-handling)
4. [File Access & Duplication](#file-access--duplication)
5. [Process Management](#process-management)
6. [Process Termination](#process-termination)

---

## I/O Functions

### `open()`
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

### `close()`
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

### `read()`
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

### `write()`
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

## Memory Management

### `malloc()`
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

### `free()`
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

## Error Handling

### `perror()`
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

### `strerror()`
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

## File Access & Duplication

### `access()`
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

### `dup()`
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

### `dup2()`
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

## Process Management

### `execve()`
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

### `exit()`
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

### `fork()`
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

### `pipe()`
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

## Process Termination

### `unlink()`
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

### `wait()`
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

### `waitpid()`
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
