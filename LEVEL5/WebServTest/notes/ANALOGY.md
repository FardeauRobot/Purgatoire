# The City and the Server

> *"A server does not think. It listens, opens doors, and passes notes. A city does the same."*

---

## Part I — The City

Imagine a city. Not a modern metropolis — a quiet, ordered city where everyone has a precise role and nothing happens by accident.

The city has an **address**: a unique location in the world. Nobody can reach it without knowing that address. Everyone inside the city shares that same address.

Within the city, there are **districts**. Each district handles a different kind of business. District 80 handles general correspondence. District 443 handles confidential letters. District 8080 is where the new, experimental post office was just built. You can have many districts inside the same city, each hosting a different service, all reachable through the same city address.

---

### The Post Office

In one of those districts, someone decides to build a **post office**. Before it can receive a single letter, three things must happen — in order, without shortcuts.

**First, the building is constructed.** It exists. It has walls and a roof. But it has no address yet — no sign, no entry in any directory. It is anonymous, unreachable. Nobody knows it is there.

**Then, the address plate is screwed onto the door.** A clerk from city hall walks over with the official ledger and registers the building at its location: *"District 8080, this city."* From this moment on, letters addressed to that location will find their way here. The building is now officially on the map.

**Then, the post office opens.** A sign goes up: *"Open for business. Visitors welcome."* The building existed, the address was registered — but without this sign, no visitor would dare approach. The act of opening is a public declaration of readiness.

These three steps cannot be skipped or reordered. A building without an address cannot receive mail. An address without an open door receives no visitors.

---

### The Receptionist

The post office has a **reception desk** near the front door. A receptionist sits there all day, watching for arrivals.

When a citizen appears — a browser, a phone, another machine from across the city — they walk up to the desk. The receptionist does something that seems counterintuitive at first: **she does not handle their request herself**. Instead, she opens a **private office** specifically for that citizen, leads them inside, and returns to the front desk.

The next visitor can arrive immediately. The front desk is never occupied for long.

Each citizen gets their own room. Their conversation happens in private, separated from every other conversation happening simultaneously in other rooms. Two citizens never overhear each other.

---

### The Traffic Controller

The post office now has many private offices running at once. Citizen A is composing a letter. Citizen B is waiting for a response. A new visitor just appeared at the front door. Citizen D's office went silent — they may have left.

Somebody must watch all of this at the same time. A **traffic controller** stands at the center of the building, eyes moving between every door and window. They do not read letters. They do not open doors. They do one thing: the moment something moves, they raise a flag.

*"Front door: new visitor."*
*"Office 4: letter incoming."*
*"Office 7: ready to receive a response."*
*"Office 2: citizen has left, door is locked."*

The traffic controller never sleeps. They never wait for one office to finish before watching the next. All doors are watched simultaneously, all the time.

---

### The Letters

When a citizen is settled in their private office, they communicate by sliding **letters** under the door. The server picks up each letter, reads it, writes a response, and slides it back.

The server never receives two citizens' letters through the same slot. Every office has its own slot. The routing is perfect.

When a conversation ends — the citizen's question has been fully answered — the office closes. The door locks. The room is freed for a future visitor.

---

## Part II — The Technical Depth

The analogy maps cleanly to the POSIX socket API. Here is each character named precisely.

---

### `socket()` — Constructing the building

```cpp
int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
```

Creates a bare file descriptor — a kernel-managed communication endpoint with no address, no port, no connection. Just the structure.

`AF_INET` specifies IPv4. `SOCK_STREAM` specifies TCP — a continuous, ordered byte stream. The alternative, `SOCK_DGRAM` (UDP), delivers discrete packets with no ordering guarantee. For HTTP, you always want `SOCK_STREAM`.

The returned integer — 3, 4, 7, whatever — is a file descriptor like any other. In Unix, a socket is just a file. You read and write it the same way.

---

### `bind()` — Screwing on the address plate

```cpp
struct sockaddr_in addr;
std::memset(&addr, 0, sizeof(addr));
addr.sin_family      = AF_INET;
addr.sin_port        = htons(8080);
addr.sin_addr.s_addr = htonl(INADDR_ANY);

bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
```

Stamps an IP address and port onto the socket in the kernel's routing table. After this call, packets arriving at port 8080 on this machine are directed to this fd.

`INADDR_ANY` means all network interfaces — the post office accepts visitors arriving by any road into the city, not just one specific gate.

`htons()` converts the port number to **network byte order** (big-endian). Forget it and you silently bind to the wrong port — one of the more frustrating silent bugs in socket programming.

The `struct sockaddr *` cast is C-era polymorphism: `bind()` accepts IPv4, IPv6, and Unix socket addresses through the same function. The kernel reads `sin_family` first to know which struct it is actually dealing with.

---

### `listen()` — Hanging the "Open" sign

```c
listen(listen_fd, SOMAXCONN);
```

Marks the socket as **passive**. The kernel now completes TCP handshakes on your behalf and queues the resulting connections. The second argument is the backlog — how many completed connections can wait in the queue before `accept()` collects them. `SOMAXCONN` lets the kernel pick the system maximum.

The implication: **TCP connections exist before your code sees them**. By the time you call `accept()`, the three-way handshake is already done. The citizen is already inside the lobby, waiting for their private office.

---

### `accept()` — Opening the private office

```cpp
int client_fd = accept(listen_fd, NULL, NULL);
```

Pops one completed connection off the queue and returns a **new fd** — the private office. The listening fd never carries conversation data. It only ever produces new fds, one per arriving visitor.

This distinction is the most important structural fact of the whole API:

