# `poll.h`

> *"poll() is attention — it doesn't communicate, it just decides who deserves to be heard."*

Provides `poll()` and `struct pollfd` — the mechanism for watching multiple file descriptors simultaneously without blocking on any of them.

---

## Type

### `struct pollfd` — one fd to watch

```c
struct pollfd {
    int   fd;       // the file descriptor to watch
    short events;   // what you want to know about (you fill this)
    short revents;  // what actually happened (kernel fills this)
};
```

You fill `fd` and `events` before the call. The kernel fills `revents` after `poll()` returns. You never touch `revents` before the call — the kernel overwrites it regardless.

---

## Constants

### Event flags — used in both `events` and `revents`

```c
POLLIN    // data available to read without blocking
POLLOUT   // space available in the send buffer — safe to write
POLLERR   // an error occurred on the fd (always reported, even if not requested)
POLLHUP   // peer closed their end of the connection
POLLNVAL  // fd is not open — a bug on your side
```

`POLLERR`, `POLLHUP`, and `POLLNVAL` are always reported in `revents` regardless of what you put in `events`. You don't need to request them.

---

## Function

### `poll()` — wait for events

```c
#include <poll.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
// Returns: number of fds with events > 0, 0 = timeout, -1 on error
```

**`fds`** — pointer to an array of `pollfd` structs.

**`nfds`** — number of elements in the array.

**`timeout`** — milliseconds to wait:
- `-1` → wait forever until at least one fd is ready
- `0`  → return immediately (non-blocking check)
- `N`  → wait up to N milliseconds

After `poll()` returns, iterate the array and check `revents` on each element.

---

## Usage pattern

```cpp
struct pollfd fds[2];

// watch the listening socket for new connections
fds[0].fd     = listen_fd;
fds[0].events = POLLIN;

// watch a connected client for incoming data
fds[1].fd     = client_fd;
fds[1].events = POLLIN;

while (true) {
    int ready = poll(fds, 2, -1);
    if (ready == -1)
        break;  // error — check errno

    if (fds[0].revents & POLLIN)
        // new client arrived — call accept()

    if (fds[1].revents & POLLIN)
        // client sent data — call read()

    if (fds[1].revents & POLLHUP)
        // client disconnected — call close(), remove from array
}
```

The `&` check is necessary because `revents` can have multiple flags set simultaneously. `POLLIN | POLLHUP` means data arrived *and* the connection closed — read what's left, then clean up.

---

## The invariant that must never break

**Never call `read()` or `write()` without `poll()` permission first.**

A `read()` on an unready fd blocks the entire process. While it blocks, every other client's fd is frozen — the traffic controller has fallen asleep. The whole point of `poll()` is that you only touch an fd when the kernel tells you it's safe.

---

## Managing a dynamic fd array

Your server starts with N listening sockets and grows as clients connect. A fixed-size array doesn't work. The typical pattern:

```cpp
std::vector<struct pollfd> fds;

// add a new fd
struct pollfd pfd;
pfd.fd     = new_fd;
pfd.events = POLLIN;
fds.push_back(pfd);

// remove a closed fd
fds.erase(fds.begin() + i);
```

After erasing, adjust your loop index — the element at position `i` is now the next element.

---

## `poll()` vs `select()` vs `epoll()`

| | `poll()` | `select()` | `epoll()` |
|---|---|---|---|
| fd limit | unlimited | 1024 (FD_SETSIZE) | unlimited |
| API | array of structs | three fd_sets | separate create/ctl/wait |
| performance | O(n) scan | O(n) scan | O(1) per event |
| portability | POSIX | POSIX | Linux only |

For the 42 webserv project, `poll()` is the right choice — portable, no fd limit, straightforward. `epoll()` is only worth it at thousands of simultaneous connections.

---

## `EINTR` — signal interruption

`poll()` returns `-1` with `errno == EINTR` when a signal is delivered during the wait. This is not an error — it's normal. The correct handling:

```cpp
int ready = poll(fds, nfds, timeout);
if (ready == -1 && errno == EINTR)
    continue;   // signal delivered, go back to poll
if (ready == -1)
    throw std::runtime_error("poll failed");
```

---

## Related

- [`SYS_SOCKET_H.md`](SYS_SOCKET_H.md) — socket, accept, send, recv
- [`UNISTD_H.md`](UNISTD_H.md) — read, write, close
- [`../functions/02_IO_MULTIPLEXING.md`](../functions/02_IO_MULTIPLEXING.md) — deep-dive on poll, select, epoll, kqueue
