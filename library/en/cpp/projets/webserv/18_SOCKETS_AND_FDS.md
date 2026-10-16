# Sockets, File Descriptors, and the Event Loop

> *"The kernel speaks TCP. You speak `read()` / `write()`. Sockets are the translation layer."*

This file covers the **OS layer beneath HTTP** — the one your parser sits on top of. Understanding it stops the "but how does that actually work?" questions from piling up mid-implementation.

---

## 1. File Descriptors: the universal handle

In Unix, **everything is a file** — not metaphorically, but at the kernel API level. A file descriptor is just an integer index into a per-process table the kernel maintains. It can point to:

| fd type      | How you get it |
|---|---|
| Regular file | `open()`       |
| Pipe         | `pipe()`       |
| Terminal     | inherited from shell |
| **Socket**   | `socket()`     |

A socket is just an fd. You `read()` / `write()` on it like any other fd. There is no special socket-read function — the abstraction is already there.

---

## 2. The socket lifecycle

```
socket()   → creates the listening fd (e.g. fd=3)
bind()     → attaches it to port 8080
listen()   → marks it as passive — kernel starts accepting TCP handshakes
accept()   → blocks until a client connects, returns a NEW fd (e.g. fd=4)
read(fd=4) → reads the HTTP request bytes
write(fd=4)→ sends the HTTP response
close(fd=4)→ terminates the connection
```

**Key distinction:** `socket()` gives you the *listening* fd — it never carries payload data. `accept()` gives you a *connection* fd per client. They are two different fds with two different roles.

---

## 3. `sockaddr_in` — giving the kernel an address

`bind()` (and `connect()`, `accept()`) don't take an IP and port as separate arguments. They take a single struct that packages both: `sockaddr_in`.

```c
#include <netinet/in.h>

struct sockaddr_in {
    sa_family_t    sin_family;   // address family — always AF_INET for IPv4
    in_port_t      sin_port;     // port number in network byte order
    struct in_addr sin_addr;     // IP address
    char           sin_zero[8];  // padding — always zero
};

struct in_addr {
    uint32_t s_addr;   // IP as a 32-bit integer, network byte order
};
```

**`sin_family`** — always `AF_INET`. Tells the kernel which address family you're using.

**`sin_port`** — the port in **network byte order**. Always wrap with `htons()`:
```cpp
addr.sin_port = htons(8080);   // correct
addr.sin_port = 8080;          // WRONG — silently binds to port 36895
```

**`sin_addr.s_addr`** — the IP address as a 32-bit integer, also in network byte order:
```cpp
addr.sin_addr.s_addr = htonl(INADDR_ANY);      // 0.0.0.0 — all interfaces
addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1 — loopback only
```

**`sin_zero[8]`** — padding to make `sockaddr_in` the same size as `sockaddr`. Must be zeroed — which is why you always `memset` the whole struct first.

### The cast

`bind()`, `connect()`, `accept()` all take `struct sockaddr *`, not `struct sockaddr_in *`. You cast:

```cpp
bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
```

The kernel reads `sin_family` first to know which concrete type it's dealing with (IPv4, IPv6, Unix socket...), then interprets the remaining bytes accordingly. This is C-era polymorphism.

### Full setup pattern

```cpp
struct sockaddr_in addr;
std::memset(&addr, 0, sizeof(addr));       // zero everything including sin_zero
addr.sin_family      = AF_INET;
addr.sin_port        = htons(8080);
addr.sin_addr.s_addr = htonl(INADDR_ANY);

bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
```

### Why `bind()` fails

| Error | Cause | Fix |
|---|---|---|
| `EADDRINUSE` | Port in `TIME_WAIT` after server restart | Set `SO_REUSEADDR` before `bind()` |
| `EADDRINUSE` | Another process owns the port | `lsof -i :8080` to find it |
| Silent wrong port | Forgot `htons()` on the port | Always wrap port in `htons()` |
| Undefined behavior | Forgot `memset` on the struct | Always `memset(&addr, 0, sizeof(addr))` |
| `EACCES` | Port below 1024 | Use ports ≥ 1024 in webserv |
| `EINVAL` | Socket already bound | Only call `bind()` once per socket fd |

---

## 4. How a connection is established

You never touch TCP yourself. The kernel runs the **3-way handshake** on your behalf as soon as you call `listen()`:

```
Client                          Kernel (on your behalf)
  |--- SYN --------------------------->|   "I want to connect"
  |<-- SYN-ACK ------------------------|   "OK, noted"
  |--- ACK --------------------------->|   "We're connected"
  |                                    |
  [ completed connection sits in the kernel's accept queue ]
```

Your process sleeps in `accept()` (or watches with `poll()`). When you call `accept()`, it **pops one entry off that queue** and gives you an fd. The TCP connection already exists at this point — bytes are already flowing at the kernel level. You are just getting the handle.

A TCP connection is uniquely identified by a **4-tuple**:

```
{ server_ip : server_port , client_ip : client_port }
```

