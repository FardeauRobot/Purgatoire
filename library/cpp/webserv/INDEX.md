# 🌐 HTTP & Webserv — Support Library

> *"A server is just a polite parser that knows when to stop reading and start writing."*

Everything you need to **understand HTTP at the practitioner level**, so that when the 42 `webserv` subject lands on your desk it reads like a spec you already know — not like a foreign language.

This library is **protocol-first, then project-first**:
- Files `01` → `15` teach you HTTP itself. They are language-agnostic.
- Files `16` → `17` apply HTTP to the actual `webserv` C++98 project.
- The [GLOSSARY](GLOSSARY.md) is for quick lookup when you forget what `Transfer-Encoding: chunked` means at 2am.

---

## 🗺️ Reading order

If you've never written a server before, read these in order. Each builds on the last.

| # | File | What you learn |
|---|---|---|
| 1 | [`01_FUNDAMENTALS.md`](01_FUNDAMENTALS.md) | What HTTP **is**: a stateless text protocol over TCP. Where it sits in the OSI / TCP-IP stack. Request-response model. |
| 2 | [`02_MESSAGE_ANATOMY.md`](02_MESSAGE_ANATOMY.md) | The four parts of every HTTP message: start-line, headers, blank line, body. Read by your parser, written by your responder. |
| 3 | [`03_METHODS.md`](03_METHODS.md) | GET, POST, PUT, DELETE, HEAD, OPTIONS — what each one *means*, plus safety, idempotency, cacheability. |
| 4 | [`04_STATUS_CODES.md`](04_STATUS_CODES.md) | The five families (`1xx`–`5xx`), the ~15 codes you'll actually use, and how to pick the right one. |
| 5 | [`05_HEADERS.md`](05_HEADERS.md) | The headers that matter, grouped by family. Catalogue you can grep when implementing. |
| 6 | [`06_FRAMING.md`](06_FRAMING.md) | **The most important file for webserv.** How a server knows where a body ends: `Content-Length` vs `Transfer-Encoding: chunked`. |
| 7 | [`07_CONNECTION.md`](07_CONNECTION.md) | Connection lifecycle: `Connection: close`, keep-alive, pipelining. Why HTTP/1.1 defaults to persistent. |
| 8 | [`08_URLS.md`](08_URLS.md) | URI anatomy (`scheme://host:port/path?query#fragment`), percent-encoding, query string parsing. |
| 9 | [`09_CONTENT_NEGOTIATION.md`](09_CONTENT_NEGOTIATION.md) | MIME types, `Content-Type` vs `Accept`, charsets, how a client and server agree on a representation. |
| 10 | [`10_REDIRECTS.md`](10_REDIRECTS.md) | The `3xx` family: 301, 302, 303, 307, 308 — when each one is correct. |
| 11 | [`11_CACHING.md`](11_CACHING.md) | `Cache-Control`, `ETag`, `If-None-Match`, `304 Not Modified`. How the same byte gets sent fewer times. |
| 12 | [`12_COOKIES_SESSIONS.md`](12_COOKIES_SESSIONS.md) | How a stateless protocol acquires state. `Set-Cookie`, attributes, sessions vs tokens. |
| 13 | [`13_HTTPS_TLS.md`](13_HTTPS_TLS.md) | What HTTPS adds on top of HTTP. Just enough to be dangerous — webserv doesn't require implementing TLS. |
| 14 | [`14_CGI.md`](14_CGI.md) | How a server delegates request handling to an external program. **Required by the webserv subject.** |
| 15 | [`15_TOOLS.md`](15_TOOLS.md) | Your daily toolkit: `curl -v`, `nc`, browser devtools, `wireshark`, `httpbin`. |
| 16 | [`16_TINY_SERVER_LAB.md`](16_TINY_SERVER_LAB.md) | **Build the dumbest possible HTTP server in ~50 lines of C++**. Single connection, one request, fixed response. Demystifies the whole protocol. |
| 17 | [`17_WEBSERV_SUBJECT.md`](17_WEBSERV_SUBJECT.md) | What the 42 webserv subject **actually demands**: nginx-style config, multiplexed I/O via `poll`/`select`/`kqueue`, CGI, error pages, the whole feature list. |
| 18 | [`18_SOCKETS_AND_FDS.md`](18_SOCKETS_AND_FDS.md) | The OS layer beneath HTTP: file descriptors, socket lifecycle, TCP as a stream, per-client fd isolation, and the `poll()` event loop. |
| 19 | [`19_CONFIGURATION.md`](19_CONFIGURATION.md) | A copy-able NGINX-style config: the minimal one that boots, a realistic annotated one, how a request resolves against it, and C++98 parsing notes. |
| 20 | [`20_HTTP.md`](20_HTTP.md) | **The HTTP hub.** One-screen overview + map to `01`–`12`, the cross-cutting design notes (stateless vs open connection; parse→dispatch→respond), and version handling (1.0 vs 1.1: the enum + three behaviour forks). |

