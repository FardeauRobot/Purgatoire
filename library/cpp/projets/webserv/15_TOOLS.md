# 15 — The Toolkit

> *"You can't debug what you can't see."*

The right tools turn webserv from "mysterious black box that sometimes returns nothing" into "concrete bytes I can inspect." Internalise this kit before you write a line of server code; it will save you hours per day during the project.

---

## `curl` — the swiss army knife

The de-facto HTTP client. Sends real requests, shows real bytes.

### The essentials

```sh
curl http://localhost:8080/                       # plain GET
curl -v http://localhost:8080/                    # show request and response headers
curl -i http://localhost:8080/                    # include response headers in output
curl -I http://localhost:8080/                    # HEAD request only
curl -X DELETE http://localhost:8080/file.txt     # custom method
curl -d "user=alice&pass=hunter2" http://localhost:8080/login
                                                  # POST form data (sets Content-Type)
curl -d '{"key":"value"}' \
     -H "Content-Type: application/json" \
     http://localhost:8080/api                    # POST JSON
curl -F "file=@photo.jpg" http://localhost:8080/upload
                                                  # multipart/form-data file upload
curl -H "X-Custom: foo" http://localhost:8080/    # add a header
curl --resolve example.com:8080:127.0.0.1 http://example.com:8080/
                                                  # spoof a Host header for virtual host testing
curl -L http://localhost:8080/old                 # follow redirects
curl --max-time 5 http://localhost:8080/          # timeout
curl -o output.bin http://localhost:8080/big.zip  # save body to file
```

### Reading `-v` output

```
$ curl -v http://localhost:8080/index.html
*   Trying 127.0.0.1:8080...
* Connected to localhost (127.0.0.1) port 8080
> GET /index.html HTTP/1.1                     ← request lines (you sent these)
> Host: localhost:8080
> User-Agent: curl/8.4.0
> Accept: */*
>                                              ← blank line ending request
< HTTP/1.1 200 OK                              ← response lines (server sent these)
< Server: webserv/1.0
< Content-Type: text/html; charset=utf-8
< Content-Length: 137
< Connection: close
<                                              ← blank line ending response headers
<!DOCTYPE html>...                             ← body
* Closing connection
```

The `> ` prefix means **you sent it**. The `< ` prefix means **the server sent it**. The `* ` prefix is curl's own commentary.

### Useful flags for debugging

| Flag | What |
|---|---|
| `-v` | Show request and response headers |
| `--trace-ascii -` | Show ALL bytes including body (printed to stdout) |
| `--trace -` | Show ALL bytes as hex + ASCII |
| `--http1.0` | Force HTTP/1.0 |
| `--http1.1` | Force HTTP/1.1 (default) |
| `-k` | Don't verify TLS certs (useful for self-signed) |
| `-s` | Silent (for scripting) |
| `-w "%{http_code}\n" -o /dev/null` | Print only the status code |

---

## `nc` (netcat) — raw socket

When `curl` is too high-level. Talk HTTP byte-for-byte:

```sh
$ nc localhost 8080
GET / HTTP/1.1
Host: localhost:8080
                              ← press enter on the blank line; nc sends what you typed

HTTP/1.1 200 OK
Server: webserv/1.0
...
```

The reason this is invaluable: **you control every byte**. Send malformed requests to verify your error handling. Send a request that lies about its Content-Length. Send a request without the blank line and watch your server hang (then fix it).

### Useful nc one-liners

```sh
# Send a precise byte sequence (no terminal-induced \r\n weirdness)
printf 'GET / HTTP/1.1\r\nHost: x\r\n\r\n' | nc localhost 8080

# Send a chunked request
printf 'POST /upload HTTP/1.1\r\nHost: x\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n6\r\n world\r\n0\r\n\r\n' | nc localhost 8080

# Listen on a port like a fake server (for studying real clients)
nc -l 9000                    # then point a browser at http://localhost:9000
```

### macOS `nc` differences

macOS's `nc` (BSD-derived) is slightly different from Linux's (GNU). For listening with `-l`, the BSD form is `nc -l 9000` (no `-p`). For sending a chunk and closing, use `-q 1` (BSD) to wait 1 second after EOF before closing.

If macOS `nc` is too quirky, install `ncat` (`brew install nmap`) — it's the same project as nmap, more consistent across platforms.

---

## `telnet` — the original

```sh
$ telnet localhost 8080
Trying ::1...
Connected to localhost.
Escape character is '^]'.
GET / HTTP/1.1
Host: localhost:8080

HTTP/1.1 200 OK
...
```

Same idea as `nc` but with a different UX. Less useful these days since `nc` exists, but the 42 subject mentions telnet explicitly:

> *"Please read the RFCs defining the HTTP protocol, and perform tests with telnet and NGINX before starting this project."*

If you want to follow the subject's wording literally, use `telnet`. Otherwise `nc` is more flexible.

---

## Browser dev tools

Press `F12` (or `Cmd+Opt+I` on macOS) in Chrome / Firefox / Safari. The **Network tab** shows every HTTP request the page makes:

