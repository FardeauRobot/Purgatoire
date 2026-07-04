# 07 — Connection Management

> *"A connection is a resource. The protocol disagrees with itself about how long to hold one."*

What happens to the TCP socket after a request-response exchange completes? HTTP/1.0 says "close." HTTP/1.1 says "keep it open by default and reuse it." Webserv needs to handle both — and your `poll()`-driven event loop needs to track the lifecycle of every active connection.

---

## The HTTP/1.0 model — one connection per request

```
client                       server
  │   socket()                 socket()
  │                            bind()
  │                            listen()
  │                            poll() waiting for accept
  ├──── connect() ────────────►│
  │                            ├── accept() → new socket
  ├──── GET / HTTP/1.0 ───────►│
  │                            │
  │◄──── HTTP/1.0 200 OK ──────┤
  │                            │
  ├── close() ─── FIN ────────►│
  │                            ├── close() new socket
  │                                              
  │   (new request? new socket. new connect. new accept.)
```

One request = one TCP three-way-handshake = one teardown. **Slow.** Each handshake adds a round-trip; each teardown wastes capacity. Acceptable in 1996. Not in 2026.

---

## The HTTP/1.1 model — persistent connections

```
client                       server
  ├──── connect() ────────────►│ accept()
  ├──── GET /a HTTP/1.1 ──────►│
  │◄──── HTTP/1.1 200 OK ──────┤
  ├──── GET /b HTTP/1.1 ──────►│  ← same TCP socket
  │◄──── HTTP/1.1 200 OK ──────┤
  ├──── GET /c HTTP/1.1 ──────►│
  │◄──── HTTP/1.1 200 OK ──────┤
  │  ... eventually ...
  ├── close() ─── FIN ────────►│
```

The TCP connection stays open for multiple requests. **HTTP/1.1 makes this the default** — no client opt-in needed. The server must be prepared for the client to send another request after each response.

---

## The `Connection` header — explicit control

| Header | Sender | Meaning |
|---|---|---|
| `Connection: close` | either | "After this message, close the connection." |
| `Connection: keep-alive` | either | "I want to reuse this connection." (Implicit default in HTTP/1.1.) |

Examples:

```
GET / HTTP/1.1\r\n
Host: example.com\r\n
Connection: close\r\n          ← client says: don't keep this open
\r\n
```

```
HTTP/1.1 200 OK\r\n
Content-Length: 53\r\n
Connection: close\r\n          ← server says: I'm closing after this
\r\n
<body>
```

### Negotiation rules

- If **either side** sends `Connection: close`, the connection closes after that exchange.
- If both sides are happy to keep it open (no `Connection: close` from either), it stays open.
- Servers should echo their decision: if you're closing, send `Connection: close`. If you're keeping open, you can send `Connection: keep-alive` (optional in HTTP/1.1, mandatory in HTTP/1.0 if you opt in).

---

## Why keep-alive matters

A TCP handshake costs 1 RTT (round-trip time). On a 100ms link, that's 100ms before any HTTP data flows. For a webpage that loads 50 small resources (CSS, JS, images), keep-alive saves 49 × 100ms = ~5 seconds.

For webserv on localhost, the saving is negligible. But the **architectural** difference matters: your server must be able to **parse multiple requests on the same socket** without crashing or leaking.

---

## Implementing keep-alive in webserv

You'll have a `Connection` object per accepted socket. State machine:

```
States:    READING_REQUEST  →  SENDING_RESPONSE  →  IDLE
                  ▲                                    │
                  └────────── next request ────────────┘

On READING_REQUEST:  parse incoming bytes; when full request is in, transition.
On SENDING_RESPONSE: flush response bytes via writeable poll(); when fully written:
                       - if Connection: close → close socket, drop the Connection
                       - else                  → transition to READING_REQUEST (or IDLE)
On IDLE:             waiting for next request bytes; restart the cycle.
```

**Crucial:** the moment `SENDING_RESPONSE` completes, **leftover bytes from the next request might already be in your buffer** (the client pipelined). Your parser needs to handle the case where the read buffer already contains the start of the next request line.

---

## Idle timeouts

A keep-alive connection that sits idle forever leaks file descriptors. You need a per-connection **idle timer**: if no bytes arrive within N seconds (commonly 5-60), close it.

Implementation: track `last_activity` per connection. Each `poll()` iteration, sweep for stale connections and close them. The subject says *"A request to your server should never hang indefinitely"* — this is what enforces that.

You may send `408 Request Timeout` before closing, but you may also just close silently. Either is acceptable.

---

## Pipelining

A client may send **multiple requests back-to-back without waiting** for responses:

```
GET /a HTTP/1.1\r\nHost: x\r\n\r\nGET /b HTTP/1.1\r\nHost: x\r\n\r\n
```

The server should respond **in order**:

