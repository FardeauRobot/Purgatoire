# 03 — Images and Dockerfiles

> An image is a **stack of read-only layers plus metadata**. A Dockerfile is the recipe that produces it. Most Dockerfile "style rules" are really consequences of how layers and the cache work — learn the mechanism and the rules stop needing memorization.

---

## 1. Images, layers, and the cache

Each instruction that changes the filesystem creates a new layer. Layers are content-addressed: identical inputs produce an identical hash, and identical hashes are reused.

**The cache rule:** Docker walks your instructions top to bottom. For each one it asks *"do I already have a layer built from this exact parent, by this exact instruction?"* On the first miss, the cache is dead — that instruction and **every instruction after it** rebuilds.

This single rule dictates instruction order:

```dockerfile
# ✗ BAD — copying source first invalidates the package install on every code change
FROM alpine:3.XX
COPY . /app
RUN apk add --no-cache php82 php82-fpm

# ✓ GOOD — code changes only invalidate the last line
FROM alpine:3.XX
RUN apk add --no-cache php82 php82-fpm
COPY . /app
```

**Stable things at the top, volatile things at the bottom.** In Inception you'll edit `conf/` and `tools/` constantly, so `COPY` those late — otherwise every tweak to an nginx config reinstalls nginx and you'll wait through it a hundred times.

For `COPY`, "changed" means content hash + mode. For `RUN`, it's the *literal command string* — Docker does not know what `apk upgrade` would do today versus yesterday. Cached is cached, however stale the result.

### The build context

`docker build .` **tars up that directory and sends it to the daemon** before anything runs. A `node_modules/` or a `.git/` in the context is uploaded every build, even if no instruction touches it.

`.dockerignore` (same syntax as `.gitignore`) excludes files from the context. This is a **security control**, not just a speed one — a `COPY . .` with a `.env` or a `secrets/` in the context bakes your credentials into an image layer, permanently, whether or not you delete them later.

```
.git
*.md
secrets/
.env
```

The subject's expected tree gives each service its own `.dockerignore`. That's why.

---

## 2. The instructions

### `FROM` — the base

```dockerfile
FROM alpine:3.XX
```

**Pin an explicit tag.** `FROM alpine` and `FROM alpine:latest` mean "whatever is newest at build time" — your build stops being reproducible, and Inception bans `latest` outright. Look up the penultimate stable release when you write it ([`10_INCEPTION.md`](10_INCEPTION.md) has the method).

For genuine reproducibility you can pin a digest — `FROM alpine@sha256:…` — which is immutable in a way tags aren't. Overkill here, but worth knowing exists.

### `RUN` — execute at build time

```dockerfile
RUN apk add --no-cache nginx openssl
```

Each `RUN` is a layer, so **chain related commands** with `&&` to avoid layer sprawl:

```dockerfile
RUN apk add --no-cache nginx \
 && mkdir -p /var/www/html \
 && adduser -D -H www-data
```

`--no-cache` on Alpine skips writing the package index to disk — the `apk` equivalent of Debian's `rm -rf /var/lib/apt/lists/*`. Cleanup must be **in the same `RUN`**: a later `RUN rm` only writes a whiteout marker, and the bytes stay in the earlier layer ([`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md)).

### `COPY` and `ADD`

```dockerfile
COPY conf/nginx.conf /etc/nginx/http.d/default.conf
COPY --chown=www-data:www-data tools/ /usr/local/bin/
```

**Use `COPY`.** `ADD` additionally auto-extracts tarballs and fetches URLs — implicit behaviour that surprises people. If you need to download something, `RUN wget` is explicit about what happened and lets you verify a checksum.

### `WORKDIR`

```dockerfile
WORKDIR /var/www/html
```

Sets the working directory for everything after it, creating it if needed. Use it instead of `RUN cd /x` — each `RUN` is a separate shell, so a `cd` in one doesn't survive into the next.

### `ENV` and `ARG`

```dockerfile
ARG PHP_VERSION=82        # build time only
ENV PATH="/usr/local/bin:${PATH}"   # build time AND runtime
```

| | `ARG` | `ENV` |
|---|---|---|
| Available at build | yes | yes |
| Available at runtime | **no** | yes |
| Set from CLI | `--build-arg` | `-e` / compose `environment:` |
| Visible in the image | in build history | yes, in `docker inspect` |

