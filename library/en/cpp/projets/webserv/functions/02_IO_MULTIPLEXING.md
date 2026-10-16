# I/O Multiplexing — `poll`, `select`, `epoll_*`, `kqueue`/`kevent`, `fcntl`

> *"One thread, N clients, zero blocking. That's the entire architectural constraint of webserv."*

The subject mandates exactly **one** `poll()` (or equivalent) call that watches all file descriptors — listening sockets, client sockets, CGI pipes — simultaneously. This file explains why, how each multiplexer works at the kernel level, and how to structure the event loop.

---

## Why multiplexing is necessary

Without it, the only alternatives are:

1. **One thread per client** — fork or thread per connection. The subject explicitly forbids forking for anything except CGI. Thread-per-client doesn't scale past a few hundred clients and makes shared state hard.
2. **Blocking on one fd at a time** — `read(fd=4)` blocks until client A sends data. Meanwhile client B, C, D, and CGI pipe E are starved. Unacceptable.

Multiplexing lets one thread manage many fds: ask the kernel "tell me *which* fd is ready right now, then I'll act on exactly that one." Your event loop never blocks on a single client.

---

## `fcntl()` — making fds non-blocking

```c
#include <fcntl.h>

int fcntl(int fd, int cmd, ...);
// Returns: depends on cmd; usually 0 or a positive value on success, -1 on error
```

**This must come before `poll()`** — putting a blocking fd into a poll set is technically legal but defeats the purpose: if `poll()` says "fd 5 is readable" and you call `read(fd=5)`, a non-blocking fd returns immediately with `EAGAIN` if no data; a blocking fd might still block (edge cases with zero-copy and kernel buffers).

**Parameters:**
- `fd` — the file descriptor to configure; a socket, pipe, or any other fd
- `cmd` — the operation to perform; webserv only uses `F_SETFL` (set file status flags like `O_NONBLOCK`) and `F_SETFD` (set fd flags like `FD_CLOEXEC`)
- `...` — for `F_SETFL` / `F_SETFD`: an `int` flag value; e.g., `O_NONBLOCK` to make the fd non-blocking, `FD_CLOEXEC` to close it automatically in child processes

**The subject allows only three flags with `fcntl()`:**
- `F_SETFL` + `O_NONBLOCK` — make a socket non-blocking
- `F_SETFD` + `FD_CLOEXEC` — close this fd in child processes (after `fork()`)
- Any other `fcntl()` flag is forbidden.

**All `fcntl()` commands (reference — most are forbidden by the subject):**

| Command | Arg | What it does |
|---|---|---|
| `F_GETFL` | — | Return current file status flags (same set as `F_SETFL`); use to read before OR-ing in `O_NONBLOCK` |
| `F_SETFL` | `int` flags | Set file status flags: `O_NONBLOCK`, `O_APPEND`, `O_ASYNC`, `O_DSYNC`, `O_SYNC` |
| `F_GETFD` | — | Return the fd flags (`FD_CLOEXEC` bit) |
| `F_SETFD` | `int` flags | Set fd flags: only `FD_CLOEXEC` defined by POSIX |
| `F_DUPFD` | `int` min | Duplicate fd to the lowest available number ≥ min; like `dup()` but you control the minimum number |
| `F_DUPFD_CLOEXEC` | `int` min | Like `F_DUPFD` but sets `FD_CLOEXEC` on the new fd atomically |
| `F_GETLK` | `struct flock*` | Query whether a file region is locked (advisory record locking) |
| `F_SETLK` | `struct flock*` | Set or release an advisory file lock; returns immediately with `EACCES`/`EAGAIN` if blocked |
| `F_SETLKW` | `struct flock*` | Like `F_SETLK` but blocks until the lock is acquired |

**Correct way to set `O_NONBLOCK` without clobbering existing flags:**

```cpp
int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
// Skipping F_GETFL and passing O_NONBLOCK directly also works
// but overwrites any O_APPEND or other flags already set.
```

```cpp
// Make a socket non-blocking — call after socket() and after accept()
fcntl(fd, F_SETFL, O_NONBLOCK);
```

**Under the hood:** `O_NONBLOCK` sets a flag in the kernel's open-file description (not just your fd — shared if you `dup()` it). From this point, any `read()` / `write()` / `accept()` on this fd that would normally sleep instead returns `-1` with `errno = EAGAIN` (or `EWOULDBLOCK` — same value on Linux/macOS). Your code checks for `EAGAIN` and goes back to `poll()` rather than blocking.

