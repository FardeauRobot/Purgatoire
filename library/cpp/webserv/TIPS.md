# Webserv — Tips & Tricks

> *Tools, habits, and mental shortcuts that make the project less painful — without stealing the problems you're supposed to solve.*

This file does not give you solutions. It gives you better instruments so that when you hit a wall, you hit it faster and understand it more clearly.

---

## Debug tools

### `curl -v` — see the raw conversation

```bash
curl -v http://localhost:8080/index.html
```

`-v` prints every header sent and received, prefixed with `>` (your request) and `<` (server response). This is your primary verification tool. If curl gets a response and the browser doesn't, the bug is almost certainly in keep-alive or `Content-Length`.

Useful flags:

```bash
curl -v -X POST -d "name=foo" http://localhost:8080/form   # POST with body
curl -v -X DELETE http://localhost:8080/file.txt            # DELETE
curl -v --max-time 5 http://localhost:8080/slow             # timeout after 5s
curl -v -H "Connection: close" http://localhost:8080/       # force close after response
```

---

### `nc` (netcat) — send raw bytes

```bash
printf "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080
```

`nc` gives you a raw TCP pipe. No headers added, no interpretation — whatever you type goes on the wire, whatever comes back is printed. This is the fastest way to test whether your server parses a malformed request correctly or whether it hangs waiting for more data.

```bash
nc localhost 8080
# now type your request manually, one line at a time
GET / HTTP/1.1
Host: localhost

# press enter twice (the blank line = end of headers)
```

If your server hangs here, it's waiting for bytes that never come — look at your `\r\n\r\n` detection.

---

### `lsof -i :8080` — who owns the port

```bash
lsof -i :8080
```

When `bind()` fails with `EADDRINUSE`, this tells you exactly which process holds the port. The `PID` column lets you `kill` it.

---

### `strace` / `dtruss` — watch syscalls in real time

On Linux:
```bash
strace -e trace=network,read,write ./webserv
```

On macOS:
```bash
sudo dtruss ./webserv 2>&1 | grep -E "read|write|accept|poll"
```

This shows you every `read()`, `write()`, `accept()`, `poll()` call your server makes. If your server is supposed to be reading but isn't, syscall tracing reveals it immediately. It's noisy — filter aggressively.

---

### `ab` — Apache Bench for stress testing

```bash
ab -n 1000 -c 10 http://localhost:8080/index.html
```

`-n 1000` = 1000 total requests, `-c 10` = 10 concurrent. The evaluator will run something like this. If your server crashes or hangs under concurrency, find out now. Common failure modes: fd leaks (you run out of fds), zombie processes (CGI), double-close.

---

### Python's built-in server — reference behavior

```bash
python3 -m http.server 9090
```

When you're unsure how a correct server should behave (directory listing, 404 format, redirect), run Python's server on a different port and compare with `curl -v`. It's your reference implementation.

---

### Build with sanitizers — catch bugs before valgrind

```make
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -O0 -fsanitize=address,undefined
```

AddressSanitizer catches buffer overflows, use-after-free, double-free at runtime — immediately, with a stack trace. Run your webserv under ASan during development. When it's clean, then run valgrind for the final leak check.

---

## Logging — the single best investment

Build a one-liner logger early and use it everywhere:

```cpp
// Not the implementation — the habit
[POLL]   fd=5 POLLIN  — client A has data
[READ]   fd=5 got 47 bytes — "GET /index.html HTTP/1.1\r\n..."
[PARSE]  fd=5 method=GET path=/index.html
[SEND]   fd=5 sending 512 bytes
[CLOSE]  fd=5 connection closed
```

Print the fd number with every log line. When two clients talk simultaneously, you need to untangle which event belongs to whom. Without fd tags your logs are useless under concurrency.

A flag like `DEBUG=1` lets you silence it before evaluation:
```bash
make DEBUG=1   # verbose
make           # silent
```

---

## Development order — don't skip steps

The urge to build everything at once kills this project. This order works:

```
1. Single connection, blocking — serve one GET, exit
   → proves your socket setup and HTTP parsing are correct

2. Single connection, keep-alive — serve multiple GETs on one connection
   → proves your request boundary detection (\r\n\r\n) works

3. Multiple connections with poll() — serve N clients simultaneously
   → proves your event loop doesn't block

4. Static files — GET a real file from disk
   → proves Content-Type, Content-Length, 404/403

5. Directory listing (autoindex)
   → proves your opendir/readdir loop

6. POST + file upload
   → proves body reading (Content-Length accumulation)

7. DELETE
   → proves your method dispatch

8. CGI — one script, hardcoded
   → proves fork/execve/pipe/dup2 are wired correctly

9. CGI — driven by config
   → proves your config parser maps extensions to interpreters

10. Multiple ports from config
    → proves your server block parsing and multiple listen sockets
```