**Neither is safe for secrets.** `ARG` values are recorded in the image's build history and readable with `docker history`. `ENV` is in the image metadata forever. The subject's "no password in a Dockerfile" covers both — see [`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md).

### `EXPOSE`

```dockerfile
EXPOSE 9000
```

**Pure documentation.** It publishes nothing and opens nothing. It records intent for humans and for `docker run -P`. Actual exposure is `ports:` in compose. Worth knowing precisely, because "does EXPOSE open the port?" is a stock evaluator question — the answer is no.

### `USER`

```dockerfile
USER www-data
```

Drops privileges for subsequent instructions and at runtime. Containers run as root by default, which is a real risk worth mitigating in production. In Inception it's often impractical (nginx wants to bind 443, MariaDB wants specific ownership) but knowing *why* you stayed root is better than not having considered it.

### `HEALTHCHECK`

```dockerfile
HEALTHCHECK --interval=10s --timeout=3s --retries=3 \
  CMD mariadb-admin ping -h localhost || exit 1
```

Lets Docker report a container as `healthy` / `unhealthy` rather than merely `running`. Combined with `depends_on: condition: service_healthy` in compose, it's the clean fix for the WordPress-starts-before-MariaDB-is-ready race. See [`04_CONTAINERS.md`](04_CONTAINERS.md).

### `ENTRYPOINT` and `CMD`

The two everyone confuses. Both define what runs at start; they compose:

| Dockerfile | Runs |
|---|---|
| `CMD ["nginx", "-g", "daemon off;"]` | `nginx -g "daemon off;"` |
| `ENTRYPOINT ["nginx"]` + `CMD ["-g", "daemon off;"]` | `nginx -g "daemon off;"` |
| `ENTRYPOINT ["/entrypoint.sh"]` + `CMD ["php-fpm82", "-F"]` | `/entrypoint.sh php-fpm82 -F` |

- **`CMD`** is the default command, easily overridden: `docker run myimage sh` replaces it entirely.
- **`ENTRYPOINT`** is the fixed executable; `CMD` becomes its default arguments. `docker run myimage --foo` appends rather than replaces.

The **setup-script pattern** is what you want for MariaDB and WordPress: `ENTRYPOINT ["/entrypoint.sh"]` with `CMD ["mariadbd"]`, where the script does initialization and then hands off. Which brings us to the important part.

---

## 3. PID 1 — the section that decides your project

The subject bans `tail -f`, `sleep infinity`, `while true`, and `bash` as an entrypoint, then tells you to "read about PID 1 and the best practices for writing Dockerfiles." That instruction is not decoration. Here's what it's pointing at.

### The container lives exactly as long as PID 1

Your `ENTRYPOINT`/`CMD` process **is** PID 1 in the container's PID namespace. When it exits, the kernel tears down the namespace and everything in it dies. The container's lifetime *is* PID 1's lifetime. Not a Docker rule — a kernel behaviour.

### Why daemons break this

Every service you're installing wants to **daemonize**: fork a background copy, and have the parent exit immediately so your shell returns. That's correct behaviour on a normal system with an init managing services.

In a container it's fatal:

```dockerfile
CMD ["nginx"]        # nginx forks a daemon, parent exits, PID 1 is gone,
                     # container stops. Instantly.
```

Docker *is* your init. Nothing needs to background anything. So you tell each service to stay in the foreground:

| Service | Foreground invocation | What it means |
|---|---|---|
| nginx | `nginx -g "daemon off;"` | Inject the `daemon off` directive |
| php-fpm | `php-fpm82 -F` | `-F` = stay in foreground |
| MariaDB | `mariadbd` (not `mariadbd_safe`) | Run the server directly, not the wrapper |

The `_safe` wrappers deserve a note: `mysqld_safe` is a shell script that restarts the server if it crashes. Under Docker, that's the **restart policy's** job — a supervisor inside the container duplicates it and hides failures from Docker's view.

### Why `tail -f /dev/null` is worse than ugly

The tempting hack:

```dockerfile
CMD ["sh", "-c", "nginx && tail -f /dev/null"]   # ✗ FORBIDDEN
```

It "works" — the container stays up. It's banned for four concrete reasons, and you should be able to give at least two:

1. **Signals go nowhere useful.** `docker stop` sends SIGTERM to PID 1 — now `tail`. `tail` doesn't forward it. nginx never hears it, never shuts down gracefully. After the 10s grace period everything gets SIGKILL'd. For MariaDB, that's a hard kill mid-transaction on *every single stop*.
2. **PID 1 ignores signals it doesn't handle.** As covered in [`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md), PID 1 has no default signal dispositions. A process that doesn't explicitly handle SIGTERM ignores it — so this isn't even a slow shutdown, it's no shutdown.
3. **Docker loses sight of the service.** The container is "up" as long as `tail` is alive. nginx can crash and Docker will never know — no restart, no unhealthy status. The restart policy the subject requires (rule V5) becomes decorative.
4. **Zombies accumulate.** PID 1 is supposed to reap orphaned children. `tail` doesn't. Long-running containers slowly fill the process table.

**The correct fix is always the same: make the real service PID 1, in the foreground.**

### The entrypoint script pattern, done right

For MariaDB and WordPress you need setup before the service starts. The shape:

```sh
#!/bin/sh
set -e

# ... initialization: create DB, users, wp-config, whatever ...

exec "$@"      # ← the important line
```

**`exec` is the whole point.** Without it, the shell stays PID 1 and your service becomes a *child* — signals hit the shell, which doesn't forward them, and you've reinvented the `tail -f` problem with extra steps. `exec` **replaces** the shell process with your service, so the service inherits PID 1 directly.

