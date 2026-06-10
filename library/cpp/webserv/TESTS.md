# 🧪 TESTS — Load & Stress Testing webserv

> *"A server that works for one `curl` is a demo. A server that survives `siege -b` for a minute without leaking, hanging, or crashing is a webserv."*

`curl`/`nc` ([15_TOOLS](15_TOOLS.md)) prove your server is *correct* for one request. **Load testers prove it's *resilient* under many.** The subject is blunt about this:

> *"Stress test your server to ensure it stays available at all times."*

And the evaluation makes it concrete: run a load tool, and your server must keep an **availability near 100%**, never crash, never hang, and **never grow in memory**. This file is the deep reference for the two tools that measure that — **`ab`** and **`siege`**. The brief mentions in [15_TOOLS](15_TOOLS.md) and [TIPS](TIPS.md) point here for the full story.

---

## The two tools at a glance

| | **`ab`** (ApacheBench) | **`siege`** |
|---|---|---|
| Comes from | Apache httpd | Jeffrey Fulmer (standalone) |
| Threading model | Single-threaded | Multi-threaded (simulates *N users*) |
| Best at | Raw throughput + latency **percentiles** | Sustained **availability** under realistic load |
| Headline metric | **Requests per second** | **Availability %** |
| Multiple endpoints | No (one URL) | Yes (a `urls.txt` file) |
| Think-time between reqs | No (always hammers) | Optional (`-d`), or `-b` to hammer |
| The eval cares about | "can it handle a burst?" | "does it stay up indefinitely?" |
| On your machine | `/usr/sbin/ab` (**preinstalled**) | `/opt/homebrew/bin/siege` (**`brew install siege`**) |

Short version: **`ab` for a quick throughput/latency snapshot, `siege` for the sustained-availability test the evaluator actually runs.** Use both.

---

## ApacheBench (`ab`)

A single-threaded HTTP benchmarker. You give it a total request count and a concurrency level; it fires them as fast as it can and reports timing statistics. Preinstalled on macOS (`ab -V` → `Version 2.3`). On Linux: `apt install apache2-utils` or `brew install httpd`.

### Basic usage

```sh
ab -n 1000 -c 50 http://localhost:8080/        # 1000 requests total, 50 at a time
```

⚠️ **The URL must have a path** —I at minimum a trailing `/`. `ab http://localhost:8080` errors with *"invalid URL"*; `ab http://localhost:8080/` works.

### Key flags

| Flag | Meaning |
|---|---|
| `-n N` | total number of requests to perform |
| `-c N` | concurrency — simultaneous connections |
| `-k` | use **HTTP keep-alive** (reuse connections) — see gotcha below |
| `-t N` | time limit in seconds (caps a run; implies `-n 50000`) |
| `-p file` | POST the contents of `file` as the body |
| `-T type` | `Content-Type` for `-p` (e.g. `-T application/x-www-form-urlencoded`) |
| `-H "K: V"` | add a request header (e.g. `-H "Host: example.com"` for vhost tests) |
| `-s N` | per-request socket timeout (seconds) |
| `-r` | don't exit on socket receive errors (keep going under stress) |

### Reading the output

```
Concurrency Level:      50
Time taken for tests:   0.842 seconds
Complete requests:      1000
Failed requests:        0            ← MUST be 0 (but see the length gotcha)
Non-2xx responses:      0            ← anything here means wrong status codes
Requests per second:    1187.6 [#/sec] (mean)    ← throughput headline
Time per request:       42.1 [ms] (mean)         ← latency across all concurrent
Time per request:       0.842 [ms] (mean, across all concurrent requests)
Transfer rate:          312.4 [Kbytes/sec]

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    1   0.4      1       3
Processing:     2   40  12.1     38      95
Waiting:        2   39  12.0     37      94
Total:          2   41  12.2     39      96

Percentage of the requests served within a certain time (ms)
  50%     39
  95%     63
  99%     88            ← tail latency — the slowest requests
 100%     96 (longest request)
```

The numbers that matter for webserv:
- **`Failed requests: 0`** and **`Non-2xx responses: 0`** — under load you should still serve every request correctly.
- **`Requests per second`** — your throughput. The subject sets no target; you just want it stable, not collapsing.
- **The percentile table** — `99%` and `100%` are your *tail latency*. A huge gap between `50%` and `100%` means some requests are starving (often a sign of a poll/accept fairness bug).

### webserv recipes

```sh
# Smoke test: can it handle a burst?
ab -n 1000 -c 50 http://localhost:8080/index.html

# Keep-alive path (HTTP/1.1 persistent connections)
ab -k -n 5000 -c 100 http://localhost:8080/index.html

# Push concurrency ABOVE your fd limit to test EMFILE resilience
ab -n 2000 -c 500 http://localhost:8080/      # server must NOT crash or spin

# POST stress (file upload path)
ab -n 500 -c 20 -p body.txt -T application/x-www-form-urlencoded \
   http://localhost:8080/upload
```

