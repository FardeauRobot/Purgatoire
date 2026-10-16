# 05 — Headers

> *"Headers are how HTTP attaches metadata to text. Most of the protocol's expressiveness lives here, not in the start line."*

A catalogue of the headers you'll **parse from requests** and **emit on responses** for webserv. Grouped by family. Each entry: what it is, where it appears, why it matters, what trap is associated with it.

The full RFC lists hundreds of headers. You need ~20.

---

## Header families (RFC vocabulary)

Headers are conventionally grouped:

- **General** — applicable to both requests and responses (`Date`, `Connection`)
- **Request** — appear only on requests (`Host`, `User-Agent`, `Accept`, `Cookie`)
- **Response** — appear only on responses (`Server`, `Location`, `Set-Cookie`, `Allow`)
- **Entity / Representation** — describe the body (`Content-Length`, `Content-Type`, `Content-Encoding`)

In modern terms (RFC 7230+) the "entity" family is renamed to "payload" or "representation," but the original four-bucket model is still a useful mental model.

---

## Parsing rules (recap, must internalise)

- **Names are case-insensitive.** Lowercase on storage, lowercase on lookup.
- **Values may have leading/trailing whitespace.** Trim them.
- **Multiple values** can be comma-separated on one line, or appear as multiple lines with the same name.
- **No header is mandatory for HTTP/1.0.** `Host` is mandatory for HTTP/1.1.

---

## General headers

### `Host`
**Mandatory on HTTP/1.1 requests.** Tells the server which "virtual host" the request is for, when multiple sites share an IP and port.

```
Host: example.com
Host: api.example.com:8443
```

