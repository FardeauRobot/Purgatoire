# Configuration File — a working example to start from

> *"The config is the contract. Everything the server does, it does because a directive told it to."*

This file gives you a **copy-able starting config**, from the smallest one that boots up to a realistic annotated one. For *what the subject demands* of your config (the feature checklist) see [`17_WEBSERV_SUBJECT.md` §Configuration](17_WEBSERV_SUBJECT.md#configuration-file); for *what to get right before writing the parser* see [`TIPS.md` §Config file](TIPS.md#config-file--read-the-subject-carefully). This file is the **concrete artefact** those two describe.

You invent your own syntax — NGINX-style is the suggested convention and the one used here. The grader checks that the listed features are *reachable through your config*, not that you match a specific grammar.

---

## 1. The minimal config that boots

The smallest thing that gives you a running server answering `GET /` with a static file:

```nginx-ish
server {
    listen 0.0.0.0:8080;

    location / {
        root    /var/www;
        index   index.html;
        methods GET;
    }
}
```

Three facts the server learns from it:
- **Where to listen** — `0.0.0.0:8080` (all interfaces, port 8080). This is the `interface:port` pair your `poll()` loop opens a listening socket for.
- **Where files live** — requests under `/` map onto `/var/www` on disk.
- **What's allowed** — only `GET`. A `POST` here returns `405 Method Not Allowed`.

Boot it, then `curl -v http://localhost:8080/` should hand you `/var/www/index.html`. If that works, your socket + parse + static-serve spine is alive.

---

## 2. A realistic annotated config

Everything the subject expects to be reachable, in one file. Comments explain each directive — your real `.conf` would not need them.

```nginx-ish
server {
    # --- Top-level: bind targets ---
    listen 0.0.0.0:8080;          # one server block can listen on
    listen 127.0.0.1:8081;        # several interface:port pairs
    server_name example.com;      # optional: only matters if you do virtual hosts

    # --- Top-level: limits & error pages (apply to every location) ---
    client_max_body_size 10M;     # reject bodies bigger than this -> 413
    error_page 404 /errors/404.html;
    error_page 500 502 503 504 /errors/5xx.html;

    # --- Static site root ---
    location / {
        root      /var/www;       # URL /foo  ->  /var/www/foo
        index     index.html;     # what to serve when the path is a directory
        methods   GET;            # GET only here
        autoindex off;            # no directory listing; 403/404 on a bare dir
    }

    # --- A browsable directory ---
    location /files {
        root      /var/www/files;
        methods   GET;
        autoindex on;             # generate an HTML listing when no index file
    }

    # --- Upload + delete endpoint ---
    location /uploads {
        root        /var/www/uploads;
        methods     GET POST DELETE;
        upload_dir  /var/www/uploads;   # where POSTed file bodies get written
        client_max_body_size 50M;       # override the server-wide cap here
    }

    # --- CGI by extension ---
    location /cgi-bin {
        root    /var/www/cgi-bin;
        methods GET POST;
        cgi     .py  /usr/bin/python3;  # .py  -> run through python3
        cgi     .php /usr/bin/php-cgi;   # .php -> run through php-cgi
    }

    # --- A redirect ---
    location /old {
        return 301 /new;          # 301 + Location: /new, no body served
    }
}
```

### What each block is doing

| Directive | Scope | Effect |
|---|---|---|
| `listen` | server | Open a listening socket on this `interface:port`. Repeatable. |
| `server_name` | server | Hostname for virtual-host dispatch. **Optional** — webserv dispatches by `interface:port`, see [`17` §Multiple ports](17_WEBSERV_SUBJECT.md). |
| `client_max_body_size` | server / location | Cap on request body bytes. Exceed it → `413 Payload Too Large`. |
| `error_page` | server | Map status code(s) → a file to serve as that error's body. |
| `location` | server | A URL-prefix rule block. The request path picks the matching one. |
| `root` | location | Filesystem prefix the URL path is appended to. |
| `index` | location | File served when the resolved path is a directory. |
| `methods` | location | Allowed HTTP methods. Anything else → `405`. |
| `autoindex` | location | `on` = generate a directory listing; `off` = forbid bare dirs. |
| `upload_dir` | location | Destination for uploaded bodies (used with `POST`). |
| `cgi` | location | Map a file extension → interpreter binary. Repeatable. |
| `return` | location | Short-circuit with a redirect (`3xx` + `Location`). |

---

## 3. How a request resolves against this config

Walking one request through the config makes the data flow concrete:

```
GET /files/report.pdf  arrives on the socket bound to 0.0.0.0:8080
        │
        ▼
1. Which server block? -> the one whose listen matches 0.0.0.0:8080
2. Which location?     -> longest prefix match: "/files" beats "/"
3. Method allowed?     -> GET is in `methods GET;`       (else 405)
4. Resolve path        -> root /var/www/files + "/report.pdf"
                          = /var/www/files/report.pdf
5. Exists? regular file?-> stat() it (else 404)
6. Send it             -> 200 OK + Content-Type by extension + body
```

The **root mapping** is the subject's `/kapouet` example: the location prefix is *stripped* and replaced by `root`. `/files/report.pdf` under `root /var/www/files` becomes `/var/www/files/report.pdf` — note the `/files` URL segment is consumed by the match, not appended twice. Decide your stripping rule and write it down; it's the single most common off-by-one in config handling.

---

## 4. C++98 parsing notes

You parse this by hand — no `<regex>` worth the trouble, no JSON/YAML libs. Reach for `std::ifstream` + `std::stringstream` and tokenize on whitespace and `{ } ;`.

- **No `std::stoi`.** `client_max_body_size 10M` needs you to parse digits *and* the `K`/`M`/`G` suffix yourself with `std::stringstream` or manual digit accumulation. See [`cpp/io-errors/FSTREAM_GUIDE.md`](../io-errors/FSTREAM_GUIDE.md).
- **Validate at load, not at request time.** A missing `root`, an unbound port, an unreadable error-page file — catch these when you parse, print a clear error, and refuse to start. A server that boots on a broken config and crashes on the first request fails the eval.
- **Print what you parsed.** Before wiring the parser to the server, dump the in-memory config tree to stdout and eyeball it against the file. If the parse is wrong, everything above it is wrong — this is the [`TIPS.md`](TIPS.md#config-file--read-the-subject-carefully) advice and it's worth repeating.
- **Defaults matter.** Decide the behaviour when a directive is *absent*: no `methods` → which methods? no `autoindex` → `off`. Bake the defaults into your config struct's constructor so every location starts sane.

---

## 🔗 Related

- [`17_WEBSERV_SUBJECT.md` §Configuration](17_WEBSERV_SUBJECT.md#configuration-file) — the subject's required directive list (the *what*).
- [`TIPS.md` §Config file](TIPS.md#config-file--read-the-subject-carefully) — pre-parser discipline and the validator habit.
- [`08_URLS.md`](08_URLS.md) — how the request path you match against is structured.
- [`14_CGI.md`](14_CGI.md) — what the `cgi` directive feeds into.
- [`OVERVIEW.md`](OVERVIEW.md) — where `ConfigParser` sits in the class layering.
