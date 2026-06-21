# HTTP — Overview, Design Notes & Version Handling

> *"A web server is a parser, a dispatcher, and a responder. Everything else is detail."*

This is the **hub** for the HTTP side of webserv. The deep references are files `02`–`12` — each owns one topic in full. This file ties them together: a one-screen overview, the **cross-cutting design notes** that don't belong to any single file, and the **version-handling** rules for speaking HTTP/1.0 and 1.1 from one parser.

If you want history and the protocol's place in the stack, start at [`01_FUNDAMENTALS.md`](01_FUNDAMENTALS.md). If you're building, use the map below to jump to the right file.

---

## 1. HTTP in one screen

Five truths underpin everything (expanded in [`01_FUNDAMENTALS.md`](01_FUNDAMENTALS.md)):

1. **Text over TCP.** Every byte is human-readable ASCII until the body. No magic.
2. **Stateless by design.** Each request is independent; the server remembers nothing unless you bolt state on top — see [§3](#3-design-note-stateless--a-closed-connection).
3. **One message shape.** Request and response differ only in the first line; headers + blank line + optional body are identical. ([`02_MESSAGE_ANATOMY.md`](02_MESSAGE_ANATOMY.md).)
4. **Framing is the hard part.** The start-line doesn't say how long the body is — you learn it from `Content-Length`, from chunked encoding, or by reading to close. ([`06_FRAMING.md`](06_FRAMING.md).)
5. **Server = parse → dispatch → respond.** Parse bytes to a structured request, decide what to do, serialise a response.

### The map — where each topic lives

| Topic | File |
|---|---|
| What HTTP is, versions, the stack | [`01_FUNDAMENTALS.md`](01_FUNDAMENTALS.md) |
| Message anatomy (start-line / headers / body) | [`02_MESSAGE_ANATOMY.md`](02_MESSAGE_ANATOMY.md) |
| Methods (GET/POST/DELETE…), safety, idempotency | [`03_METHODS.md`](03_METHODS.md) |
| Status codes (`1xx`–`5xx`) | [`04_STATUS_CODES.md`](04_STATUS_CODES.md) |
| Headers, by family | [`05_HEADERS.md`](05_HEADERS.md) |
| **Framing:** `Content-Length` vs chunked | [`06_FRAMING.md`](06_FRAMING.md) |
| Connection lifecycle: keep-alive, `close` | [`07_CONNECTION.md`](07_CONNECTION.md) |
| URLs, percent-encoding, query strings | [`08_URLS.md`](08_URLS.md) |
| Content negotiation, MIME types | [`09_CONTENT_NEGOTIATION.md`](09_CONTENT_NEGOTIATION.md) |
| Redirects (the `3xx` family) | [`10_REDIRECTS.md`](10_REDIRECTS.md) |
| Caching (`ETag`, `304`) | [`11_CACHING.md`](11_CACHING.md) |
| Cookies & sessions (state on top) | [`12_COOKIES_SESSIONS.md`](12_COOKIES_SESSIONS.md) |
| **Version handling (1.0 vs 1.1)** | this file, [§4](#4-handling-http-versions--10-and-11) |

---

## 2. Design note: the server is three stages

Whatever class layout you choose ([`OVERVIEW.md`](OVERVIEW.md)), the data flow is always the same three stages:

```
bytes ──► [ PARSE ] ──► Request ──► [ DISPATCH ] ──► handler ──► [ RESPOND ] ──► bytes
          resumable                 router/config              build + buffer
```

- **Parse** must be *resumable* — `recv` hands you partial data, so the parser keeps state and resumes on the next `poll()` wake-up. (This is why a [`Connection`](OVERVIEW.md) holds a read buffer + parse progress.)
- **Dispatch** matches the parsed request against the config ([`19_CONFIGURATION.md`](19_CONFIGURATION.md)) → static file, CGI, or redirect.
- **Respond** serialises a status line + headers + body and buffers it for non-blocking writes.

Every feature in webserv slots into one of these three stages. When stuck, ask: *which stage is this?*

---

## 3. Design note: stateless ≠ a closed connection

A common confusion (worth nailing because it's an eval favourite): HTTP is **stateless**, yet the **fd stays open** across requests. These are two different layers and they don't contradict.

| Layer | What it is | Open connection? | Memory of past requests? |
|---|---|---|---|
| **Transport** (TCP fd) | a byte pipe between two sockets | yes — keep-alive holds it open | n/a |
| **Protocol** (HTTP) | the meaning of each request | irrelevant to it | **no** — each request is self-contained |

The amnesiac-help-desk analogy: the **phone line stays connected** (the fd), but the agent has **amnesia between sentences** (statelessness). You re-introduce yourself every time — which is exactly why a client re-sends its cookie/token on *every* request, even over the same open connection. ([`12_COOKIES_SESSIONS.md`](12_COOKIES_SESSIONS.md) is how state gets bolted on.)

**Why keep-alive exists if it adds no memory:** pure performance. A new TCP connection costs a round-trip handshake (plus TLS if HTTPS); a page pulls dozens of resources, so HTTP/1.1 reuses one open fd for many request/response pairs. ([`07_CONNECTION.md`](07_CONNECTION.md).)

**What this means for your code:** your `Connection` object *does* hold state — read buffer, parse progress, which request it's mid-way through — because the **connection** is stateful even though the **protocol** isn't. But once a request is fully served you **reset the parser** and treat the next request on that same fd as brand new. You never carry "this client asked for `/foo` last time" forward; if a CGI wants that, it manages a session via a cookie the client re-sends.

---

## 4. Handling HTTP versions — 1.0 and 1.1

The subject "suggests" HTTP/1.0, but real browsers speak HTTP/1.1 — so your server handles **both**. They share the same wire format ([`02_MESSAGE_ANATOMY.md`](02_MESSAGE_ANATOMY.md)); "supporting both" is **one parser plus ~3 conditionals**, not two code paths.

### 4.1 Read the version from the request line

```
GET /index.html HTTP/1.1
                ^^^^^^^^
```

Parse it into an enum stored on your `Request`:

```cpp
enum HttpVersion {
    HTTP_VERSION_1_0,
    HTTP_VERSION_1_1,
    HTTP_VERSION_UNKNOWN
};
```

Two distinct failure modes if the token isn't supported:

| Token | Meaning | Response |
|---|---|---|
| `HTTP/banana` | malformed — not a valid version | `400 Bad Request` |
| `HTTP/2.0` | well-formed but unsupported | `505 HTTP Version Not Supported` |

Don't collapse these — broken syntax and "a version I don't speak" are different, and a grader may probe both.

### 4.2 The three behaviours that differ

Everything else (methods, status codes, static serving, CGI) is version-agnostic.

| Behaviour | HTTP/1.0 | HTTP/1.1 |
|---|---|---|
| **Connection default** | close after response | **keep-alive** by default |
| **`Host` header** | optional | **required** (absent → `400`) |
| **Chunked transfer** | not supported | `Transfer-Encoding: chunked` allowed |

### 4.3 Branch at those three points

**Keep-alive decision** — consulted after every response; the explicit `Connection` header overrides the per-version default ([`07_CONNECTION.md`](07_CONNECTION.md)):

```cpp
bool shouldKeepAlive(const Request& req) {
    std::string conn = req.header("Connection"); // already lowercased

    if (req.version() == HTTP_VERSION_1_1)
        return conn != "close";          // 1.1: persistent unless told to close
    else
        return conn == "keep-alive";     // 1.0: close unless explicitly opted in
}
```

**Host requirement — 1.1 only:**

```cpp
if (req.version() == HTTP_VERSION_1_1 && req.header("Host").empty())
    return makeError(400);   // 1.1 mandates Host (it's what enables virtual hosts)
```

**Chunked bodies — 1.1 only:** a 1.0 request can't legitimately send `Transfer-Encoding: chunked`; a 1.1 one can, and you must un-chunk it before handing the body on (especially to CGI). Framing keys off the header — [`06_FRAMING.md`](06_FRAMING.md).

### 4.4 The response version string

Two defensible policies for the status line (`HTTP/1.1 200 OK`):

1. **Always respond `HTTP/1.1`** — simplest, what most implementations do, what modern clients expect.
2. **Echo the request's version** — more semantically correct, slightly more code.

Because browsers send `Host:`, expect persistence, and may send chunked bodies, **you must implement the 1.1 behaviours regardless of what you echo.** Pragmatic choice: policy 1.

### 4.5 The mental model

```
request line ──► version enum ──► stored on Request
                                       │
                 ┌─────────────────────┼─────────────────────┐
                 ▼                     ▼                      ▼
          keep-alive?            Host required?          chunked allowed?
        (Connection hdr)         (1.1 only)              (1.1 only)
```

Parse the token once, then consult it at those three forks. No second parser, no second code path.

---

## 🔗 Related

- [`01_FUNDAMENTALS.md`](01_FUNDAMENTALS.md) — version history and the "1.0 suggested" browser-compatibility trap.
- [`07_CONNECTION.md`](07_CONNECTION.md) — keep-alive / `Connection: close` lifecycle in full; the single-`poll()` rule.
- [`06_FRAMING.md`](06_FRAMING.md) — `Content-Length` vs `Transfer-Encoding: chunked`; how a body ends.
- [`12_COOKIES_SESSIONS.md`](12_COOKIES_SESSIONS.md) — how a stateless protocol acquires state.
- [`OVERVIEW.md`](OVERVIEW.md) — the class layering the three stages map onto.
- [`19_CONFIGURATION.md`](19_CONFIGURATION.md) — what the dispatch stage matches against.
