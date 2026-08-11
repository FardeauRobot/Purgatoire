# 08 — Secrets and environment variables

> The subject is blunt: *any credentials, API keys, or passwords found in your Git repository will result in project failure.* This is the one rule where being wrong once costs the whole project — and where the damage is done at commit time, not at defense time.

Do this page's §6 **before** you write your first password.

---

## 1. The two mechanisms

| | Environment variables | Docker secrets |
|---|---|---|
| Exposed as | Process environment | File at `/run/secrets/<name>` |
| Declared in | `.env` + `env_file:` / `environment:` | `secrets:` block, backed by a file |
| Visible in `docker inspect` | **Yes, plain text** | No |
| Inherited by child processes | Yes | No |
| Can be baked into an image | Yes, via `ENV`/`ARG` | No |
| Use for | Configuration | Credentials |

The short rule: **if leaking it would matter, it's a secret; otherwise it's an environment variable.**

---

## 2. Why environment variables leak

Not theoretical. Every one of these is a real path by which an env var escapes:

```bash
docker inspect mariadb | grep -A20 '"Env"'    # plain text, to anyone with daemon access
docker compose config                          # prints resolved values
docker compose exec mariadb env                # the whole environment
cat /proc/1/environ                             # inside the container
```

Beyond that:

- **Child processes inherit the entire environment.** A PHP script that shells out passes your database password to whatever it spawned, whether or not that program has any business with it.
- **Crash handlers and logging frameworks dump environments.** Error pages, Sentry-style reporters, and verbose logs routinely include the full environment. This is how production credentials end up in a third-party dashboard.
- **`ENV` in a Dockerfile is permanent.** It's stored in image metadata. Anyone with the image has the value, forever, in every layer that follows.
- **`ARG` is not safer.** Build args are recorded in the build history — `docker history <image>` prints them.

None of this makes env vars bad. It makes them wrong *for credentials*.

---

## 3. How Docker secrets work

```yaml
secrets:
  db_password:
    file: ../secrets/db_password.txt

services:
  mariadb:
    secrets:
      - db_password
```

At runtime the file's contents appear inside the container at `/run/secrets/db_password`, on a `tmpfs` (in memory, never written to the container's disk). The service reads it as a file:

```sh
DB_PASS=$(cat /run/secrets/db_password)
```

Not in the environment. Not in `docker inspect`. Not in any image layer. Not inherited by children.

### The honest caveat — say this at defense

With plain `docker compose` — which is what Inception uses — `secrets:` reads a **local file on the host** and mounts it in. **It is not encrypted at rest.** The "encrypted in the cluster's Raft log, distributed over mutual TLS, never persisted on the node" story belongs to **Docker Swarm** and Kubernetes, not to compose on a laptop.

So what do you actually gain here? Real, and narrower than the word "secret" implies:

1. The value never enters the environment, closing every leak path in §2.
2. It can't be baked into an image.
3. It's a file with permissions, opened deliberately at the moment it's needed.
4. Everything confidential lives in one directory, which is one obvious thing to gitignore — and that is the failure mode the subject actually punishes.

Being able to state both the benefit and its limit is worth more at defense than reciting "secrets are encrypted", which in this setup isn't true.

---

## 4. The split for Inception

**`srcs/.env`** — configuration. Mandatory per rule R3.

```bash
DOMAIN_NAME=<login>.42.fr

MYSQL_DATABASE=wordpress
MYSQL_USER=wpuser
MYSQL_HOST=mariadb

WP_TITLE=Inception
WP_ADMIN_USER=captain          # ← must NOT contain admin/administrator (rule V7)
WP_ADMIN_EMAIL=captain@42.fr
WP_USER=redacted
WP_USER_EMAIL=redacted@42.fr
```

**`secrets/`** — credentials.

```
secrets/
├── db_password.txt          # the WordPress DB user's password
├── db_root_password.txt     # MariaDB root password
└── credentials.txt          # WordPress admin/user passwords
```

One value per file, and **watch the trailing newline** — `echo "pass" > f` appends `\n`, which becomes part of the password when you `cat` it. Use `printf '%s' 'pass' > f`, or strip it when reading. This produces a wonderfully baffling "the password is correct but authentication fails" bug.

### Rule V7 — the admin username

The WordPress administrator's name must not contain `admin` or `administrator`, in any casing or combination. Rejected: `admin`, `Admin`, `administrator`, `Administrator`, `admin-123`, `wpadmin`, `theadministrator`. Pick something unrelated. Evaluators check this specifically, and it's a silly way to lose points.

---

