# Processes and CGI — `fork`, `execve`, `waitpid`, `pipe`, `dup`, `dup2`, `socketpair`, `kill`, `signal`, `chdir`

> *"CGI is the only place in webserv where you create new processes. Everything in this file serves exactly one purpose: making that work correctly."*

The subject is explicit: **fork only for CGI, nowhere else.** This entire group of syscalls exists to:
1. Create a child process (`fork`)
2. Connect your pipes to its stdin/stdout (`dup2`)
3. Replace it with the CGI script (`execve`)
4. Harvest it when it exits (`waitpid`)
5. Clean up on abnormal exit (`kill`, `signal`)
6. Run it in the right directory (`chdir`)

---

## The CGI data flow — mental model first

```
                    Webserv process (parent)
                    ┌──────────────────────────────────────┐
                    │                                      │
                    │  client_fd ──► recv() HTTP request   │
                    │                    │                 │
                    │        ┌───────────┴──────────┐      │
                    │        │   pipe(pipefd_in)     │      │
                    │        │   pipe(pipefd_out)    │      │
                    │        │   fork()              │      │
                    │        └───────────┬──────────┘      │
                    │                   │                  │
  parent: write request body            │ child: exec CGI  │
  to pipefd_in[1]                       │                  │
  read CGI response                     │                  │
  from pipefd_out[0]                    │                  │
                    └──────────────────────────────────────┘
                                        │
                    CGI child process   │
                    ┌───────────────────▼──────────────────┐
                    │  stdin  ← pipefd_in[0]               │
                    │  stdout → pipefd_out[1]              │
                    │  env vars set: REQUEST_METHOD, etc.  │
                    │  execve("/usr/bin/python3", ...)      │
                    └──────────────────────────────────────┘
```

---

## `pipe()` — create a unidirectional channel

```c
#include <unistd.h>

int pipe(int pipefd[2]);
// Returns: 0 on success, -1 on error
// pipefd[0] = read end, pipefd[1] = write end
```

**Parameters:**
- `pipefd[2]` — output array; `pipefd[0]` is set to the read end fd, `pipefd[1]` to the write end fd; bytes written to `pipefd[1]` come out when reading from `pipefd[0]`

**What it does:** creates two fds. Whatever you `write()` to `pipefd[1]` comes out when you `read()` from `pipefd[0]`. This is how the parent and child process communicate — the kernel manages a circular buffer between the two ends.

**The buffer:** a pipe has a kernel buffer (typically 64 KB on Linux, 16 KB on macOS). `write()` to the write end copies data in; `read()` from the read end copies data out. If the buffer is full, `write()` blocks (or returns `EAGAIN` if non-blocking). If empty, `read()` blocks (or `EAGAIN`).

**For CGI you need two pipes:**

```cpp
int pipefd_in[2];   // parent writes request body → child reads as stdin
int pipefd_out[2];  // child writes CGI response → parent reads response

pipe(pipefd_in);
pipe(pipefd_out);
```

**Set both pipes non-blocking before adding to poll set:**

```cpp
fcntl(pipefd_in[1],  F_SETFL, O_NONBLOCK);  // parent writes to this
fcntl(pipefd_out[0], F_SETFL, O_NONBLOCK);  // parent reads from this
```

---

## `fork()` — split the process in two

```c
#include <unistd.h>

pid_t fork(void);
// Returns: child's PID in parent, 0 in child, -1 on error
```

**What it does:** creates an exact copy of the current process. Both parent and child continue execution from the line after `fork()`. The return value is the only way to tell which copy you are.

**Under the hood:** the kernel uses copy-on-write (COW). The child's memory pages are initially *shared* with the parent's, marked read-only. When either process writes to a page, the kernel makes a private copy for the writer. This means `fork()` is fast — it doesn't copy 200 MB of process memory upfront.

**What the child inherits:**
- All open file descriptors (including your sockets — this is why you need `FD_CLOEXEC`)
- The working directory (use `chdir()` to change it for CGI)
- Environment variables
- Signal dispositions

**The fork pattern for CGI:**

```cpp
pid_t pid = fork();
if (pid < 0) {
    // fork failed — send 500 to client
    close(pipefd_in[0]);  close(pipefd_in[1]);
    close(pipefd_out[0]); close(pipefd_out[1]);
    send_error(client_fd, 500);
    return;
}

if (pid == 0) {
    // ── CHILD ──
    // Wire up pipes, then execve. Never returns.
    setup_cgi_child(pipefd_in, pipefd_out, cgi_path, request);
    // If execve fails:
    std::exit(1);
}

// ── PARENT ──
// Close ends the child uses; add our ends to the poll set.
close(pipefd_in[0]);    // child reads from this — parent doesn't need it
close(pipefd_out[1]);   // child writes to this — parent doesn't need it

// Add pipefd_in[1]  to poll set with POLLOUT  (write request body)
// Add pipefd_out[0] to poll set with POLLIN   (read CGI response)
// Store pid to waitpid() later
```

