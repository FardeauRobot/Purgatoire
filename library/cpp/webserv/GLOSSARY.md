# 📖 Glossary — HTTP & Webserv

Quick lookup of the vocabulary that appears across this library. Alphabetical. Cross-references in `[[brackets]]` point to other glossary entries; numbered links point to library files.

---

### Absolute URL
A URL that includes the scheme and host: `https://example.com/page`. Opposite: [[Relative URL]].

### Accept (header)
Request header listing the MIME types the client is willing to receive. Webserv ignores it for static files; passes it to CGI as `HTTP_ACCEPT`. → [09](09_CONTENT_NEGOTIATION.md).

### Allow (header)
Response header listing the methods permitted on the resource. **Mandatory on 405 responses.** → [05](05_HEADERS.md).

### Authority-form
A request-line target consisting of `host:port`. Only used with `CONNECT`. Webserv ignores. → [08](08_URLS.md).

### Authority (URI part)
The `host:port` portion of a URL.

### Backlog
The queue size for incoming TCP connections waiting to be `accept()`ed. Set as the second argument to `listen()`. → [16](16_TINY_SERVER_LAB.md).

### Base64
Encoding scheme that maps any byte sequence to ASCII (`A-Za-z0-9+/`). Used in `Authorization: Basic` and inside cookies sometimes. Not relevant to webserv core.

### Body
The payload bytes that follow the blank line of an HTTP message. Optional on requests (only for methods that carry data); optional on responses. → [02](02_MESSAGE_ANATOMY.md), [06](06_FRAMING.md).

### Boundary
The separator string in `multipart/form-data` request bodies. Specified in the `Content-Type: multipart/form-data; boundary=...` parameter. On the wire, each occurrence is prefixed with `--`, the final occurrence with `--...--`. → [09](09_CONTENT_NEGOTIATION.md).

### Cache-Control
General header controlling caching behaviour. Common values: `no-store`, `no-cache`, `max-age=N`, `private`, `public`. → [11](11_CACHING.md).

### Cacheable
A response is cacheable if a client or intermediary may store it for reuse. Defaults vary by status code and method. → [03](03_METHODS.md), [11](11_CACHING.md).

### CGI (Common Gateway Interface)
Protocol (RFC 3875) by which a web server delegates request handling to an external program via environment variables, stdin, stdout. Mandatory in webserv. → [14](14_CGI.md).

### Chunked encoding
Transfer encoding where the body is sent as a series of length-prefixed chunks, terminated by a zero-length chunk. Used when the body size isn't known up front. → [06](06_FRAMING.md).

### Connection (header)
Controls TCP connection reuse. Values: `close` (close after this exchange), `keep-alive` (HTTP/1.1 default — reuse). → [07](07_CONNECTION.md).

### Conditional request
A request with `If-None-Match` or `If-Modified-Since` header asking the server to confirm the client's cached copy is still valid. Server replies `304 Not Modified` if so. → [11](11_CACHING.md).

### Content-Length
Entity header giving the byte count of the body. → [06](06_FRAMING.md).

### Content-Type
Entity header giving the body's MIME type (`text/html; charset=utf-8`, `application/json`, ...). → [05](05_HEADERS.md), [09](09_CONTENT_NEGOTIATION.md).

### Cookie (header)
Request header echoing cookies the client has stored for this server. → [12](12_COOKIES_SESSIONS.md).

### CRLF
Carriage return + line feed = `\r\n` = bytes `0x0D 0x0A`. The HTTP line terminator. → [02](02_MESSAGE_ANATOMY.md).

### Date (header)
General header giving the time the message was generated. Format: RFC 1123 (`Sun, 30 May 2026 14:23:11 GMT`). Always GMT. → [05](05_HEADERS.md).

### DELETE
HTTP method: remove the resource at the URL. Idempotent. Required by webserv. → [03](03_METHODS.md).

### Entity header
Headers describing the body: `Content-Length`, `Content-Type`, `Content-Encoding`. Appears on whichever side has a body. (Modern term: "representation" / "payload" header.)

### ETag
Response header giving an opaque version identifier for the resource. Used in conditional requests via `If-None-Match`. → [11](11_CACHING.md).

### Expires (header)
HTTP/1.0 caching directive giving an absolute expiry date. Superseded by `Cache-Control: max-age=N` in HTTP/1.1. → [11](11_CACHING.md).