You've done this before: `exec` here is the same `execve()` you called in minishell — the child image is replaced, the PID is kept.

Two more requirements on that script:

- **`set -e`** — abort on the first failure. Without it a failed setup step is ignored and the service starts against a broken state.
- **Idempotence.** The script runs on *every* start, not just the first. The volume may already hold an initialized database. Every step must be "create it if it isn't there" — an unconditional `CREATE DATABASE` fails on the second `up`, which is exactly the sequence an evaluator runs.

### Exec form vs shell form

```dockerfile
CMD ["nginx", "-g", "daemon off;"]      # exec form — JSON array
CMD nginx -g "daemon off;"              # shell form — becomes /bin/sh -c "..."
```

**Always use exec form.** Shell form wraps your command in `/bin/sh -c`, which makes *the shell* PID 1, with the same signal-forwarding problem. (Some shells `exec` a lone final command as an optimization — relying on that is relying on an implementation detail.)

Exec form is also why `CMD ["echo", "$HOME"]` prints a literal `$HOME`: there's no shell to expand it. If you genuinely need shell features, do it deliberately in your entrypoint script and `exec` at the end.

---

## 4. Two worked examples

The shape, not the full files — writing those is the project.

**nginx** — the certificate needs the domain name, which is the one place people get bitten:

```dockerfile
FROM alpine:3.XX

ARG DOMAIN_NAME              # ← required: RUN cannot see runtime env

RUN apk add --no-cache nginx openssl \
 && mkdir -p /etc/nginx/ssl /var/www/html

RUN openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
      -keyout /etc/nginx/ssl/inception.key \
      -out    /etc/nginx/ssl/inception.crt \
      -subj "/C=FR/ST=IDF/L=Paris/O=42/CN=${DOMAIN_NAME}"

COPY conf/nginx.conf /etc/nginx/nginx.conf

EXPOSE 443
CMD ["nginx", "-g", "daemon off;"]
```

> **The trap.** `env_file:` and `environment:` in compose are **runtime only** — they are invisible to `RUN`, which executes at build time. Without the `ARG` line *and* a matching `args:` in compose, `${DOMAIN_NAME}` expands to an empty string and you get a certificate with an empty `CN` — which fails silently until a browser complains. Compose side:
>
> ```yaml
> nginx:
>   build:
>     context: ./requirements/nginx
>     args:
>       DOMAIN_NAME: ${DOMAIN_NAME}
> ```
>
> The alternative — often cleaner — is to generate the certificate in an **entrypoint script** instead, where the runtime environment does exist. Then no `ARG` is needed at all. Pick one; don't half-do both.

**MariaDB** — needs initialization, so entrypoint + CMD:

```dockerfile
FROM alpine:3.XX

RUN apk add --no-cache mariadb mariadb-client

COPY conf/my.cnf /etc/my.cnf
COPY tools/entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

EXPOSE 3306
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["mariadbd", "--user=mysql"]
```

Note what's *absent* from both: no passwords, no `latest`, no `tail -f`, no `ENV MYSQL_ROOT_PASSWORD=`. Credentials arrive at runtime via secrets and env — never at build time.

---

## 5. Multi-stage builds

Not needed for Inception, but the answer to a common bonus question. Build in one image, ship from another, so compilers and toolchains never reach production:

```dockerfile
FROM golang:1.XX AS builder
WORKDIR /src
COPY . .
RUN go build -o /app

FROM alpine:3.XX
COPY --from=builder /app /app
CMD ["/app"]
```

The final image contains the binary and nothing else — no Go toolchain, no source. Relevant to a static-site bonus with a build step.

---

## 6. Checklist for every Dockerfile you write

- [ ] `FROM` pins an explicit, penultimate-stable tag — never `latest`
- [ ] No password, key, or token anywhere — not in `ENV`, not in `ARG`, not in a `COPY`'d file
- [ ] A `.dockerignore` excludes `.git`, `secrets/`, `.env`
- [ ] Stable instructions first, volatile `COPY`s last
- [ ] Related `RUN`s chained; cleanup in the same layer that created the mess
- [ ] `CMD`/`ENTRYPOINT` in **exec form** (JSON array)
- [ ] The real service is PID 1, running in the **foreground**
- [ ] Any entrypoint script ends with `exec "$@"`, starts with `set -e`, and is idempotent
- [ ] No `tail -f`, `sleep infinity`, `while true`, or `bash` as the entrypoint

---

## 7. 🚪 Gate

1. You changed one line in `conf/nginx.conf` and rebuilt. Which layers were reused, and why exactly those?
2. Why does `CMD ["nginx"]` exit immediately while `CMD ["nginx","-g","daemon off;"]` doesn't?
3. Why is `exec "$@"` the last line of an entrypoint script rather than just `"$@"`?
4. Does `EXPOSE 443` make the port reachable from your browser?
5. You wrote a password in a `RUN`, then deleted the file in the next `RUN`. Is it gone?

---

**Next:** [`04_CONTAINERS.md`](04_CONTAINERS.md) — running the images you just built.
