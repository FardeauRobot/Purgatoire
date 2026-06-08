#### Socket Lifecycle — `socket`, `bind`, `listen`, `accept`, `connect`, `setsockopt`, `getsockname`

> *The socket is just an fd. Everything else is bookkeeping the kernel does on your behalf.*

These seven calls are the entire TCP server setup in sequence. You call them once at startup, then never again for the lifetime of your server — except `accept()`, which you call once per client.

---

## The kernel's side of sockets

Before touching the API, know what the kernel maintains when you open a socket:

```
Per-process fd table:
  fd 0 → stdin
  fd 1 → stdout
  fd 2 → stderr
  fd 3 → socket (after socket())

Kernel socket structure (simplified):
  {
    type:         SOCK_STREAM  (TCP)
    state:        CLOSED → BOUND → LISTENING → ...
    local_addr:   (empty until bind())
    remote_addr:  (empty until accept())
    send_buf:     [kernel ring buffer, ~4–128 KB by default]
    recv_buf:     [kernel ring buffer]
    backlog_queue: [SYN-received connections waiting for accept()]
    accept_queue:  [completed 3-way handshakes waiting for accept()]
  }
```

The socket starts in `CLOSED` state. Each syscall advances it. Nothing goes on the wire until `listen()`. The buffers are kernel-managed — your `send()` copies into them, your `recv()` copies out of them.

---

## `socket()`

```c
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
// Returns: a new fd on success, -1 on error
```

**What it does:** asks the kernel to allocate a new socket structure and return you its fd. No network activity. No port. Just "give me an endpoint I can configure."

**Parameters for webserv:**

```cpp
int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
//              ^^^^^^^ ^^^^^^^^^^ ^
//              IPv4    TCP        0 = let kernel pick protocol (IPPROTO_TCP)
```

- `AF_INET` → IPv4 address family. Use `AF_INET6` if you want IPv6 or dual-stack (not required by the subject but worth knowing exists).
- `SOCK_STREAM` → TCP. `SOCK_DGRAM` would be UDP — wrong for HTTP.
- `0` → protocol 0 within that domain+type combination. For `AF_INET + SOCK_STREAM` there is exactly one option: TCP. Passing `IPPROTO_TCP` explicitly is equivalent.

**Under the hood:** the **kernel** allocates a `struct socket` in kernel space, puts its index into your fd table, and returns that index. At this point the socket is unbound — it has no IP address and no port. The kernel has not touched the network at all.

**Webserv tip:** if you're listening on multiple ports (required by the subject), call `socket()` once per listening port. Each listen socket is independent. Store them all and add them to your poll set.

```cpp
// Webserv: multiple ports from config
std::vector<int> listen_fds;
for (size_t i = 0; i < config.ports.size(); ++i) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        throw std::runtime_error("socket: " + std::string(strerror(errno)));
    listen_fds.push_back(fd);
}
```

---

## `setsockopt()`

```c
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen);
// Returns: 0 on success, -1 on error
```

**What it does:** sets a configurable option on a socket. For webserv, you need exactly one call to this — `SO_REUSEADDR` — and you call it immediately after `socket()`, before `bind()`.

**Parameters:**
- `sockfd` — the socket fd to configure; returned by `socket()`
- `level` — protocol level: `SOL_SOCKET` for generic socket options, `IPPROTO_TCP` for TCP-specific options
- `optname` — the option to set; `SO_REUSEADDR` is the only one you need in basic webserv
- `optval` — pointer to the option value; for boolean options, a pointer to `int yes = 1`
- `optlen` — size of `*optval` in bytes; `sizeof(int)` for boolean options

**The mandatory call:**

```cpp
int yes = 1;
setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
```

**Why `SO_REUSEADDR` is not optional:** TCP has a `TIME_WAIT` state. When a connection closes, the kernel keeps the 4-tuple (`src_ip:src_port:dst_ip:dst_port`) reserved for up to 2×MSL (Maximum Segment Lifetime ≈ 60–120 seconds) to handle any stray duplicated packets still in transit. During `TIME_WAIT`, `bind()` on the same port returns `EADDRINUSE`.

Without `SO_REUSEADDR`: kill your server, restart it within a minute → `bind()` fails, server won't start. This will happen constantly during development. Set `SO_REUSEADDR` unconditionally.

**Under the hood:** `SO_REUSEADDR` tells the kernel to skip the `TIME_WAIT` exclusivity check for `bind()`. For a listening server (not a client connecting *to* a port it already used), it has no downside — two separate processes can't both bind successfully anyway; the kernel still enforces that.

**Other option you may need (macOS-specific note):**

