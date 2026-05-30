# 14 — CGI (Common Gateway Interface)

> *"CGI is `fork+execve` made polite. The server hands the request to a program; the program writes the response."*

The webserv subject **mandates** CGI support: *"Your server should support at least one CGI (php-CGI, Python, and so forth)."* This is the single most complex feature in the project — it touches process management, I/O, environment, and protocol all at once.

This file walks you through the entire dance: what CGI is, what your server must do, what the CGI script expects, and the traps.

---

## What CGI is, in one sentence

> Web server gets a request; for certain URLs (matching a configured extension), it **spawns a program**, **gives that program a structured copy of the request**, and **uses the program's stdout as the HTTP response body** (with a small header section).

The CGI spec (RFC 3875) defines the boundary: what env vars to set, where the body goes (stdin), what to make of the output (parsed for headers, then body).

---

## When CGI kicks in

In your config:

```nginx-ish
location / {
    root /var/www;
    cgi .php /usr/bin/php-cgi;
    cgi .py  /usr/bin/python3;
}
```

The match rule: when the request's path ends in `.php` (or matches the configured CGI extension), don't serve the file as static — **execute** the configured interpreter with the file as argument.

A request like `GET /scripts/hello.php?name=alice` should:
1. Locate the file `/var/www/scripts/hello.php` (root + path).
2. Recognise `.php` → CGI rule.
3. Spawn `/usr/bin/php-cgi` with the request data piped in.
4. Capture php-cgi's stdout.
5. Parse the output (it'll start with headers, then a body).
6. Send the result back to the client.

---

## The CGI environment

When you `execve` the CGI program, you set environment variables. The CGI spec defines a standard list. Here are the ones webserv must set:

### Request meta

| Variable | Value |
|---|---|
| `REQUEST_METHOD` | `GET`, `POST`, `DELETE`, etc. |
| `SERVER_PROTOCOL` | `HTTP/1.1` |
| `SERVER_NAME` | The hostname (from `Host:` header or config) |
| `SERVER_PORT` | The port the request came in on |
| `GATEWAY_INTERFACE` | `CGI/1.1` |
| `SERVER_SOFTWARE` | Your server's identifier (`webserv/1.0`) |
| `REMOTE_ADDR` | Client IP (from `accept()`'s `sockaddr`) |
| `REMOTE_PORT` | Client port (same) |

### URL parsing

| Variable | Value |
|---|---|
| `SCRIPT_NAME` | The portion of the URL path that maps to the CGI script (e.g. `/scripts/hello.php`) |
| `SCRIPT_FILENAME` | The full filesystem path to the script (e.g. `/var/www/scripts/hello.php`) — used by php-cgi |
| `PATH_INFO` | Extra path after the script name (e.g. for `/hello.php/foo/bar`, `PATH_INFO=/foo/bar`) |
| `PATH_TRANSLATED` | `PATH_INFO` joined with the document root |
| `QUERY_STRING` | Everything after `?` in the URL, **un-decoded** (the CGI does its own decoding) |

### Body

| Variable | Value |
|---|---|
| `CONTENT_LENGTH` | Decimal string of body byte count (only for POST/PUT) |
| `CONTENT_TYPE` | Body's MIME type from the request's `Content-Type` header |

### Echoed headers

Every other request header is passed as `HTTP_<NAME>` with hyphens turned to underscores and uppercased:

| Header | Env var |
|---|---|
| `User-Agent: curl/8.4` | `HTTP_USER_AGENT=curl/8.4` |
| `Cookie: a=1; b=2` | `HTTP_COOKIE=a=1; b=2` |
| `Accept: text/html` | `HTTP_ACCEPT=text/html` |
| `Host: example.com` | `HTTP_HOST=example.com` |

`Content-Type` and `Content-Length` are exceptions — they're not duplicated as `HTTP_CONTENT_TYPE` etc.; they go straight to `CONTENT_TYPE` and `CONTENT_LENGTH`.

---

## The CGI I/O model

```
       webserv                       CGI process
       ───────                       ───────────
                                          stdin  ← request body (you write)
                                          stdout → response (you read)
                                          stderr → log it (or /dev/null)
       env vars (the table above)
```

Three streams:
- **stdin** — the **request body** is piped in by the server. For GET (no body), close immediately. For POST, write the body bytes and close stdin so the CGI sees EOF.
- **stdout** — the **response** comes out here, *with a CGI-style header section* (see next section).
- **stderr** — the CGI's error output. Forward to your server's log or discard.

---

## CGI output format

The CGI doesn't write a full HTTP response. It writes a header section + body, like this:

```
Content-Type: text/html\r\n
\r\n
<html><body>Hello world</body></html>
```

Or, with a status:

```
Status: 404 Not Found\r\n
Content-Type: text/plain\r\n
\r\n
The page you wanted is missing.
```

Your server must:
1. Read the CGI's stdout until you've consumed the header section (ended by `\r\n\r\n` or `\n\n`).
2. Parse those headers.
3. Convert them to a full HTTP response:
   - Start line: derive from `Status:` header (default `200 OK` if not present).
   - All other headers from the CGI: pass through.
   - Body: the rest of the CGI's stdout.
4. Send to the client.

### Special CGI-only headers

| Header | Server behaviour |
|---|---|
| `Status: <code> <reason>` | Use this in your response's start line; do not pass as a header. |
| `Location: <url>` (relative starting with `/`) | The server should redirect internally — fetch the new URL and serve that. **Or** treat as an external redirect (302) — simpler. |
| `Location: <absolute URL>` | Treat as a 302 redirect. |

### Framing the CGI's body

Two cases:

1. **CGI sends `Content-Length`.** Easy — your response forwards that `Content-Length` and the body bytes.
2. **CGI doesn't send `Content-Length`.** Per the subject: *"If no content_length is returned from the CGI, EOF will mark the end of the returned data."* Read CGI stdout until the pipe closes, then:
   - Either buffer the whole thing and compute `Content-Length` yourself (recommended).
   - Or send `Transfer-Encoding: chunked`.
   - Or send `Connection: close` and stream.

Buffering is simplest and works for the subject's stress tests.

---

## The dance — full sequence

```
1. Request arrives, matches CGI route.

2. Server:
     a. pipe()  → cgi_stdin_pipe   (server writes, child reads)
     b. pipe()  → cgi_stdout_pipe  (child writes, server reads)
     c. fork()
        ↓
     CHILD:
        - dup2(cgi_stdin_pipe[0],  STDIN_FILENO)
        - dup2(cgi_stdout_pipe[1], STDOUT_FILENO)
        - close all unused pipe fds
        - chdir(directory of the CGI script)        ← subject mandates
        - build envp[] from the env-var table above
        - build argv[] = { interpreter, script_path, NULL }
        - execve(interpreter, argv, envp)
        - if exec fails: write error to stderr, exit(1)
        ↓
     PARENT:
        - close cgi_stdin_pipe[0] and cgi_stdout_pipe[1] (only-write/only-read in parent)
        - set both remaining fds to non-blocking
        - register them in your poll() set
        - record the (pid, fds) pair so you can match output to client later

3. As poll() reports the cgi pipes ready:
     - Write request body chunks to cgi_stdin_pipe[1] when POLLOUT fires.
     - When body fully written: close cgi_stdin_pipe[1] so CGI sees EOF.
     - Read CGI output chunks from cgi_stdout_pipe[0] when POLLIN fires.
     - Accumulate until you've seen the \r\n\r\n boundary plus enough body.

4. CGI exits. You waitpid() it (non-blocking) and reap. Build the final HTTP response from the accumulated CGI output. Send to client.
```

### Chunked-request gotcha

The subject:

> *"for chunked requests, your server needs to un-chunk them, the CGI will expect EOF as the end of the body."*

If a client sent the request body as `Transfer-Encoding: chunked`, **you must decode** ([file 06](06_FRAMING.md)) before piping to the CGI. The CGI doesn't speak chunked — it reads from stdin and stops at EOF. So:

1. Fully decode the chunked body into a buffer.
2. Set `CONTENT_LENGTH` to the decoded length.
3. Pipe the decoded bytes to the CGI's stdin.
4. Close the CGI's stdin.

(Or stream-decode if you want to be fancy. Buffer is fine.)

---

## CGI in your poll() set

This is where things get architecturally tricky. The single-poll rule means the **CGI pipes must be in the same `poll()` as the client sockets**.

Each active CGI execution adds two fds to your poll set:
- The write end of the stdin pipe (you have data to send to the CGI).
- The read end of the stdout pipe (you're waiting for CGI output).

When the CGI finishes:
- Its stdout pipe will return 0 on read (EOF).
- Remove the fds from the poll set.
- `waitpid(pid, &status, WNOHANG)` to reap.
- Build and send the final response to the corresponding client.

---

## CGI timeout

A buggy or hung CGI will sit forever. Add a per-CGI timeout (e.g. 10–30 seconds):

- When you fork, record the start time.
- Each poll iteration, check elapsed time for each CGI.
- If exceeded: `kill(pid, SIGTERM)`, then (after grace) `SIGKILL`. Send `504 Gateway Timeout` to the client.

The subject's allowed functions list includes `kill` and `signal` precisely for this.

---

## What you can ignore

- **NPH (Non-Parsed Header) scripts** — old CGI variant where the script writes a full HTTP response (start line included) and the server passes it through verbatim. Modern CGIs don't use this; webserv doesn't need to support it.
- **FastCGI / SCGI** — protocol-level alternatives to CGI that keep the interpreter alive across requests. Different protocol. Out of scope.
- **mod_*** style in-process interpreters (mod_php, mod_python) — not CGI. Not relevant.

---

## A minimal Python CGI for testing

`hello.py`:

```python
#!/usr/bin/env python3
import os
print("Content-Type: text/plain")
print()
print("Hello from CGI")
print(f"Method: {os.environ.get('REQUEST_METHOD')}")
print(f"Query:  {os.environ.get('QUERY_STRING')}")
print(f"Path info: {os.environ.get('PATH_INFO')}")
print(f"User agent: {os.environ.get('HTTP_USER_AGENT')}")
```

Make it executable (`chmod +x hello.py`), drop it in your web root, configure `.py` as a CGI extension pointing to `/usr/bin/python3` (or wherever your Python is). Hit `GET /hello.py?name=alice` and you should see the env vars echoed back.

Once that works, swap in `php-cgi` and try a `.php` file. Same plumbing.

---

## Common bugs

| Bug | Symptom | Fix |
|---|---|---|
| CGI hangs waiting for stdin | Server hangs too | Always close cgi_stdin_pipe[1] after writing (or right away for GET) |
| CGI's output starts with `HTTP/1.1` | Looks like double headers | That's NPH style — your CGI is misconfigured; should start with `Content-Type:` not `HTTP/1.1` |
| Wrong working directory in CGI | CGI fails to find sibling files | `chdir` to the script's directory in the child before exec |
| Fork/exec without setting close-on-exec on sockets | CGI inherits your listening socket | Set `FD_CLOEXEC` on every socket you don't explicitly want the child to see |
| Zombie CGI processes accumulate | `ps` shows many `<defunct>` entries | Call `waitpid(pid, &status, WNOHANG)` after each CGI completes |
| CGI reads from your stdin (and blocks) | Server inputs get hijacked | Always dup2 the pipe to STDIN in the child; otherwise child shares your stdin |
| Headers/body boundary detection misses `\n\n` | Some CGI scripts use LF only | Accept both `\r\n\r\n` and `\n\n` as the boundary |

---

## TL;DR mental model

> CGI = fork + execve + pipes + structured env. Your server **builds an environment**, **pipes the request body to the CGI's stdin**, and **reads the CGI's stdout as the response (with a small header section)**. **Un-chunk before piping. Close stdin so CGI sees EOF. Buffer stdout to know the final length. Reap with waitpid. Add a timeout.** Get this right and you've finished webserv's hardest feature.

**Continue to [`15_TOOLS.md`](15_TOOLS.md)** for the toolkit you'll use to debug all of this.