**`FD_CLOEXEC` for CGI:** when you `fork()` for CGI, the child inherits all open fds. If you don't set `FD_CLOEXEC` on your listening sockets and client sockets, the CGI process holds them open — this prevents the server from cleanly shutting down those sockets. Mark every fd you don't want in children with `FD_CLOEXEC`.

```cpp
// After accept(), before forking CGI:
fcntl(client_fd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
// Or two separate calls — same effect:
fcntl(client_fd, F_SETFL, O_NONBLOCK);
fcntl(client_fd, F_SETFD, FD_CLOEXEC);  // F_SETFD, not F_SETFL for FD_CLOEXEC
```

Wait — `FD_CLOEXEC` is set with `F_SETFD` (file descriptor flags), not `F_SETFL` (file status flags). The subject lists `FD_CLOEXEC` as a valid flag. Be careful which `cmd` to use.

---

## `poll()` — the recommended approach for webserv

```c
#include <poll.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
// Returns: number of fds with events, 0 on timeout, -1 on error
```

**Parameters:**
- `fds` — pointer to an array of `struct pollfd`; one entry per fd to watch; the kernel writes results into each entry's `revents` field on return
- `nfds` — number of entries in the `fds` array; must match the actual array size
- `timeout` — milliseconds to wait: `-1` = block until any fd is ready, `0` = return immediately (non-blocking poll), positive = wait at most that many milliseconds

**Structure:**

```c
struct pollfd {
    int   fd;       // which fd to watch
    short events;   // what events we want
    short revents;  // what events happened (kernel fills this in)
};
```

**Event flags:**

| Flag | Meaning |
|---|---|
| `POLLIN` | fd has data to read (or a new connection on a listen socket) |
| `POLLOUT` | fd has room in its send buffer — you can write without blocking |
| `POLLERR` | error on fd; always reported even if not in `events`; check `revents` only, never set in `events` |
| `POLLHUP` | peer closed or half-closed the connection; always reported; reading after this returns 0 (EOF) |
| `POLLNVAL` | fd is not open or invalid; always reported; indicates a programming error (using a closed fd) |
| `POLLPRI` | urgent / out-of-band data available; TCP urgent pointer set by sender; almost never used in HTTP servers |
| `POLLRDHUP` | Linux 2.6.17+: peer shut down the writing half of the connection (sent FIN); cleaner half-close detection than `POLLHUP`; not available on macOS |

**The webserv event loop skeleton:**

```cpp
std::vector<struct pollfd> poll_fds;

// Add listening sockets
for (size_t i = 0; i < listen_fds.size(); ++i) {
    struct pollfd pfd;
    pfd.fd     = listen_fds[i];
    pfd.events = POLLIN;
    poll_fds.push_back(pfd);
}

while (true) {
    int n = poll(poll_fds.data(), poll_fds.size(), 5000); // 5s timeout
    if (n < 0) {
        if (errno == EINTR) continue;  // interrupted by signal — retry
        throw std::runtime_error(std::string("poll: ") + strerror(errno));
    }
    if (n == 0) {
        sweep_idle_connections();      // timeout: close stale clients
        continue;
    }

    // Iterate only the fds that changed (revents != 0)
    for (size_t i = 0; i < poll_fds.size(); ++i) {
        if (poll_fds[i].revents == 0)
            continue;

        if (is_listen_fd(poll_fds[i].fd)) {
            // New connection on a listen socket
            accept_client(poll_fds[i].fd, poll_fds);
        } else if (poll_fds[i].revents & POLLIN) {
            // Existing client has data — read into its buffer
            read_client(poll_fds[i].fd);
        } else if (poll_fds[i].revents & POLLOUT) {
            // Client's send buffer has space — flush pending response
            write_client(poll_fds[i].fd);
        }
        if (poll_fds[i].revents & (POLLERR | POLLHUP)) {
            close_client(poll_fds[i].fd, poll_fds);
        }
    }
}
```

**Under the hood:** `poll()` is a syscall. It blocks your process in the kernel, registered with the kernel's wait queue for each fd. When any of the watched fds becomes ready, the kernel wakes your process and fills in the `revents` fields. From your process's perspective: you called `poll()`, time passed, it returned with the ready count. The kernel did all the waiting — no busy-loop, no CPU burn.

