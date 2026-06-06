# 📋 OVERVIEW — What has to be done for webserv

> *"Pin this to the wall. Re-read it whenever you feel lost in the codebase."*

The single-page execution plan: **what to build**, **in what order**, **and when you're done**. Synthesised from the full subject and the 18 supporting files in this folder.

Use this as the "step back and look at the whole" document. For depth on any item, follow the links to the numbered files.

---

## 🎯 In one paragraph

You're building **an HTTP/1.1 server in C++98** that reads a config file at startup, listens on the configured `interface:port` pairs, accepts client connections, parses HTTP requests, routes each one to a static file / directory listing / file upload / file delete / CGI script / redirect / error page based on per-route config rules, and serves the result back — **all via a single non-blocking `poll()` event loop, never blocking, never calling `read`/`write` without poll permission, and never reading `errno` after I/O.**

That sentence is the entire project. Everything below is decomposition.

---

## 🏛️ Architecture — one diagram

```
                                        ┌──────────────────────┐
                                        │   config file (.conf)│
                                        └──────────┬───────────┘
                                                   │ parsed at startup
                                                   ▼
              ┌──────────────────────────────────────────────────────────┐
              │   Server (the singleton event loop)                       │
              │                                                            │
              │   ┌────────────┐   ┌──────────────┐   ┌────────────────┐ │
              │   │ Listeners  │   │ Connections  │   │ CGI processes  │ │
              │   │ (sockets)  │   │ (per client) │   │ (forked pipes) │ │
              │   └─────┬──────┘   └──────┬───────┘   └────────┬───────┘ │
              │         └────────────┬────┴────────────────────┘         │
              │                      ▼                                    │
              │           ┌──────────────────────┐                       │
              │           │   poll() — single    │                       │
              │           │   manages every fd   │                       │
              │           └──────────────────────┘                       │
              └──────────────────────────────────────────────────────────┘
                          │                                  │
                          ▼                                  ▼
                  ┌──────────────┐                  ┌──────────────┐
                  │   Browsers   │                  │  curl / wrk  │
                  └──────────────┘                  └──────────────┘
```

---

## 🧱 The seven modules of code

You're roughly writing these classes. Names are suggestive; pick your own.

| Module              | Responsibility                                                                 | Heavy uses                                            |
| ------------------- | ------------------------------------------------------------------------------ | ----------------------------------------------------- |
| **`ConfigParser`**  | Read & validate the config file → in-memory `Config` tree                      | string parsing                                        |
| **`Server`**        | Owns the poll loop, listening sockets, connection list, CGI list               | `poll`, `socket`, `bind`, `listen`, `accept`, `fcntl` |
| **`Connection`**    | Per-client state machine: read → parse → dispatch → write                      | `recv`, `send`, the parser                            |
| **`RequestParser`** | Bytes → structured `Request` (method, target, headers, body); resumable        | `std::string` manipulation, chunked decoder           |
| **`Router`**        | Match a `Request` against `Config` → pick the right `Handler`                  | longest-prefix matching                               |
| **`Handlers`**      | StaticFile, DirectoryListing, FileUpload, FileDelete, Redirect, CGI, ErrorPage | `stat`, `open`, `read`, `opendir`, `readdir`, etc.    |
| **`CGIRunner`**     | Fork + execve + pipe management; integrates fds into the server's poll set     | `fork`, `pipe`, `dup2`, `execve`, `waitpid`, `kill`   |

→ See [`16_TINY_SERVER_LAB.md`](16_TINY_SERVER_LAB.md) for the socket plumbing; [`14_CGI.md`](14_CGI.md) for the CGI dance; [`06_FRAMING.md`](06_FRAMING.md) for the parser's hardest part.

---

## 🗺️ Build order — 14 chronological steps

**Linear, not parallel.** Each step should be a single commit-sized chunk. Resist the temptation to interleave.

