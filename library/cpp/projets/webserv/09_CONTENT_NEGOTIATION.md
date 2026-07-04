# 09 — Content Types & Negotiation

> *"The body is bytes. The Content-Type is how the client knows what to do with them."*

A `.png` file looks identical to a `.txt` file at the byte level if you blur your eyes — both are sequences of `unsigned char`. The `Content-Type` header is what tells the browser "render this as an image" vs "render this as text." Get it wrong and your CSS arrives as plain text, your images as nothing, your JavaScript as an HTML page.

---

## What MIME types are

**MIME** stands for *Multipurpose Internet Mail Extensions* — they were invented for email and re-used by HTTP. Format:

```
type/subtype
type/subtype; parameter=value
type/subtype; parameter=value; parameter=value
```

Examples:

```
text/html
text/html; charset=utf-8
application/json
multipart/form-data; boundary=----WebKitFormBoundaryX
image/png
```

The standard list is maintained by IANA and runs to thousands. You need ~15.

---

## The MIME types webserv must serve

A minimum table for static file serving:

| Extension | MIME type | Notes |
|---|---|---|
| `.html`, `.htm` | `text/html; charset=utf-8` | Always include charset for text types. |
| `.css` | `text/css` | |
| `.js`, `.mjs` | `application/javascript` | Some servers say `text/javascript`; both work. |
| `.json` | `application/json` | |
| `.xml` | `application/xml` | |
| `.txt`, `.md` | `text/plain; charset=utf-8` | |
| `.png` | `image/png` | |
| `.jpg`, `.jpeg` | `image/jpeg` | |
| `.gif` | `image/gif` | |
| `.svg` | `image/svg+xml` | |
| `.webp` | `image/webp` | |
| `.ico` | `image/x-icon` | Browsers request `/favicon.ico` by default. |
| `.pdf` | `application/pdf` | |
| `.zip` | `application/zip` | |
| `.mp4` | `video/mp4` | |
| `.mp3` | `audio/mpeg` | |
| anything else | `application/octet-stream` | "Unknown binary blob" — browser will offer to download. |

### Why `application/octet-stream` is the safe default

If you don't recognise the extension, send `application/octet-stream`. The browser will either save it as a download or refuse to render it inline — both safe outcomes. The wrong move is to guess `text/html` and have a binary file rendered as garbled text.

---

## Charset — when to include

For **text** types (`text/html`, `text/css`, `text/plain`, `text/javascript`), include `; charset=utf-8`. Without it, browsers may guess wrong and render `é` as `Ã©`.

For binary types (`image/*`, `application/*`), charset is meaningless — omit it.

For `application/json`, the spec mandates UTF-8 implicitly; you don't strictly need `charset=utf-8`, but it doesn't hurt.

---

## Implementing a MIME lookup table

A `std::map<std::string, std::string>` from extension to type:

```cpp
// pseudo-code
std::map<std::string, std::string> build_mime_table() {
    std::map<std::string, std::string> m;
    m[".html"] = "text/html; charset=utf-8";
    m[".css"]  = "text/css";
    m[".js"]   = "application/javascript";
    m[".json"] = "application/json";
    m[".png"]  = "image/png";
    m[".jpg"]  = "image/jpeg";
    m[".jpeg"] = "image/jpeg";
    m[".gif"]  = "image/gif";
    m[".txt"]  = "text/plain; charset=utf-8";
    m[".pdf"]  = "application/pdf";
    return m;
}

std::string mime_for(const std::string& path) {
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos) return "application/octet-stream";

    std::string ext = path.substr(dot);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    static const std::map<std::string, std::string> table = build_mime_table();
    std::map<std::string, std::string>::const_iterator it = table.find(ext);
    return it == table.end() ? "application/octet-stream" : it->second;
}
```

**C++98 trap:** `static const std::map<…>` initialised by a function call is fine in C++98 (`static` initialisation happens once). But you can't use brace-init lists or `std::initializer_list` to populate it — that's C++11. Hence the explicit `m[...] = ...;` calls.

---

## Content negotiation — the `Accept` header

Clients can express preferences via the `Accept` header:

```
Accept: text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8
```

Read as: "I prefer HTML, then XHTML, then XML (weight 0.9), then anything (weight 0.8)."

