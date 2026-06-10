# Session 02 — The poll() Loop: Theory, Failure Modes, and a Real Busy-Loop Bug

> *From "a connection arrives" to "what actually happens under load" — and a busy-loop that was hiding the server's own output.*

---

## What was worked on

The server from Session 01 already accepted one connection. This session went deep on the **event loop** that turns that into a real server:

- The exact semantics of `poll()` — what it does and, crucially, what it does *not* do
- How the `pollfd` array grows and shrinks, and how many entries it should hold
- What happens to connections **under overload** (the funnel: SYN queue → accept queue → fd limit)
- Generalising from one listening socket to **many**, and from one client to **many**
- Detecting and cleaning up a **client disconnect**
- Debugging a **real busy-loop** in `server/main.cpp` that drowned the real output under 468 K blank lines

Plus: editor/formatting fixes and two new reference docs (`POLL_DEEP_DIVE.md` additions, `TESTS.md`).

---

## Concepts covered

### poll() observes — it never manipulates

`poll()` reads no bytes, sends nothing, accepts nothing, closes nothing. It answers **one** question — *"which of these fds are ready?"* — and stops. Every socket is in the identical state before and after the call.

The only two things it "changes":
1. **It writes the `revents` fields** — its output channel. Never touches your `fd` or `events`.
2. **It consumes time** — parks the process until something is ready or the timeout fires.

> The lookout on a ship shouts *"fd 4 has cargo, fd 5 has room!"* — it never touches the cargo. You do the `recv`/`send`/`accept`.

### The pollfd array is dynamic — "how many?" is the wrong question

You don't pre-size for clients. The array **starts with listen sockets only** and grows on each `accept`, shrinks on each disconnect.

| Kind | How many | Added | Removed |
|---|---|---|---|
| Listen sockets | one per unique `host:port` | once, before the loop | never (until shutdown) |
| Client connections | however many are connected now | after each `accept()` | on disconnect |
| CGI pipes (later) | one or two per running CGI | on fork | when CGI finishes |

Listen-socket count = number of **distinct `host:port` pairs**, *not* `server` blocks. Two `server` blocks on `:8080` (virtual hosts) **share one socket**; you disambiguate later via the `Host:` header. Dedupe by `host:port` or the second `bind()` fails with `EADDRINUSE`.

### The connection funnel — what's between a SYN and your accept()

```
incoming → [ SYN queue ] → handshake → [ accept queue / backlog ] → accept() → your pollfd array
            (kernel)                     (kernel, sized by listen)   (← your first touch)
```

- The **SYN queue** (half-open) and **accept queue** (`listen(fd, backlog)`) are **entirely the kernel's** — no syscall reads them from your program. Your first contact with a connection is the fd that `accept()` returns.
- A client that connects while you're busy **waits in the backlog**, not lost. The backlog is capped by `somaxconn` (`kern.ipc.somaxconn` on macOS).

### Overload & the EMFILE trap

Each connection is an fd, and the process has a ceiling (`RLIMIT_NOFILE`, **256 by default on macOS**). When it's hit, `accept()` returns `-1` / `EMFILE`. The trap: the pending connection is **still in the backlog**, so the listen socket stays readable → `poll()` fires instantly → **100 % CPU busy-loop**.

Fixes:
- **Gate the listen socket** — when at capacity, clear `POLLIN` from the *listen* fd's `events`; new clients wait politely in the backlog; re-enable when an fd frees.
- (or) the **spare-fd trick** — keep one fd in reserve to drain a stuck backlog entry.

### The kernel owns the handshake — retransmission is automatic

- A full queue means the kernel **drops** the SYN/ACK and **stays silent**. TCP's reliability layer turns that silence into an **automatic retransmit with exponential backoff** (~1 s, 2 s, 4 s…). *Silence = backpressure.* To the client this looks like **latency, not an error**, until it exhausts retries → `ETIMEDOUT`.
- A `RST` (nothing listening) is different: **immediate `ECONNREFUSED`**, no retry.
- **`connect()` does not loop** — you call it once; the kernel retransmits the SYN *inside* that one call (blocking), or returns `EINPROGRESS` and finishes in the background (non-blocking → wait for `POLLOUT`, then check `SO_ERROR`). You only loop at the *application* level to retry a whole failed connection (new socket each time).