| # | Step | Files to read first | Done when |
|---|---|---|---|
| 1 | **Tiny server** (~70 lines, blocking, single client) | [16](16_TINY_SERVER_LAB.md) | `curl http://localhost:8080/` returns a fixed HTML response |
| 2 | **Convert to `poll()`** — one client still, but driven by poll | [07](07_CONNECTION.md) | Same result as step 1, but `poll()` mediates every read/write |
| 3 | **Multiple concurrent clients** — add per-client `Connection` state | [02](02_MESSAGE_ANATOMY.md), [07](07_CONNECTION.md) | Two `curl`s in parallel get served independently |
| 4 | **Request parser** — turn raw bytes into structured `Request` | [02](02_MESSAGE_ANATOMY.md), [05](05_HEADERS.md), [06](06_FRAMING.md) | `Request{method, target, headers, body}` for both simple and `Content-Length` requests |
| 5 | **Config parser** — read & validate `webserv.conf` | [17](17_WEBSERV_SUBJECT.md) §Configuration | `./webserv config.conf` starts; bad config rejected cleanly |
| 6 | **Multi-port listening** — bind to every `listen` directive in the config | [17](17_WEBSERV_SUBJECT.md) | Different ports serve different content |
| 7 | **Static file serving** — match route, resolve path, send file with right MIME | [04](04_STATUS_CODES.md), [08](08_URLS.md), [09](09_CONTENT_NEGOTIATION.md) | Browser loads a multi-file static site (HTML, CSS, JS, images) |
| 8 | **Default + configured error pages** — 404, 403, 405, 500 with bodies | [04](04_STATUS_CODES.md) | All error paths return a body; config can override per-code |
| 9 | **POST + multipart uploads** — write client files into the configured upload dir | [03](03_METHODS.md), [09](09_CONTENT_NEGOTIATION.md) | Browser form with `<input type="file">` uploads successfully |
| 10 | **DELETE** — `unlink(2)` files under permitted routes | [03](03_METHODS.md) | `curl -X DELETE` removes the file; 204 / 404 / 403 as appropriate |
| 11 | **Redirects** — per-route `return 301/302 <url>` | [10](10_REDIRECTS.md) | `curl -L` follows the chain to the destination |
| 12 | **Chunked request bodies** — decode `Transfer-Encoding: chunked` | [06](06_FRAMING.md) | A `curl --data-binary @file -H "Transfer-Encoding: chunked"` upload works |
| 13 | **CGI** — fork + exec + pipes + env + un-chunk | [14](14_CGI.md) | A `.py` or `.php` script runs; GET query string + POST body both reach it; output flows back |
| 14 | **Keep-alive + idle timeout + stress test** | [07](07_CONNECTION.md), [15](15_TOOLS.md) | `wrk -t4 -c100 -d30s` doesn't crash, leak, or hang; idle sockets close after N seconds |

**Bonus (only after 1–14 are watertight):**
- 15. Cookies + simple session example → [12](12_COOKIES_SESSIONS.md)
- 16. Multiple CGI extensions → trivial extension of step 13

---

## ⛔ The non-negotiables — get these wrong and you fail outright

The subject is strict on a small set of rules. Any violation = grade 0:

| Rule                                                                | Where covered                                  |
| ------------------------------------------------------------------- | ---------------------------------------------- |
| Server must never crash, ever (including OOM)                       | architecture-wide; design for resilience       |
| Compile with `-Wall -Wextra -Werror` under `-std=c++98`             | [`meta/FLAGS.md`](../../meta/FLAGS.md)         |
| Only the allowed external functions                                 | [17](17_WEBSERV_SUBJECT.md) §Allowed functions |
| **Every** socket/pipe read/write goes through `poll()` first        | [07](07_CONNECTION.md)                         |
| **Single** `poll()` — not one per connection, not one per port      | [07](07_CONNECTION.md)                         |
| **Never** check `errno` after `read`/`recv`/`write`/`send`          | [07](07_CONNECTION.md)                         |
| `fork()` only for CGI                                               | [14](14_CGI.md)                                |
| On macOS: `fcntl()` only with `F_SETFL`, `O_NONBLOCK`, `FD_CLOEXEC` | [07](07_CONNECTION.md)                         |
| README with the 4 required sections, in English                     | [17](17_WEBSERV_SUBJECT.md) §README            |

`grep -n 'errno' src/*.cpp` should return nothing near read/write/recv/send call sites. Do this audit before submitting.

---

## ✅ Done criteria — the night before evaluation

Walk through each:

