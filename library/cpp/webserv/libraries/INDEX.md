# Libraries — Header Reference Index

One file per header used in the WebServTest project. Each covers what the header provides (types, constants, functions) and how it fits into a webserv implementation.

---

## System / POSIX headers

| Header | What it gives you | File |
|---|---|---|
| `sys/socket.h` | Grammar — `socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv` | [`SYS_SOCKET_H.md`](SYS_SOCKET_H.md) |
| `netinet/in.h` | Vocabulary — `sockaddr_in`, `INADDR_ANY`, `htons`, `htonl` | [`NETINET_IN_H.md`](NETINET_IN_H.md) |
| `poll.h` | Attention — `poll`, `struct pollfd`, `POLLIN`, `POLLOUT` | [`POLL_H.md`](POLL_H.md) |
| `unistd.h` | Speaking — `read`, `write`, `close`, `fork`, `execve`, `pipe`, `dup2` | [`UNISTD_H.md`](UNISTD_H.md) |

## C++ standard library headers

| Header | What it gives you | File |
|---|---|---|
| `<iostream>` | `std::cout`, `std::cerr` — terminal logging | [`IOSTREAM.md`](IOSTREAM.md) |
| `<string>` | `std::string` — managed byte buffer with rich interface | [`STRING.md`](STRING.md) |
| `<sstream>` | `std::ostringstream` — build strings with stream syntax | [`SSTREAM.md`](SSTREAM.md) |
| `<stdexcept>` | `std::runtime_error`, `std::logic_error` — named exceptions | [`STDEXCEPT.md`](STDEXCEPT.md) |
| `<exception>` | `std::exception` — the base class for all standard exceptions | [`EXCEPTION.md`](EXCEPTION.md) |
| `<cstring>` | `memset`, `memcpy`, `strerror` — raw memory and C strings | [`CSTRING.md`](CSTRING.md) |

---

## The language metaphor

```
netinet/in.h   →  Vocabulary   — the nouns, the names for things
sys/socket.h   →  Grammar      — the rules that structure a conversation
unistd.h       →  Exchange     — the physical act of sending and receiving
poll.h         →  Attention    — deciding who to listen to and when
signal.h/errno →  Reflexes     — reactions to unexpected events
```

---

## Related

- [`../functions/INDEX.md`](../functions/INDEX.md) — per-function deep dives
- [`../18_SOCKETS_AND_FDS.md`](../18_SOCKETS_AND_FDS.md) — how it all fits together
