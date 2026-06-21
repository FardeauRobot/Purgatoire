# 08 — URLs and URIs

> *"A URL is a sentence. Parse it word by word."*

The request target on the first line of every request is (almost always) a URL path. To map that path to a file on disk — or to a CGI script — you need to understand how URLs are built, how they get encoded for safe transport, and how to undo that encoding.

---

## Vocabulary: URI, URL, URN

- **URI** (Uniform Resource Identifier) — the umbrella term.
- **URL** (Uniform Resource Locator) — a URI that **tells you where the thing is** (`https://example.com/page`).
- **URN** (Uniform Resource Name) — a URI that **names the thing without locating it** (`urn:isbn:0451450523`).

For webserv: **URL.** URNs don't appear.

---

## URL anatomy

The full form:

```
  scheme ://  user:pass @  host  :port  / path     ? query    # fragment
   │            │          │     │       │         │           │
   │            │          │     │       │         │           └── client-side only,
   │            │          │     │       │         │               never sent to server
   │            │          │     │       │         └── ?key=value&key=value
   │            │          │     │       └── /this/is/the/file
   │            │          │     └── usually 80 (http) or 443 (https)
   │            │          └── example.com
   │            └── almost never used; legacy
   └── http, https, ftp, file, ...
```

Worked example:

```
https://alice:hunter2@api.example.com:8443/users/42?format=json&fields=name#bio
└──┬──┘  └─────┬─────┘ └──────┬──────┘ └┬┘ └───┬───┘ └─────────┬────────┘ └┬┘
 scheme    userinfo            host    port  path             query     fragment
```

### What arrives at your server

When a browser requests `https://example.com:8443/users/42?format=json#bio`, your server sees in the request line:

```
GET /users/42?format=json HTTP/1.1\r\n
Host: example.com:8443\r\n
```

- **Scheme** — known from the connection (HTTP because they connected to your HTTP port; HTTPS if you terminate TLS).
- **Userinfo** — almost never present; ignore.
- **Host** — in the `Host` header.
- **Port** — implied by which listening socket received the connection (you accept on port 8443 → port is 8443).
- **Path + query** — in the request line.
- **Fragment** — **never sent to the server.** The `#bio` is browser-only; it stays client-side.

### The "request-target" forms

RFC 7230 defines four forms for the request line's target. You only need to care about:

| Form | Looks like | Used by |
|---|---|---|
| **origin-form** | `/path?query` | Standard requests from clients to origin servers. **This is what webserv sees.** |
| **absolute-form** | `http://example.com/path` | Requests to proxies. You can ignore. |
| **authority-form** | `example.com:443` | `CONNECT` only. Ignore. |
| **asterisk-form** | `*` | `OPTIONS *`. Ignore (or treat as 501). |

---

## The path

The slash-separated portion after the host. Each segment between slashes is a "path segment."

