# Data Transfer — `read`, `write`, `send`, `recv`

> *"poll() says the fd is ready. These are the four functions that actually move bytes."*

All four functions copy data between your user-space buffer and the kernel's socket buffer. The kernel buffer then handles transmission to/from the wire. Understanding where the copy happens, and what "ready" really means, prevents the most common webserv bugs.

---

## The kernel buffer model

Every TCP socket has two ring buffers in kernel space:

```
Your process (user space)
    │ read() / recv()        write() / send() │
    ▼                                         ▼
┌─────────────────────────────────────────────────┐
│             Kernel socket buffers               │
│  recv_buf [64 KB]   │   send_buf [64 KB]        │
└─────────────────────┴───────────────────────────┘
    ▲                                         ▼
    │         Network (TCP/IP stack)          │
```

- **`read()`/`recv()`:** copies bytes from the kernel `recv_buf` into your buffer. If the buffer is empty → `EAGAIN` (non-blocking) or block.
- **`write()`/`send()`:** copies bytes from your buffer into the kernel `send_buf`. The kernel's TCP stack drains the send buffer onto the wire at its own pace. If the buffer is full → `EAGAIN` (non-blocking) or block.

**Key insight:** `send()` returning success does not mean the bytes have left your machine. It means they're in the kernel buffer. The TCP stack will actually transmit them; you'll never know exactly when.

---

## `recv()` — reading from a socket

```c
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
// Returns: bytes read (>0), 0 on connection closed, -1 on error
```

**Parameters:**
- `sockfd` — a connected socket fd returned by `accept()`; the connection must be established
- `buf` — pointer to your buffer where received bytes are written
- `len` — maximum number of bytes to read; usually `sizeof(buf)` or a fixed chunk size (e.g., 4096)
- `flags` — modifiers: `0` for normal reads; `MSG_PEEK` to inspect bytes without consuming them; `MSG_WAITALL` to block until the full `len` bytes arrive (avoid in non-blocking servers)

**`recv` vs `read`:** for sockets they are equivalent when `flags = 0`. `recv()` adds the flags parameter; `read()` does not. Use `recv()` for sockets — it signals intent to the reader.

**The three return values:**

```cpp
ssize_t n = recv(client_fd, buf, sizeof(buf), 0);

if (n > 0) {
    // Got n bytes. May be less than sizeof(buf). Append to this client's buffer.
    client.recv_buf.append(buf, n);
    try_parse_request(client);
}
else if (n == 0) {
    // Client closed the connection (sent TCP FIN).
    // Do NOT call recv() again — it will keep returning 0.
    close_client(client_fd);
}
else { // n == -1
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // No data available right now — non-blocking fd. Go back to poll().
        return;
    }
    // Real error (ECONNRESET, EIO, etc.)
    close_client(client_fd);
}
```

**The subject says: never check `errno` after `read`/`write` → grade 0.**
Read that rule carefully: it means don't use `errno` to re-try or re-read after a successful operation. You *are* allowed to check `errno` when `recv()` returns `-1` to distinguish `EAGAIN` from a real error. The forbidden pattern is:

```cpp
recv(fd, buf, n, 0);
if (errno != 0) { /* WRONG — errno after successful recv is undefined */ }
```

**Short reads — guaranteed in TCP:**

TCP is a stream. One HTTP request may arrive as:

```
recv() → "GET /index.html HTT"
recv() → "P/1.1\r\nHost: local"
recv() → "host:8080\r\n\r\n"
```

You cannot know how many `recv()` calls it takes to get a complete request. **Always accumulate into a per-client buffer and search for `\r\n\r\n` to detect end-of-headers.** Never assume one `recv()` = one complete message.

**All `recv()` flags:**

| Flag | What it does |
|---|---|
| `0` | Normal read — use this in webserv |
| `MSG_PEEK` | Copy bytes from kernel buffer without consuming them; next `recv()` returns the same bytes again; useful for inspecting the start of a request before committing to read |
| `MSG_WAITALL` | Block until the full `len` bytes have arrived; returns less only on error or connection close; avoid in non-blocking servers — it defeats the event loop |
| `MSG_DONTWAIT` | One-shot non-blocking: return `EAGAIN` immediately if no data, regardless of `O_NONBLOCK` on the fd; lets you make a single non-blocking read without permanently changing the fd |
| `MSG_OOB` | Receive out-of-band (TCP urgent) data; the sender must have used `MSG_OOB` on `send()`; almost never used in modern protocols |
| `MSG_TRUNC` | UDP only: return the real datagram length even if `buf` was too small (the excess is silently discarded) |

