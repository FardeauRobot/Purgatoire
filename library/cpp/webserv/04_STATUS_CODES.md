# 04 — Status Codes

> *"The status code is the server's one-sentence summary of what just happened."*

Three digits in the response's status line. Picked correctly, the client knows how to react without even reading the body. Picked sloppily, your server is technically working but semantically broken — and the webserv subject explicitly says **"Your HTTP response status codes must be accurate."**

---

## The five families

| Range | Family | Meaning |
|---|---|---|
| `1xx` | Informational | "Request received, processing continues." Rare in webserv. |
| `2xx` | Success | "It worked." |
| `3xx` | Redirection | "Look somewhere else." |
| `4xx` | Client error | "Your request is broken — don't retry as-is." |
| `5xx` | Server error | "I broke. Retry might work." |

The first digit tells you which way to point fingers. Internalise this much and you can guess most status codes from context.

---

## The codes you'll actually use

The HTTP spec lists ~60 codes. Webserv needs maybe 15. Here's the shortlist:

### 2xx — success

| Code                 | Reason                                                                                  | When                                              |
| -------------------- | --------------------------------------------------------------------------------------- | ------------------------------------------------- |
| **`200 OK`**         | The default success. `GET` worked, body contains the resource.                          | Most successful responses.                        |
| **`201 Created`**    | Resource was created. Should include a `Location:` header pointing to the new resource. | After a successful `POST` that creates something. |
| **`204 No Content`** | Success but no body. Headers still terminate with `\r\n\r\n`, but no bytes after.       | `DELETE` that succeeded; `PUT` that updated.      |

### 3xx — redirection

See [file 10](10_REDIRECTS.md) for the full table.

| Code                        | Reason                                                                                                                  | When                                                                            |
| --------------------------- | ----------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------- |
| **`301 Moved Permanently`** | Resource is now at the URL in `Location:`. Browsers will update bookmarks, search engines will replace the index entry. | Config file's `redirect` rule, when permanent.                                  |
| **`302 Found`**             | Temporary redirect. Client should re-request at the new URL but not memorise the move.                                  | Most `redirect` config rules. (`303` and `307` are more precise — see file 10.) |
| **`304 Not Modified`**      | "Your cached copy is still valid." No body.                                                                             | When implementing caching ([file 11](11_CACHING.md)). Not required by webserv.  |

### 4xx — client error

| Code | Reason | When |
|---|---|---|
| **`400 Bad Request`** | Malformed request — your parser couldn't make sense of it. | Missing `Host` header on HTTP/1.1, broken chunked encoding, line not terminated by `\r\n`, etc. |
| **`403 Forbidden`** | You understood the request, you found the resource, but you refuse to serve it. | File exists but permissions deny read; directory listing disabled and no index file. |
| **`404 Not Found`** | The resource does not exist. | The target after applying `root` doesn't map to a real file. |
| **`405 Method Not Allowed`** | Method known but not allowed on this route. **Must include `Allow:` header listing what is allowed.** | Route config says `methods: GET POST`, client sent `DELETE`. |
| **`408 Request Timeout`** | Client opened connection but took too long to send. | Idle connection cleanup. Not strictly required by subject but good for resilience. |
| **`411 Length Required`** | Body present but no `Content-Length`. | Optional, since you can also accept chunked. Some servers enforce this strictly. |
| **`413 Payload Too Large`** | Request body exceeds your `client_max_body_size`. **Configurable via the subject's config file.** | Reading body exceeds the limit. |
| **`414 URI Too Long`** | The path/query is bigger than you'll parse. | Optional protection. |
| **`415 Unsupported Media Type`** | Body's `Content-Type` is one you can't handle. | Rare in webserv. |
| **`431 Request Header Fields Too Large`** | Header section bigger than your buffer. | Optional protection. |

### 5xx — server error

