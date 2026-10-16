# `unistd.h`

> *"The Unix standard interface. Read, write, close — the three verbs everything else is built on."*

Provides POSIX system calls for I/O, process control, and file descriptor management. The name stands for **Unix Standard** — it's the direct C interface to the kernel.

---

## Constants

```c
STDIN_FILENO   // 0 — standard input
STDOUT_FILENO  // 1 — standard output
STDERR_FILENO  // 2 — standard error
```

These are the three fds every process inherits at birth. CGI relies on them heavily — the server wires its pipes to fd 0 and fd 1 of the child process so the CGI script reads input and writes output through them.

---

## I/O functions

### `read()` — receive bytes from an fd

```c
ssize_t read(int fd, void *buf, size_t count);
// Returns: bytes read, 0 = EOF / peer closed, -1 on error
```

Works on any fd — file, socket, pipe, terminal. The uniform interface is the Unix philosophy in action.

**Return value 0** means the writer closed their end. For a socket, this is the peer's TCP FIN — your signal to `close()` and remove the fd from the poll array.

**Short reads are normal.** TCP is a stream; one `read()` may return fewer bytes than you asked for. Always accumulate:

```cpp
char buf[4096];
int n = read(fd, buf, sizeof(buf));
// n may be anything from 1 to 4096
// append to your per-client buffer and check for \r\n\r\n
```

---

### `write()` — send bytes to an fd

```c
ssize_t write(int fd, const void *buf, size_t count);
// Returns: bytes written, -1 on error
```

**Short writes are normal.** The kernel's send buffer may be full. Always check the return value and track how many bytes remain:

```cpp
size_t sent = 0;
while (sent < total) {
    int n = write(fd, buf + sent, total - sent);
    if (n == -1) break;  // check errno
    sent += n;
}
```

In a non-blocking `poll()` server, don't loop here — instead, buffer the remainder and wait for `POLLOUT` before writing more.

---

### `close()` — release a file descriptor

```c
int close(int fd);
// Returns: 0 on success, -1 on error
```

Releases the fd back to the kernel. For a socket, also sends TCP FIN to the peer when the last reference is closed.

**Every fd must be closed exactly once.** Track every `socket()`, `accept()`, `open()`, `pipe()` — each returns an fd that must reach a `close()`. Leaking fds is a slow-motion resource exhaustion bug.

Remove the fd from your `poll` array before or immediately after closing it. Reading or writing a closed fd is undefined behavior.

---

## Process functions

### `fork()` — create a child process

```c
pid_t fork(void);
// Returns: child PID in parent, 0 in child, -1 on error
```

Creates an exact copy of the current process. Both processes continue from the line after `fork()`. Used for CGI: the child execs the script, the parent stays in the event loop.

```cpp
pid_t pid = fork();
if (pid == 0) {
    // child — set up pipes, exec the CGI
} else {
    // parent — continue the event loop, watch the pipe fds
}
```

---

### `execve()` — replace process image

```c
int execve(const char *path, char *const argv[], char *const envp[]);
// Does not return on success. Returns -1 on error.
```

Replaces the calling process with a new program. Used in the CGI child after `fork()`. The new program inherits the fd table — that's how the pipe wiring done before `execve()` reaches the script.

---

### `pipe()` — create a unidirectional channel

```c
int pipe(int pipefd[2]);
// Returns: 0 on success, -1 on error
// pipefd[0] = read end, pipefd[1] = write end
```

Creates two fds: you write into `pipefd[1]`, you read from `pipefd[0]`. Used for CGI stdin and stdout channels between parent and child.

---

### `dup2()` — redirect an fd

```c
int dup2(int oldfd, int newfd);
// Returns: newfd on success, -1 on error
```

Makes `newfd` point to the same thing as `oldfd`. If `newfd` was already open, it's closed first. The CGI idiom:

```cpp
// in the child, before execve:
dup2(pipe_stdin[0],  STDIN_FILENO);   // CGI reads from pipe
dup2(pipe_stdout[1], STDOUT_FILENO);  // CGI writes to pipe
```

After this, any `read()` from stdin in the CGI script reads from your pipe, and any `write()` to stdout writes back to the server.

---

### `access()` — check file permissions

```c
int access(const char *path, int mode);
// Returns: 0 if allowed, -1 if not
```

Common modes:

```c
F_OK   // file exists
R_OK   // readable
W_OK   // writable
X_OK   // executable
```

Used before serving a static file to check existence and read permission before trying to `open()` it.

---

### `chdir()` — change working directory

```c
int chdir(const char *path);
// Returns: 0 on success, -1 on error
```

Used in the CGI child to set the working directory to the script's location, as the CGI spec requires.

---

## Summary

```
Constants    STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO

I/O          read()  — bytes from any fd (file, socket, pipe)
             write() — bytes to any fd
             close() — release fd, send TCP FIN for sockets

Process      fork()   — duplicate process
             execve() — replace process image
             pipe()   — unidirectional channel between fds
             dup2()   — redirect an fd to another
             access() — check file permissions
             chdir()  — change working directory
```

---

## Related

- [`SYS_SOCKET_H.md`](SYS_SOCKET_H.md) — send/recv, socket-specific I/O with flags
- [`POLL_H.md`](POLL_H.md) — deciding when to call read/write
- [`../functions/03_DATA_TRANSFER.md`](../functions/03_DATA_TRANSFER.md) — read/write deep-dive
- [`../functions/05_PROCESS_AND_CGI.md`](../functions/05_PROCESS_AND_CGI.md) — fork/execve/pipe for CGI
