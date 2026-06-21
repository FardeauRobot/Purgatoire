# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A **C++98 sandbox** for exploring POSIX socket APIs and HTTP concepts ahead of the 42 webserv project. `main.cpp` is the scratch-pad; `Server` is an OCF-compliant stub that will grow into the real thing.

The full project architecture and HTTP knowledge base live in `../../library/cpp/webserv/` — read `OVERVIEW.md` there before designing anything structural.

## Build commands

```sh
make          # build ./WebServ
make re       # clean rebuild
make debug    # rebuild with -g3 -O0 (readable backtraces)
make asan     # rebuild with AddressSanitizer + UBSan
make leaks    # rebuild debug, then run leaks (macOS) / valgrind (Linux)
make watch    # auto-rebuild on source change (needs: brew install fswatch)
```

Compiler: `c++`, flags: `-Wall -Wextra -Werror -Wswitch -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast -std=c++98 -MMD -MP`. Objects land in `objs/`.

## Code conventions

- **C++98 only** — no `auto`, `nullptr`, range-`for`, lambdas, `<thread>`, `<chrono>`, brace-init, `override`, `=default`, `=delete`.
- Every class needs the four OCF members: default ctor, copy ctor, copy-assign, destructor.
- Member variables: `m_` prefix (e.g. `m_name`).
- `utils.hpp` provides ANSI color macros (`BOLD_CYAN`, `RESET`, etc.) and the `endofline` stream manipulator — use them instead of raw escape codes.
- No comments unless the *why* is non-obvious.

## Architecture intent (target webserv design)

`main.cpp` experiments drive toward this layered design (see `../../library/cpp/webserv/OVERVIEW.md`):

| Class | Role |
|---|---|
| `ConfigParser` | Parse `.conf` file → in-memory config tree |
| `Server` | Owns the `poll()` loop, listening sockets, connection list |
| `Connection` | Per-client state machine: read → parse → dispatch → write |
| `RequestParser` | Bytes → structured `Request`; must be resumable |
| `Router` | Match request against config → select handler |
| `Response` | Build and buffer the HTTP response |
| `CgiHandler` | Fork + pipe; CGI stdout → response body |

**Core constraint**: single non-blocking `poll()` loop — never block, never call `read`/`write` without poll permission. All fds (listen sockets, client sockets, CGI pipes) register in the same `pollfd` array.