## 5. Tracing a password end to end

The defense question is *"show me where this password lives."* The answer, for the WordPress DB user:

```
1. secrets/db_password.txt          host file, gitignored, mode 600
                ↓  compose `secrets:` block
2. /run/secrets/db_password         tmpfs inside mariadb AND wordpress
                ↓  entrypoint script reads it
3. $DB_PASS in the script's memory   never exported, never in the environment
                ↓  passed to the setup command
4. MariaDB's user table              hashed, in the volume
4'. wp-config.php                    plaintext, in the wordpress volume
```

Two things to be honest about at step 4′: **WordPress requires the database password in plaintext in `wp-config.php`**. That's WordPress's design, not something you can configure away. The mitigation is that the file sits in a volume, is not in the repository, and is only readable inside the container.

And be precise at step 2: the secret is mounted into *every service that declares it*. Both MariaDB and WordPress need this one. MariaDB's **root** password should be declared only for MariaDB — scope each secret to the services that genuinely need it, and be able to say why.

---

## 6. Git hygiene — do this first

**Before** creating any file that will contain a credential:

```gitignore
# Inception
srcs/.env
secrets/
*.pem
*.key
*.crt
```

Then prove it:

```bash
git check-ignore -v srcs/.env secrets/db_password.txt
git status --ignored | grep -E 'secrets|\.env'
```

`git check-ignore -v` prints which rule matched. If it prints nothing, the file is **not** ignored — fix it now.

### If you already committed one

`git rm` does not help. The blob stays in history and is recoverable from any clone. The real fix:

1. **Rotate the credential.** Assume it's compromised. This step is not optional and it comes first.
2. Rewrite history — `git filter-repo` (or BFG). `filter-branch` is deprecated and slow.
3. Force-push, and tell anyone with a clone to re-clone.

For this project, if it's early and the history is yours alone, the pragmatic route is often to rewrite the history properly rather than to hope nobody looks. The subject's wording is "found in your Git repository", and history is part of the repository.

### A commit-time safety net

Optional, but it makes the mistake structurally hard rather than merely discouraged:

```bash
# .git/hooks/pre-commit  (chmod +x)
#!/bin/sh
if git diff --cached --name-only | grep -qE '(^|/)(\.env$|secrets/)'; then
    echo "refusing: staged file matches .env or secrets/"
    exit 1
fi
```

---

## 7. Provide-a-file, not a value: the `_FILE` convention

Many official images accept `MYSQL_PASSWORD_FILE=/run/secrets/db_password` alongside `MYSQL_PASSWORD=…`. You're building your own images, so you implement the equivalent yourself — in your entrypoint:

```sh
#!/bin/sh
set -e

read_secret() {
    if [ -f "$1" ]; then
        tr -d '\n' < "$1"
    else
        echo "missing secret: $1" >&2
        exit 1
    fi
}

DB_PASS=$(read_secret /run/secrets/db_password)
DB_ROOT_PASS=$(read_secret /run/secrets/db_root_password)

# ... use them, never export them ...

exec "$@"
```

Note `tr -d '\n'` — the trailing-newline fix from §4. And note the absence of `export`: a local shell variable dies with the script, while an exported one is inherited by everything the script spawns, which would put you straight back in §2.

---

## 8. Checklist

- [ ] `.gitignore` covers `srcs/.env`, `secrets/`, and certificate files — **written before the first credential exists**
- [ ] `git check-ignore -v` confirms it, rather than you assuming it
- [ ] No password in any Dockerfile — not `ENV`, not `ARG`, not a `COPY`'d file (rule I4)
- [ ] `.dockerignore` in each service excludes `secrets/` and `.env` from the build context
- [ ] Secrets read as files, never exported into the environment
- [ ] Secret files have no trailing newline, or the reader strips it
- [ ] Each secret is declared only for the services that need it
- [ ] `WP_ADMIN_USER` contains no form of `admin` (rule V7)
- [ ] `git log -p | grep -i password` returns nothing interesting

---

## 9. 🚪 Gate

1. Name three ways an environment variable leaks that a secret doesn't.
2. Are Docker secrets encrypted at rest under `docker compose`? Answer precisely.
3. Trace the DB password from disk to the running MariaDB. Where does it exist along the way?
4. You committed `.env` three commits ago and deleted it since. Is the project safe?
5. Why is `wp-config.php` holding a plaintext password acceptable, when a Dockerfile holding one isn't?

---

**Next:** [`09_MAC_AND_LINUX.md`](09_MAC_AND_LINUX.md) — running it on both platforms.
