# 16 — Tiny Server Lab

> *"Build the smallest possible HTTP server. Once it works, you understand HTTP."*

A guided walkthrough. By the end of this file, you'll have written **a ~70-line C++98 HTTP server** that:
- Listens on a port
- Accepts one connection at a time (blocking)
- Reads one request
- Sends back a fixed `200 OK` HTML response
- Repeats

It is **deliberately wrong** in webserv terms — blocking, single-threaded, no parser, no config, no CGI. That's the point: you isolate the **socket dance** so you can practice it without the architecture overhead.

Once this works, scaling up to the full webserv is "fix one limitation at a time."

---

## What you'll learn

| Concept | Where it appears in this lab |
|---|---|
| Socket creation | `socket()` |
| Address binding | `bind()` |
| Server queue | `listen()` |
| Accepting connections | `accept()` |
| Reading from a socket | `recv()` (or `read()`) |
| Writing to a socket | `send()` (or `write()`) |
| The HTTP wire format | the literal string you send |

What you **won't** learn here (deliberately):
- Non-blocking I/O / poll() — that comes in the real webserv
- Multi-client handling — single accept loop only
- Request parsing — we ignore the request bytes entirely
- Anything dynamic — fixed response

---

## The full source

Save as `tiny.cpp`:

```cpp
#include <iostream>
#include <string>
#include <cstring>          // memset
#include <cstdlib>          // exit
#include <unistd.h>         // close, read, write
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int  PORT     = 8080;
const int  BACKLOG  = 16;
const char RESPONSE_BODY[] =
    "<!DOCTYPE html><html><body><h1>Hello from tiny server</h1></body></html>";

void die(const char* what) {
    std::perror(what);
    std::exit(1);
}

int main() {
    // 1. Create a TCP socket.
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) die("socket");

    // 2. Allow rebinding the port after restart (avoid "Address already in use").
    int yes = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    // 3. Build the address we want to bind to: 0.0.0.0:PORT
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   // bind to all interfaces
    addr.sin_port        = htons(PORT);

    // 4. Bind the socket to that address.
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) die("bind");

    // 5. Mark the socket as listening (accept-able).
    if (listen(listen_fd, BACKLOG) < 0) die("listen");

    std::cout << "Listening on http://localhost:" << PORT << std::endl;

    // 6. Accept loop.
    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            std::perror("accept");
            continue;                   // skip this client, keep server alive
        }

        // 7. Read the request bytes. We don't parse — just consume.
        char buf[8192];
        ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (n > 0) {
            buf[n] = '\0';
            std::cout << "--- request (" << n << " bytes) ---\n" << buf;
            std::cout << "--- end request ---\n" << std::endl;
        }

        // 8. Build the response.
        size_t body_len = std::strlen(RESPONSE_BODY);

        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n"
            << "Server: tiny/0.1\r\n"
            << "Content-Type: text/html; charset=utf-8\r\n"
            << "Content-Length: " << body_len << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
            << RESPONSE_BODY;

        std::string response = oss.str();

        // 9. Write it out.
        send(client_fd, response.c_str(), response.size(), 0);

        // 10. Close — we promised "Connection: close" in the headers.
        close(client_fd);
    }

    close(listen_fd);
    return 0;
}
```

Add to the top: `#include <sstream>` for `std::ostringstream`.

Compile and run:

```sh
c++ -Wall -Wextra -Werror -std=c++98 tiny.cpp -o tiny
./tiny
```

Test:

```sh
curl -v http://localhost:8080/
# In a browser: http://localhost:8080/
```

---

## What each step does

### `socket(AF_INET, SOCK_STREAM, 0)`
Creates a file descriptor representing a TCP socket. `AF_INET` is IPv4; `SOCK_STREAM` is TCP. Returns `-1` on error.

### `setsockopt(... SO_REUSEADDR ...)`
Without this, if your server crashes and restarts within 60s, `bind()` will fail with `EADDRINUSE` because the kernel keeps the address in `TIME_WAIT`. This option says "let me bind anyway." **Always set this on your listening socket.**

### `struct sockaddr_in addr`
The IPv4 address structure. `AF_INET`, IPv4 address (`htonl(INADDR_ANY)` = "all interfaces, 0.0.0.0"), and port (`htons(PORT)` — `htons` flips byte order for network).

> **Network byte order** is big-endian. Most CPUs are little-endian. `htons`, `htonl` (host-to-network), `ntohs`, `ntohl` (network-to-host) handle the conversion. **Always wrap port numbers in `htons`** when filling sockaddr_in.

