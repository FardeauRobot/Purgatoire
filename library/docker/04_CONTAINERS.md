# 04 — Containers: lifecycle and the CLI

> The image is the class; the container is the instance. This page is about running them, watching them, and the two behaviours Inception depends on: restart policies and readiness.

---

## 1. The lifecycle

```
   docker create        docker start
  image ──────────► created ──────────► running ──────────┐
                                          │  │            │
                          docker pause ───┘  │ PID 1 exits│
                                  ▼          ▼            │
                               paused      exited ◄───────┘
                                             │
                                   docker rm │
                                             ▼
                                          removed
```

`docker run` = `create` + `start`. The transition that matters is **running → exited**, and it has exactly one cause: PID 1 exited ([`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md)).

An exited container still exists — its writable layer, its logs, its exit code are all still there. That's why `docker ps -a` shows a graveyard and why `docker logs` works on a dead container. It's also the first thing to check when something won't start.

---

## 2. The commands you'll actually use

### Running

```bash
docker run alpine echo hi              # run, print, exit
docker run -d nginx-img                # detached — background
docker run -it alpine sh               # interactive + TTY: a shell inside
docker run --rm alpine echo hi         # auto-remove on exit
docker run --name web -p 8080:80 nginx-img
```

`-it` is the pair everyone types without knowing: `-i` keeps stdin open, `-t` allocates a pseudo-TTY. Without `-t` you get no prompt; without `-i` you can't type. You met both in minishell — this is the same pty machinery.

### Inspecting

```bash
docker ps                    # running
docker ps -a                 # including the dead — check here first
docker logs <name>           # stdout/stderr of PID 1
docker logs -f --tail 50 <name>
docker inspect <name>        # everything, as JSON
docker inspect -f '{{.State.ExitCode}}' <name>
docker stats                 # live CPU/memory
docker top <name>            # processes inside
```

**`docker logs` shows PID 1's stdout and stderr, nothing else.** This is the deep reason containers are supposed to log to stdout rather than to files: a service writing to `/var/log/nginx/error.log` inside the container is invisible to `docker logs`, and you'll be left staring at empty output while the answer sits in a file you forgot exists. When debugging Inception, if the logs are empty, check whether the service is logging to a file.

### Interacting

```bash
docker exec -it <name> sh            # shell in a RUNNING container
docker exec <name> ps aux
docker cp <name>:/etc/nginx/nginx.conf ./   # copy out
```

`exec` starts a *new* process in the container's namespaces. It is not a login and it does not affect PID 1.

**A discipline point:** `docker exec` is for *looking*, not for *fixing*. Anything you change this way vanishes on the next `up`, and worse, it makes your stack work in a way your repository doesn't reproduce. If you find a fix by hand, put it in the Dockerfile and rebuild ([`01_PHILOSOPHY.md`](01_PHILOSOPHY.md), immutability).

### Stopping and cleaning

```bash
docker stop <name>       # SIGTERM, wait 10s, then SIGKILL
docker kill <name>       # SIGKILL immediately
docker restart <name>
docker rm <name>
docker rm -f <name>      # stop and remove

docker system df         # what's eating your disk
docker system prune      # remove stopped containers, unused networks, dangling images
docker system prune -a --volumes   # ← nuclear. removes volumes. read twice.
```

That 10-second gap in `docker stop` is where PID 1 discipline shows up. A well-built container handles SIGTERM, shuts down cleanly, and exits in under a second. A `tail -f`-based one ignores it and gets SIGKILL'd every time — which, for MariaDB, means an unclean shutdown on every single stop.

---

## 3. Restart policies

Inception rule V5: *containers must restart in case of a crash.*

```yaml
services:
  nginx:
    restart: unless-stopped
```

| Policy | Behaviour |
|---|---|
| `no` | Default. Never restart. |
| `on-failure[:N]` | Restart only on non-zero exit, optionally capped at N tries |
| `always` | Always restart — **including after a daemon/host reboot** |
| `unless-stopped` | Like `always`, except a container you stopped manually stays stopped across reboots |

**`unless-stopped` is the sane default here.** It satisfies "restart on crash", survives a VM reboot (which the evaluator may test), and doesn't fight you when you deliberately stop something.

The distinction between `always` and `unless-stopped` is a real question: with `always`, a container you stopped by hand comes *back* when Docker restarts. With `unless-stopped`, your manual stop is remembered.

Docker applies an exponential backoff between restart attempts, so a container in a crash loop doesn't spin the CPU. If you see `Restarting (1) 3 seconds ago` in `docker ps`, that's a boot loop — read the logs.

---

## 4. Readiness: `depends_on` is not enough

The race everyone hits: WordPress starts, tries to connect to MariaDB, and MariaDB is still initializing. WordPress exits. With a restart policy it retries, and *usually* succeeds on the second or third attempt — which means the bug looks fixed while remaining entirely present.

```yaml
depends_on:
  - mariadb
```

**This only controls start order, not readiness.** Docker starts MariaDB's container first, then immediately starts WordPress. It has no idea whether the database is accepting connections — a process existing is not a service being ready.

Three real fixes, in ascending order of quality:

### a) Healthcheck + condition (cleanest)

```yaml
services:
  mariadb:
    healthcheck:
      test: ["CMD", "mariadb-admin", "ping", "-h", "localhost"]
      interval: 10s
      timeout: 5s
      retries: 5
      start_period: 30s

  wordpress:
    depends_on:
      mariadb:
        condition: service_healthy
```

Now Docker actually waits for MariaDB to answer before starting WordPress. `start_period` is the grace window during which failures don't count against `retries` — useful for a database's first-run initialization.

### b) Wait loop in the entrypoint

```sh
until mariadb-admin ping -h "$MYSQL_HOST" --silent; do
    sleep 1
done
exec "$@"
```

Explicit and dependency-free. Note this is a **bounded wait on a condition**, not an infinite loop keeping the container alive — it terminates and then `exec`s. That's the distinction from the banned `while true` patterns, and it's worth being able to articulate, because at a glance they look similar.

### c) Application-level retry

The most robust in principle — the app reconnects on failure — but WordPress doesn't do this well, so it's not your route here.

**Use (a) if you can, (b) if you need something quick.** Either way, be ready to explain why `depends_on` alone doesn't solve it.

---

## 5. Compose-level equivalents

Everything above has a compose form, which is what you'll actually type:

```bash
docker compose up -d --build     # build and start everything
docker compose ps                # status of all services
docker compose logs -f mariadb   # follow one service
docker compose exec wordpress sh # shell into a service
docker compose restart nginx
docker compose down              # stop + remove containers and networks
docker compose down -v           # ← also destroys volumes
```

Note that compose commands take the **service name** from your YAML, not the container name. `docker compose logs mariadb` works regardless of what Docker named the container.

---

## 6. Debugging a container that won't stay up

The single most common Inception problem, in order:

```bash
docker compose ps -a                    # is it exited? restarting?
docker compose logs <service>           # what did PID 1 say before dying?
docker inspect -f '{{.State.ExitCode}}' <container>
```

Exit codes worth recognizing:

| Code | Usually means |
|---|---|
| 0 | Clean exit — your daemon backgrounded itself. **The classic PID 1 bug.** |
| 1 | Application error — read the logs |
| 126 | Command found but not executable — forgot `chmod +x` on the entrypoint |
| 127 | Command not found — typo, or the binary isn't in the image |
| 137 | SIGKILL — OOM, or a `stop` timeout after ignoring SIGTERM |
| 139 | Segfault |

**Exit code 0 is the one to internalize.** It means the container did exactly what you told it to and then finished. It's not a crash — it's a daemon that forked and exited, which is [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md)'s entire PID 1 section in one number.

If the logs are empty and it dies instantly, override the entrypoint to get inside and look:

```bash
docker run -it --entrypoint sh <image>
```

Then run your `CMD` by hand and watch what it actually does.

More symptom-driven diagnosis in [`12_TROUBLESHOOTING.md`](12_TROUBLESHOOTING.md).

---

## 7. 🚪 Gate

1. Your container's status is `Exited (0)`. What happened, and where's the bug?
2. What's the difference between `always` and `unless-stopped`?
3. Why doesn't `depends_on` prevent WordPress from racing MariaDB?
4. A wait-until-ready loop in an entrypoint versus the banned `while true` — what's the difference?
5. `docker logs` is empty but the service is clearly running. What's the likely cause?

---

**Next:** [`05_NETWORKING.md`](05_NETWORKING.md) — how the containers find each other.