### `FD_CLOEXEC`
File descriptor flag — when set on an fd, that fd is closed automatically during `execve`. Critical to set on sockets before forking for CGI, so the CGI child doesn't inherit your listening sockets. → [07](07_CONNECTION.md).

### Fragment
The `#name` portion of a URL. **Never sent to the server.** → [08](08_URLS.md).

### Framing
The mechanism by which the receiver knows where a message body ends. Via `Content-Length`, `Transfer-Encoding: chunked`, or connection close. → [06](06_FRAMING.md).

### GET
HTTP method: fetch the representation of a resource. Safe, idempotent, cacheable. → [03](03_METHODS.md).

### HEAD
HTTP method: like `GET` but the response has no body. Useful for metadata-only queries. → [03](03_METHODS.md).

### Header
A `Key: Value` line in the headers section of an HTTP message. Case-insensitive name, trimmed value. → [05](05_HEADERS.md).

### Host (header)
Request header naming the target host. **Mandatory on HTTP/1.1 requests.** Webserv accepts but ignores (virtual hosts are out of scope). → [05](05_HEADERS.md).

### HTTPS
HTTP over TLS. Encrypted, authenticated, integrity-protected. Out of scope for webserv. → [13](13_HTTPS_TLS.md).

### Idempotent
A method whose effect is the same whether invoked once or N times. `GET`, `PUT`, `DELETE` are idempotent; `POST`, `PATCH` are not. → [03](03_METHODS.md).

### `If-Modified-Since`
Conditional request header. Server replies `304 Not Modified` if the resource is unchanged since the given date. → [11](11_CACHING.md).

### `If-None-Match`
Conditional request header. Server replies `304 Not Modified` if the resource's current ETag matches one of the provided ETags. → [11](11_CACHING.md).

### `INADDR_ANY`
Special IP address `0.0.0.0` meaning "bind to all interfaces." Used in `sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);`. → [16](16_TINY_SERVER_LAB.md).

### Keep-alive
Persistent TCP connection that survives multiple HTTP request-response exchanges. Default in HTTP/1.1. → [07](07_CONNECTION.md).

### Last-Modified
Response header giving the time the resource was last changed. Used by clients in `If-Modified-Since` for conditional requests. → [11](11_CACHING.md).

### Location (header)
Response header naming a URL. Used on 3xx (where to redirect) and 201 Created (where the new resource lives). → [05](05_HEADERS.md), [10](10_REDIRECTS.md).

### MIME type
A `type/subtype` label identifying a content format. Examples: `text/html`, `application/json`, `image/png`. → [09](09_CONTENT_NEGOTIATION.md).

### `multipart/form-data`
MIME type used by HTML forms to upload files. Body is segmented by a boundary string. → [09](09_CONTENT_NEGOTIATION.md).

### Network byte order
Big-endian. The byte order used in network protocol fields. Convert with `htons`/`htonl` (host → network) and `ntohs`/`ntohl` (network → host). → [16](16_TINY_SERVER_LAB.md).

### Non-blocking
A file descriptor mode where I/O syscalls (`read`, `write`, `accept`, ...) return immediately instead of waiting. Set with `fcntl(fd, F_SETFL, O_NONBLOCK)`. **Required throughout webserv.** → [07](07_CONNECTION.md).

### NPH (Non-Parsed Headers)
Legacy CGI variant where the script writes a full HTTP response (start line included). Webserv doesn't need to support. → [14](14_CGI.md).

### `O_NONBLOCK`
The flag passed to `fcntl(fd, F_SETFL, ...)` to make a file descriptor non-blocking. → [07](07_CONNECTION.md).

### `Origin-form`
A request-target in the form `/path?query`. The only form your webserv needs to handle. → [08](08_URLS.md).

### `Path`
The slash-separated portion of a URL between host and query string. → [08](08_URLS.md).

### Path traversal
Attack where the URL contains `..` segments to escape the configured root. Must be rejected (400 or 403). → [08](08_URLS.md).

### Percent-encoding
URL encoding scheme where bytes are written as `%XX` in hex. → [08](08_URLS.md).

### Pipelining
Sending multiple HTTP requests on the same connection without waiting for each response. Optional, mostly dead in practice. → [07](07_CONNECTION.md).

### `poll(2)`
Syscall that waits on multiple file descriptors for I/O readiness. The webserv subject mandates **one** `poll` for all socket I/O. Alternatives: `select`, `epoll` (Linux), `kqueue` (BSD/macOS). → [07](07_CONNECTION.md).