### `bind(listen_fd, ...)`
Attaches the socket to a specific IP+port. Fails if the port is in use (and you didn't set `SO_REUSEADDR`).

### `listen(listen_fd, BACKLOG)`
Marks the socket as one that will accept connections. `BACKLOG` is the queue size for connections that arrive faster than you `accept()` them.

### `accept()`
**Blocks** until a client connects. Returns a new socket fd for that specific client (the listening socket stays open for further connections).

In webserv, `accept()` will be non-blocking and you'll only call it when `poll()` says the listening socket is readable. Here, blocking is fine.

### `recv()`
Read bytes from the socket. We ignore what we read (real webserv parses it).

### Building the response
Pure string assembly. The HTTP "wire format" is just bytes you concatenate. Note the `\r\n` line endings — not `\n`.

### `send()`
Write bytes to the socket. May return fewer bytes than requested (a "short write"); a real server would loop until all bytes are sent. For this lab, the response is small enough that one `send()` will do it.

### `close()`
Closes the client socket. Since we sent `Connection: close`, this matches our promise.

---

## Things you can test

Once `tiny` is running, exercise it:

```sh
# Basic curl
curl -v http://localhost:8080/

# Hit it from a browser
# (open http://localhost:8080/ in your browser, see the H1)

# Try a HEAD request — server will still send a body (we're not implementing HEAD)
curl -I http://localhost:8080/

# Try a POST — server still returns the same response
curl -X POST -d "name=alice" http://localhost:8080/

# Send a malformed request (no CRLF)
printf 'BLEHHHH' | nc localhost 8080
# → server still responds with the canned response. Real webserv would 400.

# Hit it concurrently — second client waits because we're single-threaded
curl http://localhost:8080/ &
curl http://localhost:8080/ &
wait
```

The last test shows the **first limitation we'll fix when scaling up**: handling more than one client at a time requires `poll()` and a per-connection state machine ([file 07](07_CONNECTION.md)).

---

## From tiny to webserv — the gap

The full webserv differs from `tiny.cpp` in these axes:

| Tiny | Webserv |
|---|---|
| 1 client at a time, blocking | N clients, all handled via single `poll()` |
| No parser — buf is ignored | Full HTTP/1.1 parser with chunked body decoder |
| Fixed response | Routing → static file / directory listing / CGI / redirect / error |
| No config | Nginx-style config file with virtual routes, error pages, body size limits, CGI mappings |
| One response, one shape | Status code + `Content-Type` derived per request |
| `Connection: close` always | Keep-alive support, timeout sweep |
| No CGI | Fork + execve + pipes + env vars + un-chunk + framing |
| `recv` reads everything | Partial reads, resumable parser state |

Each row is a chunk of webserv. Don't try to add them all at once — add one row at a time, run `curl` and a browser against each iteration.

---

## Suggested roadmap from here to webserv

1. **Tiny works.** You have this file's code running. Run it against a browser.
2. **Make it non-blocking with `poll()`.** Replace the `accept` loop with a `poll`-driven loop. One client at a time still, but now via `poll`. (Hardest single step in the project — take time.)
3. **Multiple clients.** Add accepted clients to the poll set. Per-client buffer. Read incrementally.
4. **Real parser.** Implement the request parser. Handle `Content-Length` request bodies.
5. **Routing + static files.** Match request path against config; serve files from `root`. Set `Content-Type` from extension.
6. **Error pages.** 404, 403, 405, 500 with default and configurable bodies.
7. **POST + uploads.** Handle `multipart/form-data`.
8. **DELETE.** `unlink(2)` files under permitted routes.
9. **CGI.** The big one. Fork, exec, pipes, env, un-chunk, buffer output.
10. **Chunked request bodies.** Decode `Transfer-Encoding: chunked` into your buffer.
11. **Keep-alive.** Don't close after the first response; loop the connection state machine.
12. **Idle timeouts.** Sweep stale connections.
13. **Stress test.** `wrk -t4 -c100 -d30s`. Fix everything it breaks.
14. **Bonus** (if time): cookies/sessions, multiple CGI types.

Each step is one ~commit-sized chunk of work. **Linear, not parallel.** Resist the temptation to write the parser and the CGI dispatcher and the config loader simultaneously — you'll debug a four-dimensional bug.

---

## TL;DR mental model

> The HTTP server pattern is: **socket → bind → listen → accept → recv → process → send → close**. The "tiny" version of that is ~70 lines of C++98. Webserv is the same pattern with each step replaced by "do that, but non-blocking, for N concurrent clients, with config-driven routing and CGI." Build the tiny version first to internalise the bones.

**Continue to [`17_WEBSERV_SUBJECT.md`](17_WEBSERV_SUBJECT.md)** — the actual 42 subject, distilled, mapped to this library.