| fd | Role |
|---|---|
| `listen_fd` | The front desk. Produces new fds. Never carries data. |
| `client_fd` | A private office. Carries exactly one client's data. |

Your event loop must watch both categories simultaneously, for different reasons.

---

### `poll()` — The traffic controller

```cpp
struct pollfd fds[N];
fds[0].fd = listen_fd;  fds[0].events = POLLIN;  // new visitor?
fds[1].fd = client_fd;  fds[1].events = POLLIN;  // data incoming?

poll(fds, N, -1);   // sleep until any fd is ready

if (fds[0].revents & POLLIN)   accept();           // open a new office
if (fds[1].revents & POLLIN)   read(client_fd, …); // read the letter
```

`poll()` suspends your process until at least one watched fd becomes ready. The kernel fills `revents` with bitflags describing what happened:

| Flag | What it means |
|---|---|
| `POLLIN` | Data is available — safe to `read()` without blocking |
| `POLLOUT` | Buffer has space — safe to `write()` without blocking |
| `POLLHUP` | The peer closed their end of the connection |
| `POLLERR` | An error occurred on this fd |

The invariant that must never break: **never call `read()` or `write()` without `poll()` permission first**. A `read()` on an unready fd blocks the entire process — and while it blocks, every other client's office is frozen. The traffic controller has fallen asleep. The city stops.

---

### `read()` / `write()` — The letters

```cpp
char buf[4096];
int  n = read(client_fd, buf, sizeof(buf));
write(client_fd, response, response_len);
```

TCP is a **stream**, not a message system. Think of a garden hose, not a postal system. You pour bytes in one end and they arrive at the other end in order — but with no promise about where one "letter" ends and the next begins.

One `write()` on the client may produce any of these on the server:

```
read() → 200 bytes              // full message, all at once
read() →  80 bytes
read() → 120 bytes              // same message, split across two reads
read() →  14 bytes
read() →  53 bytes
read() → 133 bytes              // same message, three pieces
```

The consequence: **each client needs its own accumulation buffer**. You keep appending bytes until you see the HTTP end-of-headers marker (`\r\n\r\n`), then parse. A single `read()` is never guaranteed to hold a complete request.

```
loop:
    read() some bytes → append to this client's buffer
    does buffer contain \r\n\r\n ?
        no  → back to poll(), wait for more
        yes → parse headers; check Content-Length; dispatch
```

---

### `close()` — Locking the office

```c
close(client_fd);
```

Releases the fd and sends a TCP FIN to the client — the network-level signal that the conversation is over. Remove it from your `poll()` array immediately. Watching a closed fd is undefined behavior waiting to surface at the worst possible moment.

---

## The Full Picture

```
                   ┌─────────────────────────────────────┐
                   │             poll() loop              │
                   │  watching: listen_fd, fd4, fd5, fd6  │
                   └──────────────┬──────────────────────┘
                                  │
          ┌───────────────────────┼────────────────────────┐
          │                       │                        │
  listen_fd POLLIN          fd4 POLLIN              fd5 POLLOUT
  (new visitor)           (letter incoming)      (ready to respond)
          │                       │                        │
       accept()               read(fd4)              write(fd5)
       → fd7                  parse HTTP             send response
    open new office
```

No thread sleeps. No office blocks another. The controller watches everything. The front door stays open.

---

## Where the Analogy Ends

Every analogy has a wall. Here is where this one meets it:

- **A city handles citizens one at a time.** Your server handles all clients in the same thread, interleaved by `poll()`. There is no true parallelism — only fast enough interleaving that it does not matter.
- **A private office lasts for one meeting.** An HTTP connection with `Connection: keep-alive` can carry many request-response pairs before closing. The "office" lifetime does not always equal one conversation.
- **Letters have envelopes.** TCP bytes have no start marker, no end marker, no boundary. Your parser must impose structure on what is, at the kernel level, a raw flow of bytes.

The gap between the analogy and reality is exactly where the interesting engineering lives.

---

## The Headers as Language

The socket API is spread across several headers. Each one maps to a distinct layer of the language metaphor:

| Layer | Linguistic equivalent | Headers |
|---|---|---|
| Address structures, constants, byte-swap (`sockaddr_in`, `INADDR_ANY`, `htons`) | **Vocabulary** — the nouns, the names for things | `netinet/in.h` |
| `socket`, `bind`, `listen`, `accept`, `connect` | **Grammar** — the rules that structure a conversation | `sys/socket.h` |
| `read`, `write`, `close` | **Speaking and listening** — the actual exchange | `unistd.h` |
| `poll` | **Attention** — deciding who to listen to and when | `poll.h` |
| `signal`, `errno` | **Reflexes** — reactions to unexpected events | `signal.h`, `errno.h` |

You can know every word in a language and still not form a sentence without grammar. You can form perfect sentences and still say nothing without actually speaking. And without attention, you would try to listen to everyone at once and understand nothing.

The order you learn them is also the order they matter: first understand what you are talking about, then how to structure the connection, then how to exchange data, then how to manage many conversations at once.

---

## Related

- [`18_SOCKETS_AND_FDS.md`](../../library/cpp/webserv/18_SOCKETS_AND_FDS.md) — technical deep-dive on fds and the event loop
- [`01_FUNDAMENTALS.md`](../../library/cpp/webserv/01_FUNDAMENTALS.md) — HTTP as text over TCP, where it sits in the stack
- [`16_TINY_SERVER_LAB.md`](../../library/cpp/webserv/16_TINY_SERVER_LAB.md) — build a minimal server; this analogy is the theory behind it