Test each step with `curl -v` before moving to the next. **Do not proceed until the current step is solid.**

---

## Smart concepts

### State machine per client

Each client has a lifecycle. Name the states explicitly:

```
READING_HEADERS → READING_BODY → BUILDING_RESPONSE → SENDING_RESPONSE → DONE
```

Store the state in your client struct. When `poll()` wakes you up for a client, the state tells you what to do next. Without explicit states, you get a tangle of flags and `if`s that break under edge cases.

---

### Never guess — always print

When something doesn't work, don't stare at the code. Print:
- The raw bytes you received (`\r` → `<CR>`, `\n` → `<LF>`)
- The result of every parse step
- Which fd triggered `poll()`

You will be surprised how often "the request looks correct" is wrong — a missing `\r`, an extra byte, a `Content-Length` off by one.

---

### Test the error paths, not just the happy path

Evaluators will:
- Send a request with no `Host:` header
- Send a POST with no `Content-Length`
- Send a request to a route that doesn't exist
- Upload a file larger than your configured limit
- Connect and then immediately disconnect without sending anything

Handle these early. A server that crashes on bad input is worse than one that returns the wrong status code.

---

### `SIGPIPE` — kill it on day one

The moment a client disconnects mid-response, the next `write()` to their fd sends `SIGPIPE` to your process. Default behavior: your server dies instantly, silently.

```cpp
signal(SIGPIPE, SIG_IGN);  // call this once at startup, before anything else
```

After this, `write()`/`send()` returns `-1` with `errno = EPIPE` instead of killing you. Close the client, continue. This is not optional.

---

### The `TIME_WAIT` trap

Kill your server. Restart it. `bind()` fails. You didn't change anything.

TCP keeps a port reserved for ~60 seconds after a connection closes — `TIME_WAIT` state. `SO_REUSEADDR` on the listen socket bypasses this check. Set it unconditionally, right after `socket()`, every time.

---

### Close unused pipe ends — always

In CGI, after `fork()`:
- The parent must close the pipe ends it handed to the child
- The child must close the pipe ends it handed to the parent

If you leave a write end open in the parent, `read()` on the read end will never return EOF — the pipe looks "still writable." Your server hangs waiting for a CGI script that has already exited. This is the most common CGI bug.

Draw the pipe diagram on paper before writing the code.

---

### The poll set is a data structure — manage it explicitly

`poll()` takes an array. You add fds, remove fds, change events. Keep this array and your client map in sync at all times. A dead fd left in the poll set causes `POLLNVAL` on the next iteration. An fd removed from the map but kept in the poll set causes a segfault when you look up the client.

Pick one data structure decision early and stick to it. A `std::map<int, Client>` keyed by fd is simple and correct.

---

### POLLOUT is not free

`POLLOUT` fires as long as the socket's send buffer has space — which is almost always. If you set `POLLOUT` on every fd all the time, `poll()` returns immediately on every iteration and your server burns 100% CPU doing nothing. Only set `POLLOUT` when you have bytes queued to send. Clear it the moment the queue is empty.

---

## Config file — read the subject carefully

The subject specifies an nginx-like config format. Key things to get right before writing the parser:

- A `server` block can have multiple `listen` directives
- A `location` block overrides the server-level directive for that path prefix
- `root` and `index` can appear at both server and location level
- `limit_except` restricts which HTTP methods are allowed on a route
- `client_max_body_size` limits POST body size — you must enforce this

Write a config validator that prints what it parsed before you connect it to the server. If the parser is wrong, everything built on top of it is wrong.

---

## Related

- [`15_TOOLS.md`](15_TOOLS.md) — full tool reference (curl, nc, wireshark, httpbin)
- [`17_WEBSERV_SUBJECT.md`](17_WEBSERV_SUBJECT.md) — what the subject actually requires
- [`18_SOCKETS_AND_FDS.md`](18_SOCKETS_AND_FDS.md) — the OS layer your event loop runs on
- [`functions/02_IO_MULTIPLEXING.md`](functions/02_IO_MULTIPLEXING.md) — poll() internals
- [`functions/05_PROCESS_AND_CGI.md`](functions/05_PROCESS_AND_CGI.md) — fork/pipe/execve for CGI