| Code | Reason | When |
|---|---|---|
| **`500 Internal Server Error`** | Generic catch-all when something went wrong on your side. | CGI crashed, file system error you didn't anticipate. |
| **`501 Not Implemented`** | The method is unknown to your server. | Client sent `PROPFIND` or some nonsense — you don't recognise it. |
| **`502 Bad Gateway`** | A CGI you spawned crashed or returned garbage. | CGI returned non-HTTP output, or no output at all. |
| **`503 Service Unavailable`** | You're overloaded or about to shut down. | Optional. |
| **`504 Gateway Timeout`** | CGI took too long. | Required if you implement CGI timeouts. |
| **`505 HTTP Version Not Supported`** | Request used `HTTP/2.0` or some version you don't speak. | Polite to send instead of just disconnecting. |

---

## Picking the right one — decision tree

```
Did the client's request make sense as bytes?
├── No → 400 Bad Request
└── Yes
    │
    Was the method recognised?
    ├── No → 501 Not Implemented
    └── Yes
        │
        Is the method allowed on this route?
        ├── No → 405 Method Not Allowed (with Allow: header)
        └── Yes
            │
            Does the resource exist?
            ├── No  → 404 Not Found
            └── Yes
                │
                Are you allowed to access it?
                ├── No  → 403 Forbidden
                └── Yes
                    │
                    Did everything below succeed?
                    ├── No  → 500 / 502 / 504
                    └── Yes → 200 / 201 / 204 / 3xx
```

This tree handles 95% of your status code decisions.

---

## Reason phrases — what to put after the number

The standard phrases are:

| Code | Phrase |
|---|---|
| 200 | `OK` |
| 201 | `Created` |
| 204 | `No Content` |
| 301 | `Moved Permanently` |
| 302 | `Found` |
| 400 | `Bad Request` |
| 403 | `Forbidden` |
| 404 | `Not Found` |
| 405 | `Method Not Allowed` |
| 413 | `Payload Too Large` |
| 500 | `Internal Server Error` |
| 501 | `Not Implemented` |
| 502 | `Bad Gateway` |
| 505 | `HTTP Version Not Supported` |

> Strictly, clients should ignore the wording and rely only on the number. In practice: stick to the standard phrases. Some grep-based tooling matches on them.

A C++98 lookup function:

```cpp
const char* status_reason(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 505: return "HTTP Version Not Supported";
        default:  return "Unknown";
    }
}
```

This will be one of the first functions you write. Promote it to a header so the rest of the codebase can reach it.

---

## Error pages — what the subject demands

The subject says: *"Your server must have default error pages if none are provided."*

Implementation:

1. The config file allows the user to specify error pages per status code:
   ```
   error_page 404 /errors/404.html;
   error_page 500 502 503 504 /errors/5xx.html;
   ```
2. When you need to send an error, check the config: is a custom page configured? If yes, read and send it (with the **error's** status code, not 200).
3. If no custom page is configured, send a built-in default — a tiny HTML body like:
   ```html
   <html><head><title>404 Not Found</title></head>
   <body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>
   ```

Build a helper that generates these on the fly so you don't need 10 default-page files in your repo.

---

## Common bugs around status codes

| Bug | Fix |
|---|---|
| Sending `200 OK` with an HTML body that *says* "404 Not Found" | The status line is what matters. Set the actual status code. |
| Forgetting the `Allow:` header on a 405 | Always include `Allow: GET, POST, DELETE` (or whatever the route allows). |
| Returning 500 for client mistakes | If the request was malformed, that's 400. If they asked for something that doesn't exist, that's 404. 500 means *your code* failed. |
| Returning 200 with empty body when you mean "deleted" | Use 204 No Content — and make sure you don't send any body bytes. |
| Returning 404 when the *route* doesn't exist | Same code, fine. 404 covers both "no route" and "route exists but file missing." |
| Returning 405 when the *route* doesn't exist | Wrong — 405 implies the resource exists, just not via this method. Use 404. |

---

## TL;DR mental model

> First digit = blame assignment. Second & third digits = specificity. **Pick the most specific code that is true.** 200 is the default success; 400/404/405/500 cover most error cases. Always send a body with errors (default page or configured one) — clients display it to the user.

**Continue to [`05_HEADERS.md`](05_HEADERS.md)** for the catalogue of headers that carry the actual semantics.