---

## `read()` — generic fd read (sockets, pipes, files)

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
// Returns: bytes read (>0), 0 on EOF, -1 on error
```

**Parameters:**
- `fd` — any readable fd: connected socket, pipe read end, regular file, or terminal
- `buf` — pointer to your buffer where read bytes are written
- `count` — maximum number of bytes to read; may receive fewer (short read) even on success

**Use `read()` for:** pipe fds (CGI stdin/stdout), regular file fds (serving static files — though for files you can read in one shot since they're always ready). For socket fds, `recv()` with `flags=0` is identical.

**Reading CGI output through a pipe:**

```cpp
// cgi_stdout_fd is the read end of a pipe — set non-blocking
ssize_t n = read(cgi_stdout_fd, buf, sizeof(buf));
if (n > 0) {
    cgi_response.append(buf, n);
}
else if (n == 0) {
    // CGI process closed its write end (exited) → EOF on our read end
    // cgi_response is complete — now build HTTP response and send to client
    finalize_cgi_response(client_fd, cgi_response);
    close(cgi_stdout_fd);
    remove_from_poll(cgi_stdout_fd);
}
else if (errno == EAGAIN) {
    // CGI hasn't written anything yet — go back to poll()
}
```

**Reading a file to serve statically:**

```cpp
int file_fd = open(path.c_str(), O_RDONLY);
if (file_fd < 0) { /* 404 or 403 */ }

char file_buf[8192];
ssize_t n;
std::string body;
while ((n = read(file_fd, file_buf, sizeof(file_buf))) > 0)
    body.append(file_buf, n);
close(file_fd);
// Regular files are always "ready" — you can read them in a loop without poll()
// The subject says: poll() not required for regular disk files.
```

---

## `send()` — writing to a socket

```c
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
// Returns: bytes written (>0), -1 on error
// NOTE: send() never returns 0
```

**Parameters:**
- `sockfd` — a connected socket fd returned by `accept()`
- `buf` — pointer to the data to send
- `len` — number of bytes to send from `buf`; if tracking an offset for partial sends, pass `buf + offset` and `total_len - offset`
- `flags` — modifiers: `0` for normal sends; `MSG_NOSIGNAL` on Linux to suppress `SIGPIPE` when the peer closes (on macOS use `signal(SIGPIPE, SIG_IGN)` instead)

**`send` vs `write`:** equivalent for sockets when `flags = 0`. `send()` has an additional flag: `MSG_NOSIGNAL`, which prevents `SIGPIPE` on a closed connection. On macOS, the equivalent is `SO_NOSIGPIPE` socket option or `signal(SIGPIPE, SIG_IGN)`.

**All `send()` flags:**

| Flag | What it does |
|---|---|
| `0` | Normal send — use this in webserv |
| `MSG_NOSIGNAL` | Linux: suppress `SIGPIPE` when writing to a closed socket; `send()` returns -1 with `errno=EPIPE` instead of killing your process; macOS equivalent is `SO_NOSIGPIPE` setsockopt |
| `MSG_DONTWAIT` | One-shot non-blocking: return `EAGAIN` immediately if the send buffer is full, regardless of `O_NONBLOCK` on the fd |
| `MSG_OOB` | Send out-of-band urgent data; sets the TCP urgent pointer; almost never used in HTTP |
| `MSG_MORE` | Linux only: hint that more data is coming soon; kernel delays transmission to batch with the next `send()` call; equivalent to `TCP_CORK` per-call |
| `MSG_DONTROUTE` | Bypass the routing table; send directly to the local interface; same as `SO_DONTROUTE` but per-call |

**Short writes — send() may not send everything:**

```cpp
// WRONG — assumes all bytes sent in one call
send(client_fd, response.c_str(), response.size(), 0);

// CORRECT — loop until all bytes sent or error
size_t total_sent = 0;
while (total_sent < response.size()) {
    ssize_t n = send(client_fd, response.c_str() + total_sent,
                     response.size() - total_sent, 0);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Send buffer full — store unsent offset, go back to poll() with POLLOUT
            client.response_offset = total_sent;
            return;
        }
        close_client(client_fd);
        return;
    }
    total_sent += n;
}
```

**The right pattern for non-blocking send in webserv:**

Don't try to send everything in one `poll()` iteration. Instead:
1. Build your response string and store it in a per-client send buffer.
2. When `poll()` reports `POLLOUT`, call `send()` once with however many bytes are pending.
3. Update an offset. If all bytes sent, clear `POLLOUT` from events. If not, leave it and continue next iteration.

```cpp
// In your poll loop, when client_fd has POLLOUT:
Client& c = clients[client_fd];
ssize_t n = send(client_fd,
                 c.response.c_str() + c.response_offset,
                 c.response.size()  - c.response_offset, 0);