This is how the kernel knows which fd to deliver incoming packets to. Two clients can connect to the same `server_ip:8080` simultaneously because their source ports differ.

---

## 5. TCP is a stream, not a message pipe

This is where most people get burned.

You might expect: one `write()` on the sender = one `read()` on the receiver. That is UDP (datagrams). TCP makes no such promise.

**TCP guarantees:** all bytes arrive, in order, without duplicates.  
**TCP does not guarantee:** that chunk boundaries are preserved.

Think of a garden hose, not a mail system. You pour water in one end; it comes out the other end as a continuous flow, not as labeled glasses.

### In practice

You send a 200-byte HTTP request with one `write()`:

```c
write(fd, request, 200);
```

The receiver might see any of these, all equally valid:

```
read() → 200 bytes          // got it all at once

read() → 80 bytes
read() → 120 bytes          // split in two

read() → 14 bytes
read() → 53 bytes
read() → 133 bytes          // split in three
```

The OS, the network stack, and routers in between can all fragment or delay packets.

### The consequence for your parser

```c
// WRONG — assumes one read() = one complete HTTP request
char buf[4096];
read(fd, buf, 4096);
parse_http_request(buf);    // might be incomplete
```

The fix: **accumulate bytes until you see the HTTP end marker** (`\r\n\r\n`), then parse.

```
loop:
    read() some bytes → append to per-client buffer
    does buffer contain \r\n\r\n ?
        no  → go back to poll(), wait for more data
        yes → parse headers; check Content-Length for body; dispatch
```

---

## 6. Multiple clients: nothing is mixed

Each `accept()` returns a distinct fd:

```
fd 3  → listening socket (the "door" — never carries data)
fd 4  → client A
fd 5  → client B
fd 6  → client C
```

`read(fd=4)` gives you **only** client A's bytes. The kernel routes incoming packets to the right fd via the 4-tuple. Client B's bytes never appear on fd 4.

### The problem: who has data right now?

You can't block on one fd while others also have incoming data:

```c
read(fd=4, ...);  // blocks if client A is silent
read(fd=5, ...);  // never reached
```

`poll()` solves this — it watches all fds simultaneously and wakes up only when one is ready:

```c
struct pollfd fds[3];
fds[0].fd = 4;  fds[0].events = POLLIN;
fds[1].fd = 5;  fds[1].events = POLLIN;
fds[2].fd = 6;  fds[2].events = POLLIN;

poll(fds, 3, -1);   // sleep until ANY fd has data

if (fds[1].revents & POLLIN)
    read(fd=5, ...);    // only client B sent something
```

### Per-client state

Since each client's request may arrive across multiple `read()` calls, each client needs its own accumulation buffer that persists between iterations:

```cpp
struct Client {
    int         fd;
    char        buf[4096];  // accumulation buffer for this client only
    int         buf_len;
    // parsed state: method, path, headers...
};
```

Client A's buffer and client B's buffer never touch.

---

## 7. The full picture

```
                   ┌─────────────────────────────────────┐
                   │           poll() / select()          │
                   │  watches: listen_fd, fd4, fd5, fd6   │
                   └──────────────┬──────────────────────┘
                                  │ "fd 5 is ready"
                   ┌──────────────▼──────────────────────┐
                   │         read(fd=5, buf, N)           │
                   │   append to clients[5].buf           │
                   └──────────────┬──────────────────────┘
                                  │
                   ┌──────────────▼──────────────────────┐
                   │   \r\n\r\n found in buffer?          │
                   │   no  → back to poll()               │
                   │   yes → parse request                │
                   └──────────────┬──────────────────────┘
                                  │
                   ┌──────────────▼──────────────────────┐
                   │   dispatch → build response          │
                   │   write(fd=5, response, len)         │
                   └─────────────────────────────────────┘
```

---

## 8. Layer summary

| Layer | Who handles it | Mechanism |
|---|---|---|
| Byte routing between clients | Kernel | Each connection = its own fd, keyed on 4-tuple |
| "Which fd has data now?" | `poll()` / `select()` | Watches all fds, wakes on any |
| Incomplete messages | Your code | Per-client accumulation buffer |
| HTTP message structure | Your parser | Look for `\r\n\r\n`, then `Content-Length` |

The kernel does the network demultiplexing. `poll()` does the scheduling. Your structs do the memory isolation. None of it is magic — just three independent layers stacked on each other.

---

## Related

- [`01_FUNDAMENTALS.md`](01_FUNDAMENTALS.md) — HTTP as text over TCP, where it sits in the stack
- [`06_FRAMING.md`](06_FRAMING.md) — how you know when a body ends (`Content-Length` / chunked)
- [`16_TINY_SERVER_LAB.md`](16_TINY_SERVER_LAB.md) — build a minimal server; this file is the theory behind it
- [`17_WEBSERV_SUBJECT.md`](17_WEBSERV_SUBJECT.md) — the 42 subject requires `poll()`/`select()`/`kqueue` — now you know why
