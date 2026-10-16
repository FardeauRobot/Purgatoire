# 11 — Caching

> *"The fastest response is the one you don't have to send."*

HTTP caching is how the same bytes get sent across the network fewer times. Browsers cache, CDNs cache, proxies cache. Done correctly, caching makes pages feel instant. Done incorrectly, users see stale content for hours.

For webserv, **caching is optional** — the subject doesn't require it. But understanding the headers is useful: even if your server doesn't *generate* caching hints, browsers will *send* conditional requests based on what they already have, and ignoring them politely (with `200 OK` and the full body) is correct but wasteful.

This file is shorter than its peers — it's a "good to know" rather than "must implement."

---

## The two questions caching answers

1. **Can I cache this response?** Decided by `Cache-Control`, `Expires`, and (for shared caches) `Vary`.
2. **Is my cached copy still fresh?** Decided by **conditional requests** using `ETag` and `Last-Modified`.

---

## `Cache-Control`

The modern, all-in-one caching directive header.

| Value | Meaning |
|---|---|
| `Cache-Control: no-store` | Don't cache at all, ever. |
| `Cache-Control: no-cache` | You may cache it, but **always revalidate** before reusing. |
| `Cache-Control: private` | Only the user's browser may cache, not shared proxies/CDNs. |
| `Cache-Control: public` | Anyone may cache. |
| `Cache-Control: max-age=3600` | Fresh for 3600 seconds. After that, must revalidate. |
| `Cache-Control: must-revalidate` | Once stale, must check with origin before reusing. |
| `Cache-Control: immutable` | Bytes will never change for this URL — never revalidate. Used with hashed filenames (`app.a1b2c3.js`). |

Combinations:

```
Cache-Control: public, max-age=86400          ← cache for a day
Cache-Control: private, max-age=0, must-revalidate  ← personalised pages
Cache-Control: no-store                       ← bank statements
```

For webserv: you could emit `Cache-Control: no-cache` on every response and call it done. Conservative, correct, performant-enough for the project.

---

## `ETag` — opaque version identifier

```
ETag: "abc123def456"
```

An `ETag` is a server-chosen string that uniquely identifies a specific version of a resource. Two ways to derive it for a static file:

- **Strong ETag** — based on the file's full hash (MD5/SHA). Differs even if bytes shift by one.
- **Weak ETag** — prefixed `W/`, may match across "semantically equivalent" versions.

For static files, an easy ETag derivation is `inode-size-mtime`:

```cpp
// pseudo-code
struct stat st;
stat(path.c_str(), &st);
std::stringstream s;
s << '"' << st.st_ino << '-' << st.st_size << '-' << st.st_mtime << '"';
std::string etag = s.str();
```

This isn't cryptographically strong but it's enough for caching purposes — if the file changes, at least one of those three fields changes.

---

## `Last-Modified` — when the resource was last changed

```
Last-Modified: Wed, 15 Apr 2025 10:13:22 GMT
```

Derived from `stat()`'s `st_mtime`, formatted as RFC 1123:

```cpp
char buf[64];
struct tm* gmt = gmtime(&st.st_mtime);
strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);
```

---

## Conditional requests

When the client has a cached copy, it can ask the server "is my copy still valid?" using one of:

### `If-None-Match`

```
GET /style.css HTTP/1.1\r\n
Host: example.com\r\n
If-None-Match: "abc123def456"\r\n
\r\n
```

"I already have a copy with ETag `"abc123def456"`. If your current version's ETag matches, just tell me to keep mine."

Server response if the ETag matches:

```
HTTP/1.1 304 Not Modified\r\n
ETag: "abc123def456"\r\n
\r\n
```

**No body.** `304` is "your copy is fine; here are some metadata headers, save the bandwidth on the body."

Server response if the ETag doesn't match: normal `200 OK` with the new body and new ETag.

### `If-Modified-Since`

```
GET /style.css HTTP/1.1\r\n
Host: example.com\r\n
If-Modified-Since: Wed, 15 Apr 2025 10:13:22 GMT\r\n
\r\n
```

"My copy is from this date. If you haven't changed it since, tell me to keep mine."

Server compares to the file's `mtime`. If unchanged, return `304`. If changed, return `200` with the new content.

### Priority

If the client sends both `If-None-Match` and `If-Modified-Since`, **`If-None-Match` wins** — it's more reliable (a file can be `touch`ed without its content changing, which would falsely invalidate by `Last-Modified` but correctly validate by ETag).

---

## A minimal conditional-GET implementation for webserv

```cpp
// pseudo-code
void handle_get(const Request& req, const std::string& filesystem_path) {
    struct stat st;
    if (stat(filesystem_path.c_str(), &st) < 0) {
        return serve_error(404);
    }

    std::string etag = derive_etag(st);
    std::string last_modified = format_http_date(st.st_mtime);

    // Check conditional headers
    if (req.headers.count("if-none-match")
        && req.headers["if-none-match"] == etag) {
        return serve_304(etag, last_modified);
    }
    if (req.headers.count("if-modified-since")) {
        time_t since = parse_http_date(req.headers["if-modified-since"]);
        if (since != (time_t)-1 && st.st_mtime <= since) {
            return serve_304(etag, last_modified);
        }
    }

    // Full response with caching metadata
    serve_200(filesystem_path, etag, last_modified);
}
```

Implementing this lets browsers re-validate cached copies cheaply — about 50 lines of code, saves a lot of bandwidth. **Optional**, but a nice quality-of-life touch.

---

## `Expires` (legacy)

The HTTP/1.0 version of `Cache-Control: max-age=...`:

```
Expires: Thu, 01 Jun 2026 12:00:00 GMT
```

If you only support HTTP/1.0, use this. If you support 1.1, `Cache-Control: max-age=N` is preferred (it's relative to "now", so clock skew doesn't matter).

---

## The cache hierarchy

```
client ─────► browser cache ─────► CDN ─────► reverse proxy ─────► origin server
                                                                       │
                                                                       │
                                                                  (your webserv)
```

Each layer may cache. `Cache-Control: private` restricts caching to the browser. `Cache-Control: public` permits intermediaries to cache.

For webserv (a single origin server with no upstream caches in the eval setup), only the browser cache matters. Use `private` (or omit, since `private` is the default behaviour for un-authenticated responses).

---

## What webserv can safely ignore

- `Vary` (only matters if you have multiple cacheable variants)
- `Age` (set by caches, not origin servers)
- `Cache-Control: stale-while-revalidate`, `stale-if-error` (CDN features)
- `Cache-Control: s-maxage` (shared cache TTL — irrelevant without intermediaries)
- `Pragma: no-cache` (HTTP/1.0 legacy; ignore on request, don't generate on response)
- Heuristic caching (when browsers guess freshness based on `Last-Modified` alone — your server doesn't control this)

---

## TL;DR mental model

> Caching is the server saying "you may keep this for N seconds" (`Cache-Control: max-age=N`) and the client coming back later saying "still good?" (`If-None-Match` or `If-Modified-Since`). The server replies with **`304 Not Modified`** (no body) if the cache is still valid, or **`200 OK`** with the new content if not. **Optional for webserv** — but implementing it for static files is ~50 lines and saves real bandwidth.

**Continue to [`12_COOKIES_SESSIONS.md`](12_COOKIES_SESSIONS.md)** for the other "optional but interesting" topic.
