# Session 01 — Sockets, Clients, and the First Handshake

> *First working client-server connection. Conceptual foundations laid.*

---

## What was built

A working TCP handshake between two programs on the same machine:

- `./WebServ` — listens on port 8080, uses `poll()` to detect incoming connections, prints the client fd on arrival
- `./WebServ-client` — creates a socket, sets parameters, calls `connect()`, confirms the connection

Both binaries build from `make re`. Run the server first, then the client in a second terminal.

---

## Concepts covered

### IPC — the family socket belongs to

`socket()` and `pipe()` are both kernel-managed communication channels accessed through file descriptors. The broader family:

| Mechanism | Direction | Scope |
|---|---|---|
| `pipe()` | unidirectional | parent↔child only |
| `socket()` | bidirectional | local or network |
| shared memory | N/A | same machine, fastest |
| semaphore | N/A | synchronization only |
| signal | one-way | async events |

### The Unix philosophy

Everything is a file descriptor. A socket, a pipe, a regular file — the kernel hands you an integer and `read()`/`write()` work on all of them. `poll()` can watch all fd types in the same array because the interface is uniform. The cost: the abstraction leaks — a socket and a file both have `read()` but behave differently under blocking conditions, which is exactly why `poll()` exists.

### The headers as language

Each header maps to a distinct layer of what the socket API provides:

| Layer | Linguistic equivalent | Header |
|---|---|---|
| `sockaddr_in`, `INADDR_ANY`, `htons` | Vocabulary — names for things | `netinet/in.h` |
| `socket`, `bind`, `listen`, `accept`, `connect` | Grammar — how to structure a conversation | `sys/socket.h` |
| `read`, `write`, `close` | Speaking and listening — the exchange | `unistd.h` |
| `poll` | Attention — deciding who to listen to | `poll.h` |
| `signal`, `errno` | Reflexes — reactions to unexpected events | `signal.h`, `errno.h` |

### Byte order

Ports are 16-bit (`htons` / `ntohs`), addresses are 32-bit (`htonl` / `ntohl`). The `s`/`l` suffix stands for short/long. On a little-endian machine (your Mac), these swap the bytes. On a big-endian machine they compile to a no-op. `INADDR_ANY` is `0x00000000` — zero in any byte order, so `htonl` doesn't change it, but wrapping it is still the correct idiom.

---

## The socket lifecycle

### Server side

```
socket()   → bare fd, no address
bind()     → stamps IP + port into kernel routing table
listen()   → kernel starts completing TCP handshakes on your behalf
poll()     → blocks until listen_fd shows POLLIN (new visitor)
accept()   → pops one connection, returns a NEW client_fd
read()     → receives the HTTP request bytes
write()    → sends the HTTP response
close()    → releases the fd, sends TCP FIN
```

### Client side

```
socket()   → bare fd
            (no bind — kernel assigns an ephemeral port)
connect()  → completes the TCP handshake with the server
read()     → receives the server's response
write()    → sends the HTTP request
close()    → done
```

---

## The poll() loop pattern

```cpp
struct pollfd fds[1];
fds[0].fd     = sockfd1;
fds[0].events = POLLIN;

while (g_running) {
    int ret = poll(fds, 1, 1000);
    if (ret == -1 && g_running)
        throw runtime_error("poll failed");

    if (fds[0].revents & POLLIN) {
        int client_fd = accept(sockfd1, NULL, NULL);
        cout << "Client connected on fd " << client_fd << endl;
        close(client_fd);
    }
}
```

`poll()` only answers: *which fds can I touch right now without blocking?* It knows nothing about HTTP or request completeness. Accumulation is your responsibility — each client needs its own buffer, and you keep appending bytes until `\r\n\r\n` appears.

The full future loop shape:

```
poll() → POLLIN on listen_fd  → accept() → add client_fd to poll array
poll() → POLLIN on client_fd  → read() → accumulate → parse when complete → write()
poll() → POLLOUT on client_fd → flush remaining response bytes
poll() → POLLHUP on client_fd → close() → remove from poll array
```

---

## Bugs caught and fixed

| Location | Bug | Fix |
|---|---|---|
| `server/main.cpp` | `sockaddr_in` not `memset` to zero | `memset(&addr, 0, sizeof(addr))` before filling fields |
| `server/main.cpp` | Missing `#include <poll.h>` | Added |
| `client/Client.cpp` | `setParameters` only set `sin_family`, ignored port and address | Fill `sin_port` with `htons()`, `sin_addr.s_addr` with `htonl()` |
| `client/Client.cpp` | `setParameters` had no `return` statement | Added `return (true)` |
| `client/Client.cpp` | `tryConnect` passed `&m_addr` without cast | Cast to `(struct sockaddr *)` |
| `client/Client.cpp` | `m_addrlen = sizeof(sockaddr)` | Should be `sizeof(sockaddr_in)` |
| `client/Client.cpp` | Copy-assign only copied `m_name` | Copy `m_sockfd`, `m_addr`, `m_addrlen` too |

### Recurring runtime issue

Port 8080 stays held if the server doesn't exit cleanly (SIGKILL, crash before `close()`). `SO_REUSEADDR` bypasses `TIME_WAIT` state but not an active listener. Diagnosis and fix:

```sh
lsof -i :8080          # find the zombie PID
kill <PID>             # release the port
# or in one shot:
kill $(lsof -ti :8080)
```

---

## What comes next

- Keep `client_fd` alive instead of immediately closing it
- `read()` the HTTP request bytes into a per-client buffer
- Detect `\r\n\r\n` to know when headers are complete
- Parse method, path, headers
- Build and `write()` an HTTP response

That is where the HTTP layer begins.

---

## Related notes

- [`ANALOGY.md`](../ANALOGY.md) — the city and the server, with technical depth
- [`18_SOCKETS_AND_FDS.md`](../../../library/cpp/webserv/18_SOCKETS_AND_FDS.md) — deep-dive on fds and the event loop
- [`01_FUNDAMENTALS.md`](../../../library/cpp/webserv/01_FUNDAMENTALS.md) — HTTP as text over TCP
