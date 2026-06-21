# poll() — How It Really Works

> *"poll() doesn't watch anything. It asks the kernel to watch, then goes to sleep. The kernel does the work."*

---

## The problem it solves

Your server has multiple clients connected simultaneously. Each one has a file descriptor. You need to know when any of them sends data.

The naive approach:

```cpp
read(fd_client_A, buf, 4096);   // blocks until A sends something
read(fd_client_B, buf, 4096);   // never reached if A is silent
```

While you're blocked waiting for client A, client B could be screaming. The server is frozen.

The other naive approach — check each fd in a loop:

```cpp
while (true) {
    read(fd_A, ...);   // returns immediately if nothing → EAGAIN
    read(fd_B, ...);   // returns immediately if nothing → EAGAIN
    read(fd_C, ...);   // returns immediately if nothing → EAGAIN
}
```

This works but burns 100% CPU spinning constantly, checking empty fds thousands of times per second.

`poll()` solves both problems: **it blocks without burning CPU, and it watches many fds simultaneously**.

---

## What poll() actually does — the kernel side

When you call `poll()`, your process does not "watch" anything. It hands a list of fds to the kernel and goes to sleep. The kernel does the watching.

Internally:

```
Your process calls poll()
    ↓
Kernel registers your process on the "wait queue" of every fd in your list
    ↓
Your process is suspended — removed from the CPU scheduler
    ↓
    ... time passes, CPU is used by other processes ...
    ↓
A packet arrives on client A's socket
    ↓
Kernel places bytes in client A's receive buffer
    ↓
Kernel sees your process is on client A's wait queue
    ↓
Kernel wakes your process up, fills revents on that pollfd
    ↓
poll() returns — you handle the event
```

This is why `poll()` doesn't burn CPU: your process is literally not running while it waits. The kernel parks it. The CPU is free for other work. When something happens, the kernel taps your process on the shoulder and hands it back to the scheduler.

---

## poll() observes — it never touches your data

`poll()` is **purely informational**. It reads no bytes, sends no bytes, accepts no connections, closes nothing. Every socket and pipe is in the exact same state before and after the call. It answers one question — *"which of these fds are ready?"* — and stops there.

Think of it as the **lookout** on a ship. It shouts *"fd 4 has cargo waiting, fd 5 has room to load!"* — but it never touches the cargo. You're the one who then walks over and does the `recv()` / `send()` / `accept()`.

The only two things poll() "changes":

1. **It writes the `revents` fields.** That's its output channel — it has to put the answer somewhere, and that somewhere is `revents`. It never touches your `fd` or `events`, only `revents`. So it mutates *its own report*, not your sockets.
2. **It consumes time.** It parks your process until something is ready (or the timeout fires). That's a side effect on your program's *flow*, not on any fd.

The clean one-liner: **poll() observes the fds and reports readiness; it never performs I/O.** Every real action happens in *your* code, afterward, in response to the report.

---

## struct pollfd — the form you fill out

```c
struct pollfd {
    int   fd;       // which fd to watch
    short events;   // what you want to know about  ← YOU fill this
    short revents;  // what actually happened        ← KERNEL fills this
};
```

Think of each `pollfd` as a form you fill out and hand to the kernel:

```
fd:     5          "watch file descriptor number 5"
events: POLLIN     "tell me when there's data to read"
```

After `poll()` returns, the kernel has filled `revents`:

```
revents: POLLIN    "there is data on fd 5"
```

You never set `revents` — only the kernel writes it. You never read `events` after the call — only the kernel reads it.

### The flags

```c
POLLIN    // data is available — safe to read() without blocking
POLLOUT   // send buffer has space — safe to write() without blocking
POLLHUP   // peer closed their end (TCP FIN received)
POLLERR   // an error occurred on this fd
POLLNVAL  // fd is not open — a bug on your side
```

`POLLERR`, `POLLHUP`, and `POLLNVAL` are always reported in `revents` regardless of what you put in `events`. You don't need to request them — the kernel reports them automatically.

---

## The call itself

```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

- `fds` — pointer to your array of `pollfd` structs
- `nfds` — how many elements in the array
- `timeout` — how long to wait in milliseconds:
  - `-1` → sleep forever until something happens
  - `0`  → return immediately (don't sleep at all)
  - `N`  → sleep at most N milliseconds, then wake up regardless

Returns: the number of fds that have events, `0` if timeout, `-1` on error.

---

## Level-triggered behavior

`poll()` is **level-triggered** — it reports a fd as ready as long as the condition is true, not just when it transitions.

```
Client sends 100 bytes.
    → kernel fills receive buffer: [100 bytes]
    → poll() wakes up, reports POLLIN on this fd

You read 40 bytes.
    → receive buffer still has [60 bytes]
    → next poll() call: still reports POLLIN
    → because the condition "data available" is still true

You read the remaining 60 bytes.
    → receive buffer is empty
    → next poll() call: POLLIN not set