**Critical: close unused pipe ends.** If the parent keeps `pipefd_out[1]` (the write end) open, the child's write-end reference count stays above zero. The parent's `read()` on `pipefd_out[0]` will never see EOF — it hangs forever because the pipe appears still writable (parent holds the write end). This is the number one CGI bug.

---

## `dup()` and `dup2()` — redirect file descriptors

```c
#include <unistd.h>

int dup(int oldfd);
// Returns: the lowest available fd number that is a copy of oldfd

int dup2(int oldfd, int newfd);
// Returns: newfd (now a copy of oldfd), or -1 on error
// If newfd was open, it is closed first.
```

**`dup` parameters:**
- `oldfd` — the fd to duplicate; the returned fd points to the same open-file description (shared offset and file status flags)

**`dup2` parameters:**
- `oldfd` — the source fd to duplicate
- `newfd` — the target fd number; if already open, it is silently closed first; use `STDIN_FILENO` (0) or `STDOUT_FILENO` (1) to wire CGI pipes to the child's standard streams

**`dup2` is the CGI tool.** You use it in the child (after `fork()`, before `execve()`) to wire the pipe ends to stdin (fd 0) and stdout (fd 1). The CGI script's `sys.stdin` / `$_POST` / `STDIN` automatically refer to fd 0; `print()` / `echo` goes to fd 1. So wiring pipe ends to 0 and 1 makes the CGI's stdin/stdout go through your pipes.

**In the child process:**

```cpp
void setup_cgi_child(int pipefd_in[2], int pipefd_out[2],
                     const std::string& cgi_path,
                     const Request& req)
{
    // Wire stdin to the read end of pipefd_in
    dup2(pipefd_in[0], STDIN_FILENO);   // fd 0 = pipefd_in[0]
    close(pipefd_in[0]);                // original no longer needed
    close(pipefd_in[1]);                // parent's write end — child doesn't write there

    // Wire stdout to the write end of pipefd_out
    dup2(pipefd_out[1], STDOUT_FILENO); // fd 1 = pipefd_out[1]
    close(pipefd_out[1]);               // original no longer needed
    close(pipefd_out[0]);               // parent's read end — child doesn't read there

    // Close all client sockets, listen sockets — child shouldn't hold them
    // (or set FD_CLOEXEC on them before fork, which is cleaner)

    // Set working directory for CGI
    chdir(cgi_working_dir.c_str());

    // Set environment variables
    setenv("REQUEST_METHOD", req.method.c_str(), 1);
    setenv("QUERY_STRING",   req.query.c_str(), 1);
    setenv("CONTENT_TYPE",   req.content_type.c_str(), 1);
    setenv("CONTENT_LENGTH", req.content_length_str.c_str(), 1);
    setenv("SCRIPT_FILENAME", cgi_path.c_str(), 1);
    setenv("SCRIPT_NAME",    req.path.c_str(), 1);
    // ... more env vars per CGI spec

    // Replace this child process with the CGI program
    char *argv[] = { const_cast<char*>(cgi_path.c_str()), NULL };
    execve(cgi_path.c_str(), argv, environ);

    // execve only returns on failure
    std::exit(1);
}
```

**`dup2` under the hood:** both `newfd` and `oldfd` now point to the same open-file description in the kernel's file table. They share the file offset, flags, etc. When you `close(oldfd)` after `dup2`, the fd is gone but the underlying file description remains (referenced by `newfd`). The reference count drops to 1.

---

## `execve()` — replace the child process with the CGI script

```c
#include <unistd.h>

int execve(const char *pathname, char *const argv[], char *const envp[]);
// Returns: only on error (-1). On success, it never returns.
```

**Parameters:**
- `pathname` — absolute path to the executable to run (e.g., `/usr/bin/python3`); `execve` does not search `PATH` — the full path is required
- `argv` — null-terminated array of argument strings; `argv[0]` is conventionally the program name; the last element must be `NULL`; for CGI: `{ interpreter_path, script_path, NULL }`
- `envp` — null-terminated array of `"KEY=VALUE"` environment strings; pass the global `environ` after your `setenv()` calls to include all CGI variables you set

**What it does:** replaces the calling process's memory image, code, and data with a new program. The process ID stays the same. Open fds (without `FD_CLOEXEC`) are inherited.

