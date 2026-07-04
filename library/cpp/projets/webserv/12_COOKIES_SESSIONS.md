# 12 — Cookies & Sessions

> *"A stateless protocol gets state by asking the client to remember things on its behalf."*

HTTP forgets you between requests. Cookies are how it pretends not to.

**Status in webserv:** the subject lists *"Support cookies and session management (provide simple examples)"* in the **Bonus part**. Mandatory only if you're going for full marks. Even if you skip implementation, your server will receive `Cookie:` headers from browsers and must not crash on them.

---

## What a cookie is

A small key=value pair that the server **asks the browser to store**, and that the browser **echoes back** on every subsequent request to the same domain.

### The flow

```
Request 1:  client → server                          (no cookies yet)
            GET /login HTTP/1.1
            Host: example.com

Response 1: server → client                          (set a cookie)
            HTTP/1.1 200 OK
            Set-Cookie: sessionid=abc123; Path=/; HttpOnly; Max-Age=3600

Request 2:  client → server                          (echoes the cookie)
            GET /dashboard HTTP/1.1
            Host: example.com
            Cookie: sessionid=abc123
```

The server never tells the client what `sessionid=abc123` means. It's an **opaque token** the server uses to look up state in its own database/memory: "session `abc123` belongs to user Alice, last activity 2 minutes ago, cart contains 3 items."

---

## `Set-Cookie` — server sets a cookie

Sent by the server in a response header:

```
Set-Cookie: name=value; attribute1; attribute2=value
```

You may send multiple `Set-Cookie` headers to set multiple cookies in one response.

### Common attributes

| Attribute | What it does |
|---|---|
| `Expires=Wed, 01 Jun 2026 12:00:00 GMT` | Cookie expires at this absolute date |
| `Max-Age=3600` | Cookie expires N seconds from now (preferred over Expires; no clock-skew issues) |
| `Domain=example.com` | Cookie sent to this domain and subdomains (by default, only the exact host) |
| `Path=/api` | Cookie only sent on URLs under this path (by default, the path of the response) |
| `Secure` | Only sent over HTTPS connections |
| `HttpOnly` | JavaScript on the client cannot read the cookie (defence against XSS) |
| `SameSite=Strict` | Only sent on same-site requests (defence against CSRF) |
| `SameSite=Lax` | Sent on same-site requests + top-level GET navigation from other sites |
| `SameSite=None; Secure` | Sent on all cross-site requests; **requires `Secure`** |

Example with sensible defaults for a session cookie:

```
Set-Cookie: sessionid=abc123; Path=/; Max-Age=3600; HttpOnly; SameSite=Lax
```

### Deletion

To delete a cookie: send `Set-Cookie` with `Max-Age=0` (or a past `Expires`):

```
Set-Cookie: sessionid=; Path=/; Max-Age=0
```

The browser will remove it from storage.

---

## `Cookie` — client echoes cookies back

Sent by the client in subsequent requests:

```
Cookie: sessionid=abc123; theme=dark; lang=en
```

Format: `name=value` pairs separated by `; ` (semicolon-space). No attributes — the client doesn't echo `Path`, `Domain`, etc. (those are stored locally and applied by the client to decide *when* to send the cookie).

### Parsing a Cookie header

```cpp
// pseudo-code
std::map<std::string, std::string> parse_cookies(const std::string& header) {
    std::map<std::string, std::string> cookies;
    size_t pos = 0;
    while (pos < header.size()) {
        size_t semi = header.find(';', pos);
        if (semi == std::string::npos) semi = header.size();

        std::string pair = header.substr(pos, semi - pos);
        // Trim leading whitespace
        size_t start = pair.find_first_not_of(' ');
        if (start != std::string::npos) pair = pair.substr(start);

        size_t eq = pair.find('=');
        if (eq != std::string::npos) {
            cookies[pair.substr(0, eq)] = pair.substr(eq + 1);
        }
        pos = semi + 1;
    }
    return cookies;
}
```

---

## Sessions — what cookies actually enable

A **session** is server-side state keyed by a cookie value. The architecture:

```
1. Client logs in.
2. Server generates a random session ID (e.g. a 32-char hex string).
3. Server stores in memory or a database:
       sessions["abc123"] = { user_id: 42, expires: 1748800000, … }
4. Server sends Set-Cookie: sessionid=abc123 to the client.
5. On every subsequent request, the client sends Cookie: sessionid=abc123.
6. Server looks up sessions["abc123"], finds user_id=42, processes the request as that user.
7. After the session expires (or on logout), the server removes the entry and tells the client to delete the cookie.
```