- Request URL, method, status code
- Request headers (yours) and response headers (server's)
- Request body and response body (rendered or raw)
- Timing breakdown (DNS, connect, TLS, send, wait, receive)
- Cookies attached and `Set-Cookie` headers received

For webserv, hit `http://localhost:8080/` in a real browser, open dev tools, and watch your server's behaviour from the client's perspective. This is the closest thing to "what an evaluator will see."

**The "Preserve log" checkbox** keeps history across navigations — turn it on when debugging redirect chains or session flows.

---

## `wireshark` / `tcpdump` — packet-level

When you suspect the bytes on the wire aren't what you think they are. `tcpdump` is CLI; wireshark is the GUI for the same packet-capture engine.

```sh
sudo tcpdump -i lo -A 'port 8080'      # capture loopback traffic on port 8080,
                                        # print ASCII bodies
```

Wireshark's "Follow TCP Stream" view stitches a TCP connection back into a continuous stream — invaluable when debugging chunked or pipelined requests.

For webserv on localhost: `tcpdump -i lo0` (macOS) or `tcpdump -i lo` (Linux).

**Heavy weapon. Use only when curl/nc don't tell you enough.**

---

## NGINX — the reference

The subject:

> *"NGINX may be used to compare headers and answer behaviours (pay attention to differences between HTTP versions)."*

`nginx` is the industrial-grade HTTP server. When your webserv behaves oddly, compare with nginx serving the same file:

```sh
# Minimal nginx setup
brew install nginx                  # macOS
# write a small config in /usr/local/etc/nginx/nginx.conf with:
#   server {
#       listen 8081;
#       root /tmp/www;
#       location / { try_files $uri $uri/ =404; }
#   }
nginx                               # start
nginx -s reload                     # reload after config change
nginx -s stop                       # stop
```

Then `curl -v http://localhost:8081/` vs `curl -v http://localhost:8080/` (your webserv). The headers should be substantially the same. If nginx sends a `Server: nginx/...` and you send `Server: webserv/1.0`, that's expected. If nginx sends 3 headers and you send 1, that's worth investigating.

---

## `httpbin.org` — testing client behaviour

`https://httpbin.org` is a public service that echoes back what you send:

```sh
$ curl https://httpbin.org/get
{
  "args": {},
  "headers": {
    "Accept": "*/*",
    "Host": "httpbin.org",
    "User-Agent": "curl/8.4.0"
  },
  "origin": "1.2.3.4",
  "url": "https://httpbin.org/get"
}
```

Other endpoints:
- `/post` — accepts POST, echoes
- `/status/418` — returns any status code you want
- `/delay/5` — waits 5s before responding (test timeouts)
- `/redirect/3` — chained redirects (test client following)
- `/cookies/set/name/value` — sets cookies

Useful when you want to test **a client** (yours, curl, browser) without setting up a server.

---

## ApacheBench (`ab`) and `wrk` — load testers

The subject:

> *"Stress test your server to ensure it remains available at all times."*

Two common stress testers:

```sh
ab -n 1000 -c 50 http://localhost:8080/        # 1000 requests, 50 concurrent
wrk -t4 -c100 -d10s http://localhost:8080/     # 4 threads, 100 conns, 10s
```

These hammer your server. Things you want to see:
- No crashes.
- No leaked file descriptors (`lsof -p $(pgrep webserv)` before/after).
- No memory growth (`top -pid $(pgrep webserv)`).
- Reasonable throughput (the subject doesn't define a number).

Install: `brew install httpd` for `ab` (it comes with Apache); `brew install wrk` for wrk.

→ **Full reference:** [`TESTS.md`](TESTS.md) — `ab` and `siege` in depth: every flag, how to read the output, the availability/leak/fd checks the eval actually runs.

---

## `valgrind` and AddressSanitizer

For memory bugs in webserv:

```sh
make asan                                     # build with -fsanitize=address
./webserv config.conf                          # ASan reports leaks/UB on exit

valgrind --leak-check=full --show-leak-kinds=all ./webserv config.conf
                                              # Linux; on macOS use `leaks`:
make leaks                                    # from the repo's templates
```

Run your stress test under ASan or valgrind — slow but catches the leaks that only manifest under load.

---

## `strace` / `dtruss` — syscall tracing

When you don't understand what your server is actually doing at the syscall level:

```sh
# Linux
strace -f -e trace=network ./webserv config.conf
                              # show every network-related syscall, follow forks

# macOS (similar idea, different tool)
sudo dtruss -f ./webserv config.conf
```

Heavy and noisy, but useful to confirm "yes I really am calling `accept()` and `read()` in the order I think."

---

## Browsers as tests

After all the synthetic tests, hit your server with a real browser. The subject mandates:

> *"Your server must be compatible with standard web browsers of your choice."*

Open `http://localhost:8080/` in Chrome. Look at:
- Does the page render? (Check `Content-Type` is right.)
- Does CSS load? (`Content-Type: text/css` is critical here.)
- Do images load?
- Does navigation work? Reload work?
- Does file upload work via an `<input type="file">` form?
- Does a JS-driven `fetch()` against your server work?

Each of these exercises a different code path. The browser is your final integration test.

---

## TL;DR mental model

> **`curl -v`** for everyday testing. **`nc`** when you need byte-level control. **Browser dev tools** for the client perspective. **NGINX** as a reference implementation to compare against. **ApacheBench / wrk** for resilience under load. **Valgrind / ASan** for memory correctness. **Wireshark** only when bytes seem to be lying.

**Continue to [`16_TINY_SERVER_LAB.md`](16_TINY_SERVER_LAB.md)** — actually build something tiny before tackling webserv.