if (n > 0) {
    c.response_offset += n;
    if (c.response_offset == c.response.size()) {
        // Done sending — clear POLLOUT
        set_events(client_fd, POLLIN);  // only watch for reads now
        if (c.connection == CLOSE)
            close_client(client_fd);
        else
            c.reset_for_next_request();  // keep-alive
    }
}
```

---

## `write()` — generic fd write

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
// Returns: bytes written (>0), -1 on error
```

**Parameters:**
- `fd` — any writable fd: connected socket, pipe write end, regular file, or terminal
- `buf` — pointer to the data to write
- `count` — number of bytes to write from `buf`; may write fewer (short write) if the kernel buffer is partially full — always check the return value

**Use `write()` for:** pipe fds (writing request body to CGI stdin), log files. For sockets, `send()` is preferred.

**Writing to CGI stdin (POST body):**

```cpp
// cgi_stdin_fd is the write end of a pipe going to CGI process's stdin
// Set non-blocking; monitor with POLLOUT in the poll set
ssize_t n = write(cgi_stdin_fd,
                  request_body.c_str() + cgi_stdin_offset,
                  request_body.size()  - cgi_stdin_offset);
if (n > 0) {
    cgi_stdin_offset += n;
    if (cgi_stdin_offset == request_body.size()) {
        // All of the request body written to CGI
        close(cgi_stdin_fd);  // send EOF to CGI (it reads until EOF)
        remove_from_poll(cgi_stdin_fd);
    }
}
```

**Why close the write end after sending:** the CGI script reads `stdin` until EOF. EOF on a pipe happens when all write ends are closed. If you don't close `cgi_stdin_fd` after writing, the CGI process hangs forever waiting for more input.

---

## Non-blocking write — the `POLLOUT` dance

The subject rule: never call `write()`/`send()` without going through `poll()`. In practice:

1. You have a response to send.
2. You set `POLLOUT` on the client's fd in the poll array.
3. `poll()` returns and says this fd is writable (its send buffer has space).
4. You call `send()` once to drain as much as possible.
5. If done, clear `POLLOUT`. If not, leave it set for the next iteration.

The kernel's send buffer is typically 64 KB–128 KB. A small static file response (< 64 KB) will send in one shot. A large file or CGI response may need multiple iterations.

---

## Comparison table

| Function | For | Flags | Notes |
|---|---|---|---|
| `recv(fd, buf, len, 0)` | Sockets | Yes | Use for HTTP request reading |
| `read(fd, buf, len)` | Any fd | No | Use for pipes (CGI), regular files |
| `send(fd, buf, len, 0)` | Sockets | Yes | Use for HTTP response sending |
| `write(fd, buf, len)` | Any fd | No | Use for pipes (CGI stdin), logs |

For sockets: `recv(fd, buf, len, 0)` == `read(fd, buf, len)`. Same for `send`/`write`. The socket versions just expose the flags parameter.

---

## Common bugs

**Bug 1: Trusting that one `recv()` = one HTTP request.** See above — short reads are normal. Accumulate.

**Bug 2: Trusting that one `send()` sends everything.** See above — short writes are normal. Track offset.

**Bug 3: Reading CGI output into a fixed buffer then truncating.** CGI scripts can produce arbitrarily large output (think a PHP script generating a big HTML page). Use a dynamically-growing buffer or `std::string`.

**Bug 4: Not closing CGI stdin pipe after writing body.** CGI hangs forever.

**Bug 5: Calling `send()` when `poll()` hasn't reported `POLLOUT`.** May work (kernel buffer has space), but defeats the non-blocking design. Stick to the protocol.

---

## Related
- [`02_IO_MULTIPLEXING.md`](02_IO_MULTIPLEXING.md) — `poll()` decides *when* to call these
- [`05_PROCESS_AND_CGI.md`](05_PROCESS_AND_CGI.md) — `pipe()`, `dup2()` for connecting these to CGI
- [`../06_FRAMING.md`](../06_FRAMING.md) — how you know when to stop reading (Content-Length, chunked, EOF)
- [`../18_SOCKETS_AND_FDS.md`](../18_SOCKETS_AND_FDS.md) — TCP stream model that explains short reads