Already building? Jump to the [**By implementation part**](#-by-implementation-part) view below — the same files regrouped by the module you're working on.

Quick reference: [`GLOSSARY.md`](GLOSSARY.md).
**Practical tips & debug tools:** [`TIPS.md`](TIPS.md) — tools, logging habits, development order, and common traps.
**Load & stress testing:** [`TESTS.md`](TESTS.md) — `ab` & `siege` in depth, and the availability/leak/fd checks the eval runs under load.
**Single-page execution plan:** [`OVERVIEW.md`](OVERVIEW.md) — pin this to your wall.
**Syscall reference:** [`functions/INDEX.md`](functions/INDEX.md) — every allowed function explained at the kernel level, with webserv-specific examples.

---

## 🧩 By implementation part

The table above is the **learning** order (protocol first). This is the **building** order — the same files regrouped by the part of `webserv` you're working on. When you're knee-deep in one module, start here.

### Server handling — sockets, the event loop, connections
The OS layer and the non-blocking core. Everything that moves bytes before HTTP starts.

- [`18_SOCKETS_AND_FDS.md`](18_SOCKETS_AND_FDS.md) — fds, the socket lifecycle, TCP-as-a-stream, the poll loop *(the mental model)*
- [`16_TINY_SERVER_LAB.md`](16_TINY_SERVER_LAB.md) — the smallest server that works; build this first
- [`07_CONNECTION.md`](07_CONNECTION.md) — keep-alive, single-`poll()`, the errno-forbidden rule
- [`functions/01_SOCKET_LIFECYCLE.md`](functions/01_SOCKET_LIFECYCLE.md) — `socket`/`bind`/`listen`/`accept`/`setsockopt`
- [`functions/02_IO_MULTIPLEXING.md`](functions/02_IO_MULTIPLEXING.md) — `poll`/`select`/`kqueue`/`fcntl`
- [`POLL_DEEP_DIVE.md`](../../../LEVEL5/WebServTest/notes/POLL_DEEP_DIVE.md) — **deep dive:** the `poll()` loop line by line — kernel-side mechanics, the dynamic pollfd array + backlog, teardown *(sandbox note, companion to `functions/02`)*
- [`functions/03_DATA_TRANSFER.md`](functions/03_DATA_TRANSFER.md) — `read`/`write`/`send`/`recv`, short reads & writes

### HTTP parsing — raw bytes → structured request, structured response → bytes
- [`02_MESSAGE_ANATOMY.md`](02_MESSAGE_ANATOMY.md) — the four parts of every message
- [`05_HEADERS.md`](05_HEADERS.md) — the headers that matter, by family
- [`06_FRAMING.md`](06_FRAMING.md) — **where a body ends:** `Content-Length` vs chunked *(the hardest part)*
- [`20_HTTP.md`](20_HTTP.md) §4 — handling 1.0 vs 1.1: the version enum and its three behaviour forks
- [`03_METHODS.md`](03_METHODS.md) — GET/POST/DELETE semantics, safety, idempotency
- [`04_STATUS_CODES.md`](04_STATUS_CODES.md) — picking the right code

### Routing & static serving — match a request, resolve a path, send a file
- [`08_URLS.md`](08_URLS.md) — URI anatomy, percent-encoding, query strings
- [`09_CONTENT_NEGOTIATION.md`](09_CONTENT_NEGOTIATION.md) — MIME types, `Content-Type`
- [`10_REDIRECTS.md`](10_REDIRECTS.md) — the `3xx` family for per-route `return`
- [`11_CACHING.md`](11_CACHING.md) — `ETag`, `304` *(optional, but cheap wins)*
- [`functions/06_FILESYSTEM.md`](functions/06_FILESYSTEM.md) — `stat`/`open`/`opendir`/`readdir` for files & autoindex

### CGI — delegate a request to an external program
- [`14_CGI.md`](14_CGI.md) — the env/stdin/stdout contract, end to end
- [`functions/05_PROCESS_AND_CGI.md`](functions/05_PROCESS_AND_CGI.md) — `fork`/`execve`/`pipe`/`dup2`/`waitpid`
- [`functions/04_ADDRESS_CONVERSION.md`](functions/04_ADDRESS_CONVERSION.md) — `getsockname`/`ntohs` for `SERVER_NAME`/`SERVER_PORT`

### Config — read & validate the `.conf`
- [`19_CONFIGURATION.md`](19_CONFIGURATION.md) — **a working annotated config to copy from**, plus request-resolution walkthrough and C++98 parsing notes
- [`17_WEBSERV_SUBJECT.md`](17_WEBSERV_SUBJECT.md) §Configuration — the nginx-style directives you must support
- [`TIPS.md`](TIPS.md) §Config file — what to get right before writing the parser

### Cross-cutting references — reach for these from any part
- [`libraries/INDEX.md`](libraries/INDEX.md) — **header reference:** what each `#include` gives you (the *what*)
- [`functions/INDEX.md`](functions/INDEX.md) — **function reference:** how each call behaves at the kernel level (the *how*)
- [`functions/07_ERRORS.md`](functions/07_ERRORS.md) — `strerror`/`gai_strerror`/`errno` discipline
- [`15_TOOLS.md`](15_TOOLS.md) + [`TIPS.md`](TIPS.md) — debugging toolkit and habits
- [`GLOSSARY.md`](GLOSSARY.md) — A→Z lookup

> **The three reference tiers:** `libraries/` answers *"what does this header expose?"*, `functions/` answers *"how does this call behave?"*, and the conceptual files (`18`, `16`, the protocol docs) answer *"why does it fit together this way?"*. When two of them mention the same thing, the deeper one owns the full explanation and the others link to it.

---

## ✅ Validation checklist — "Practitioner" level

You're ready to start `webserv` when you can:

- [ ] Write a raw HTTP/1.1 request by hand into `nc` and get a real reply
- [ ] Read a raw HTTP response in `curl -v` output and explain every line of it
- [ ] Explain the difference between `Content-Length` and `Transfer-Encoding: chunked` and **parse both**
- [ ] Name the methods that are safe, idempotent, and cacheable
- [ ] Pick the correct status code for 8 out of 10 made-up scenarios
- [ ] Explain what keep-alive does and why HTTP/1.1 enables it by default
- [ ] Write a 50-line C++ server that handles one request correctly ([Lab 16](16_TINY_SERVER_LAB.md))
- [ ] Explain how CGI works end-to-end — environment variables, stdin, stdout

If any of these is shaky, re-read the relevant doc.

---

## 🎯 The mental model

Five truths that underpin everything else. Internalize these and the rest is detail.

1. **HTTP is text over TCP.** Every byte you read and write is, in principle, human-readable ASCII (until the body). Open `telnet www.example.com 80`, type a request, see the response. There is no magic.
2. **Stateless by design.** Every request is independent. The server remembers nothing about prior requests *unless* you bolt state on top (cookies, sessions, tokens).
3. **Messages have one shape.** Request and response differ only in the first line. Everything below — headers, blank line, optional body — is identical.
4. **Framing is the hardest part.** The protocol does not tell you how long a body is in the request line. You learn the length from `Content-Length`, from `Transfer-Encoding: chunked`, or you read until the connection closes. Get this wrong and the next request looks like garbage. ([File 06](06_FRAMING.md).)
5. **The server is a parser + a dispatcher + a responder.** Parse a request into structured form, decide what to do (static file? CGI? proxy?), serialise a response. Every web server in the world is variations on this loop.

---

## 📚 Suggested study schedule

For a 42 student starting webserv in ~1 month:

| Week | Goal | Files |
|---|---|---|
| **1** | Understand the protocol end-to-end | `01` → `06` + Lab `16` |
| **2** | Fill in the headers, caching, negotiation | `07` → `12` |
| **3** | CGI + tools + tiny server experiments | `14`, `15`, redo Lab `16` from scratch |
| **4** | Read the actual webserv subject; map it back to this library | `17` then **start coding** |

`HTTPS_TLS` (file `13`) is optional reading — useful career-wide, not required for the webserv subject.

---

## 🔗 Related

- [`netpractice/INDEX.md`](../../netpractice/INDEX.md) — the layer **below** HTTP (IP, routing). HTTP runs over TCP/IP; netpractice teaches the IP part.
- [`cpp/io-errors/FSTREAM_GUIDE.md`](../io-errors/FSTREAM_GUIDE.md) — you'll be doing a lot of streaming I/O in webserv.
- [`cpp/tooling/MAKEFILE_CPP.md`](../tooling/MAKEFILE_CPP.md) — webserv is multi-file; a clean Makefile is non-negotiable.
- [`meta/FLAGS.md`](../../meta/FLAGS.md) — compile webserv with the strictest flag set this repo recommends.

---

## 📌 Notes on this library

- **Spec-faithful but not spec-verbose.** When I say "the spec says X" I mean RFC 7230/7231/7235 (the HTTP/1.1 family). I don't quote the RFCs at length — the goal is mental models, not legal precision.
- **C++98 callouts.** When a topic has a webserv-specific C++ trap (e.g. `std::stringstream` for parsing, no `std::stoi`), the file notes it inline.
- **All ASCII diagrams.** They render in any terminal, any editor, any Obsidian-flavored markdown viewer.
- **Cross-link liberally.** When file `06` mentions chunked encoding, it points to file `02` for the header it lives in. Follow the links when reading.
