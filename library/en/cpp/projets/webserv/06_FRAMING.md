# 06 — Framing (Content-Length vs Chunked)

> *"If you can't tell where a message ends, you can't read the next one."*

**The most important file in this library.** Get framing wrong and every other layer of webserv breaks: requests bleed into each other, CGI hangs, keep-alive crashes, responses get truncated. The subject explicitly calls this out twice (chunked requests, chunked-vs-content-length on the CGI return path).

---

## The problem

TCP is a stream of bytes. It has **no message boundary**. When you call `recv()` you might get:

- A fraction of one request
- One full request
- One full request plus part of the next
- Several requests at once

The HTTP protocol does not put a "message length" in the start line. So how does the server know **when to stop reading the body** of a request? And how does the client know **when to stop reading the body** of a response?

Three ways, in order of priority:

1. **`Content-Length: N`** — read exactly N more bytes after the header section.
2. **`Transfer-Encoding: chunked`** — body is a sequence of length-prefixed chunks, terminated by a special "zero-length" chunk.
3. **No length header, no chunked** — read until the connection closes (`Connection: close` semantics).

If both `Content-Length` and `Transfer-Encoding` are present, **`Transfer-Encoding` wins** and `Content-Length` must be ignored (the spec says the request is suspect — old proxies use this for smuggling attacks; reject it with 400 to be safe).

---

## Method 1 — `Content-Length`

The simple case. The header tells you the byte count of the body.

### On a request

```
POST /upload HTTP/1.1\r\n
Host: example.com\r\n
Content-Type: text/plain\r\n
Content-Length: 11\r\n
\r\n
Hello world
```

Your parser:
1. Reads headers until blank line.
2. Looks up `Content-Length` → `11`.
3. Reads **exactly** 11 more bytes. The body is `"Hello world"`.
4. Anything after those 11 bytes belongs to the **next** request on this connection.

### On a response

```
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 53\r\n
\r\n
<!DOCTYPE html><html><body>Hello world</body></html>
```

Client does the same: read headers, look up `Content-Length`, read that many bytes, stop.

### Pitfalls

- **Counting bytes, not characters.** UTF-8 strings have variable-width characters. `"hé"` is 3 bytes (`0x68 0xC3 0xA9`), not 2.
- **Off-by-one from `\r\n`.** The body starts **after** the blank line that ends the headers. The blank line itself is not part of the body.
- **Trailing `\r\n` in the body.** Some senders accidentally include one. The `Content-Length` decides — if it says 11 and someone added a `\r\n` after "Hello world", you read 11 bytes (`"Hello world"`) and the `\r\n` is part of the next request (which is then malformed).
- **`Content-Length: 0`** is valid and means "no body". Common on `DELETE` requests.

---

## Method 2 — `Transfer-Encoding: chunked`

When the sender doesn't know the body length in advance (streaming, dynamic generation), they use chunked encoding. The body becomes a sequence of length-prefixed chunks.

### Wire format

```
<chunk-size-in-hex>\r\n
<chunk-data>\r\n
<chunk-size-in-hex>\r\n
<chunk-data>\r\n
...
0\r\n
\r\n
```

Each chunk:
1. **Size** in hexadecimal ASCII, followed by `\r\n`.
2. **Data** — exactly that many bytes.
3. Trailing `\r\n` (not counted in the size).

A chunk of size `0` marks the end. After that, optionally, some "trailer" headers, then `\r\n`.

### Worked example

The body `"Hello world"` (11 bytes) sent in two chunks of 5 and 6 bytes:

```
5\r\n
Hello\r\n
6\r\n
 world\r\n
0\r\n
\r\n
```

(Note the space at the start of `" world"`.)

Total bytes on the wire: 24, even though the actual content is 11. The overhead is the price of "we don't know in advance how big this'll be."

### Hex size — gotcha

The size is in **hexadecimal**, not decimal. A chunk of 255 bytes is `ff\r\n`, not `255\r\n`. A chunk of 10 is `a\r\n`, not `10\r\n`. (Decimal `10` in hex means 16 bytes — easy bug.)

### Decoder sketch

```cpp
// pseudo-code
std::string decode_chunked(SocketBuffer& sock) {
    std::string body;
    while (true) {
        std::string size_line = sock.read_line();   // reads up to and consumes \r\n
        size_t size = parse_hex(size_line);          // strtol(s.c_str(), NULL, 16)
        if (size == 0) {
            sock.read_line();                        // consume the final empty line
            break;
        }
        std::string chunk = sock.read_n_bytes(size);
        sock.read_line();                            // consume the \r\n after the chunk
        body += chunk;
    }
    return body;
}
```