**On success, `execve` never returns.** The child process is now the CGI script. Your C++ code after `execve()` is only reached on error.

**`argv` and `envp`:**

```cpp
// argv: { path_to_interpreter, optional_script_path, NULL }
// For python: ["/usr/bin/python3", "/var/www/scripts/form.py", NULL]
// For PHP:    ["/usr/bin/php-cgi", NULL]  (PHP-CGI reads SCRIPT_FILENAME from env)

char *argv_cgi[] = {
    const_cast<char*>(interpreter.c_str()),
    const_cast<char*>(script_path.c_str()),
    NULL
};

// envp: the CGI environment variables
// You can use environ (the global variable, already includes your setenv() calls)
extern char **environ;
execve(interpreter.c_str(), argv_cgi, environ);
```

**The PATH lookup:** `execve` requires the full absolute path. `/usr/bin/python3`, not `python3`. The subject says your config maps extensions to interpreters — store those as absolute paths in your config.

**Why `execve` and not `system()` or `popen()`:** `system()` and `popen()` spawn a shell, which doesn't exist in the subject's allowed function list. `execve` directly replaces the process with the target binary — no shell involved, no injection risk.

---

## `waitpid()` — reap the CGI child

```c
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options);
// Returns: PID of reaped child, 0 if no child ready (with WNOHANG), -1 on error
```

**Parameters:**
- `pid` — which child to wait for: positive = that specific PID, `-1` = any child, `0` = any child in the same process group
- `status` — output: pointer to an `int` the kernel fills with exit information; decode with `WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`, `WTERMSIG`; pass `NULL` to discard
- `options` — behavior flags: `0` = block until the child changes state; `WNOHANG` = return `0` immediately if no child has exited (essential for the non-blocking event loop)

**What it does:** waits for a child process to change state (exit, killed by signal). Required to avoid **zombie processes** — children that have exited but whose exit status hasn't been collected. A zombie holds an entry in the kernel's process table. If you never call `waitpid()`, your webserv leaks zombie processes every time a CGI script runs.

**The right place to call `waitpid`:** in `SIGCHLD` handler or with `WNOHANG` in the event loop.

**Option 1 — `WNOHANG` in the event loop (simpler for webserv):**

```cpp
// After reading all of pipefd_out[0] (EOF on the CGI pipe), reap the child:
int child_status;
pid_t result = waitpid(cgi_pid, &child_status, WNOHANG);
if (result == cgi_pid) {
    // Child has exited
    if (WIFEXITED(child_status) && WEXITSTATUS(child_status) != 0)
        send_error(client_fd, 500);  // CGI exited with error code
    // proceed to send cgi_response to client
} else if (result == 0) {
    // Child not done yet — shouldn't happen if we already got EOF from pipe
    // Try again next poll iteration
}
```

**Option 2 — `SIGCHLD` signal handler:**

```cpp
signal(SIGCHLD, sigchld_handler);  // or sigaction()

void sigchld_handler(int sig) {
    (void)sig;
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0)
        ;  // reap all finished children
}
```

**All `waitpid()` options:**

| Option | What it does |
|---|---|
| `0` | Block until the child changes state (exits, is killed, or is stopped) |
| `WNOHANG` | Return `0` immediately if no child has changed state; essential for non-blocking event loops — use this always in webserv |
| `WUNTRACED` | Also report children that have been *stopped* (e.g. by `SIGSTOP` or `SIGTSTP`); useful for job control shells, not needed for CGI |
| `WCONTINUED` | Also report stopped children that have been *resumed* with `SIGCONT`; rarely useful in webserv |

**`WNOHANG`:** without this flag, `waitpid` blocks until the child exits. With it, `waitpid` returns `0` immediately if no child has finished yet. Always use `WNOHANG` in a non-blocking server — you cannot afford to block waiting for a slow CGI script.

**`WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`, `WTERMSIG`:** macros to decode the status:

```cpp
if (WIFEXITED(status))   std::cout << "exit code: " << WEXITSTATUS(status) << "\n";
if (WIFSIGNALED(status)) std::cout << "killed by signal: " << WTERMSIG(status) << "\n";
```

---

## `kill()` — send a signal to a process

```c
#include <signal.h>

int kill(pid_t pid, int sig);
// Returns: 0 on success, -1 on error
```

**Parameters:**
- `pid` — target process ID; positive = a specific process; `0` = all processes in the current process group; `-1` = all processes you have permission to signal (dangerous — avoid)
- `sig` — signal number to deliver: `SIGTERM` (15) requests graceful termination, `SIGKILL` (9) forces immediate kill and cannot be caught or ignored

