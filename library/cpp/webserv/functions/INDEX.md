# Webserv Functions — Reference Index

Every syscall and libc function the 42 webserv subject allows, organized by concept. Each file explains what the function does, how it works at the kernel level, and where exactly it appears in a webserv implementation.

**Reading order if you're starting from scratch:** 01 → 02 → 03 → 04 → 05 → 06 → 07.

---

## Files

| # | File | Functions covered | When you need it |
|---|---|---|---|
| 01 | [`01_SOCKET_LIFECYCLE.md`](01_SOCKET_LIFECYCLE.md) | `socket`, `bind`, `listen`, `accept`, `connect`, `setsockopt`, `getsockname` | Server startup, accepting clients |
| 02 | [`02_IO_MULTIPLEXING.md`](02_IO_MULTIPLEXING.md) | `poll`, `select`, `epoll_create/ctl/wait`, `kqueue`, `kevent`, `fcntl` | The event loop — core architecture |
| 03 | [`03_DATA_TRANSFER.md`](03_DATA_TRANSFER.md) | `read`, `write`, `send`, `recv` | Every request/response I/O |
| 04 | [`04_ADDRESS_CONVERSION.md`](04_ADDRESS_CONVERSION.md) | `htons`, `htonl`, `ntohs`, `ntohl`, `getaddrinfo`, `freeaddrinfo`, `gai_strerror`, `getsockname`, `getprotobyname` | Binding to config addresses, CGI env |
| 05 | [`05_PROCESS_AND_CGI.md`](05_PROCESS_AND_CGI.md) | `fork`, `execve`, `waitpid`, `pipe`, `dup`, `dup2`, `socketpair`, `kill`, `signal`, `chdir` | CGI execution |
| 06 | [`06_FILESYSTEM.md`](06_FILESYSTEM.md) | `open`, `close`, `stat`, `access`, `opendir`, `readdir`, `closedir` | Serving static files, directory listing |
| 07 | [`07_ERRORS.md`](07_ERRORS.md) | `strerror`, `gai_strerror`, `errno` | Error handling discipline |

---

## Full function → file map

```
accept          → 01_SOCKET_LIFECYCLE.md
access          → 06_FILESYSTEM.md
bind            → 01_SOCKET_LIFECYCLE.md
chdir           → 05_PROCESS_AND_CGI.md
close           → 06_FILESYSTEM.md
closedir        → 06_FILESYSTEM.md
connect         → 01_SOCKET_LIFECYCLE.md
dup             → 05_PROCESS_AND_CGI.md
dup2            → 05_PROCESS_AND_CGI.md
epoll_create    → 02_IO_MULTIPLEXING.md
epoll_ctl       → 02_IO_MULTIPLEXING.md
epoll_wait      → 02_IO_MULTIPLEXING.md
errno           → 07_ERRORS.md
execve          → 05_PROCESS_AND_CGI.md
fcntl           → 02_IO_MULTIPLEXING.md
fork            → 05_PROCESS_AND_CGI.md
freeaddrinfo    → 04_ADDRESS_CONVERSION.md
gai_strerror    → 04_ADDRESS_CONVERSION.md / 07_ERRORS.md
getaddrinfo     → 04_ADDRESS_CONVERSION.md
getprotobyname  → 04_ADDRESS_CONVERSION.md
getsockname     → 01_SOCKET_LIFECYCLE.md / 04_ADDRESS_CONVERSION.md
htonl           → 04_ADDRESS_CONVERSION.md
htons           → 04_ADDRESS_CONVERSION.md
kevent          → 02_IO_MULTIPLEXING.md
kill            → 05_PROCESS_AND_CGI.md
kqueue          → 02_IO_MULTIPLEXING.md
listen          → 01_SOCKET_LIFECYCLE.md
ntohl           → 04_ADDRESS_CONVERSION.md
ntohs           → 04_ADDRESS_CONVERSION.md
open            → 06_FILESYSTEM.md
opendir         → 06_FILESYSTEM.md
pipe            → 05_PROCESS_AND_CGI.md
poll            → 02_IO_MULTIPLEXING.md
read            → 03_DATA_TRANSFER.md
readdir         → 06_FILESYSTEM.md
recv            → 03_DATA_TRANSFER.md
select          → 02_IO_MULTIPLEXING.md
send            → 03_DATA_TRANSFER.md
setsockopt      → 01_SOCKET_LIFECYCLE.md
signal          → 05_PROCESS_AND_CGI.md
socket          → 01_SOCKET_LIFECYCLE.md
socketpair      → 05_PROCESS_AND_CGI.md
stat            → 06_FILESYSTEM.md
strerror        → 07_ERRORS.md
waitpid         → 05_PROCESS_AND_CGI.md
write           → 03_DATA_TRANSFER.md
```

