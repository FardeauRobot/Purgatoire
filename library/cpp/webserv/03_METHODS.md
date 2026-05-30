# 03 — HTTP Methods

> *"The method is a verb. The path is a noun. Together they form the sentence the client is asking the server to act on."*

The first token of every request line. HTTP defines a handful of methods; webserv requires you to support **at least `GET`, `POST`, `DELETE`** — but you should know what the others mean for the parsing layer (you'll reject them gracefully).

---

## The methods at a glance

| Method | Meaning | Safe | Idempotent | Cacheable | Webserv |
|---|---|---|---|---|---|
| `GET` | Fetch a representation of a resource | ✅ | ✅ | ✅ | **mandatory** |
| `HEAD` | Like `GET` but no body in response | ✅ | ✅ | ✅ | optional, easy |
| `POST` | Submit data — server decides what to do with it | ❌ | ❌ | ⚠️ rare | **mandatory** |
| `PUT` | Replace the resource at a given URL | ❌ | ✅ | ❌ | not required |
| `DELETE` | Remove the resource at a given URL | ❌ | ✅ | ❌ | **mandatory** |
| `PATCH` | Partial modification | ❌ | ❌ | ❌ | not required |
| `OPTIONS` | Ask what methods are allowed | ✅ | ✅ | ❌ | not required |
| `CONNECT` | Tunnel TCP through a proxy | ❌ | ❌ | ❌ | ignore |
| `TRACE` | Echo the request back | ✅ | ✅ | ❌ | ignore |

Three vocabulary words you need to understand the table:

---

## Safe

A method is **safe** if it is *intended* to be read-only — i.e. invoking it should not change server state. Web crawlers can hit safe methods freely.

- `GET`, `HEAD`, `OPTIONS`, `TRACE` are safe.
- `POST`, `PUT`, `DELETE`, `PATCH` are unsafe (they *do* things).

> ⚠️ "Safe" is a **promise about intent**, not a guarantee. A `GET /users/42/delete` URL is *technically* a GET but is **not safe** — and is widely considered an antipattern for exactly this reason. The HTTP spec assumes safe methods have no side effects; if you violate that, web crawlers will delete your data for you.

---

## Idempotent

A method is **idempotent** if making the same request **N times** has the same effect as making it **once**.

- `GET`, `HEAD`, `PUT`, `DELETE`, `OPTIONS`, `TRACE` are idempotent.
- `POST`, `PATCH` are not.

**Why this matters for clients:** if a request fails (network glitch, timeout), the client can safely **retry** an idempotent request. It can't safely retry a `POST` — the first one might have gone through, and the retry would create two records.

**Examples:**
- `DELETE /users/42` is idempotent: after the first call, the user is gone. A second call returns 404 (or 204) — the *state* is the same.
- `PUT /users/42` with a body is idempotent: you're overwriting with the same content each time.
- `POST /users` with a body is **not** idempotent: each call creates a new user.

---

## Cacheable

A method is **cacheable** if the response is allowed to be stored by intermediaries (browsers, proxies, CDNs) and reused for later requests.

- `GET`, `HEAD` are always cacheable (subject to response headers — see [file 11](11_CACHING.md)).
- `POST` is cacheable in theory but almost never in practice.
- `PUT`, `DELETE`, `PATCH` are not cacheable.

---

## The methods in detail

### `GET`

**"Give me the representation of the resource at this URL."**

- No request body (servers should ignore one if it appears).
- Response body contains the representation (HTML, JSON, image, ...).
- Should not change server state.

For webserv: look up the file on disk (after applying the config's `root` rule), set `Content-Type` from the extension ([file 09](09_CONTENT_NEGOTIATION.md)), set `Content-Length`, send.

### `HEAD`

**"Give me the response you'd give to `GET` of this URL, but without the body."**

- Same headers as the equivalent `GET`, but no body.
- Useful for clients checking metadata: does this URL exist? How big is it? When was it last modified?

For webserv: implement by running your GET handler and just not sending the body. Free feature — five extra lines of code.

### `POST`

**"Here's some data. Do something with it. The 'something' is your call."**

- Has a request body (usually).
- Server may create a resource, run a CGI, append to a log, send an email — anything.
- Response should indicate what happened (`201 Created` with `Location:` header pointing to the new resource, or `200 OK` with the result, or `204 No Content`).

For webserv: routes the request to a file uploader, a CGI, or (rarely) a static-content endpoint. The body's `Content-Type` matters:
- `application/x-www-form-urlencoded` — form fields, URL-encoded
- `multipart/form-data` — file uploads
- `application/json`, `text/plain`, etc. — passed to CGI as-is

### `DELETE`

**"Remove the resource at this URL."**

- Typically no request body.
- Response: `200 OK` with confirmation, `204 No Content` if nothing to say, or `404` if it wasn't there.

For webserv: `unlink(2)` the file under the route's `root` (or return 405 / 403 depending on config and permissions). Subject says "you need at least... DELETE methods" — implement it for files, that's enough.

### `PUT`

**"Replace the resource at this URL with this body."**

Not required by the subject. But conceptually: take the body, write it to the disk path. Differs from `POST` in that the client picks the URL (`PUT /files/foo.txt`) vs. the server picking it (`POST /files` → server names it).

### `OPTIONS`

**"What methods are allowed on this URL?"**

Response includes an `Allow:` header listing methods. Used heavily for CORS preflight requests in browser apps. Not required by webserv, but if you implement it, just return `Allow: GET, POST, DELETE` (or whatever the route config allows).

---

## Method dispatch in webserv

The config file gives you per-route allowed methods. Pseudo-code:

```cpp
// pseudo-code, not literal
const RouteConfig& route = config.match(request.target);

if (std::find(route.allowed_methods.begin(),
              route.allowed_methods.end(),
              request.method) == route.allowed_methods.end()) {
    return make_response(405, "Method Not Allowed",
                         /* Allow: */ join(route.allowed_methods, ", "));
}

if (request.method == "GET" || request.method == "HEAD") {
    handle_get(request, route);
} else if (request.method == "POST") {
    handle_post(request, route);
} else if (request.method == "DELETE") {
    handle_delete(request, route);
} else {
    return make_response(501, "Not Implemented");
}
```

**Two status codes you MUST use here:**
- `405 Method Not Allowed` when the method is known but not allowed on this route. Must include `Allow:` header.
- `501 Not Implemented` when the method is unknown to your server entirely (`PROPFIND`, made-up nonsense).

---

## CGI and method semantics

When the route maps to a CGI (e.g. `.php`), method semantics don't go away — they get passed to the CGI script:

| Request property | CGI environment variable |
|---|---|
| Method | `REQUEST_METHOD` |
| Path | `PATH_INFO`, `SCRIPT_NAME` |
| Query string | `QUERY_STRING` |
| Body | piped to CGI's stdin |
| Body length | `CONTENT_LENGTH` |
| Body type | `CONTENT_TYPE` |

So a `POST /upload.php` becomes: spawn `php-cgi`, set `REQUEST_METHOD=POST`, pipe the request body to its stdin, read its stdout, send it back as the response.

See [file 14](14_CGI.md) for the full CGI dance.

---

## TL;DR mental model

> The method tells the server **what kind of action** is being requested. Safe means "no side effects expected." Idempotent means "retries are safe." Cacheable means "intermediaries may store the response." For webserv: implement `GET`, `HEAD` (easy bonus), `POST` (for uploads + CGI), `DELETE` (for files). Reject everything else with `405` if known, `501` if unknown.

**Continue to [`04_STATUS_CODES.md`](04_STATUS_CODES.md)** to learn how to *answer* a request.