**In webserv:** used to terminate a CGI process that has exceeded a timeout.

```cpp
// CGI has been running for more than, say, 10 seconds — kill it
if (time_now - cgi_start_time > CGI_TIMEOUT) {
    kill(cgi_pid, SIGTERM);   // polite: ask it to exit
    // Give it a moment, then:
    kill(cgi_pid, SIGKILL);   // forceful: cannot be caught or ignored
    waitpid(cgi_pid, NULL, 0);
    send_error(client_fd, 504);  // 504 Gateway Timeout
}
```

**Common signals for `kill()`:**

| Signal | Number | Default action | Notes |
|---|---|---|---|
| `SIGTERM` | 15 | Terminate | Polite termination request; can be caught and handled for cleanup |
| `SIGKILL` | 9 | Kill (forced) | Unconditional kill; cannot be caught, blocked, or ignored; kernel terminates immediately |
| `SIGINT` | 2 | Terminate | Sent by Ctrl+C from the controlling terminal |
| `SIGHUP` | 1 | Terminate | Terminal closed; conventionally used to trigger config reload without restarting |
| `SIGCHLD` | 17/20 | Ignore | Child process changed state (exited, stopped, continued); default ignore causes zombies — install a handler that calls `waitpid` |
| `SIGPIPE` | 13 | Terminate | Write to a closed socket or pipe; always ignore with `signal(SIGPIPE, SIG_IGN)` in webserv |
| `SIGUSR1` | 10/30 | Terminate | User-defined signal; your server can catch it and do anything (e.g. reload config, dump stats) |
| `SIGUSR2` | 12/31 | Terminate | Second user-defined signal |
| `SIGALRM` | 14 | Terminate | Timer fired (set with `alarm()`); alternative to poll() timeout for CGI deadline |
| `SIGSTOP` | 19/17 | Stop | Pause a process; cannot be caught or ignored |
| `SIGCONT` | 18/19 | Continue | Resume a stopped process; always succeeds even if the process ignores it |
| `SIGABRT` | 6 | Abort + core | Sent by `abort()`; causes core dump; useful for debug |

Note: signal numbers differ between Linux and macOS for `SIGUSR*`, `SIGCHLD`, `SIGSTOP`, `SIGCONT`. Use the symbolic names, not the numbers.

**`SIGTERM` vs `SIGKILL`:**
- `SIGTERM` (15): can be caught and handled by the target process (allows cleanup). Best to send this first.
- `SIGKILL` (9): unconditional kill — cannot be caught, blocked, or ignored. Kernel terminates the process immediately. Use as a last resort.
- `SIGINT` (2): what Ctrl+C sends. Also terminates by default.

**`kill(-1, sig)`:** sends signal to all processes in your process group — dangerous in general, fine for sending `SIGTERM` to all your CGI children at server shutdown.

---

## `signal()` — install a signal handler

```c
#include <signal.h>

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
// Returns: previous handler, or SIG_ERR on error
```

**Parameters:**
- `signum` — the signal to configure: `SIGPIPE` (broken pipe), `SIGCHLD` (child exited), `SIGINT` (Ctrl+C), etc.
- `handler` — the new handler: a function pointer `void (*)(int)` called when the signal arrives; `SIG_IGN` to silently ignore the signal; `SIG_DFL` to restore the OS default behavior

**For webserv, three signals matter:**

```cpp
signal(SIGPIPE, SIG_IGN);   // Ignore broken pipe — client disconnected mid-write
signal(SIGCHLD, SIG_DFL);   // or a custom handler for CGI reaping
signal(SIGINT,  sigint_handler); // Ctrl+C → graceful shutdown
```

**`SIGPIPE` — the silent killer:** when you `send()` to a socket whose other end has closed, the kernel sends `SIGPIPE` to your process. Default action: terminate the process. You never want this — a dropped client connection should not kill your server. `signal(SIGPIPE, SIG_IGN)` makes `send()` return `-1` with `errno = EPIPE` instead. Then you just close the client.

**`SIGCHLD`:** sent when a child process exits or is stopped. If you ignore it (`SIG_IGN`), the kernel auto-reaps children (no zombies, but no status codes either). If you set a custom handler, call `waitpid(-1, &status, WNOHANG)` in a loop inside the handler.

**Signal handler restrictions:** you cannot do much inside a signal handler safely — most library functions are not async-signal-safe. The safe pattern is to set a flag and check it in the event loop:

```cpp
volatile sig_atomic_t g_shutdown = 0;

void sigint_handler(int) {
    g_shutdown = 1;
}

// In event loop:
if (g_shutdown) {
    cleanup_and_exit();
}
```

