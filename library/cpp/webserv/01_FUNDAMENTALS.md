# 01 — Fundamentals

> *"HTTP is just plain text over a TCP socket. Everything else is convention."*

Before headers and status codes, you need a clear picture of **what HTTP actually is**, **where it sits**, and **what it isn't**. Get this right and the rest of the library reads like commentary on a model you already hold.

---

## What HTTP is

**HTTP (HyperText Transfer Protocol)** is a *text-based*, *stateless*, *request-response* application protocol that runs **on top of TCP**.

Three words to dissect:

- **Text-based** — the protocol bytes you read off the wire are (mostly) printable ASCII. Method, path, headers, status code — all readable. The body can be binary, but the framing is text. This is why you can `telnet www.example.com 80` and type a request by hand.
- **Stateless** — the protocol mandates no memory of prior requests. The server does not "remember" you between requests. Sessions, cookies, tokens are state layered **on top** of HTTP, not by it.
- **Request-response** — every interaction is one request from a client followed by one response from a server. The server never initiates. (WebSockets are not HTTP — they're a different protocol that uses HTTP only for the initial handshake.)

---

## Where it sits in the stack

```
┌─────────────────────────────────────────┐
│  Application:  HTTP, FTP, SMTP, DNS…    │   ← you are here
├─────────────────────────────────────────┤
│  Transport:    TCP, UDP                 │   ← HTTP rides TCP
├─────────────────────────────────────────┤
│  Network:      IP (v4, v6)              │   ← see netpractice library
├─────────────────────────────────────────┤
│  Link:         Ethernet, Wi-Fi…         │
└─────────────────────────────────────────┘
```

HTTP doesn't care about IP addresses or packets — it hands a byte stream down to TCP and TCP makes it work. Conversely, TCP doesn't care that the bytes happen to be HTTP — it'd just as happily ferry a screenshot or a chess move.

**For webserv this means:** you'll spend ~30% of your code on socket plumbing (`socket`, `bind`, `listen`, `accept`, `poll`) and ~70% on parsing the bytes that show up on those sockets. The first is mechanical; the second is the interesting part.

Cross-reference: [`netpractice/INDEX.md`](../../netpractice/INDEX.md) for what's happening *below* HTTP.

---

## The request-response loop in detail

```
   CLIENT                                   SERVER
   ──────                                   ──────
   socket()                                  socket()
                                             bind()
                                             listen()
   connect() ────────── TCP handshake ────── accept()
       │                                         │
       │  ┌──────────────────────────────┐       │
       └──┤ HTTP request as text bytes   ├──────►│
          │ GET /index.html HTTP/1.1\r\n │       │ ← parse request
          │ Host: example.com\r\n        │       │ ← decide what to do
          │ \r\n                         │       │ ← build response
          └──────────────────────────────┘       │
          ┌──────────────────────────────┐       │
       ◄──┤ HTTP response as text bytes  ├───────┘
          │ HTTP/1.1 200 OK\r\n          │
          │ Content-Length: 137\r\n      │
          │ Content-Type: text/html\r\n  │
          │ \r\n                         │
          │ <html>…</html>               │
          └──────────────────────────────┘
   close() ──── or keep-alive, see file 07
```

**Five things to notice:**

1. The client always initiates. The server is a passive parser-and-responder.
2. The bytes on the wire are text up to (and including) the blank line. After that may come body bytes (which can be anything — JSON, a JPEG, raw bytes).
3. `\r\n` (CRLF, two bytes — `0x0D 0x0A`) terminates every line of the headers. **Not `\n` alone**. Get this wrong and clients will ignore you or hang.
4. The blank line between headers and body is *also* a `\r\n`. So the boundary between headers and body is `\r\n\r\n` — four bytes.
5. The server doesn't know how long the body is from the start line — it must read `Content-Length` or `Transfer-Encoding: chunked` to know when to stop. See [file 06](06_FRAMING.md).

---

## HTTP versions — what each one gave us

| Version | Year | What it added | Relevance to you |
|---|---|---|---|
| **HTTP/0.9** | 1991 | One-line protocol: `GET /path` + response body. No headers, no status. | Historical. Never seen in the wild. |
| **HTTP/1.0** | 1996 | Status codes, headers, methods. Connection closes after each request. | The 42 subject's **suggested reference**. Many features missing — no `Host` header, no chunked, no keep-alive by default. |
| **HTTP/1.1** | 1997 | `Host` header (enables virtual hosts), persistent connections by default, chunked transfer, pipelining, caching directives. | **What browsers actually speak.** Even though webserv "suggests" 1.0, your responses will need to satisfy 1.1 clients. The version string in your responses should be `HTTP/1.1`. |
| HTTP/2 | 2015 | Binary framing, multiplexing, header compression, server push. | **Out of scope.** |
| HTTP/3 | 2022 | Runs over QUIC (UDP) instead of TCP. | **Out of scope.** |

> ⚠️ **The webserv subject's "1.0 suggested" trap.** If you literally only support 1.0 semantics, modern browsers will misbehave: they'll send `Host:` headers you ignore, they'll send `Transfer-Encoding: chunked` request bodies you can't parse, they'll expect persistent connections. **Implement enough of 1.1 to be browser-compatible.** Specifically: support `Host`, support `Connection: close`, support `Content-Length` and chunked request bodies.

---

## What HTTP is **not**

| It isn't… | Because… |
|---|---|
| Encrypted | That's TLS. HTTP-over-TLS is HTTPS. See [file 13](13_HTTPS_TLS.md). |
| Stateful | Cookies/sessions are layered on. The protocol itself remembers nothing. See [file 12](12_COOKIES_SESSIONS.md). |
| Authenticated by default | Auth is its own can of worms (Basic, Bearer, OAuth…). Out of scope for webserv. |
| Bidirectional | Server can't push to client unsolicited in HTTP/1.x. (Server-Sent Events and WebSockets are workarounds.) |
| Streamed in real time | Each response is one "thing". You can chunk it, but the request/response cadence is strictly turn-taking. |

---

## A first request in raw bytes

The smallest valid HTTP/1.1 request:

```
GET / HTTP/1.1\r\n
Host: example.com\r\n
\r\n
```

Three lines: a request line, a single mandatory header (`Host`, since HTTP/1.1), a blank line. That's it. Try it yourself:

```sh
printf 'GET / HTTP/1.1\r\nHost: example.com\r\n\r\n' | nc example.com 80
```

You'll see a real HTTP response come back. **That's the protocol.** Everything else in this library is "what fields, in what order, with what rules."

---

## TL;DR mental model

> An HTTP server is a function: `bytes_in → bytes_out`. The input is a structured chunk of text; the output is also a structured chunk of text. The hard parts are: (a) knowing when the input ends, (b) deciding what to do, (c) framing the output so the client knows when *it* ends.

**Continue to [`02_MESSAGE_ANATOMY.md`](02_MESSAGE_ANATOMY.md)** to dissect the structure of those text chunks.