```cpp
// macOS: also useful for SO_NOSIGPIPE to avoid SIGPIPE on write to closed socket
// — but the subject says use only F_SETFL, O_NONBLOCK, FD_CLOEXEC with fcntl
// So handle SIGPIPE via signal(SIGPIPE, SIG_IGN) instead.
signal(SIGPIPE, SIG_IGN);  // Prevent server death when a client disconnects mid-write
```

**All `SOL_SOCKET` level options:**

| Option | Type | What it does |
|---|---|---|
| `SO_REUSEADDR` | `int` (bool) | Skip TIME_WAIT exclusivity on `bind()` — set this always on listen sockets |
| `SO_REUSEPORT` | `int` (bool) | Multiple sockets can bind the same IP:port (kernel load-balances between them); useful for multi-process servers, not needed for single-process webserv |
| `SO_KEEPALIVE` | `int` (bool) | Kernel periodically sends TCP keepalive probes on idle connections; silently closes dead connections without waiting for a read error |
| `SO_RCVBUF` | `int` | Receive buffer size in bytes; kernel may double the value you set (it adds space for bookkeeping) |
| `SO_SNDBUF` | `int` | Send buffer size in bytes; same caveat as `SO_RCVBUF` |
| `SO_LINGER` | `struct linger` | Controls `close()` behavior when unsent data remains: `l_onoff=1, l_linger=N` makes `close()` block up to N seconds; `l_linger=0` sends RST immediately (hard close) |
| `SO_RCVTIMEO` | `struct timeval` | Timeout for `recv()`; after the timeout elapses, `recv()` returns -1 with `errno=EAGAIN` — alternative to poll() timeouts |
| `SO_SNDTIMEO` | `struct timeval` | Timeout for `send()`; same mechanism as `SO_RCVTIMEO` |
| `SO_ERROR` | `int` (read-only) | Reads and clears the socket's pending async error; useful after `connect()` on a non-blocking socket to check if the connection succeeded |
| `SO_TYPE` | `int` (read-only) | Returns the socket type (`SOCK_STREAM`, `SOCK_DGRAM`, etc.) |
| `SO_NOSIGPIPE` | `int` (bool) | macOS only: suppress `SIGPIPE` on writes to this socket; Linux equivalent is `MSG_NOSIGNAL` flag on `send()` |
| `SO_BROADCAST` | `int` (bool) | Allow sending to broadcast addresses; only relevant for UDP |
| `SO_OOBINLINE` | `int` (bool) | Receive out-of-band (urgent) TCP data inline with normal data instead of via `MSG_OOB` |
| `SO_DEBUG` | `int` (bool) | Enable kernel-level TCP state machine tracing; requires root (`CAP_NET_ADMIN`); output read via platform-specific tools — never used in practice |
| `SO_DONTROUTE` | `int` (bool) | Bypass the routing table; packet sent directly to the interface; only works for hosts on a directly-connected network |

**`IPPROTO_TCP` level options (`level = IPPROTO_TCP`):**

| Option | Type | What it does |
|---|---|---|
| `TCP_NODELAY` | `int` (bool) | Disable Nagle's algorithm — send small packets immediately instead of buffering them waiting for more data; reduces latency at cost of throughput; useful for HTTP/1.1 where headers and body may be sent separately |
| `TCP_KEEPIDLE` | `int` | Seconds idle before sending the first keepalive probe (used with `SO_KEEPALIVE`) |
| `TCP_KEEPINTVL` | `int` | Seconds between keepalive probes |
| `TCP_KEEPCNT` | `int` | Number of unanswered probes before the connection is declared dead |
| `TCP_MAXSEG` | `int` | Maximum segment size (MSS) in bytes; normally negotiated during handshake — override with caution |
| `TCP_CORK` | `int` (bool) | Linux only: hold all outgoing data until the cork is removed or the buffer fills; lets you batch response headers + body into one packet without copying them first |

**For webserv: set `SO_REUSEADDR` unconditionally. `TCP_NODELAY` is optional but reduces latency on interactive HTTP.**

---

## `bind()`

```c
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// Returns: 0 on success, -1 on error
```

**What it does:** registers the socket with the kernel under a specific IP address and port. After this call, the kernel knows: "any TCP packet arriving on this port goes to this socket."

**Parameters:**
- `sockfd` — the listening socket fd returned by `socket()`; must not yet be bound
- `addr` — pointer to the address structure (`struct sockaddr_in` cast to `struct sockaddr *`); contains the IP and port to bind to
- `addrlen` — size of the structure pointed to by `addr`; pass `sizeof(struct sockaddr_in)` for IPv4