### RLIMIT_NOFILE — and why webserv can't touch it

`getrlimit`/`setrlimit` are **not on the allowed-functions list**. So in webserv you never raise the limit in code. Instead:
- Raise it from the shell **before** launching: `ulimit -n 4096`.
- In code, **survive** hitting it: cap connections, gate the listen socket, handle `EMFILE` without crashing or spinning. *That resilience is the part that's graded.*

### Using poll() with std::vector<pollfd>

`poll()` wants a contiguous C array — which `std::vector` is. Bridge with **`&fds[0]`**:

```cpp
poll(&fds[0], fds.size(), timeout);
```

- **`&fds[0]`, not `fds.data()`** — `.data()` is C++11, forbidden here.
- Never `&fds[0]` on an **empty** vector (UB) — non-issue while listen sockets are always present.
- `poll()` writes `revents` straight into the vector's storage; read it directly afterward.
- `push_back` may **reallocate** — fine, because you pass `&fds[0]` *fresh* on every `poll()` call. Never cache the raw pointer across iterations.

### Dispatch loop mechanics

After `poll()` returns you walk the vector once, acting only on entries with events:
- **Skip `revents == 0`** entries (most of them).
- Optional: stop early using poll's return value (`ready` count).
- **Init `revents = 0`** on a freshly accepted client's pollfd, so the same pass skips it until the *next* `poll()` fills it.
- Adding a client mid-loop is just `push_back` — it gets *watched* on the **next** `poll()` call (which re-reads the bigger `fds.size()`). There are two nested loops: outer `while` = one `poll()` per turn, inner `for` = handle what it reported.

Generalising to **multiple listen sockets**: replace `fd == listen_fd` with a lookup —
`std::set<int> listen_fds` (membership), or `std::map<int, Listener>` (also tells you *which* server config a new connection belongs to). `accept()` on `fds[i].fd` (the specific socket that fired), not a global.

**`accept()` creates the client socket** — you never call `socket()` per client. The listening socket is a permanent factory; `accept()` mints a fresh connected fd each call.

### Detecting a client close

> **`recv()` returning `0` is THE reliable signal a client closed** — not `POLLHUP`.

A normal client `close()` (FIN) usually surfaces as `POLLIN` + `recv()==0`, and on macOS often **without** `POLLHUP`. So relying on `POLLHUP` alone *misses* most disconnects. Use `recv()==0` as primary, `POLLHUP`/`POLLERR` as a backstop. And since the subject **forbids checking `errno` after read/write**, the whole decision is return-value-driven: `>0` data, `==0` closed, `<0` error → drop.

### Tearing down a connection (close_client / dropClient)

Three undo steps, mirroring what `accept` set up:
1. `close(fd)` — release the fd (frees the number; avoids `EMFILE`).
2. `clients.erase(fd)` — destroy the `Client` state (recv buffer, half-parsed request, CGI).
3. find-and-erase the entry in `poll_fds` — stop `poll()` watching a dead fd (else `POLLNVAL`).

**Erasing during the index loop desyncs `i`.** Two safe patterns:
- **Mark-then-sweep (cleanest):** push the fd to a `to_close` list during the loop; call `close_client` on each *after* the loop. (This is why the helper takes an **fd**, not an index — fds stay valid across shifts.)
- **Erase in place:** `poll_fds.erase(poll_fds.begin() + i)` and **don't `++i`** (the next element slid into `i`). `pop_back()` is last-only; `swap-and-pop` is the O(1) alternative when order doesn't matter.
- **Never reset `revents` manually** — `poll()` overwrites it every call. The only `revents = 0` you write is on a newly created pollfd.

---

## The bug — a busy-loop hiding the real output

**Symptom:** "the server never prints that it accepted a client."

**Reality:** it *did* — `ADDED NEW CLIENT AT FD 4` printed exactly once. It was buried under **468 495 blank lines** generated in ~2 seconds.