Examples:
- `/` — the root
- `/index.html` — a file at the root
- `/users/42/profile` — three segments deep
- `/files/Important File.pdf` — segment with a space (will be encoded — see below)
- `/foo/../bar` — contains a parent-directory reference. **DANGER.** See [Path traversal](#path-traversal-the-most-important-trap-in-webserv).

### Mapping path → file via the config's `root`

Webserv's config file maps URL paths to filesystem paths via `root` rules. The subject's example:

> *"if URL `/kapouet` is rooted to `/tmp/www`, URL `/kapouet/pouic/toto/pouet` will search for `/tmp/www/pouic/toto/pouet`."*

Note: the matched prefix is **stripped** before joining with `root`. `/kapouet/pouic/toto/pouet` minus the matched `/kapouet` prefix is `/pouic/toto/pouet`, joined to `/tmp/www` gives `/tmp/www/pouic/toto/pouet`.

This is **prefix matching**, not regex. The "longest matching prefix wins" rule (similar to longest-prefix routing in netpractice).

### Path traversal — the most important trap in webserv

```
GET /../etc/passwd HTTP/1.1
GET /files/..%2F..%2Fetc%2Fpasswd HTTP/1.1
GET /files/%2e%2e/%2e%2e/etc/passwd HTTP/1.1
```

These are all attempts to escape the `root` directory. **You must reject them.**

Two defensive strategies, use both:

1. **Reject `..` segments early** — after percent-decoding the path, split on `/`, and if any segment is `..` (or contains a null byte), return `400 Bad Request`.
2. **Verify the resolved path stays under `root`** — after computing the final filesystem path, canonicalise it (e.g. with `realpath()` — though check the subject's allowed functions; you may need to roll your own) and check it begins with the configured `root`. If not, return `403 Forbidden`.

The classic webserv eval question: *"What happens if I request `GET /../../etc/passwd`?"* The right answer is **403 or 400, never a leaked file**.

---

## The query string

Everything after `?` up to (but not including) `#` or end-of-target.

```
?format=json&fields=name&page=2
```

Structure: a list of `key=value` pairs separated by `&`. Keys and values are **percent-encoded** so they can contain `=`, `&`, and other special characters safely.

### Parsing a query string

```cpp
// pseudo-code
std::map<std::string, std::string> parse_query(const std::string& q) {
    std::map<std::string, std::string> result;
    size_t pos = 0;
    while (pos < q.size()) {
        size_t amp = q.find('&', pos);
        if (amp == std::string::npos) amp = q.size();

        std::string pair = q.substr(pos, amp - pos);
        size_t eq = pair.find('=');
        if (eq == std::string::npos) {
            result[percent_decode(pair)] = "";
        } else {
            result[percent_decode(pair.substr(0, eq))]
                 = percent_decode(pair.substr(eq + 1));
        }
        pos = amp + 1;
    }
    return result;
}
```

For webserv, **the query string itself doesn't change your file-serving behaviour** — `/foo.html?x=1` and `/foo.html?y=2` both serve `foo.html`. But for **CGI**, the query string is passed via the `QUERY_STRING` environment variable, and CGI scripts use it to know what the client requested. **Do not include the query in the filesystem path lookup.**

---

## Percent-encoding (a.k.a. URL-encoding)

Some characters can't appear literally in a URL — they have structural meaning (`/`, `?`, `#`, `&`) or are non-printable. They're encoded as `%XX` where `XX` is the two-hex-digit byte value.

| Character | Encoding |
|---|---|
| Space | `%20` (or `+` in query strings, historical) |
| `%` itself | `%25` |
| `/` | `%2F` (in path segments where you want a literal slash) |
| `?` | `%3F` |
| `#` | `%23` |
| `&` | `%26` |
| `=` | `%3D` |
| `+` | `%2B` |
| Newline (`\n`) | `%0A` |
| `é` (UTF-8 `0xC3 0xA9`) | `%C3%A9` |

Non-ASCII characters are encoded as their **UTF-8 byte sequence**, with each byte percent-encoded individually.

### Reserved vs unreserved

RFC 3986 calls characters "unreserved" if they have no structural meaning: `A-Z a-z 0-9 - . _ ~`. These never need encoding. Everything else may need encoding depending on where it appears.

### Decoder sketch

```cpp
// pseudo-code
std::string percent_decode(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            char hex[3] = { s[i+1], s[i+2], 0 };
            out += static_cast<char>(strtol(hex, NULL, 16));
            i += 2;
        } else if (s[i] == '+') {
            out += ' ';                    // only in query strings, historically
        } else {
            out += s[i];
        }
    }
    return out;
}
```

**Traps:**
- `%XY` where `XY` aren't valid hex → either reject (400) or pass through. Choose one and document.
- `+` → space only in `application/x-www-form-urlencoded` (form bodies and query strings, by convention). In path segments, `+` is literal. **Decoding paths and queries should differ.**
- A `%` followed by fewer than 2 chars → broken; reject.
- After decoding, **re-check for path traversal** (the encoded `..` becomes a literal `..`).

---

## Putting it together — your request handler's URL pipeline

```
incoming request line:   GET /api%2Fv1/users/42?fmt=json HTTP/1.1
                         │   │                  │
                         │   │                  └── query: "fmt=json"
                         │   └── target: "/api%2Fv1/users/42"
                         └── method: "GET"

1. Split target on '?': path = "/api%2Fv1/users/42", query = "fmt=json"
2. Percent-decode path: "/api/v1/users/42"
3. Reject if any segment is "..", contains null bytes, etc.
4. Match against config routes (longest prefix wins).
5. Compute filesystem path: route.root + remainder.
6. Verify the resolved path stays under route.root (anti-traversal).
7. stat() to check if file/directory/missing.
8. Serve / list / 404 / 403 / hand off to CGI / etc.
```

The query string is set aside for step 8 — only CGI consumes it.

---

## Things you don't need

For webserv, you can safely ignore:
- **Userinfo** (`user:pass@`) — pre-HTTP-Basic-Auth artifact. Browsers warn before sending these now.
- **Fragments** (`#bio`) — never reach the server.
- **Internationalised Domain Names** (IDN: `münchen.de` → `xn--mnchen-3ya.de`). Browsers Punycode before sending; you just see ASCII.
- **`absolute-form` request targets** (full URL in the request line) — only proxies see these.

---

## TL;DR mental model

> A URL is `scheme://host:port/path?query#fragment`. Your server gets `/path?query` in the request line plus `host:port` in `Host:`. **Percent-decode** the path, **strip the matched route prefix**, **join with the configured `root`**, and **verify you didn't escape the root** before opening anything. The query string is set aside for CGI; static file serving ignores it.

**Continue to [`09_CONTENT_NEGOTIATION.md`](09_CONTENT_NEGOTIATION.md)** for MIME types and the `Content-Type` header.