**The address structure (IPv4):**

```cpp
#include <netinet/in.h>
#include <arpa/inet.h>

struct sockaddr_in addr;
std::memset(&addr, 0, sizeof(addr));    // clear padding bytes — required
addr.sin_family      = AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 0.0.0.0 — all network interfaces
addr.sin_port        = htons(8080);

bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
```

**`INADDR_ANY` explained:** your machine may have multiple network interfaces (loopback `127.0.0.1`, Ethernet, Wi-Fi, VPN, etc.). `INADDR_ANY` (= `0.0.0.0`) tells the kernel "accept connections on any of these interfaces." For webserv this is usually what you want — the grader may connect from localhost, from the LAN, or from a VM. If you wanted to listen only on localhost, you'd use `inet_addr("127.0.0.1")`.

**`htons()` on the port:** network byte order is big-endian; x86/ARM are little-endian. `htons` (host-to-network short) flips the two bytes. Port 8080 in little-endian is `0x901F`; in big-endian it's `0x1F90`. Always wrap port numbers in `htons()` when filling `sockaddr_in.sin_port`. Forgetting this causes the server to listen on a garbage port with no error.

**Under the hood:** `bind()` adds an entry to the kernel's socket-to-port table. After this, incoming SYN packets to this port will be routed to this socket. The socket is still in `BOUND` state — not yet accepting. No TCP handshakes happen yet.

