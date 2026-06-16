# 02 — Message Anatomy

> *"A request and a response are the same shape with different first lines."*

Every HTTP message — request or response — has the **same four-part structure**. Memorise this once and you've memorised half the protocol.

---

## The universal shape

```
┌────────────────────────────────────────────┐
│  start-line          (one line, CRLF)      │   ← differs request vs response
├────────────────────────────────────────────┤
│  header-1: value-1   (one line, CRLF)      │
│  header-2: value-2   (one line, CRLF)      │   ← zero or more headers
│  ...                                       │
├────────────────────────────────────────────┤
│  (blank line)        (just CRLF)           │   ← MANDATORY separator
├────────────────────────────────────────────┤
│  body (optional, may be binary)            │
│  ...                                       │
└────────────────────────────────────────────┘
```

Four parts. **`\r\n`** ends every line — including the blank line that ends the header section.

---

## Request

### Start-line — the **request line**

```
METHOD  SP  request-target  SP  HTTP-version  CRLF
```

Example:
```
GET /api/users/42?fields=name HTTP/1.1\r\n
```

| Field | Notes |
|---|---|
| `METHOD` | Uppercase. `GET`, `POST`, `PUT`, `DELETE`, `HEAD`, `OPTIONS`, etc. See [file 03](03_METHODS.md). |
| `request-target` | Usually the path-and-query portion of a URL (`/api/users/42?fields=name`). Other forms exist (absolute URI for proxies, `*` for `OPTIONS`) but you can ignore them for webserv. |
| `HTTP-version` | `HTTP/1.0` or `HTTP/1.1` — capital letters, slash, two digits. |
| `SP` | A single space (`0x20`). |
| `CRLF` | `\r\n` (`0x0D 0x0A`). |

### Headers

Zero or more lines of the form:

```
Header-Name: header-value\r\n
```

**Rules that matter for parsing:**

- Header **names are case-insensitive** (`Content-Length`, `content-length`, `CONTENT-LENGTH` all mean the same thing). Your parser should lowercase-normalise keys.
- Values can have leading whitespace after the colon that should be trimmed.
- Multiple values for the same header may appear either as **multiple lines** or as a **comma-separated list** on one line. Both are valid.
- A header value can be folded across multiple lines (a continuation line begins with whitespace). **Obsoleted in modern HTTP/1.1** — RFC 7230 says senders MUST NOT generate them. For webserv, you can reject any folded header with a 400.
- Some headers are mandatory under certain circumstances (e.g., `Host` for HTTP/1.1 requests).

### Blank line

A single `\r\n`. **Mandatory.** Marks end of headers.

### Body

Present on some methods (`POST`, `PUT`), absent on most others. **The protocol does not include "body length" in the request line — you learn it from headers** (`Content-Length` or `Transfer-Encoding: chunked`). See [file 06](06_FRAMING.md).

### Worked example — request

```
POST /api/login HTTP/1.1\r\n
Host: example.com\r\n
Content-Type: application/json\r\n
Content-Length: 38\r\n
\r\n
{"user":"alice","password":"hunter2"}
```

Parser's job:
1. Read line 1: split on spaces → method `POST`, target `/api/login`, version `HTTP/1.1`.
2. Read lines 2–4: split on first `:` → store as key/value pairs.
3. Read line 5: blank → header section done.
4. Look up `Content-Length: 38` → read exactly 38 more bytes into the body.

---

## Response

### Start-line — the **status line**

```
HTTP-version  SP  status-code  SP  reason-phrase  CRLF
```

Example:
```
HTTP/1.1 404 Not Found\r\n
```

| Field           | Notes                                                                                                                                                                                          |
| --------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `HTTP-version`  | Same as request — `HTTP/1.1` is what you should send.                                                                                                                                          |
| `status-code`   | 3-digit number. See [file 04](04_STATUS_CODES.md).                                                                                                                                             |
| `reason-phrase` | Human-readable text. Clients should ignore the exact wording, but include a sensible one. The standard reasons (`OK`, `Not Found`, `Internal Server Error`, …) are conventional, not enforced. |

### Headers, blank line, body

Identical to request — same four-part structure. The body is whatever you're sending back: HTML, JSON, an image, an error page.

### Worked example — response

```
HTTP/1.1 200 OK\r\n
Content-Type: text/html; charset=utf-8\r\n
Content-Length: 53\r\n
Connection: keep-alive\r\n
\r\n
<!DOCTYPE html><html><body>Hello world</body></html>
```

---

## Parser sketch (for webserv)

Pseudo-code for the request parser you'll write:

```
read_until("\r\n")            → request_line
parse_request_line(request_line) → {method, target, version}

headers = {}
loop:
    line = read_until("\r\n")
    if line is empty: break        # blank line → headers done
    key, value = split_on_first(line, ':')
    headers[lowercase(key)] = trim(value)

if "content-length" in headers:
    body = read_exactly(int(headers["content-length"]))
elif headers.get("transfer-encoding") == "chunked":
    body = read_chunked()           # see file 06
else:
    body = ""                       # GET, DELETE, etc.

dispatch(method, target, headers, body)
```

This is your parser in three lines of logic plus a chunked-decoder. **Spend your time getting it right** — every other piece of webserv either feeds this parser or consumes its output.

---

## Common pitfalls

| Pitfall | Symptom | Fix |
|---|---|---|
| Treating `\n` as line terminator | Browser requests parse fine; some `curl` requests don't; the next request appears to "leak" into the previous one's body | Read exactly `\r\n`. Reject lines ending in lone `\n` (or accept both if you want to be lenient). |
| Storing headers case-sensitively | `Content-Length` works, `content-length` doesn't, depending on which client you tested with | Lowercase the key on insertion **and** on lookup. |
| Reading too much body | The next request gets misparsed because you swallowed its first line | Respect `Content-Length` exactly. Never read more. |
| Reading too little body | Hanging on `read` waiting for more bytes that never come | Loop until you've read `Content-Length` bytes total, accounting for partial reads. |
| Forgetting the blank line in your response | Clients hang waiting for the body to start | Always emit `\r\n` between headers and body, even if body is empty. |
| No `Content-Length` in a response with a body | Client reads until connection closes — works if you close, breaks keep-alive | Always include `Content-Length` (or `Transfer-Encoding: chunked`) on responses with bodies. |

---

## C++98 tools for parsing

You'll be doing a lot of string manipulation. The C++98 standard library gives you:

| Need | Tool |
|---|---|
| Split a string on a delimiter | `std::string::find` + `std::string::substr` in a loop |
| Trim whitespace | Custom — find first/last non-whitespace, `substr` between |
| Convert to lowercase | `std::transform(s.begin(), s.end(), s.begin(), ::tolower)` |
| String → int (for `Content-Length`) | `std::stringstream` + `>>` (C++98 has no `std::stoi`) |
| Case-insensitive comparison | Lowercase both then compare — there's no `strcasecmp` in C++ standard |
| Read line from socket buffer | You implement this. The socket isn't a `std::istream`. |

**Trap:** `std::stringstream` succeeds even on invalid input (returns 0 silently if the stream extraction fails). Always check `ss.fail()` or `ss.eof()` after extraction.

---

## TL;DR mental model

> Both messages are: **one start-line + zero or more `Key: Value` header lines + blank line + optional body bytes**. The start line is what distinguishes a request from a response. Everything below is structurally identical.

**Continue to [`03_METHODS.md`](03_METHODS.md)** to learn what those methods on the request line actually *mean*.