---

## `chdir()` — change working directory

```c
#include <unistd.h>

int chdir(const char *path);
// Returns: 0 on success, -1 on error
```

**Parameters:**
- `path` — absolute or relative path of the target directory; in CGI context, pass the directory containing the script file so that relative file paths inside the script resolve correctly

**In webserv:** called in the child process (after fork, before execve) to set the CGI script's working directory.

The subject says: *"The CGI should be run in the correct directory for relative path file access."*

If a Python CGI script does `open("data.json")`, it expects to find `data.json` relative to the script's directory. Without `chdir()`, it searches the webserv's working directory instead.

```cpp
// In the child, after dup2 and before execve:
std::string script_dir = get_parent_dir(script_path);
if (chdir(script_dir.c_str()) < 0) {
    std::exit(1);
}
// Now execve runs the script with cwd = script directory
```

---

## `socketpair()` — bidirectional pipe

```c
#include <sys/socket.h>

int socketpair(int domain, int type, int protocol, int sv[2]);
// Returns: 0 on success, -1 on error
// sv[0] and sv[1] are connected — write to sv[0] reads from sv[1] and vice versa
```

**Parameters:**
- `domain` — address family: `AF_UNIX` for local socket pairs (same machine, no network); `AF_INET` is rarely used here
- `type` — socket type: `SOCK_STREAM` for a bidirectional byte stream; `SOCK_DGRAM` for message-boundary-preserving datagrams
- `protocol` — `0` = kernel picks the appropriate protocol for the domain+type combination; always correct for `AF_UNIX`
- `sv[2]` — output array; `sv[0]` and `sv[1]` are the two connected socket ends; write to one end and read from the other

**What it does:** like `pipe()` but bidirectional — both ends can read and write. Created as a pair of connected sockets.

**In webserv:** listed as an allowed function. Less commonly used than `pipe()` for CGI (where you need two one-directional pipes for stdin/stdout). `socketpair` is useful if you want a single bidirectional channel between parent and child — for example, for a custom CGI protocol or for a worker process that sends both results and status back on the same fd.

```cpp
int sv[2];
socketpair(AF_UNIX, SOCK_STREAM, 0, sv);

pid_t pid = fork();
if (pid == 0) {
    close(sv[0]);                  // child uses sv[1]
    dup2(sv[1], STDIN_FILENO);
    dup2(sv[1], STDOUT_FILENO);
    close(sv[1]);
    execve(cgi, argv, envp);
} else {
    close(sv[1]);                  // parent uses sv[0]
    // write request to sv[0], read response from sv[0]
}
```

---

## Full CGI flow — assembled

```
1. Parse request → identify CGI route (extension match from config)
2. pipe(pipefd_in)    → channel for request body → CGI stdin
3. pipe(pipefd_out)   → channel for CGI stdout → response
4. fork()
   CHILD:
     dup2(pipefd_in[0],  0)   → stdin  = pipe from parent
     dup2(pipefd_out[1], 1)   → stdout = pipe to parent
     close all other fds
     chdir(script_dir)
     set CGI env vars (REQUEST_METHOD, CONTENT_LENGTH, ...)
     execve(interpreter, [interpreter, script, NULL], environ)
     exit(1)  // only if execve fails
   PARENT:
     close(pipefd_in[0])     // child's end
     close(pipefd_out[1])    // child's end
     set pipefd_in[1]  non-blocking → add to poll set (POLLOUT)
     set pipefd_out[0] non-blocking → add to poll set (POLLIN)
     store cgi_pid for waitpid
5. Poll loop:
     pipefd_in[1]  POLLOUT → write request body bytes
     after all body written → close(pipefd_in[1])  ← sends EOF to CGI
     pipefd_out[0] POLLIN  → read CGI response bytes into buffer
     pipefd_out[0] EOF     → waitpid(cgi_pid, WNOHANG) → parse CGI headers
                           → send HTTP response to client_fd
6. Timeout: if CGI exceeds limit → kill(cgi_pid, SIGTERM), then SIGKILL
```

---

## Related
- [`02_IO_MULTIPLEXING.md`](02_IO_MULTIPLEXING.md) — the poll set that monitors CGI pipe fds
- [`03_DATA_TRANSFER.md`](03_DATA_TRANSFER.md) — `read`/`write` on the pipe fds
- [`04_ADDRESS_CONVERSION.md`](04_ADDRESS_CONVERSION.md) — `getsockname`/`ntohs` for CGI SERVER_NAME env var
- [`../14_CGI.md`](../14_CGI.md) — the full CGI protocol, env vars list, response parsing