### POST
HTTP method: submit data; the server decides what to do with it. Not safe, not idempotent. Required by webserv. → [03](03_METHODS.md).

### PUT
HTTP method: replace the resource at the URL with the request body. Idempotent. Not required by webserv. → [03](03_METHODS.md).

### Query string
Everything after `?` in a URL, up to `#` or end. `key=value&key=value` format, percent-encoded. → [08](08_URLS.md).

### Reason phrase
The human-readable text in the status line: "OK" in `HTTP/1.1 200 OK`. Clients should ignore; stick to standard phrasing. → [04](04_STATUS_CODES.md).

### Relative URL
A URL without scheme or host: `/path`, `page.html`, `../images/x.png`. Interpreted against a base URL. Opposite: [[Absolute URL]].

### Request line
The first line of a request: `METHOD SP target SP HTTP/version CRLF`. → [02](02_MESSAGE_ANATOMY.md).

### Request-target
The second token of the request line. Usually origin-form (`/path?query`). → [08](08_URLS.md).

### Resource
The thing identified by a URL. Could be a file on disk, a CGI output, a redirect, an error.

### RFC
Request for Comments — IETF document. HTTP/1.1 lives in RFCs 7230 (message syntax), 7231 (semantics), 7232 (conditional), 7233 (range), 7234 (caching), 7235 (auth).

### Safe
A method intended not to change server state. `GET`, `HEAD`, `OPTIONS` are safe. → [03](03_METHODS.md).

### `SameSite` (cookie attribute)
Controls whether cookies are sent on cross-site requests. Values: `Strict`, `Lax`, `None; Secure`. → [12](12_COOKIES_SESSIONS.md).

### `Secure` (cookie attribute)
Cookie attribute that restricts the cookie to HTTPS connections only. → [12](12_COOKIES_SESSIONS.md).

### Server (header)
Response header identifying the server software. Set to your server's name (e.g. `Server: webserv/1.0`). → [05](05_HEADERS.md).

### Session
Server-side state keyed by a cookie value. Bonus for webserv. → [12](12_COOKIES_SESSIONS.md).

### Set-Cookie (header)
Response header asking the client to store a cookie. → [12](12_COOKIES_SESSIONS.md).

### `SO_REUSEADDR`
Socket option that allows binding to a port that's in `TIME_WAIT`. Always set on your listening socket. → [16](16_TINY_SERVER_LAB.md).

### Start-line
First line of an HTTP message. Called the **request line** in requests, **status line** in responses. → [02](02_MESSAGE_ANATOMY.md).

### Status code
3-digit number in the status line. Grouped by first digit: 1xx info, 2xx success, 3xx redirect, 4xx client error, 5xx server error. → [04](04_STATUS_CODES.md).

### Status line
The first line of a response: `HTTP/version SP status-code SP reason-phrase CRLF`. → [02](02_MESSAGE_ANATOMY.md).

### TCP
Transmission Control Protocol. The transport layer below HTTP. Provides reliable, ordered byte stream. → [01](01_FUNDAMENTALS.md).

### TLS
Transport Layer Security. The encryption layer below HTTPS. Out of scope for webserv. → [13](13_HTTPS_TLS.md).

### Transfer-Encoding
General header describing the body's encoding for transit. Common value: `chunked`. **Not the same as Content-Encoding.** → [06](06_FRAMING.md).

### Trailers
Headers sent **after** a chunked body. Rare. Webserv can ignore (read past the final empty line).

### URL / URI / URN
URI is the umbrella term; URL = "locates the thing"; URN = "names the thing." → [08](08_URLS.md).

### User-Agent
Request header identifying the client software (browser, curl, ...). → [05](05_HEADERS.md).

### Vary (header)
Tells caches that the response varies based on certain request headers. Skip for webserv. → [09](09_CONTENT_NEGOTIATION.md).

### Virtual host
Serving multiple websites from one IP+port, dispatched by `Host:` header. **Out of scope for webserv** — you dispatch by `interface:port` instead. → [01](01_FUNDAMENTALS.md), [17](17_WEBSERV_SUBJECT.md).

### `waitpid`
Syscall to reap a child process and prevent zombies. Used after CGI completes. Use `WNOHANG` for non-blocking. → [14](14_CGI.md).

### Webserv
The 42 project this library is built around. → [17](17_WEBSERV_SUBJECT.md).

### WSGI
Python's web server gateway interface. Not relevant to this library; mentioned only to clarify that it isn't CGI (it's an in-process variant).
