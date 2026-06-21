# 17 — The Webserv Subject, Mapped

> *"This is when you finally understand why URLs start with HTTP."* — the subject's own tagline.

A faithful distillation of the 42 webserv subject (version 24.0), with each requirement linked back to the file in this library that explains it.

**Always read the actual subject PDF as the source of truth.** This file is a navigation aid, not a replacement.

---

## What the subject actually demands

### Program

- **Binary name:** `webserv`
- **Invocation:** `./webserv [configuration file]`
- **Files to submit:** `Makefile`, `*.h` / `*.hpp`, `*.cpp`, `*.tpp` / `*.ipp`, configuration files
- **Makefile rules:** `$(NAME)`, `all`, `clean`, `fclean`, `re`. Must not perform unnecessary relinking.
- **Compiler:** `c++` with `-Wall -Wextra -Werror`
- **Standard:** must compile with `-std=c++98` and respect the standard
- **C++ over C:** prefer `<cstring>` to `<string.h>`, etc. C functions allowed only when no C++ alternative exists.
- **External libraries:** none. No Boost.

→ See [`meta/FLAGS.md`](../../meta/FLAGS.md) for the recommended flag set.

### Allowed external functions

The full list from the subject:

```
execve, pipe, strerror, gai_strerror, errno, dup, dup2, fork,
socketpair, htons, htonl, ntohs, ntohl, select, poll, epoll
(epoll_create, epoll_ctl, epoll_wait), kqueue (kqueue, kevent),
socket, accept, listen, send, recv, chdir, bind, connect,
getaddrinfo, freeaddrinfo, setsockopt, getsockname, getprotobyname,
fcntl, close, read, write, waitpid, kill, signal, access, stat,
open, opendir, readdir, closedir
```

If a function isn't on this list, you can't use it. Notable absences: `strncpy`, `memcpy` (you'll write loops), `strtol` (you'll use `std::stringstream` or roll your own), nothing for hash maps (no `<unordered_map>` in C++98 anyway).

→ Build everything on top of the C++98 standard library + this list.

### Behaviour — non-blocking I/O

The most architecturally important section:

| Rule | Where covered |
|---|---|
| Server must be non-blocking at all times | [07_CONNECTION](07_CONNECTION.md) |
| **Only 1 `poll()`** (or equivalent) for all client/server I/O — listen included | [07_CONNECTION](07_CONNECTION.md) |
| `poll()` must monitor reading **and** writing simultaneously | [07_CONNECTION](07_CONNECTION.md) |
| Never `read`/`write` without going through `poll()` | [07_CONNECTION](07_CONNECTION.md) |
| **`errno` check after `read`/`write` is forbidden** → grade 0 | [07_CONNECTION](07_CONNECTION.md) |
| `poll()` not required for regular disk files (they're always ready) | [07_CONNECTION](07_CONNECTION.md) |
| Requests must never hang indefinitely | [07_CONNECTION](07_CONNECTION.md) — idle timeouts |
| Server must be browser-compatible | [15_TOOLS](15_TOOLS.md) — test with a real browser |
| Status codes must be accurate | [04_STATUS_CODES](04_STATUS_CODES.md) |
| Default error pages if none provided | [04_STATUS_CODES](04_STATUS_CODES.md) |
| Cannot `fork` for anything except CGI | [14_CGI](14_CGI.md) |
| Must serve a fully static website | [08_URLS](08_URLS.md) + [09_CONTENT_NEGOTIATION](09_CONTENT_NEGOTIATION.md) |
| Clients must be able to upload files | [09_CONTENT_NEGOTIATION](09_CONTENT_NEGOTIATION.md) — multipart/form-data |
| At minimum: `GET`, `POST`, `DELETE` methods | [03_METHODS](03_METHODS.md) |
| Must be stress-tested for resilience | [TESTS](TESTS.md) — `ab` / `siege`, availability & leak checks |
| Must be able to listen on multiple `interface:port` pairs | configuration file |

### macOS-specific

- `fcntl()` allowed **only** with `F_SETFL`, `O_NONBLOCK`, `FD_CLOEXEC`. Any other flag is forbidden.
- Use non-blocking fds via `fcntl(fd, F_SETFL, O_NONBLOCK)`.

→ [07_CONNECTION](07_CONNECTION.md) covers this.

### Out of scope

The subject explicitly says:

> *"We deliberately chose to offer only a subset of the HTTP RFC. In this context, the virtual host feature is considered out of scope. But you are allowed to implement it if you want."*

So dispatching by `Host:` header (virtual hosts) is optional. You dispatch by **`interface:port`** pair, as defined in the config.

You're also not required to implement: TLS / HTTPS, HTTP/2, HTTP/3, full RFC compliance, every status code, every method, every header.

---

## Configuration file

You design your own config syntax (NGINX-style is suggested). Per the subject, your config must let the user define:

### Top-level

- All `interface:port` pairs to listen on. Multiple pairs allowed.
- Default error pages.
- Maximum allowed size for client request bodies (`client_max_body_size` in NGINX).

### Per route / location

- List of accepted HTTP methods (e.g. `methods: GET POST`).
- HTTP redirection (e.g. `return 301 /new`).
- Root directory mapping. Per the subject's example: *"if URL `/kapouet` is rooted to `/tmp/www`, URL `/kapouet/pouic/toto/pouet` will search for `/tmp/www/pouic/toto/pouet`."*
- Enable / disable directory listing (autoindex in NGINX terms).
- Default file when the requested resource is a directory (e.g. `index index.html;`).
- Upload destination directory (when uploads are permitted).
- CGI execution by file extension (e.g. `cgi .php /usr/bin/php-cgi;`).

You may invent additional config directives (server name for virtual hosts if you implement them, log file paths, etc.).

### Example config (illustrative — design your own)

```nginx-ish
server {
    listen 0.0.0.0:8080;
    listen 0.0.0.0:8081;
    server_name example.com;

    client_max_body_size 10M;

    error_page 404 /errors/404.html;
    error_page 500 502 503 504 /errors/5xx.html;

    location / {
        root        /var/www;
        index       index.html;
        methods     GET POST;
        autoindex   on;
    }

    location /uploads {
        root        /var/www/uploads;
        methods     POST DELETE;
        upload_dir  /var/www/uploads;
    }

    location /scripts {
        root        /var/www/scripts;
        methods     GET POST;
        cgi         .php /usr/bin/php-cgi;
        cgi         .py  /usr/bin/python3;
    }

    location /old {
        return      301 /new;
    }
}
```

The parser you write must accept some such syntax. The grader does **not** require a specific syntax — they need to verify that the features the subject lists are *reachable* via your config.

---

## CGI details from the subject

The subject calls out four CGI-specific rules:

1. *"Have a careful look at the environment variables involved in the web server-CGI communication. The full request and arguments provided by the client must be available to the CGI."*
2. *"For chunked requests, your server needs to un-chunk them, the CGI will expect EOF as the end of the body."*
3. *"If no `content_length` is returned from the CGI, EOF will mark the end of the returned data."*
4. *"The CGI should be run in the correct directory for relative path file access."*
5. *"Your server should support at least one CGI (php-CGI, Python, and so forth)."*

→ All five covered in detail in [14_CGI](14_CGI.md).

---

## README requirements (Chapter V)

Your repo must include a `README.md` at the root with:

- **First line, italicised:** *"This project has been created as part of the 42 curriculum by `<login1>`[, `<login2>`[, `<login3>`[...]]]."*
- A **"Description"** section explaining what the project is.
- An **"Instructions"** section covering compilation, install, execution.
- A **"Resources"** section listing references **and** describing how AI was used (for which tasks, which parts).
- Written in **English**.

→ Use the README template (or evolve from your existing project READMEs in `library/projects/...`).

---

## Bonus part

Only assessed if mandatory is **fully** working:

- Support cookies and session management (provide simple examples). → [12_COOKIES_SESSIONS](12_COOKIES_SESSIONS.md).
- Handle multiple CGI types. → trivial once one CGI works (extend the config mapping).

If you're aiming for bonus, plan it from day one — retrofitting sessions onto a working webserv is harder than building them in.

---

## AI instructions (Chapter III)

The subject's stance on AI use is explicit. Summary:

✅ **Allowed**:
- Reducing repetitive / tedious tasks
- Developing prompting skills
- Learning *with* peer review

❌ **Bad practice that fails you**:
- Copy-pasting code you can't explain
- Letting an AI write key parts that you can't justify during defence
- Relying solely on AI without peer review

→ The subject treats AI use as a meta-skill. **Every line of code in your webserv must be one you can explain on demand.** The evaluator will ask.

---

## Defence / peer-evaluation (Chapter VII)

The evaluator may request a *brief modification* of your project during the defence — a small behaviour change, a few-line addition, etc. This is to verify your understanding.

Be ready to:
- Add a new error page mapping in the config
- Add a new HTTP method to the supported list
- Change a status code on a specific path
- Add a new CGI extension

Anything that should take 5 minutes if you wrote the code yourself.

→ Practice this. Have someone ask "now make it support PUT" and time yourself.

---

## A study/build plan mapped to this library

Roughly 4 weeks, full-time:

| Week | Goal | Library files | Code goal |
|---|---|---|---|
| **0 (prep)** | Understand HTTP | `01` → `08` + Lab `16` | Tiny server runs |
| **1** | Socket + poll architecture | `07`, `15` | Multi-client poll-driven loop |
| **2** | Parser + routing + static files | `02`, `04`, `05`, `06`, `08`, `09` | `curl` can `GET` files; status codes correct |
| **3** | POST + DELETE + uploads + error pages | `03`, `04`, `09` | Browser file-upload form works |
| **4** | CGI + chunked + stress + bonus | `06`, `14`, optionally `11`, `12` | `wrk -t4 -c100 -d30s` doesn't kill it |

---

## Checklist for evaluation day

The night before:

- [ ] `make re` builds with **zero warnings** under `-Wall -Wextra -Werror -std=c++98`
- [ ] All 5 Makefile rules work (`all`, `clean`, `fclean`, `re`, and `$(NAME)`)
- [ ] Static website served correctly in 2 different browsers
- [ ] File upload works from a browser form (multipart/form-data)
- [ ] `GET`, `POST`, `DELETE` all work
- [ ] CGI works for at least one language
- [ ] Configurable error pages render correctly
- [ ] Custom `client_max_body_size` is enforced → returns 413
- [ ] `301`/`302` redirects work per config
- [ ] Directory listing works when enabled; 403 / index file when disabled
- [ ] Multiple ports → different content (test both)
- [ ] `wrk -t4 -c50 -d30s http://localhost:8080/` — no crashes, no leaks
- [ ] README in English with the four required sections
- [ ] No `errno` checks after read/write (`grep -n 'errno' src/*` should not find any next to read/recv/write/send)
- [ ] All used external functions are in the allowed list

---

## TL;DR mental model

> The subject is a list of HTTP-conformance requirements + a single architectural straitjacket (one `poll()`, never block, never check `errno` after I/O). Build the tiny server first, then add features one row at a time. Test against browsers + nginx + `wrk`. **Don't add what isn't in the subject** — virtual hosts, HTTPS, websockets, advanced caching are all rabbit holes. Stay disciplined: GET/POST/DELETE + static + CGI + config + multiple ports. That's the project.

---

## Where to go from here

- Re-read the subject PDF in full.
- Re-read this library in order: [`INDEX`](INDEX.md) → 01 → 02 → ... → 17.
- Build [`Lab 16`](16_TINY_SERVER_LAB.md) end-to-end. Make it run in a browser.
- Then start your webserv from a clean directory, copy the appropriate Makefile from [`templates/cpp/complete`](../../../templates/cpp/complete/), and begin step 2 of the roadmap.