**Why `poll()` over `select()`:** `select()` uses fixed-size bitmasks (`fd_set`) limited to `FD_SETSIZE` (typically 1024) fds. `poll()` takes a dynamically-sized array with no upper limit. For webserv, 1024 is almost never enough under stress. Use `poll()`.

---

## Managing the poll array — adding and removing fds

You need to:
- Add a new client fd after each `accept()`
- Remove a client fd when the connection closes
- Add a CGI pipe fd while waiting for CGI output

**Removing from a vector:** swap-and-pop to avoid O(n) shifting:

```cpp
void remove_fd(std::vector<struct pollfd>& fds, int fd_to_remove) {
    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd == fd_to_remove) {
            fds[i] = fds.back();   // overwrite with last element
            fds.pop_back();        // shrink by one
            return;
        }
    }
}
```

**POLLOUT is expensive — only set it when you have data to send:**

```cpp
// When you have a complete response ready to send:
for (size_t i = 0; i < poll_fds.size(); ++i) {
    if (poll_fds[i].fd == client_fd) {
        poll_fds[i].events |= POLLOUT;  // request write notification
        break;
    }
}

// After you've sent everything:
poll_fds[i].events &= ~POLLOUT;  // stop requesting write notification
```

If you set `POLLOUT` on every fd all the time, `poll()` returns immediately on every iteration (sockets are almost always writable) — you spin forever at 100% CPU. Only set `POLLOUT` when you actually have bytes to send.

---

## `select()` — the older alternative

```c
#include <sys/select.h>

int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
// Returns: total number of ready fds, 0 on timeout, -1 on error
```

**Parameters:**
- `nfds` — the highest fd number across all three sets, plus 1; the kernel only examines fds 0 through `nfds-1`; compute as `max_fd + 1`
- `readfds` — set of fds to monitor for readability (data available, or new connection on a listen socket); build with `FD_ZERO` / `FD_SET`; pass `NULL` if not needed
- `writefds` — set of fds to monitor for writability (send buffer has space); pass `NULL` if not needed
- `exceptfds` — set of fds to monitor for exceptional conditions (out-of-band data); almost always `NULL` in webserv
- `timeout` — pointer to `struct timeval`; `NULL` = wait forever, zero struct = return immediately (poll), positive = wait that many seconds + microseconds

The subject allows `select()` as an alternative to `poll()`. The mechanics are the same conceptually — you give it sets of fds, it blocks, it tells you which ones are ready.

**The critical difference:** you must rebuild the `fd_set` from scratch on every call:

```cpp
fd_set read_fds, write_fds;
FD_ZERO(&read_fds);
FD_ZERO(&write_fds);

int max_fd = 0;
for (each connected client fd) {
    FD_SET(fd, &read_fds);
    if (client has pending response)
        FD_SET(fd, &write_fds);
    if (fd > max_fd) max_fd = fd;
}

struct timeval tv;
tv.tv_sec  = 5;
tv.tv_usec = 0;
int n = select(max_fd + 1, &read_fds, &write_fds, NULL, &tv);
```

**Why `poll()` is preferred:** `select()` is limited to `FD_SETSIZE` fds (1024 on most systems), and the need to rebuild the fd set every call adds overhead. For webserv's purposes either works; `poll()` is cleaner.

---

## `epoll` — Linux-specific, high-performance

```c
#include <sys/epoll.h>

int  epoll_create(int size);  // size is ignored since Linux 2.6.8, but must be > 0
int  epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int  epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

**`epoll_create` parameters:**
- `size` — historically the expected fd count; ignored by the kernel since Linux 2.6.8 but must be > 0; pass `1`

**`epoll_ctl` parameters:**
- `epfd` — the epoll instance fd returned by `epoll_create()`
- `op` — operation: `EPOLL_CTL_ADD` (start watching `fd`), `EPOLL_CTL_MOD` (change events for `fd`), `EPOLL_CTL_DEL` (stop watching `fd`)
- `fd` — the fd to add, modify, or remove from the epoll instance
- `event` — pointer to `struct epoll_event` specifying which events to watch and optional user data; pass `NULL` for `EPOLL_CTL_DEL`

**`epoll_wait` parameters:**
- `epfd` — the epoll instance fd
- `events` — output array of `struct epoll_event`; the kernel fills it with ready events on return
- `maxevents` — maximum number of events to return; must be > 0; should match the size of your `events` array
- `timeout` — milliseconds to wait: `-1` = wait forever, `0` = return immediately, positive = wait at most that many ms

**Why epoll is fast:** `poll()` and `select()` scan all monitored fds on every call — O(n) per iteration. `epoll` maintains a kernel-side data structure; only *changed* fds are reported — O(1) per ready fd, regardless of total fd count.

**For 42 webserv at 100 concurrent clients, you will not notice the difference.** Use `poll()` unless you specifically want to learn epoll. The subject allows either.

**Basic epoll pattern (for reference):**

```cpp
int epfd = epoll_create(1);  // size ignored, must be > 0