**Common errors:** `EADDRINUSE` (port in use / `TIME_WAIT` without `SO_REUSEADDR`), `EACCES` (port < 1024 needs root). Full failure table — including the silent-wrong-port and missing-`memset` traps — in [`../18_SOCKETS_AND_FDS.md`](../18_SOCKETS_AND_FDS.md#why-bind-fails).

---

## `listen()`

```c
#include <sys/socket.h>

int listen(int sockfd, int backlog);
// Returns: 0 on success, -1 on error
```

**What it does:** transitions the socket from `BOUND` to `LISTENING`. From this point, the kernel will answer TCP SYN packets on your behalf — it runs the 3-way handshake without your process doing anything. Completed handshakes queue up waiting for you to call `accept()`.

**Parameters:**
- `sockfd` — the bound socket fd; must have been passed to `bind()` first
- `backlog` — maximum number of completed (fully handshaked) connections that can queue up waiting for `accept()`; use 128 or higher for webserv under stress

**The `backlog` parameter:**

```cpp
listen(listen_fd, 128);
```

The kernel maintains *two* queues:
1. **SYN queue (incomplete connections):** connections where SYN received but 3-way handshake not finished yet.
2. **Accept queue (complete connections):** connections where handshake is done, waiting for `accept()`.

`backlog` controls the *accept queue* length (POSIX). On Linux it affects both; the SYN queue has a separate kernel parameter. If the accept queue is full when a new connection completes, the kernel silently drops it — the client will retransmit and retry. Under stress (thousands of connections/second), a small backlog causes visible connection refusals.

**For webserv:** use `128` or higher. The 42 graders stress-test with `ab`/`wrk`. A backlog of 5 is correct according to old manual pages but will cause drops under load.

**Under the hood:** calling `listen()` sets a flag in the kernel socket structure that enables the passive TCP state machine. The kernel now handles SYN → SYN-ACK → ACK autonomously. Your process is not scheduled for any of this. The completed connections accumulate in the accept queue until you call `accept()`.

---

## `accept()`

```c
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// Returns: a new fd for the client connection, or -1 on error
```

**What it does:** dequeues one completed connection from the accept queue and gives you a new fd representing that specific client's TCP channel.

**Critical distinction:** `accept()` does not create a new connection. The connection already exists — the 3-way handshake already completed. `accept()` just hands you the file descriptor for an already-established connection.

**Parameters:**
- `sockfd` — the listening socket fd (the one that went through `socket → bind → listen`); not a client fd
- `addr` — output: the kernel fills this with the client's IP and port (`struct sockaddr_in`); pass `NULL` if you don't need the client address (you usually do — needed for CGI's `REMOTE_ADDR`)
- `addrlen` — in/out: set to `sizeof(struct sockaddr_in)` before calling; updated by the kernel to the actual size written into `addr`

**In blocking mode (tiny server):**

```cpp
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);
int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
// Blocks here if nobody is connected
```

**In non-blocking mode (real webserv — after fcntl O_NONBLOCK on listen_fd):**

```cpp
// poll() said listen_fd is readable → a connection is waiting
int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
if (client_fd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
        continue;   // spurious wakeup — no connection waiting after all
    // real error
}
// Set client_fd non-blocking too, then add to poll set
fcntl(client_fd, F_SETFL, O_NONBLOCK);
```

**Getting the client IP (useful for logging, CGI's REMOTE_ADDR):**

```cpp
char client_ip[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
int client_port = ntohs(client_addr.sin_port);
// REMOTE_ADDR = client_ip, REMOTE_PORT = client_port → set in CGI env
```

**Under the hood:** `accept()` removes one entry from the accept queue and creates a new `struct socket` in kernel space with its own send/recv buffers, bound to the 4-tuple `{server_ip:server_port, client_ip:client_port}`. That's your `client_fd`. The listening socket is unaffected — it stays open and keeps accepting more connections.

**The two-socket model:**

```
fd 3  →  listening socket — one per port, never carries payload data
fd 4  →  client A — its own 64KB send buffer + 64KB recv buffer in kernel
fd 5  →  client B — completely independent
fd 6  →  client C
```

**Webserv trap:** after `accept()`, immediately set `O_NONBLOCK` on the client fd. If you forget, a `read()` on that fd can block your entire event loop — blocking one client blocks every other client.

---

## `connect()`

```c
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// Returns: 0 on success, -1 on error
```

**What it does:** initiates a TCP connection to a remote server. This is the *client-side* call. Your webserv doesn't call `connect()` for normal HTTP serving — but you need it for **CGI over a socket** or if you ever proxy a request upstream.

**Parameters:**
- `sockfd` — an unconnected socket fd created with `socket()`; must not have been bound to a port (or bound to port 0 to let the kernel pick)
- `addr` — pointer to the target server's address; fill `sin_family`, `sin_addr`, and `sin_port` with the destination IP and port
- `addrlen` — size of the address structure; `sizeof(struct sockaddr_in)` for IPv4

**Where it appears in webserv:** the 42 subject lists it as an allowed function. You may not use it directly, but understanding it helps with `socketpair()`. If you implement HTTP proxying (not required), you'd use it.

```cpp
// Not in basic webserv, but pattern for completeness:
int sock = socket(AF_INET, SOCK_STREAM, 0);
struct sockaddr_in target;
memset(&target, 0, sizeof(target));
target.sin_family = AF_INET;
target.sin_port   = htons(80);
inet_pton(AF_INET, "93.184.216.34", &target.sin_addr);
connect(sock, (struct sockaddr*)&target, sizeof(target));
// After this: sock can send() HTTP request bytes, recv() response
```

---

## `getsockname()`

```c
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// Returns: 0 on success, -1 on error
```

**What it does:** retrieves the local address (IP + port) that a socket is bound to. In webserv its main use is populating the CGI `SERVER_NAME` / `SERVER_PORT` env vars — call it on the **client fd**, not the listen fd, so you get the concrete interface the request arrived on (the listen fd reads back `0.0.0.0` when bound to `INADDR_ANY`).

→ Parameters and the full CGI env-var example live with the other address-reading calls in [`04_ADDRESS_CONVERSION.md`](04_ADDRESS_CONVERSION.md#getsockname--find-what-address-a-socket-is-bound-to).

---

## Full setup sequence — webserv-ready

```cpp
int make_listen_socket(const std::string& ip, int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        throw std::runtime_error(std::string("socket: ") + strerror(errno));

    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    if (ip.empty() || ip == "0.0.0.0")
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error(std::string("bind: ") + strerror(errno));

    if (listen(fd, 128) < 0)
        throw std::runtime_error(std::string("listen: ") + strerror(errno));

    // Make it non-blocking before adding to poll set
    fcntl(fd, F_SETFL, O_NONBLOCK);

    return fd;
}
```

---

## State machine summary

```
socket()   → CLOSED (fd exists, no address, no port)
bind()     → BOUND  (fd has IP:port)
listen()   → LISTEN (kernel runs TCP handshakes; accept queue fills)
accept()   → returns new fd in ESTABLISHED state (full duplex, data can flow)
read/write → use the client fd
close()    → FIN sent, connection tears down
```

---

## Related
- [`02_IO_MULTIPLEXING.md`](02_IO_MULTIPLEXING.md) — how `poll()` sits on top of these fds to avoid blocking
- [`03_DATA_TRANSFER.md`](03_DATA_TRANSFER.md) — `read`, `write`, `send`, `recv` on the client fd
- [`04_ADDRESS_CONVERSION.md`](04_ADDRESS_CONVERSION.md) — `htons`, `getaddrinfo` and the byte-order details
- [`../18_SOCKETS_AND_FDS.md`](../18_SOCKETS_AND_FDS.md) — the mental model this file implements
