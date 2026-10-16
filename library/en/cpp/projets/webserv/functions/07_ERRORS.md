# Error Handling — `strerror`, `gai_strerror`, `errno`

> *"The subject says no errno after read/write. That doesn't mean ignore errors — it means understand which functions set errno and which don't."*

---

## `errno` — the global error variable

```c
#include <errno.h>

extern int errno;   // set by the kernel after a failed syscall
```

**What it is:** a thread-local integer that the kernel sets when a syscall returns `-1`. It holds the error code for the *most recent* failing syscall. It is overwritten by the next syscall, successful or not.

**The rule from the subject:**

> "You must never check errno after read / write operations." → Grade 0.

This doesn't mean "never use errno." It means: if `read()` or `write()` returns -1, you must handle the error (close the connection) without inspecting errno to decide what to do. The forbidden pattern is:

```cpp
// FORBIDDEN — errno after read/write
read(fd, buf, n);
if (errno != 0) { /* retry or take special action based on error code */ }
```

**Allowed patterns:**

```cpp
// Checking errno after accept() to detect EAGAIN — fine
int client_fd = accept(listen_fd, ...);
if (client_fd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
        continue;  // no connection waiting — allowed
}

// Checking errno after open() to distinguish 404 vs 403 — fine
int fd = open(path, O_RDONLY);
if (fd < 0) {
    send_error(client_fd, errno == ENOENT ? 404 : 403);
}

// Checking errno after bind() to diagnose configuration error — fine
if (bind(listen_fd, ...) < 0) {
    throw std::runtime_error(std::string("bind: ") + strerror(errno));
}
```

**`errno` is only valid immediately after the failing call.** Call any other function (including `strerror()` itself before you read `errno`) and it may be overwritten:

```cpp
int fd = open(path, O_RDONLY);
int saved_errno = errno;       // save immediately if you need it later
if (fd < 0) {
    log_error(saved_errno);    // log uses saved copy
    send_error(client_fd, ...);
}
```

**Common `errno` values in webserv:**

| `errno` | Value | Meaning in context |
|---|---|---|
| `ENOENT` | 2 | File not found → 404 |
| `EACCES` | 13 | Permission denied → 403 |
| `EADDRINUSE` | 98 | Port in use → server startup failure |
| `EAGAIN` / `EWOULDBLOCK` | 11/11 | Non-blocking fd has no data yet → retry |
| `EPIPE` | 32 | Client closed — write to dead socket |
| `ECONNRESET` | 104 | Client forcibly closed connection |
| `EMFILE` | 24 | Too many open fds — fd leak |
| `EINTR` | 4 | Syscall interrupted by signal → retry |

---

## `strerror()` — errno value to human-readable string

```c
#include <string.h>

char *strerror(int errnum);
// Returns: pointer to a static string describing the error
```

**Parameters:**
- `errnum` — an `errno` value captured immediately after a failing syscall (e.g., `ENOENT`, `EACCES`, `EADDRINUSE`); read `errno` before calling any other function that might overwrite it

**What it does:** converts an errno value to a human-readable description. The returned pointer is to a static buffer — it's overwritten on the next call (not thread-safe).

**The pattern in webserv:**

```cpp
// In startup code — translate syscall failures into exceptions
if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    throw std::runtime_error(std::string("bind port ") +
                             to_string(port) + ": " + strerror(errno));

// In request handling — log and send 500
int fd = open(path.c_str(), O_RDONLY);
if (fd < 0) {
    std::cerr << "open(" << path << "): " << strerror(errno) << std::endl;
    send_error(client_fd, errno == ENOENT ? 404 : 403);
    return;
}
```

**C++98 safe usage:** since `strerror()` returns a `char*` to a static buffer, convert it to `std::string` immediately:

```cpp
std::string err_msg = strerror(errno);   // copies the string
// err_msg is safe to use even after other calls
```

**`perror()` is simpler but less flexible:**

```cpp
perror("bind");
// Prints: "bind: Address already in use" to stderr
// Equivalent to: fprintf(stderr, "bind: %s\n", strerror(errno));
// Fine for tiny server / debugging, not for production error handling
```

---

## `gai_strerror()` — error strings for `getaddrinfo()`

```c
#include <netdb.h>

const char *gai_strerror(int errcode);
```

**Parameters:**
- `errcode` — the non-zero integer returned directly by `getaddrinfo()` on failure; this is **not** an `errno` value — do not pass `errno` here

**Critical distinction:** `getaddrinfo()` does **not** set `errno` on failure. It returns a non-zero error code directly. You must call `gai_strerror()` on that return value — calling `strerror(errno)` gives nonsense.

```cpp
struct addrinfo hints, *res;
// ... set up hints ...

int status = getaddrinfo("localhost", "8080", &hints, &res);
if (status != 0) {
    // WRONG:
    std::cerr << strerror(errno) << std::endl;     // errno is unrelated

    // CORRECT:
    std::cerr << gai_strerror(status) << std::endl;  // status is the error code
    throw std::runtime_error("getaddrinfo failed");
}
freeaddrinfo(res);
```

**Common `gai_strerror` codes:**

| Code | Meaning |
|---|---|
| `EAI_NONAME` | Hostname not found (DNS failure or typo) |
| `EAI_SERVICE` | Service/port not valid for socket type |
| `EAI_SOCKTYPE` | Socket type not supported |
| `EAI_AGAIN` | DNS temporarily unavailable — retry |
| `EAI_FAIL` | Non-recoverable DNS failure |

---

## The error handling discipline for webserv

**Startup errors** (bind, listen, socket): unrecoverable — throw an exception or call `exit(1)` with a clear message. The server cannot start without working listen sockets.

**Per-request errors** (open, stat, accept): recoverable — log the error, send the appropriate HTTP error response to the client, continue the event loop. Never kill the server over a bad request.

**Per-client I/O errors** (read, write returning -1): close the client fd, remove from poll set, continue. Do not inspect errno per subject rules.

**Template for request handler error handling:**

```cpp
void handle_get(int client_fd, const Request& req, const Config& cfg) {
    std::string path = cfg.root + req.path;

    struct stat st;
    if (stat(path.c_str(), &st) < 0) {
        send_error(client_fd, errno == ENOENT ? 404 : 403);
        return;
    }
    if (S_ISDIR(st.st_mode)) {
        handle_directory(client_fd, path, req, cfg);
        return;
    }
    if (!S_ISREG(st.st_mode)) {
        send_error(client_fd, 403);
        return;
    }

    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        send_error(client_fd, errno == EACCES ? 403 : 404);
        return;
    }

    // Stream file — on write error, just close client (no errno check)
    char buf[8192];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (send(client_fd, buf, n, 0) < 0) {
            close(fd);
            close_client(client_fd);
            return;
        }
    }
    close(fd);
}
```

---

## Related
- [`01_SOCKET_LIFECYCLE.md`](01_SOCKET_LIFECYCLE.md) — startup failures (bind, listen) use strerror
- [`04_ADDRESS_CONVERSION.md`](04_ADDRESS_CONVERSION.md) — getaddrinfo errors use gai_strerror
- [`06_FILESYSTEM.md`](06_FILESYSTEM.md) — open/stat failures use strerror + errno
- [`../04_STATUS_CODES.md`](../04_STATUS_CODES.md) — which HTTP error to send for which errno
