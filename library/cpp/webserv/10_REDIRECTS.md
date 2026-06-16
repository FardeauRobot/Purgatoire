# 10 — Redirects (3xx)

> *"Sorry, that's not here — try over there."*

The 3xx family tells the client to fetch the resource from a different URL. The webserv subject requires you to support **HTTP redirection per route** in the config file. Five 3xx codes matter; the differences between them are subtle and the wrong pick will have search engines do funny things to your site.

---

## The shape of a redirect response

```
HTTP/1.1 302 Found\r\n
Location: https://example.com/new-path\r\n
Content-Length: 0\r\n
Connection: close\r\n
\r\n
```

Three things:
1. A 3xx status code.
2. A **`Location:` header** with the new URL.
3. Optionally a small body explaining the redirect (for clients that don't follow automatically). Most browsers ignore it.

The `Location:` URL can be **absolute** (`https://example.com/new`) or **relative** (`/new`). Relative is interpreted against the original request URL.

---

## The five codes

| Code                         | Meaning                                                            | Permanent? | Method-preserving?   | When to use                                       |
| ---------------------------- | ------------------------------------------------------------------ | ---------- | -------------------- | ------------------------------------------------- |
| **`301 Moved Permanently`**  | This URL is gone forever; use the new one from now on	             | Yes        | ⚠️ kinda (see below) | Site reorgs, switching to a canonical URL         |
| **`302 Found`**              | This URL is temporarily elsewhere; come back here for the original | No         | ⚠️ kinda             | Generic temporary redirect; historical default    |
| **`303 See Other`**          | Use the new URL with `GET`, regardless of original method          | No         | No, **forces GET**   | Post-Redirect-Get pattern after a form submission |
| **`307 Temporary Redirect`** | Like 302 but explicitly preserves the method                       | No         | Yes                  | Modern replacement for 302                        |
| **`308 Permanent Redirect`** | Like 301 but explicitly preserves the method                       | Yes        | Yes                  | Modern replacement for 301                        |

### The "method-preserving" gotcha

Originally (HTTP/1.0), 301 and 302 were defined as preserving the method. In practice, **browsers broke that rule** for 302 (and sometimes 301): if you POST and get a 302, browsers will follow with a GET. This was nonsensical but became de-facto behaviour.

HTTP/1.1 ratified the practical reality by adding:
- **303** — "follow with GET, always" (codifies the broken-302 behaviour)
- **307** — "follow with the same method, always" (codifies the original 302 spec)
- **308** — "follow with the same method, always, and remember it" (permanent 307)

**Modern recommendation:**
- Want permanent redirect, method-preserving? → **`308`**
- Want temporary redirect, method-preserving? → **`307`**
- Want to force a GET after a POST? → **`303`**
- Want a permanent redirect for a GET-only resource (typical for site moves)? → **`301`** still works

For webserv's config-driven redirects, `301` and `302` are almost always sufficient. The config might look like:

```nginx-ish
location /old-path {
    return 301 /new-path;
}
```

---

## Cacheability

| Code | Cacheable by default? |
|---|---|
| 301 | ✅ yes (permanent → cache forever unless `Cache-Control` says otherwise) |
| 302 | ❌ no |
| 303 | ❌ no |
| 307 | ❌ no |
| 308 | ✅ yes |

This is why `301` is so dangerous if used by mistake: browsers and CDNs will **remember the redirect forever** (or until cache TTL expires). Set up `/old-page` to `301`-redirect to `/new-page`, then later want to bring `/old-page` back? Too late — every browser that hit it once is permanently redirected. Use `302` for any redirect that *might* be reverted.

---

## Relative vs absolute URLs in `Location`

Both are valid:

```
Location: https://example.com/new-page    ← absolute
Location: /new-page                       ← relative to host
Location: new-page                        ← relative to request path (rare)
```

For webserv, **prefer absolute** when redirecting between hosts; **relative paths are fine** when redirecting within the same host. The browser handles the resolution either way.

---

## Implementing redirects in webserv

From the subject's config file requirements:

> *"HTTP redirection."*

A minimal config might be:

```nginx-ish
location /old {
    return 301 /new;
}
location /search {
    return 302 https://duckduckgo.com/;
}
```

Pseudo-code handler:

```cpp
// pseudo-code
if (route.redirect.has_value()) {
    int code = route.redirect.code;             // 301, 302, etc.
    std::string url = route.redirect.url;

    std::stringstream s;
    s << "HTTP/1.1 " << code << " " << status_reason(code) << "\r\n";
    s << "Location: " << url << "\r\n";
    s << "Content-Length: 0\r\n";
    s << "Connection: close\r\n";
    s << "\r\n";
    send_response(s.str());
    return;
}
```

**C++98 note:** there's no `std::optional`. Use a sentinel value (`code == 0` means "no redirect") or a boolean flag alongside the redirect struct.

---

## Common bugs

| Bug | Symptom | Fix |
|---|---|---|
| Forgetting the `Location:` header | Client gets a 3xx but doesn't know where to go; usually displays an empty page | Always include `Location:` on 3xx. |
| Using `301` when you might revert | Site visitors get stuck on the old URL forever due to browser cache | Use `302` unless you're 100% sure it's permanent. |
| Relative `Location:` that resolves wrong | `Location: new` from `/old/page` resolves to `/old/new`, not `/new` | Use leading `/` for "from host root" or full absolute URLs. |
| Redirecting POST to GET implicitly via 302 | Form data is lost; the new endpoint expected the body | Use `307` to preserve the method, or document that the redirect is GET-only. |
| Redirect loop | Browser shows "ERR_TOO_MANY_REDIRECTS" | Make sure the destination URL doesn't itself redirect back, or that any chain is finite. |
| Body with `Content-Length: 0` mismatch | Some clients hang | If you send body bytes, set the length correctly; if no body, set `Content-Length: 0` explicitly. |

---

## Browser behaviour

By default, browsers follow up to **20 redirects** in a chain before giving up. Most APIs and `curl` have similar limits. You can usually tweak with flags.

`curl -L` follows redirects (off by default in `curl`). `curl -v -L` shows the full chain — great for debugging your webserv redirect implementation:

```
$ curl -v -L http://localhost:8080/old
> GET /old HTTP/1.1
< HTTP/1.1 301 Moved Permanently
< Location: /new
> GET /new HTTP/1.1
< HTTP/1.1 200 OK
```

---

## TL;DR mental model

> Redirects are a 3xx status + `Location:` header. Use **301** (or 308) for permanent moves, **302** (or 307) for temporary, **303** to force a GET after a POST. **Permanent redirects are cached forever** — be conservative. For webserv: implement via config-driven per-route rules; always send `Location:` and `Content-Length: 0`.

**Continue to [`11_CACHING.md`](11_CACHING.md)** for how to *avoid* sending bytes that the client already has.