### Gotchas

- **`ab` defaults to HTTP/1.0**, which closes the connection after every response. To exercise your **keep-alive** code path, add `-k`. Test *both* — they hit different parts of [07_CONNECTION](07_CONNECTION.md).
- **"Failed requests" can be a false positive.** `ab` flags a request as failed if its body **length** differs from the first response's. For *dynamic* pages (autoindex, CGI, error pages of varying size) this counts harmless variation as failures. Fix: **benchmark a fixed-size static file** for clean numbers, and read the breakdown — if failures are all `Length` (not `Connect`/`Exceptions`), they're not real failures.
- **The client hits limits too.** At high `-c` on macOS, `ab` itself can run out of fds (default `ulimit -n` is **256**). Raise it in the testing shell: `ulimit -n 4096`. (This is the *client* side — separate from your server's limit.)

---

## `siege`

A multi-threaded load tester that simulates *N concurrent users* hitting your server, optionally with random think-time, optionally across a list of URLs — much closer to real traffic. Its headline metric, **Availability**, is the number the webserv eval watches. Install: `brew install siege` (already done — `siege -V` → `SIEGE 4.1.7`). First run writes a config to `~/.siege/siege.conf` (view with `siege -C`).

### Basic usage

```sh
siege -c 25 -t 30S http://localhost:8080/      # 25 users, for 30 seconds
```

Time units are suffixed: **`S`**econds, **`M`**inutes, **`H`**ours → `-t30S`, `-t1M`. Stop a run early with `Ctrl-C`; siege prints the report on exit.

### Key flags

| Flag | Meaning |
|---|---|
| `-c N` | concurrent users (default 10) |
| `-t DUR` | run for a duration: `-t30S`, `-t1M`, `-t1H` |
| `-r N` | reps: each user makes N requests, then stops (alternative to `-t`) |
| `-b` | **benchmark mode** — no think-time delay between requests (hammer) |
| `-d N` | random delay 0–N s between a user's requests (simulates real users) |
| `-f file` | read URLs from `file` (one per line) instead of a single URL |
| `-i` | "internet" mode — hit URLs from `-f` in **random** order |
| `-H "K: V"` | add a request header |
| `-C` | print the current config and exit |

POST: put the method and body in the URL string or urls file —
`siege "http://localhost:8080/upload POST name=alice&age=30"`.

### Reading the output

```
Transactions:               18452 hits
Availability:              100.00 %        ← THE eval metric — keep it ~100
Elapsed time:               29.87 secs
Data transferred:            2.41 MB
Response time:               0.04 secs     ← mean latency
Transaction rate:          617.74 trans/sec
Throughput:                  0.08 MB/sec
Concurrency:                24.93           ← effective parallel users achieved
Successful transactions:    18452
Failed transactions:            0           ← MUST be 0
Longest transaction:         0.31
Shortest transaction:        0.00
```

What the evaluator (and you) look at:
- **`Availability: 100.00 %`** — the headline. It's `successful / (successful + failed) × 100`. The bar is high — you want this pinned at or extremely close to **100%**. A dip means connections were refused, dropped, or errored: usually a crash, an EMFILE busy-loop, or a hang.
- **`Failed transactions: 0`** — same story from the other side.
- **`Concurrency`** — the *effective* parallelism. If it balloons far above your `-c`, it means transactions are *queuing* (each taking longer), i.e. your server is falling behind.

### Hitting multiple endpoints

Real load isn't one URL. Make a `urls.txt`:

```
http://localhost:8080/
http://localhost:8080/index.html
http://localhost:8080/style.css
http://localhost:8080/upload POST name=test&value=42
http://localhost:8080/old-page
```

```sh
siege -c 30 -t1M -i -f urls.txt        # 30 users, 1 min, random URLs from the file
```

This exercises routing, static serving, uploads, and redirects all at once — the closest synthetic approximation of an evaluator clicking around.

### webserv recipes

```sh
# THE classic eval test: sustained hammer, watch availability + memory
siege -b -t1M http://localhost:8080/

# Realistic mixed load with think-time
siege -c 30 -d3 -t1M -i -f urls.txt

# Brutal concurrency to probe EMFILE / connection caps
siege -b -c 255 -t30S http://localhost:8080/
```

### Gotchas

- **Without `-b`, siege adds ~1s of think-time** between each user's requests. That's realistic, but if you want maximum pressure (the eval's "hammer" test), use **`-b`**.
- **Availability below 100% can be the *client's* fault.** At very high `-c`, siege itself hits its own fd/`ulimit` ceiling and reports failures that aren't your server's. Raise the client limit (`ulimit -n 4096`) before blaming webserv, and cross-check with `ab` and `curl`.
- **siege's config lives in `~/.siege/siege.conf`.** Defaults are sane; `siege -C` shows them. Notably `connection = close` by default — set `connection = keep-alive` there to test persistent connections.

---

## `ab` vs `siege` — which when?

| You want to know… | Reach for |
|---|---|
| Raw requests/sec and latency **percentiles** | `ab` |
| Does it stay **available** under sustained load (the eval) | `siege -b -t…` |
| Behaviour across **many endpoints** at once | `siege -f urls.txt` |
| A quick one-liner burst before committing | `ab -n 1000 -c 50` |
| Realistic users with **think-time** | `siege -c… -d…` |
| Keep-alive vs close path | `ab -k` / siege `connection=` config |

Run **`ab` first** for a fast snapshot, then **`siege -b`** for the endurance test that mirrors evaluation.

---

## What the eval actually checks (and how to watch it)

Load throughput is only half of it. The webserv eval pass criteria under stress are: **no crash · high availability · no memory growth · no fd leak · no hang.** A throughput number alone won't catch a leak — you have to *watch the process while it's under siege*. Two terminals:

**Terminal 1 — apply load:**
```sh
siege -b -t2M http://localhost:8080/
```

**Terminal 2 — watch the server (macOS):**
```sh
WPID=$(pgrep -n webserv)
# memory + cpu, refreshing — RES/MEM must stay FLAT, not climb
top -pid $WPID
# open fds — count must return to baseline after the run, not keep rising
lsof -p $WPID | wc -l
# leak report after the run (macOS replacement for valgrind)
leaks $WPID
```

The tells:
- **Memory climbs steadily during the run** → a leak per request/connection. Build `make asan` and re-run a *shorter* siege under AddressSanitizer to get the exact allocation site.
- **fd count keeps rising and never drops** → you're not `close()`ing connections (see the `dropClient()` cleanup) — you'll eventually hit `EMFILE`.
- **Availability dips, server still alive** → an EMFILE busy-loop or a connection you accepted but never serviced.
- **Availability craters / siege reports connection refused** → the server crashed or hung. Check it didn't segfault; re-run under `make asan` or `lldb`.

> **The single most important run:** `siege -b -t1M http://localhost:8080/` with `top`/`leaks` open alongside. Flat memory + 100% availability + survives = you pass the resilience bar.

---

## macOS / Apple Silicon notes

- **`ab` is preinstalled** (`/usr/sbin/ab`); **`siege` via `brew install siege`** (`/opt/homebrew/bin/siege`).
- **Raise the client-side fd limit** before high-concurrency runs: `ulimit -n 4096` in the testing shell. macOS defaults to a stingy **256**, which makes the *load tool* the bottleneck.
- **Use `leaks $PID`**, not `valgrind` — valgrind is unreliable on Apple Silicon. Your repo's `make leaks` target wraps this.
- Loopback interface is **`lo0`** (not Linux's `lo`) if you also packet-capture with `tcpdump`.

---

## C++98 / webserv callouts

- **EMFILE resilience is the point of the high-`-c` runs.** When concurrency exceeds your fd limit, `accept()` fails — your server must neither crash nor busy-spin. Push `ab -c 500` / `siege -c 255` specifically to provoke this. (Design: cap connections + gate the listen socket's `POLLIN`; see [POLL_DEEP_DIVE](../../../LEVEL5/WebServTest/notes/POLL_DEEP_DIVE.md).)
- **No `errno` after `read`/`write`** (subject rule) means you detect a dropped client purely by `recv()` returning `0`/`-1`, not by inspecting the error. Load tools that open and abandon connections (Ctrl-C mid-run) are a good test of that teardown path.
- **`ab`'s "Length" failures** are expected for variable-size dynamic responses — don't chase them; benchmark a static file for clean throughput numbers.

---

## Related

- [`15_TOOLS.md`](15_TOOLS.md) — the everyday kit: `curl`, `nc`, browser devtools, NGINX *(brief load-tester mention points here)*
- [`TIPS.md`](TIPS.md) — debugging habits and development order
- [`07_CONNECTION.md`](07_CONNECTION.md) — keep-alive vs close, the single-`poll()` rule, idle timeouts
- [`functions/02_IO_MULTIPLEXING.md`](functions/02_IO_MULTIPLEXING.md) — the `poll()` loop the load lands on
- [`../../../LEVEL5/WebServTest/notes/POLL_DEEP_DIVE.md`](../../../LEVEL5/WebServTest/notes/POLL_DEEP_DIVE.md) — the overload funnel, the backlog, the EMFILE trap *(sandbox deep-dive)*