```
HTTP/1.1 200 OK\r\n...response for /a...HTTP/1.1 200 OK\r\n...response for /b...
```

In practice, **pipelining is dead** — browsers stopped using it because broken servers couldn't handle it correctly. You can ignore it for webserv: parse requests one at a time, respond, then check for the next. If a client did pipeline, your code naturally handles it (you'll find the next request's bytes already in your buffer after responding).

---

## The "single `poll()`" rule

The webserv subject mandates:

> *"It must be non-blocking and use only 1 poll() (or equivalent) for all the I/O operations between the clients and the server (listen included)."*
>
> *"poll() (or equivalent) must monitor both reading and writing simultaneously."*
>
> *"You must never do a read or a write operation without going through poll() (or equivalent)."*

What this means architecturally:

1. **One event loop.** A single `poll()` call manages every socket: the listening socket, every accepted client socket, every CGI pipe.
2. **The listening socket is part of the poll set.** When `poll()` reports it as readable, you `accept()` a new client and add it to the set.
3. **Every read AND every write goes through poll.** You don't call `recv()` until poll says "this fd is readable." You don't call `send()` until poll says "this fd is writeable."
4. **Track per-fd state** so the loop knows what to do when an fd becomes ready.

Pseudo-code for the loop:

```cpp
// pseudo-code
std::vector<pollfd> fds;
fds.push_back(make_pollfd(listen_fd, POLLIN));

while (true) {
    poll(&fds[0], fds.size(), TIMEOUT_MS);

    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].revents & POLLIN) {
            if (fds[i].fd == listen_fd) {
                accept_new_client(fds);
            } else {
                read_from_client(fds[i].fd);
                // may transition to write-pending; update events to POLLOUT
            }
        }
        if (fds[i].revents & POLLOUT) {
            write_to_client(fds[i].fd);
            // may transition to keep-alive idle; update events to POLLIN
        }
        if (fds[i].revents & (POLLHUP | POLLERR)) {
            close_client(fds, i);
        }
    }

    sweep_timeouts(fds);
}
```

**Trap:** if a client only does a `connect()` and never sends data, `POLLIN` will not fire (no bytes to read). You'll never see them again until they send. Your timeout sweep handles this — close the socket after N seconds of no activity.

---

## `errno` is forbidden after read/write

The subject:

> *"Checking the value of `errno` to adjust the server behaviour is strictly forbidden after performing a read or write operation."*

This is a 42-specific rule that catches many students. The reasoning: on Linux, `recv()` on a non-blocking socket with no data returns `-1` with `errno == EAGAIN` (or `EWOULDBLOCK`). The "normal" way to use non-blocking I/O is to call `recv`, check errno, and react accordingly. **You can't do that here.**

The correct approach: **trust `poll()`**. If `poll()` says the fd is readable, then `recv()` will return data (or 0 for orderly close). You handle return values:

- `> 0` — that many bytes were read; process them.
- `0` — peer closed the connection; close your end.
- `-1` — error; close the connection. **Do not branch on `errno` to decide.**

Same logic for `send()`:
- `> 0` — that many bytes were written; if there's more to write, leave the fd in the poll set with `POLLOUT`.
- `-1` — error; close the connection.

This rule means you cannot reliably distinguish "would block" from "real error" without `errno`. The solution is to always treat `-1` as a fatal error and close the connection. Since `poll()` already told you the fd was ready, `-1` should be rare in practice.

---

## macOS-specific I/O quirks

The subject:

> *"Since macOS handles write() differently from other Unix-based OSes, you are allowed to use fcntl(). [...] However, you are allowed to use fcntl() only with the following flags: F_SETFL, O_NONBLOCK, and FD_CLOEXEC. Any other flag is forbidden."*

On macOS:
- Use `fcntl(fd, F_SETFL, O_NONBLOCK)` to make sockets non-blocking.
- Use `fcntl(fd, F_SETFD, FD_CLOEXEC)` to set the close-on-exec flag (so CGI fork+execve doesn't inherit all your sockets).
- **Do not** use `setsockopt(SO_NOSIGPIPE)` instead — that's a different syscall and is allowed (in the subject's allowed list `setsockopt` is permitted).
- **Do** use `signal(SIGPIPE, SIG_IGN)` to avoid getting killed by `SIGPIPE` when writing to a closed connection. (Or check writes via poll, which is what you're doing.)

---

## TL;DR mental model

> HTTP/1.1 connections are **persistent by default**. Your server must be able to parse N requests on a single socket and respond to each in order. **One `poll()`** manages everything: the listening socket, every client socket, every CGI pipe. Reads and writes only happen after poll says "ready." **Never check `errno` after read/write.** Close on `-1` or `0` returns. Sweep idle connections via a timeout.

**Continue to [`08_URLS.md`](08_URLS.md)** for the request-target's anatomy.