// Add a fd to watch
struct epoll_event ev;
ev.events  = EPOLLIN | EPOLLET;  // edge-triggered
ev.data.fd = client_fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

// Wait
struct epoll_event events[64];
int n = epoll_wait(epfd, events, 64, 5000);
for (int i = 0; i < n; ++i) {
    int fd = events[i].data.fd;
    if (events[i].events & EPOLLIN) { /* read */ }
    if (events[i].events & EPOLLOUT) { /* write */ }
}
```

**Edge-triggered (`EPOLLET`) warning:** edge-triggered means the kernel notifies you only on *transitions* (fd becomes readable), not while it *stays* readable. You must drain the fd completely in one go or you'll miss data. For a beginner, use level-triggered (no `EPOLLET`), which behaves like `poll()`.

**All epoll event flags (`events` / `revents` field of `struct epoll_event`):**

| Flag | Set in `events` | Appears in `revents` | What it means |
|---|---|---|---|
| `EPOLLIN` | Yes | Yes | Data available to read (or new connection on listen socket) |
| `EPOLLOUT` | Yes | Yes | Send buffer has space — write won't block |
| `EPOLLERR` | No | Yes | Error on fd; always reported regardless of `events`; `recv()` will return -1 |
| `EPOLLHUP` | No | Yes | Hang up — peer closed connection; always reported; implies `EPOLLIN` (you'll get EOF on read) |
| `EPOLLRDHUP` | Yes | Yes | Peer shut down writing half (sent FIN); requires Linux 2.6.17+ and `#define _GNU_SOURCE` |
| `EPOLLET` | Yes | — | Edge-triggered mode: notify only on state *transitions*, not while ready; must drain fd completely each time |
| `EPOLLONESHOT` | Yes | — | After one event fires, disable this fd in epoll; re-arm with `EPOLL_CTL_MOD` when ready |
| `EPOLLPRI` | Yes | Yes | Urgent / out-of-band data available; almost never used in HTTP servers |
| `EPOLLWAKEUP` | Yes | — | Linux 3.5+: prevent system suspend while this event is being processed |

---

## `kqueue` / `kevent` — macOS/BSD equivalent of epoll

```c
#include <sys/event.h>

int kqueue(void);
int kevent(int kq, const struct kevent *changelist, int nchanges,
           struct kevent *eventlist,   int nevents,
           const struct timespec *timeout);
```

**`kqueue` parameters:** none — creates and returns a new kernel event queue fd.

**`kevent` parameters:**
- `kq` — the kqueue fd returned by `kqueue()`
- `changelist` — array of `struct kevent` describing new interest registrations; pass `NULL` if only waiting for events, not registering any
- `nchanges` — number of entries in `changelist`; `0` if not registering changes
- `eventlist` — output array of `struct kevent`; the kernel fills it with ready events on return; pass `NULL` if only registering changes
- `nevents` — maximum number of events to return; `0` if not waiting for events; should match the size of `eventlist`
- `timeout` — pointer to `struct timespec`; `NULL` = wait forever, zero struct = return immediately, positive values = wait that many seconds + nanoseconds

**macOS is the evaluation platform at 42.** `kqueue`/`kevent` is the native high-performance event interface on macOS and BSD. It also uses a kernel-side data structure (like epoll) and avoids the O(n) scan of `poll()`.

**Basic pattern:**

```cpp
int kq = kqueue();  // create the event queue

// Register interest in reading from client_fd
struct kevent change;
EV_SET(&change, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
kevent(kq, &change, 1, NULL, 0, NULL);  // changelist only, no wait

// Wait for events
struct kevent events[64];
struct timespec timeout = {5, 0};  // 5 seconds
int n = kevent(kq, NULL, 0, events, 64, &timeout);  // eventlist only, wait
for (int i = 0; i < n; ++i) {
    int fd = (int)events[i].ident;
    if (events[i].filter == EVFILT_READ)  { /* read */ }
    if (events[i].filter == EVFILT_WRITE) { /* write */ }
    if (events[i].flags  & EV_EOF)        { /* connection closed */ }
}
```