**Implementation notes:**
- `strtol(s.c_str(), NULL, 16)` in C++98 parses hex. There's also `std::stringstream s; s >> std::hex >> n;`.
- The decoder is straightforward **but must be non-blocking-friendly** in webserv. You can't `recv()` and wait. You read what's available, save your state ("I'm halfway through chunk 3 with 47 more bytes to read"), and return to `poll()`. When more bytes arrive, resume where you left off.
- This means your chunked decoder needs to be **resumable** — it can't be a single function call. It's a small state machine.

### Pitfall: don't forward chunked bodies to CGI

The subject explicitly says:

> *"for chunked requests, your server needs to un-chunk them, the CGI will expect EOF as the end of the body."*

CGI scripts (PHP, Python, etc.) don't speak chunked. They read from stdin until EOF. So: **un-chunk first**, then pipe the decoded body to the CGI's stdin, then close that stdin so the CGI sees EOF.

---

## Method 3 — Read until close

If neither `Content-Length` nor `Transfer-Encoding: chunked` is present in a **response**, the client reads until the server closes the connection. This is the HTTP/1.0 default behaviour.

For requests, this method is **not valid** — a request without `Content-Length` on a method that has a body is malformed; you should respond with `411 Length Required`.

For responses, this is what happens if you forget `Content-Length`. It "works" but breaks keep-alive: the client must wait for `FIN` to know the body is done, which means you can't reuse the TCP connection. **Always set `Content-Length` on responses with bodies.**

---

## Output framing — your responses

For each response you generate, pick one:

| Body size known up front? | Use |
|---|---|
| Yes (static file, error page) | `Content-Length` |
| No (streaming CGI output without `Content-Length` from the CGI) | `Transfer-Encoding: chunked` (if HTTP/1.1) or just close after writing (if HTTP/1.0) |

In webserv, **default to `Content-Length`** — your static files are sized by `stat()`, your error pages are pre-built strings, your CGI output (after you've fully buffered it) has a known size.

> **Stream-vs-buffer trade-off for CGI.** Reading the entire CGI output before sending lets you compute `Content-Length` and use the simple framing. The cost is memory — a CGI returning a 100MB file is 100MB in your server's heap. The alternative is forwarding CGI bytes in chunks as they arrive — more code, more correctness risk. **For webserv: buffer.** Your stress tests won't generate 100MB CGI responses.

---

## When to send `Connection: close` vs reuse

If your response uses `Content-Length`, the client can know when the body ends and reuse the connection (keep-alive).

If your response **lacks** `Content-Length` and isn't chunked, you **must** include `Connection: close` so the client knows to expect EOF as the body terminator. Otherwise it will hang waiting for the body indefinitely.

See [file 07](07_CONNECTION.md) for the full keep-alive picture.

---

## The CGI return-side framing problem

Per the subject:

> *"If no content_length is returned from the CGI, EOF will mark the end of the returned data."*

When you spawn a CGI and read its stdout, the CGI might:

1. **Output `Content-Length:` itself** — easy. You forward that as your response's `Content-Length`.
2. **Not output `Content-Length:`** — you read until the CGI's stdout pipe closes (EOF). Then you either:
   - Re-frame with your own `Content-Length` (recommended; you know how many bytes you collected)
   - Forward with `Transfer-Encoding: chunked` (more complex)
   - Send with `Connection: close` (lazy but valid)

---

## Edge cases to test

- Body of length 0 with `Content-Length: 0` → empty body, valid.
- Body of length 0 with no `Content-Length` → no body, valid.
- `Content-Length` that disagrees with actual bytes → undefined; reject with 400.
- `Content-Length` larger than your `client_max_body_size` config → 413 Payload Too Large, do not read the body.
- Chunked body where the first chunk is `0\r\n\r\n` → empty body, valid.
- Chunked body that never sends the terminating `0\r\n` → eventually timeout (408 Request Timeout).
- A request with both `Content-Length` and `Transfer-Encoding: chunked` → 400 Bad Request (request smuggling vector).

---

## TL;DR mental model

> The body's end is not in the start line — it's in `Content-Length` (count bytes) or `Transfer-Encoding: chunked` (decode the chunk format), and if neither is present, read until close. **For webserv:** decode chunked requests before consuming them; un-chunk before piping to CGI; always set `Content-Length` on your outgoing responses; reject conflicting framing with 400.

**Continue to [`07_CONNECTION.md`](07_CONNECTION.md)** — once the body ends, what happens to the TCP connection?