- [ ] `make re` builds with **zero warnings** under the full flag set
- [ ] All 5 Makefile rules work: `all`, `clean`, `fclean`, `re`, `$(NAME)`
- [ ] `./webserv config.conf` runs with no leaks under valgrind/ASan
- [ ] Static website renders correctly in **2 different browsers** (Chrome + Firefox/Safari)
- [ ] File upload works from a browser `<form enctype="multipart/form-data">`
- [ ] `GET`, `POST`, `DELETE` all return appropriate status codes
- [ ] **CGI** works for at least one language (Python or PHP)
- [ ] Config-overridden error pages render with the right status code
- [ ] `client_max_body_size` is enforced → 413 when exceeded
- [ ] `301`/`302` redirects in config work end-to-end
- [ ] Directory listing on/off both work per config
- [ ] Default file (`index.html`) served when route resolves to a directory
- [ ] Multiple ports → different content (test both)
- [ ] `wrk -t4 -c100 -d30s http://localhost:8080/` — no crashes, **no FD leaks** (`lsof -p $(pgrep webserv) | wc -l` stable before/after)
- [ ] Idle connection closes after timeout
- [ ] README in English with: italicised first line, Description, Instructions, Resources (incl. AI usage)
- [ ] `grep -n 'errno' src/**/*.cpp` reveals no checks after I/O calls
- [ ] You can explain every file, every function, every design choice **without notes**

→ The last bullet is the most important. The subject's AI section ([17](17_WEBSERV_SUBJECT.md) §AI) makes this explicit: code you can't justify = failure.

---

## 🧠 Mental model for the whole project

> Webserv is **one event loop** (`Server`) holding **N file descriptors** (listeners, client sockets, CGI pipes), each with associated **state**. Each iteration: `poll()` reports which fds are ready; you advance the state machines those fds belong to. Eventually each request becomes a response. **Single-threaded, single-process (except for CGI children). Non-blocking everywhere. Buffered I/O at the application layer (you handle partial reads and writes yourself, because the kernel won't).**

When debugging, ask:
1. "What state is this connection in?" (reading headers? reading body? sending response?)
2. "What's the next event I'm waiting for?" (POLLIN? POLLOUT? CGI exit?)
3. "What did `poll()` last report for this fd?"

Most bugs are answered by these three questions.

---

## 🔗 Where to dig deeper

- **Protocol fundamentals** → [01](01_FUNDAMENTALS.md) – [13](13_HTTPS_TLS.md)
- **The hardest single topic** → [06_FRAMING.md](06_FRAMING.md) (chunked encoding)
- **The architecturally hardest topic** → [07_CONNECTION.md](07_CONNECTION.md) (single-poll, non-blocking, errno-forbidden)
- **The most code-heavy feature** → [14_CGI.md](14_CGI.md)
- **The tools you'll live in** → [15_TOOLS.md](15_TOOLS.md)
- **Before writing real code** → [16_TINY_SERVER_LAB.md](16_TINY_SERVER_LAB.md)
- **The subject, faithfully** → [17_WEBSERV_SUBJECT.md](17_WEBSERV_SUBJECT.md)
- **A→Z lookup** → [GLOSSARY.md](GLOSSARY.md)

---

## 📅 Suggested calendar (4 weeks, full-time)

| Week | Steps | Energy goal |
|---|---|---|
| **0 (prep)** | Read the library (1–8 + 16). Build tiny server. | Mental model of HTTP solid; no code in `webserv/` yet. |
| **1** | Steps 1–4 (sockets → poll → multi-client → parser) | Server accepts requests, returns *something* sensible. |
| **2** | Steps 5–8 (config → routing → static → error pages) | Browser can navigate a multi-file site you serve. |
| **3** | Steps 9–11 (POST + uploads → DELETE → redirects) | Full HTTP method set works. |
| **4** | Steps 12–14 (chunked → CGI → keep-alive + stress) + README + cleanup | Stress test passes; defence rehearsed. |

If a step is taking 3x longer than budgeted, **pause and re-read the corresponding library file**. The architecture cost of "I'll figure it out as I go" is much higher than the time to re-orient.

---

> **Last thought:** webserv is the project that taught most 42 students what "non-blocking" actually means at the bone. You will hate the single-poll rule on day 4. You will love it on day 21. Embrace the constraint — the discipline it forces is the entire point of the project.