For webserv, **virtual hosts are out of scope per the subject** — you dispatch by `interface:port` pair, not by `Host`. But you still must accept the header on input (returning 400 if it's missing on an HTTP/1.1 request).

### `Connection`
Controls connection lifecycle. See [file 07](07_CONNECTION.md).

```
Connection: close          ← close the TCP connection after this response
Connection: keep-alive     ← keep the TCP connection open (default in HTTP/1.1)
```

For webserv: respect `Connection: close` from clients (close after the response). Default in HTTP/1.1 is keep-alive. If you don't implement keep-alive yet, **always send `Connection: close`** in your responses and close the socket.

### `Date`
RFC 1123 formatted date.

```
Date: Sun, 30 May 2026 14:23:11 GMT
```

You should send `Date` on every response. Use `time(NULL)` and `strftime` with the format `%a, %d %b %Y %H:%M:%S GMT`. Don't forget GMT — the spec mandates it.

### `Transfer-Encoding`
**Critical for webserv.** Describes how the body is framed. See [file 06](06_FRAMING.md).

```
Transfer-Encoding: chunked
```

If a request has this header, the body is chunked-encoded — you must decode it before consuming. If a response has it, the client expects chunked output.

---

## Request headers

### `User-Agent`
Identifies the client.

```
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) ...
User-Agent: curl/8.4.0
```

For webserv: **read it for logging**, ignore it for behaviour. Some servers content-negotiate on UA — overkill here.

### `Accept`
What MIME types the client is willing to receive. See [file 09](09_CONTENT_NEGOTIATION.md).

```
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
```

For webserv: **ignore** for static files (the file's extension determines `Content-Type`). For CGI, pass it through the `HTTP_ACCEPT` environment variable.

### `Accept-Encoding`
What compression algorithms the client supports.

```
Accept-Encoding: gzip, deflate, br
```

For webserv: ignore unless you implement gzip output (you don't have to; out of scope).

### `Accept-Language`
Preferred languages.

```
Accept-Language: en-US,en;q=0.9,fr;q=0.8
```

For webserv: ignore.

### `Authorization`
Credentials.

```
Authorization: Basic dXNlcjpwYXNz
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

For webserv: not required. If you implement basic auth, decode and check; otherwise pass through to CGI.

### `Cookie`
Client-stored state echoed back to server. See [file 12](12_COOKIES_SESSIONS.md).

```
Cookie: sessionid=abc123; theme=dark
```

For webserv: **bonus only** per the subject. If you implement sessions, parse this header. Otherwise pass through to CGI (which will handle its own session tracking).

### `Referer` (sic — historical misspelling)
URL the client came from.

```
Referer: https://example.com/page1
```

For webserv: read it for logging, ignore for behaviour.

### `Content-Length` (on requests with bodies)
Number of bytes in the body. See [file 06](06_FRAMING.md).

```
Content-Length: 137
```

For webserv: **read this on POST/PUT requests** to know how much body to consume.

### `Content-Type` (on requests with bodies)
MIME type of the body.

```
Content-Type: application/json
Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryABCDE
Content-Type: application/x-www-form-urlencoded
```

For webserv: critical for file-upload parsing. `multipart/form-data` requires you to find the `boundary=` parameter and split the body on it.

---

## Response headers

### `Server`
Identifies your server.

```
Server: webserv/1.0
```

Always include. Free advertising.

### `Location`
URL for redirects (3xx) or location of newly-created resource (201).

```
Location: https://example.com/new-path
Location: /resources/42
```

Both absolute and relative URLs are valid. Mandatory on 3xx responses.

### `Allow`
Methods allowed on this resource — **mandatory on 405 responses**.

```
Allow: GET, POST, DELETE
```

### `Set-Cookie`
Sets a cookie on the client. See [file 12](12_COOKIES_SESSIONS.md).

```
Set-Cookie: sessionid=abc123; Path=/; HttpOnly; Max-Age=3600
```

For webserv: bonus only.

### `WWW-Authenticate`
Sent with 401 to tell the client what auth scheme to use.

```
WWW-Authenticate: Basic realm="Access to staging"
```

Not required by webserv.

### `Content-Length` (on responses with bodies)
**Always include** unless you're using `Transfer-Encoding: chunked`.

```
Content-Length: 8421
```

Without this, the client doesn't know when the body ends — fine if you close the connection, broken for keep-alive.

### `Content-Type` (on responses with bodies)
**Always include** when there's a body.

```
Content-Type: text/html; charset=utf-8
Content-Type: application/octet-stream
```

For static files: derive from extension. Maintain a small lookup table:

```cpp
// pseudo-table, build a std::map<std::string,std::string>
".html"  → "text/html; charset=utf-8"
".css"   → "text/css"
".js"    → "application/javascript"
".png"   → "image/png"
".jpg"   → "image/jpeg"
".gif"   → "image/gif"
".json"  → "application/json"
".txt"   → "text/plain; charset=utf-8"
default  → "application/octet-stream"
```

See [file 09](09_CONTENT_NEGOTIATION.md) for the full list of common MIME types.

### `Content-Encoding`
Body compression. **Not the same as Transfer-Encoding.**

```
Content-Encoding: gzip
```

For webserv: skip. Out of scope.

### `Last-Modified`
When the resource was last modified. Useful with conditional `GET` (see [file 11](11_CACHING.md)).

```
Last-Modified: Tue, 15 Apr 2025 10:13:22 GMT
```

For webserv static files: derive from `stat()`'s `st_mtime`.

### `ETag`
Opaque token identifying the current version of a resource. See [file 11](11_CACHING.md).

```
ETag: "abc123"
```

Not required by webserv.

---

## Entity / payload headers (appear on whichever side has the body)

Covered above with their associated side: `Content-Length`, `Content-Type`, `Content-Encoding`, `Transfer-Encoding`.

Additional ones rarely needed:
- `Content-Disposition` — suggests filename for downloads (`attachment; filename="x.pdf"`). Useful for file-server endpoints.
- `Content-Language`, `Content-Location` — niche.

---

## Custom / extension headers

Anything starting with a non-standard name is fair game. Convention is `X-` prefix historically (`X-Forwarded-For`, `X-Request-ID`), though `X-` was deprecated by RFC 6648 in favour of plain names.

Treat any unknown header as: **store it**, ignore it for behaviour, **pass it through to CGI** as `HTTP_<NAME>` env var (uppercased, hyphens to underscores). See [file 14](14_CGI.md).

---

## The minimum response headers webserv should always emit

```
HTTP/1.1 <code> <reason>\r\n
Server: webserv/1.0\r\n
Date: <rfc1123 date>\r\n
Content-Type: <derived or text/html>\r\n
Content-Length: <byte count>\r\n
Connection: close\r\n          ← or keep-alive once you implement it
\r\n
<body>
```

Six headers, every time. Building a helper:

```cpp
// pseudo-code, illustrates structure
std::string build_response(int code,
                           const std::string& body,
                           const std::string& content_type) {
    std::stringstream s;
    s << "HTTP/1.1 " << code << " " << status_reason(code) << "\r\n";
    s << "Server: webserv/1.0\r\n";
    s << "Date: " << current_http_date() << "\r\n";
    s << "Content-Type: " << content_type << "\r\n";
    s << "Content-Length: " << body.size() << "\r\n";
    s << "Connection: close\r\n";
    s << "\r\n";
    s << body;
    return s.str();
}
```

This single function will handle 80% of your responses. Special cases (304, 204, 3xx with `Location`, 405 with `Allow`) get small wrappers.

---

## TL;DR mental model

> Headers are how HTTP carries metadata. **Names are case-insensitive, values are trimmed, repeats can be comma-folded.** The four families (general / request / response / entity) is a useful taxonomy, not enforced. For webserv: a small fixed set per response (`Server`, `Date`, `Content-Type`, `Content-Length`, `Connection`) covers almost everything; reach for more only when implementing a specific feature.

**Continue to [`06_FRAMING.md`](06_FRAMING.md)** — the single most important file in this library for the webserv project.