```

This means you don't have to drain the entire buffer in one go. `poll()` will keep waking you up until the buffer is empty. It's forgiving.

---

## How many pollfds? Listen sockets, clients, and the backlog

A common misconception: that you decide the array size up front. You don't. **The array is dynamic** — it starts tiny and grows and shrinks while the loop runs.

Two-and-a-bit kinds of fds live in it:

| Kind | How many | Added | Removed |
|---|---|---|---|
| **Listen sockets** | One per unique `host:port` in the config | Once, before the loop | Never (until shutdown) |
| **Client connections** | However many are connected *right now* | After each `accept()` | When the client disconnects |
| **CGI pipes** (later) | One or two per running CGI | When you fork | When the CGI finishes |

Before the loop you create pollfds for the **listen sockets only**. Config listens on `:8080` and `:9090`? You start with **2**. Everything else gets appended on the fly.

### Listen sockets: count by host:port, not by server block

"How many listen sockets" = the number of **distinct `host:port` pairs**, *not* the number of `server` blocks. Two `server` blocks both listening on `:8080` (different `server_name`s — virtual hosts) **share one socket**; you tell them apart later by reading the `Host:` header. Dedupe by `host:port`, or the second `bind()` fails with `EADDRINUSE`.

So the real startup order is:

```
parse .conf
    ↓
collect the SET of unique host:port pairs
    ↓
for each:  socket() → setsockopt(SO_REUSEADDR) → bind() → listen()
    ↓
push one pollfd (events = POLLIN) per listen socket
    ↓
enter the poll loop
```

### "What if a client connects mid-loop and there's no pollfd for it?"

It can't lock you out, because there are **two separate queues**:

- **The kernel's accept queue (the `backlog`).** That's the second argument to `listen(fd, backlog)` — a kernel-side queue of connections that finished the TCP handshake but you haven't `accept()`ed yet. A client that knocks while you're busy elsewhere **waits here**. It is not lost, and it has nothing to do with your pollfd array.
- **Your pollfd array.** A client only enters this *after* you `accept()` it off the backlog.

```
         kernel side                        your side
   ┌────────────────────┐            ┌────────────────────┐
   │   backlog queue    │  accept()  │   pollfd vector    │
   │  (sized by the     │  ───────►  │  (grows via        │
   │   2nd arg to       │            │   push_back)       │
   │   listen())        │            │                    │
   └────────────────────┘            └────────────────────┘
            ▲
   incoming SYNs wait here     one entry per accepted client
```

So under load: clients pile into the kernel backlog → your listen socket shows `POLLIN` → you `accept()` them one at a time → each accepted one gets a fresh pollfd appended. The only thing the backlog caps is how many *un-accepted* connections can wait at once; if it overflows (you're catastrophically slow to accept), the kernel refuses *new* SYNs. Nothing about *your* array sizing causes that.

**You never pre-size for clients.** The listen socket is precisely the mechanism that lets you discover and admit them, one at a time.

---

## The loop — four steps

```cpp
std::vector<struct pollfd> fds;

// startup: add the listening socket
struct pollfd pfd;
pfd.fd     = listen_fd;
pfd.events = POLLIN;
fds.push_back(pfd);

while (g_running)
{
    // ── STEP 1 ── hand array to kernel, go to sleep ──────────────
    int ready = poll(&fds[0], fds.size(), 1000);
    // kernel suspends your process here
    // wakes you up when something happens, or after 1000ms

    // ── STEP 2 ── handle errors ───────────────────────────────────
    if (ready == -1) {
        if (errno == EINTR) continue;  // signal interrupted — retry
        break;                          // real error
    }
    if (ready == 0) continue;          // timeout — nothing happened

    // ── STEP 3 ── walk the array, check each fd ───────────────────
    for (size_t i = 0; i < fds.size();) {

        if (fds[i].revents == 0) {      // nothing happened on this fd
            i++;
            continue;
        }

        if (fds[i].fd == listen_fd && fds[i].revents & POLLIN) {
            // new client at the door
            int client_fd = accept(listen_fd, NULL, NULL);
            struct pollfd cpfd;
            cpfd.fd     = client_fd;
            cpfd.events = POLLIN;
            fds.push_back(cpfd);
            i++;
            continue;
        }

        if (fds[i].revents & POLLIN) {
            // existing client sent data
            char buf[4096];
            int n = read(fds[i].fd, buf, sizeof(buf));
            if (n <= 0) {
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                continue;               // don't increment — element slid into i
            }
            // process buf...
        }

        if (fds[i].revents & (POLLHUP | POLLERR)) {  // POLLNVAL needs different handling — see "Tearing down a dead fd"
            close(fds[i].fd);
            fds.erase(fds.begin() + i);
            continue;
        }

        i++;
    }
    // ── STEP 4 ── loop back, hand array to kernel again ───────────
}
```

---

## A timeline with two clients

```
fds array:  [ listen_fd ]

── poll() sleeps ──────────────────────────────────────────────────

Client A connects:
    kernel completes TCP handshake
    listen_fd → POLLIN

── poll() wakes up ────────────────────────────────────────────────

accept() → client_fd_A = 4
fds array: [ listen_fd, fd=4 ]

── poll() sleeps ──────────────────────────────────────────────────