A negotiating server would pick the highest-weighted type it can produce. **For webserv, you don't negotiate** — the file's extension determines the type. Just **ignore `Accept`** for static serving and pass it through to CGI via `HTTP_ACCEPT`.

The same applies to `Accept-Language`, `Accept-Encoding`, `Accept-Charset` — ignore for static, pass through for CGI.

---

## Request body content types — what `POST` brings

When a client `POST`s data, the body's `Content-Type` tells you the format:

### `application/x-www-form-urlencoded`

The default for HTML `<form>` submissions:

```
POST /login HTTP/1.1\r\n
Content-Type: application/x-www-form-urlencoded\r\n
Content-Length: 31\r\n
\r\n
username=alice&password=hunter2
```

Same format as a URL query string. Decode with the query-string parser from [file 08](08_URLS.md).

### `multipart/form-data`

Used for file uploads. The body is segmented by a **boundary string**:

```
POST /upload HTTP/1.1\r\n
Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryABCDE\r\n
Content-Length: 384\r\n
\r\n
------WebKitFormBoundaryABCDE\r\n
Content-Disposition: form-data; name="username"\r\n
\r\n
alice\r\n
------WebKitFormBoundaryABCDE\r\n
Content-Disposition: form-data; name="file"; filename="photo.jpg"\r\n
Content-Type: image/jpeg\r\n
\r\n
<binary JPEG bytes>\r\n
------WebKitFormBoundaryABCDE--\r\n
```

Structure:
- The `boundary` parameter in `Content-Type` is the separator string (with `--` prefix on each occurrence).
- Each part has its own headers (especially `Content-Disposition` with `name="..."` and optionally `filename="..."`) and then its data.
- The final boundary has a trailing `--` (`------WebKitFormBoundaryABCDE--`).

**For webserv:** you'll need a multipart parser if the subject's "client must be able to upload files" requires `multipart/form-data` (the standard browser form mechanism). Plan: split body on boundary, parse each part's headers, extract filename and content, write to disk.

This is the gnarliest parsing task in webserv. Test it against actual browser uploads.

### `application/json`

```
POST /api HTTP/1.1\r\n
Content-Type: application/json\r\n
Content-Length: 24\r\n
\r\n
{"key":"value","n":42}
```

For webserv: **don't parse JSON yourself**. Hand the body to CGI; the CGI script (Python, PHP) parses it. C++98 has no JSON parser in the standard library, and writing one from scratch is way out of webserv's scope.

### `text/plain`

```
POST /note HTTP/1.1\r\n
Content-Type: text/plain; charset=utf-8\r\n
Content-Length: 27\r\n
\r\n
Just a plain text body here
```

Easy. Body is raw text. Useful for CGI testing.

### `application/octet-stream` or unrecognised

Treat as raw bytes. Pass through to CGI as-is.

---

## The `Vary` header (advanced — skip for webserv)

If a server returns different responses for the same URL based on a request header (e.g. different content for different `Accept-Language`), it should send `Vary: Accept-Language` so caches don't serve the wrong cached version.

Webserv doesn't negotiate, so you don't need `Vary`. Skip.

---

## Common bugs

| Bug | Symptom | Fix |
|---|---|---|
| Sending `text/plain` for HTML files | Browser displays the HTML source as text instead of rendering | Look up the correct MIME by extension. |
| Forgetting `charset=utf-8` | French/Japanese characters render as mojibake | Append `; charset=utf-8` to text/* types. |
| Sending `text/html` for unknown extensions | Browser tries to render binary garbage | Default to `application/octet-stream`. |
| Multipart parser splits on `boundary` literally | Misses real boundaries because the spec adds `--` prefix | Search for `--` + boundary, not just boundary. |
| Forgetting to set `Content-Type` at all | Browser guesses (sniffs) the type; sometimes wrong, sometimes unsafe | Always include `Content-Type` on responses with bodies. |

---

## TL;DR mental model

> The `Content-Type` is a label that tells the receiver how to interpret the body bytes. For webserv: a small extension-to-MIME table covers static serving; multipart parsing is needed for file uploads; everything else can be passed through to CGI verbatim. **Ignore `Accept` for static files; pass it to CGI.**

**Continue to [`10_REDIRECTS.md`](10_REDIRECTS.md)** for the 3xx family in detail.
