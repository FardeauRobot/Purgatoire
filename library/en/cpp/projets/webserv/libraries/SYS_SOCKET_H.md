# `sys/socket.h`

> *"netinet/in.h gives you the vocabulary. sys/socket.h gives you the grammar — the verbs that structure every conversation."*

The core socket API. Defines the generic socket interface that works across all address families — IPv4, IPv6, Unix sockets. Deliberately knows nothing about what an internet address looks like; that detail lives in `netinet/in.h`.

| Header | Role |
|---|---|
| `netinet/in.h` | Vocabulary — the nouns: `sockaddr_in`, `INADDR_ANY`, `htons` |
| `sys/socket.h` | Grammar — the verbs: `socket`, `bind`, `listen`, `accept`, `connect` |

You always need both.

---

## Types

```c
sa_family_t   // unsigned short — holds an address family constant (AF_INET, etc.)
socklen_t     // unsigned int — size of an address struct, used by bind/accept/connect
```

### `struct sockaddr` — the generic address

```c
struct sockaddr {
    sa_family_t  sa_family;   // address family — the type tag
    char         sa_data[14]; // raw address bytes, opaque
};
```

Never filled directly. It's the base type all socket functions accept via pointer, so one API can handle IPv4, IPv6, and Unix sockets alike. The kernel reads `sa_family` first to know which concrete type it's dealing with.

You always fill a concrete struct (`sockaddr_in`, `sockaddr_un`...) and cast at the call site:

```cpp
bind(fd, (struct sockaddr *)&addr, sizeof(addr));
```

---

## Constants

### Address families

```c
AF_INET    // IPv4 internet addresses
AF_INET6   // IPv6 internet addresses
AF_UNIX    // local Unix socket — identified by a file path, not an IP
```

### Socket types

```c
SOCK_STREAM    // TCP — ordered, reliable, connection-oriented byte stream
SOCK_DGRAM     // UDP — discrete packets, no ordering, no delivery guarantee
SOCK_RAW       // raw IP — bypasses TCP/UDP entirely (requires root)
```

For HTTP, always `SOCK_STREAM`.

### Option levels

```c
SOL_SOCKET    // socket-level options (vs IPPROTO_TCP for TCP-level options)
```

### Socket options

```c
SO_REUSEADDR   // allow rebinding a port still in TIME_WAIT after server restart
SO_KEEPALIVE   // send periodic probes to detect dead connections
SO_RCVBUF      // set receive buffer size (bytes)
SO_SNDBUF      // set send buffer size (bytes)
SO_ERROR       // read and clear the pending error on the socket
```

The most important one for webserv:

```cpp
int yes = 1;
setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
// must be called BEFORE bind()
```

Without `SO_REUSEADDR`, restarting your server while a connection lingers in `TIME_WAIT` gives `EADDRINUSE` on `bind()`.

### Message flags — for `send()` / `recv()`

```c
MSG_PEEK       // read bytes without consuming them from the buffer
MSG_WAITALL    // block until the full requested length arrives
MSG_DONTWAIT   // non-blocking for this call only, regardless of fd flags
MSG_NOSIGNAL   // don't raise SIGPIPE if the peer has closed the connection
```

### Shutdown directions

```c
SHUT_RD    // stop receiving — further reads return 0
SHUT_WR    // stop sending — further writes fail
SHUT_RDWR  // both directions
```

---

## Functions

### `socket()` — create an endpoint

```c
int socket(int domain, int type, int protocol);
// Returns: fd ≥ 0 on success, -1 on error
```

Allocates a socket structure in the kernel and returns its fd. No network activity. No address yet.

```cpp
int fd = socket(AF_INET, SOCK_STREAM, 0);
// 0 = let the kernel pick the protocol (IPPROTO_TCP here)
```

---

### `bind()` — assign a local address

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// Returns: 0 on success, -1 on error
```

Stamps an address onto the socket. The kernel now routes incoming packets for that IP:port to this fd. Required for servers; clients skip it.

---

### `listen()` — mark as passive

```c
int listen(int sockfd, int backlog);
// Returns: 0 on success, -1 on error
```

Tells the kernel to start completing TCP handshakes and queuing results. `backlog` is the maximum accept queue depth. `SOMAXCONN` picks the system maximum.

---

### `accept()` — pop one connection

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// Returns: new client fd on success, -1 on error
```

Pops one completed connection off the queue and returns a **new fd**. The listening fd is untouched — it keeps accepting.

```cpp
int client_fd = accept(listen_fd, NULL, NULL);  // don't care about client address
```

---

### `connect()` — initiate a connection

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// Returns: 0 on success, -1 on error
```

Client-side only. Completes the TCP three-way handshake with the server at `addr`.

---

### `send()` / `recv()` — socket-aware I/O

```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf,       size_t len, int flags);
// Returns: bytes transferred, 0 = peer closed (recv only), -1 on error
```

Equivalent to `write()`/`read()` when `flags` is `0`. The flags unlock socket-specific behavior — most notably `MSG_NOSIGNAL` on `send()` to avoid `SIGPIPE` when writing to a closed connection.

Both may transfer fewer bytes than requested. Always loop.

---

### `setsockopt()` / `getsockopt()`

```c
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
// Both return: 0 on success, -1 on error
```

---

### `shutdown()` — half-close

```c
int shutdown(int sockfd, int how);
// Returns: 0 on success, -1 on error
```

Closes one or both directions without releasing the fd. Unlike `close()`, takes effect immediately even if other code holds a copy of the fd.

---

## Summary

```
Types      sockaddr, sa_family_t, socklen_t
Constants  AF_*, SOCK_*, SOL_SOCKET, SO_*, MSG_*, SHUT_*

Server     socket() → bind() → listen() → accept() → close()
Client     socket() → connect() → close()
Data       send() / recv()
Config     setsockopt() / getsockopt()
Teardown   shutdown()
```

---

## Related

- [`NETINET_IN_H.md`](NETINET_IN_H.md) — the vocabulary layer: `sockaddr_in`, `INADDR_ANY`, byte-order functions
- [`POLL_H.md`](POLL_H.md) — watching multiple fds simultaneously
- [`UNISTD_H.md`](UNISTD_H.md) — `read`, `write`, `close`
- [`../functions/01_SOCKET_LIFECYCLE.md`](../functions/01_SOCKET_LIFECYCLE.md) — deep-dive on the server setup sequence