---

## Concept map — where each function appears in the server lifecycle

```
SERVER STARTUP
  socket()        → create fd for each port in config
  setsockopt()    → SO_REUSEADDR on each listen fd
  bind()          → attach to IP:port
  listen()        → start accepting TCP handshakes
  fcntl()         → O_NONBLOCK all listen fds
  signal()        → ignore SIGPIPE, handle SIGCHLD

EVENT LOOP (runs forever)
  poll() / select() / kqueue()
    │
    ├─ listen_fd readable → accept()
    │     → fcntl(client_fd, O_NONBLOCK)
    │     → add client_fd to poll set
    │
    ├─ client_fd POLLIN → recv() / read()
    │     → accumulate into per-client buffer
    │     → search for \r\n\r\n
    │     → parse request
    │     → route to handler
    │
    ├─ client_fd POLLOUT → send() / write()
    │     → flush pending response bytes
    │     → clear POLLOUT when done
    │
    ├─ cgi_stdout POLLIN → read()
    │     → accumulate CGI response
    │     → on EOF: waitpid(), build HTTP response
    │
    └─ cgi_stdin POLLOUT → write()
          → send request body to CGI
          → close(cgi_stdin) when done (sends EOF)

STATIC FILE REQUEST (GET)
  stat()          → check existence, get size and type
  access()        → check read permission
  open()          → get fd
  read()          → stream bytes
  close()         → release fd
  opendir()       → if path is a directory with autoindex
  readdir()       → list entries
  closedir()      → release dir stream

CGI REQUEST
  pipe()          → create stdin + stdout channels
  fork()          → create child process
  CHILD:
    dup2()        → wire pipes to fd 0 (stdin) and fd 1 (stdout)
    chdir()       → set working directory to script location
    setenv()      → set CGI env vars
    execve()      → replace child with CGI program
  PARENT:
    close()       → close child's ends of pipes
    poll()        → wait for cgi_stdin writable, cgi_stdout readable
    write()       → send POST body to CGI stdin
    read()        → receive CGI response from stdout
    waitpid()     → reap child after EOF on stdout pipe
    kill()        → terminate CGI if it exceeds timeout

ADDRESS HANDLING
  htons()/htonl() → convert port/IP to network byte order for sockaddr_in
  ntohs()/ntohl() → convert back when reading client address
  getaddrinfo()   → resolve config hostname:port into sockaddr
  freeaddrinfo()  → free the result
  getsockname()   → get local IP:port for CGI env vars (SERVER_NAME, SERVER_PORT)

ERROR HANDLING
  strerror(errno) → human-readable error for logging startup/file errors
  gai_strerror()  → human-readable error for getaddrinfo() failures
```

---

## Tips to keep in mind across all files

**1. Every fd must be closed.** Track every `socket()`, `accept()`, `open()`, `pipe()`, and `dup2()` — each returns an fd that must be `close()`d exactly once.

**2. `O_NONBLOCK` everything that goes in the poll set.** A blocking fd in a poll set defeats the whole architecture. Set `O_NONBLOCK` on every socket and pipe immediately after creation.

**3. Short reads and short writes are normal.** TCP is a stream. One `recv()` call may not give you the full HTTP request. One `send()` call may not send the full response. Always accumulate / track offset.

**4. Close CGI pipe ends the parent doesn't use.** If the parent holds `pipefd_out[1]` (CGI's stdout write end), the parent's `read()` on `pipefd_out[0]` never sees EOF. The CGI hangs. Close immediately after `fork()`.

**5. `SIGPIPE` kills your server.** `signal(SIGPIPE, SIG_IGN)` at startup. Always.

**6. `gai_strerror`, not `strerror`, for `getaddrinfo` errors.** They use different error namespaces.

**7. `poll()` returns `EINTR` on signal delivery.** Check for it and retry.

**8. `waitpid()` with `WNOHANG` — never block waiting for CGI to exit.** Poll for it in the event loop.