**Root cause (two compounding):**
1. An **unconditional** `cout << " " << endl` on the listen fd, fired every loop pass (not gated on `revents`).
2. The server **accepted clients but never `recv()`'d or `close()`'d them.** Once the client sent data (then disconnected → EOF), the client fd was **permanently ready** → `poll()` returned instantly every call → the `while` loop spun → blank-line flood.

**Second iteration of the same bug:** after adding a `recv()` call, its **return value was ignored**. On `recv()==0` (client gone) the fd was never closed → still perpetually ready → still spinning, now printing `BUFFER = …`.

**Fix:** check the return value and tear down on `<= 0`.
```cpp
ssize_t n = recv(poll_fds[i].fd, buffer, STD_BUFFER - 1, 0);
if (n <= 0) {                       // 0 = client closed (FIN); <0 = error
    close(poll_fds[i].fd);
    poll_fds.erase(poll_fds.begin() + i);
    --i;                            // compensate for the for-header's i++
    continue;
}
buffer[n] = '\0';
```

> The lesson that ties the whole session together: **an unhandled ready fd makes `poll()` return instantly, forever.** Draining it (`recv`) and removing it on `recv()==0` is what lets `poll()` sleep again.

---

## Bugs caught and fixed

| Location | Bug | Fix |
|---|---|---|
| `server/main.cpp` | Unconditional `cout << " "` on listen fd → blank-line flood | Gate everything behind `revents == 0` skip; remove debug print |
| `server/main.cpp` | Accepted clients never `recv()`'d/`close()`'d → busy-loop | Add a client-read branch that drains and tears down |
| `server/main.cpp` | `recv()` return value ignored → no close on `recv()==0` | `if (n <= 0) { close(); erase(); }` |
| `server/main.cpp` | `POLLHUP` branch erased but didn't `close()`, and skipped an element | `close()` before `erase`; `--i` after |
| `server/main.cpp` | New client pollfd had uninitialised `revents` | `new_client.revents = 0` before `push_back` |
| `server/main.cpp` | Erase inside `for(...; i++)` skips next element | `--i` after erase, or mark-then-sweep |

---

## Tooling & docs

- **VSCode indentation:** root cause was `editor.detectIndentation` (defaults `true`) overriding the per-file setting. Set `tabSize: 4`, `insertSpaces: false`. Created `.clang-format` (tabs, width 4) at repo root, and a `<leader>f` → `editor.action.formatDocument` mapping. *(`gg=G` delegates to VSCode's formatter, not Vim's indent engine.)*
- **`POLL_DEEP_DIVE.md`** gained sections: *observes-not-manipulates*, *how-many-pollfds + backlog*, *tearing-down a dead fd (POLLNVAL/POLLHUP nuance)*.
- **`library/cpp/webserv/TESTS.md`** created — deep reference for `ab` & `siege` (flags verified against the installed builds: `ab` 2.3, `siege` 4.1.7), and the availability/leak/fd checks the eval runs. Cross-linked from `INDEX.md`, `15_TOOLS.md`, `17_WEBSERV_SUBJECT.md`.

---

## What comes next

The server now accepts, reads, and cleanly closes. The HTTP layer is next:

- Give each client its **own accumulation buffer** (`std::map<int, Client>` with a `recv_buf`)
- Append bytes per `recv()`; detect **`\r\n\r\n`** to know headers are complete
- Parse method, path, version, headers (resumable — a request may arrive in pieces)
- Build a response and register **`POLLOUT`** to flush it (only when you have data to send)
- Then: multiple listening sockets, idle **timeouts**, and the config file

---

## Related notes

- [`POLL_DEEP_DIVE.md`](../POLL_DEEP_DIVE.md) — the full poll() walkthrough this session deepened
- [`ANALOGY.md`](../ANALOGY.md) — the city-and-server analogy
- [`SESSION_01.md`](SESSION_01.md) — sockets, the handshake, the lifecycle
- [`TESTS.md`](../../../../library/cpp/webserv/TESTS.md) — `ab` & `siege` load testing
- [`07_CONNECTION.md`](../../../../library/cpp/webserv/07_CONNECTION.md) — keep-alive, the single-poll() rule, timeouts
- [`functions/02_IO_MULTIPLEXING.md`](../../../../library/cpp/webserv/functions/02_IO_MULTIPLEXING.md) — poll/select/kqueue reference