The session ID needs to be:
- **Unpredictable** (cryptographically random — `/dev/urandom` is fine).
- **Long enough** (~16 bytes of entropy minimum, i.e. 32 hex chars).
- **Stored server-side** (the client doesn't get to see the user's data; only the opaque ID).

### A minimal session store for webserv (bonus)

```cpp
// pseudo-code
struct SessionData {
    std::string user;
    time_t      expires;
    std::map<std::string, std::string> data;
};

std::map<std::string, SessionData> g_sessions;

std::string create_session(const std::string& user) {
    std::string id = random_hex(32);            // read /dev/urandom
    SessionData s;
    s.user    = user;
    s.expires = time(NULL) + 3600;
    g_sessions[id] = s;
    return id;
}

// On each request:
SessionData* get_session(const Request& req) {
    if (!req.headers.count("cookie")) return NULL;
    std::map<std::string, std::string> cookies =
        parse_cookies(req.headers["cookie"]);
    if (!cookies.count("sessionid")) return NULL;

    std::map<std::string, SessionData>::iterator it =
        g_sessions.find(cookies["sessionid"]);
    if (it == g_sessions.end()) return NULL;
    if (it->second.expires < time(NULL)) {
        g_sessions.erase(it);
        return NULL;
    }
    return &it->second;
}
```

That's the whole session system in ~50 lines. Sweep `g_sessions` periodically to remove expired entries (memory leak otherwise).

---

## Cookies in CGI

When the route maps to a CGI script, you should:
- Pass the request's `Cookie` header to the CGI via `HTTP_COOKIE` environment variable.
- Capture any `Set-Cookie` headers the CGI emits in its response and forward them.

This delegates the entire session concept to the CGI script. PHP and Python both have built-in session frameworks that just need this plumbing.

---

## Security notes (relevant beyond webserv)

- **Always use `HttpOnly`** for session cookies — prevents JavaScript from reading them, mitigating XSS.
- **Always use `Secure`** if you're on HTTPS — prevents the cookie from leaking on accidental HTTP requests.
- **Use `SameSite=Lax`** as a baseline — mitigates CSRF for state-changing requests.
- **Don't put sensitive data in the cookie value itself** — only the opaque session ID. The user's email, balance, role belong in your server-side store.
- **Regenerate the session ID on privilege escalation** (e.g. after login) — prevents session fixation.

For webserv: these notes are for context. The subject only asks for *"simple examples"* in the bonus.

---

## Alternatives to session cookies

| Mechanism | How it works | Why use it |
|---|---|---|
| **Session cookie + server store** (what we covered) | Opaque ID + server-side lookup | Simple, secure, easy to invalidate |
| **JWT (JSON Web Token)** | Self-contained signed token in the cookie or `Authorization` header | Stateless server — no store needed |
| **Authorization: Basic** | Username:password base64-encoded, sent every request | Simple but no logout, no expiry |
| **OAuth / OpenID Connect** | Token issued by an auth server | Cross-site SSO |

For webserv bonus, **session cookie + server store** is the right pick. Everything else is way out of scope.

---

## Common bugs

| Bug | Symptom | Fix |
|---|---|---|
| Server forgets a session because of restart | Users get logged out whenever you redeploy | Persist sessions (file, sqlite). Or accept it for webserv — restart is rare. |
| Cookie not sent because of `Path` mismatch | "Why doesn't my session work after a redirect?" | Set `Path=/` for session cookies. |
| Cookie not sent because of `Secure` on plain HTTP | Browser ignores cookie when testing on `http://localhost` | Drop `Secure` for local dev; add it back for production. |
| `Set-Cookie` per-request leaks (creating new session on every visit) | Session ID changes constantly | Only call `Set-Cookie` when actually starting/refreshing a session, not on every response. |
| Session never expires | Memory grows unbounded | Sweep periodically. |

---

## TL;DR mental model

> A cookie is a key-value pair the server asks the client to store and echo back. A session is server-side state keyed by a (random, opaque) cookie value. **Set with `Set-Cookie`, read from `Cookie`.** For webserv: cookies are bonus only; even if you skip them, parse the `Cookie` header politely (don't crash on it) and pass it through to CGI.

**Continue to [`13_HTTPS_TLS.md`](13_HTTPS_TLS.md)** — what HTTPS adds on top.