Client B connects AND client A sends "GET /":
    listen_fd → POLLIN
    fd=4      → POLLIN

── poll() wakes up (2 fds ready) ──────────────────────────────────

i=0: listen_fd POLLIN → accept() → client_fd_B = 5
     fds array: [ listen_fd, fd=4, fd=5 ]
i=1: fd=4 POLLIN → read() → "GET /"
i=2: fd=5 revents=0 → skip

── poll() sleeps ──────────────────────────────────────────────────

Client A disconnects:
    fd=4 → POLLHUP

── poll() wakes up ────────────────────────────────────────────────

i=1: fd=4 POLLHUP → close(4) → erase from fds
fds array: [ listen_fd, fd=5 ]

── poll() sleeps ──────────────────────────────────────────────────
```

The array grows and shrinks. `poll()` always watches exactly what's in the array at that moment.

---

## POLLOUT — the tricky one

`POLLIN` is simple: data arrived, read it.

`POLLOUT` requires more care. It means "the kernel's send buffer has space — you can write without blocking." The trap:

**A socket is almost always writable.** If you set `POLLOUT` on every fd all the time, `poll()` returns immediately on every call — your loop spins at 100% CPU doing nothing.

The correct pattern: **only set POLLOUT when you have data to send**.

```cpp
// You built a response and want to send it:
fds[i].events |= POLLOUT;   // start watching for writability

// poll() wakes up, POLLOUT is set:
int n = send(fds[i].fd, response.c_str() + offset, remaining, 0);
offset += n;

if (offset == response.size()) {
    fds[i].events &= ~POLLOUT;   // done — stop watching for writability
}
```

`POLLOUT` is not "I want to write now." It's "tell me when writing won't block, because I have data queued."

---

## Tearing down a dead fd: POLLHUP, POLLERR, POLLNVAL

All three mean "this fd is finished" — close it (mostly) and drop its pollfd. But two details bite people.

| Flag | Meaning | `close()` it? |
|---|---|---|
| `POLLHUP` | Peer hung up — client disconnected, pipe's other end gone | **Yes**, then erase |
| `POLLERR` | Error condition on the fd | **Yes**, then erase |
| `POLLNVAL` | The fd isn't a valid open fd | **No** — see below |

**The POLLNVAL exception — do not `close()` it.** It means the integer in that pollfd isn't an open fd: usually a bookkeeping bug where you closed it somewhere but forgot to erase the entry. Either it's already closed, or — worse — the kernel has recycled that number for a *different* fd, and `close()`ing it would kill an unrelated connection. For `POLLNVAL`: just erase the entry (and fix the bug that let a stale fd linger). For `POLLHUP`/`POLLERR`: `close()` first, *then* erase.

**POLLHUP can arrive *with* unread data.** When a client sends a full request and immediately closes its write end, you can get `POLLHUP` **and `POLLIN` set at once** — a complete request still sitting in your read buffer. Bail on `POLLHUP` instantly and you throw away a request you could have answered. So read first, *then* honor the hangup:

```cpp
if (revents & POLLIN) {
    // drain: recv() everything available
}
if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
    // now tear down: close (skip for POLLNVAL) + erase + destroy Connection
}
```

Whatever you erase, tear down the **associated state** too — the `Connection` object, its buffers, any CGI you spawned. The pollfd is just the array entry; the connection is the object behind it.

---

## Why the erase-without-increment pattern

When you call `fds.erase(fds.begin() + i)`, the vector shifts every element after `i` one position to the left:

```
Before erase at i=2:   [ fd3, fd4, fd5, fd6, fd7 ]
                                     ^
                                     i=2, erasing fd5

After erase:           [ fd3, fd4, fd6, fd7 ]
                                     ^
                                     i=2, now points to fd6
```

If you increment `i` after the erase, you skip `fd6`. So after an erase, don't increment — the next element has slid into position `i` automatically.

```cpp
for (size_t i = 0; i < fds.size();) {
    if (should_remove(fds[i])) {
        fds.erase(fds.begin() + i);
        // no i++ — fd at i+1 is now at i
    } else {
        i++;
    }
}
```

---

## What poll() is NOT

- **Not a thread.** Your process has one thread. poll() doesn't run anything in the background — it just tells you which fd to service next.
- **Not a scheduler.** If two fds are ready simultaneously, poll() reports both but you still handle them one at a time, sequentially.
- **Not instant.** There is a small kernel overhead per call. Don't call it in a tight loop with timeout=0 — that's a busy-wait.
- **Not aware of HTTP.** poll() knows nothing about requests, headers, or `\r\n\r\n`. It only knows bytes are available. What those bytes mean is your problem.

---

## Related

- [`ANALOGY.md`](ANALOGY.md) — the city analogy: poll() as the traffic controller
- [`../../library/cpp/webserv/functions/02_IO_MULTIPLEXING.md`](../../library/cpp/webserv/functions/02_IO_MULTIPLEXING.md) — complete reference: poll, select, epoll, kqueue
- [`../../library/cpp/webserv/libraries/POLL_H.md`](../../library/cpp/webserv/libraries/POLL_H.md) — header reference: flags, struct, function signature