**All kevent flags and filters:**

`struct kevent` fields: `ident` (fd or PID), `filter`, `flags`, `fflags`, `data`, `udata`.

**`filter` values — what kind of event to watch:**

| Filter | What it watches |
|---|---|
| `EVFILT_READ` | fd has data to read; for listen sockets, reports number of pending connections in `data` |
| `EVFILT_WRITE` | fd can accept writes without blocking; reports bytes of space in send buffer in `data` |
| `EVFILT_PROC` | Process state changes (exit, fork, exec); `ident` = PID |
| `EVFILT_SIGNAL` | Signal delivery; `ident` = signal number; alternative to `sigaction()` |
| `EVFILT_TIMER` | Periodic timer; fires every `data` milliseconds (or units set by `fflags`) |
| `EVFILT_VNODE` | File/directory changes (rename, write, delete) |

**`flags` values — how to register or interpret the event:**

| Flag | Direction | What it does |
|---|---|---|
| `EV_ADD` | Register | Add the event to the kqueue; if already present, modifies it |
| `EV_DELETE` | Register | Remove the event from the kqueue |
| `EV_ENABLE` | Register | Re-enable a previously disabled event |
| `EV_DISABLE` | Register | Disable the event temporarily without removing it |
| `EV_ONESHOT` | Register | Fire once then automatically delete; avoids a second `kevent()` call to remove |
| `EV_CLEAR` | Register | Reset the event state after it fires (edge-triggered mode) |
| `EV_EOF` | Result | Set in `flags` on return: peer closed the write half of the connection |
| `EV_ERROR` | Result | Set in `flags` on return: error occurred; `data` field holds the errno value |

**For webserv on macOS**, `kqueue` is arguably cleaner than `poll()` because:
- `EV_EOF` tells you the connection closed without needing to check `POLLHUP` separately.
- You register once per fd change, not rebuild the array each call.

---

## The CGI pipe in the poll set

CGI is the place where "only one poll()" matters most:

```
Client fd (POLLIN/POLLOUT)   ← HTTP request / response
CGI stdin pipe write end     (POLLOUT)  ← you write request body
CGI stdout pipe read end     (POLLIN)   ← you read CGI response
```

All of these go into the *same* `poll()` call. The poll loop becomes:

```
fd type         events         action on ready
listen_fd       POLLIN         accept() → new client fd
client_fd       POLLIN         read HTTP request bytes
client_fd       POLLOUT        flush response buffer to client
cgi_stdin_fd    POLLOUT        write request body to CGI
cgi_stdout_fd   POLLIN         read CGI response bytes
```

The CGI process runs as a separate process (fork + exec). You communicate with it purely through pipes. Both ends of both pipes go into your poll set.

---

## Pitfalls and tips

**`EINTR`:** `poll()` can return -1 with `errno == EINTR` if a signal arrives (e.g., `SIGCHLD` from a CGI process exiting). This is not an error — just retry:

```cpp
do {
    n = poll(poll_fds.data(), poll_fds.size(), timeout_ms);
} while (n < 0 && errno == EINTR);
```

**Don't modify the poll array while iterating:** if you add or remove fds inside the loop body, indices shift. Safe pattern: collect changes in a "to_add" / "to_remove" list, apply them after the loop.

**Timeout for idle connection sweep:** the subject says "requests must never hang indefinitely." Use the `poll()` timeout to tick a timer and close clients that haven't sent a complete request in, say, 60 seconds.

**`poll()` timeout is in milliseconds:** `-1` means wait forever (fine for development, not for production). `0` means return immediately (useful for testing but creates a busy loop). `5000` = 5 second tick for idle sweeps.

---

## Related
- [`01_SOCKET_LIFECYCLE.md`](01_SOCKET_LIFECYCLE.md) — the fds you're watching
- [`03_DATA_TRANSFER.md`](03_DATA_TRANSFER.md) — what you do once poll says a fd is ready
- [`05_PROCESS_AND_CGI.md`](05_PROCESS_AND_CGI.md) — CGI pipes that also go into the poll set
- [`../07_CONNECTION.md`](../07_CONNECTION.md) — connection lifecycle from the HTTP perspective
