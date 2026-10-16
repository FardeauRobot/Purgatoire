# 12 — Troubleshooting

> Symptom → diagnosis. The goal is to stop guessing: each entry starts with the command that narrows the problem, not with a fix to try.

---

## 0. The first three commands, always

```bash
docker compose ps -a                    # what's running, exited, restarting?
docker compose logs --tail 50 <service> # what did it say before dying?
docker compose config                   # what does compose actually see?
```

Most of what follows is a refinement of these. Resist changing files before you've run them.

---

## 1. The container exits immediately

**`docker compose ps -a` shows `Exited (0)`**

Exit code **0** means clean exit — your daemon forked into the background and the foreground process finished. It's the PID 1 bug, every time.

```dockerfile
CMD ["nginx"]                          # ✗ daemonizes
CMD ["nginx", "-g", "daemon off;"]     # ✓
```

Same shape for php-fpm (`-F`) and MariaDB (`mariadbd`, not the `_safe` wrapper). Full explanation in [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md).

**`Exited (127)`** — command not found. The binary isn't in the image, or the name is wrong. Alpine package names often differ from Debian's: it's `php82-fpm`, not `php-fpm`, and the binary may be `php-fpm82`. Check:

```bash
docker run --rm --entrypoint sh <image> -c 'which php-fpm82 || ls /usr/sbin'
```

**`Exited (126)`** — found but not executable. You forgot `chmod +x` on the entrypoint script:

```dockerfile
COPY tools/entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh
```

Also produced by CRLF line endings — a script starting `#!/bin/sh\r` fails with a baffling "not found" referring to the *interpreter*, not the script. `file entrypoint.sh` will say "CRLF line terminators".

**`Exited (1)` with empty logs** — get inside and run it by hand:

```bash
docker run -it --entrypoint sh <image>
# then run your CMD manually and watch
```

**`Restarting (N)` in a loop** — the restart policy is masking a crash. The logs still hold the first failure:

```bash
docker compose logs --tail 100 <service>
```

---

## 2. nginx returns 502 Bad Gateway

502 means nginx couldn't reach its upstream. Narrow it in this order:

```bash
docker compose ps                                  # 1. is wordpress running?
docker compose exec nginx nc -zv wordpress 9000    # 2. is 9000 reachable?
docker compose logs wordpress                      # 3. what does php-fpm say?
```

Common causes:

- **php-fpm is listening on a socket, not a TCP port.** Alpine's default `www.conf` often uses `listen = 127.0.0.1:9000` or a Unix socket. It must be `listen = 9000` (all interfaces) — `127.0.0.1` inside the container means *that container's* loopback, unreachable from nginx.
- **`fastcgi_pass` points at the wrong name.** It must be the compose **service** name: `fastcgi_pass wordpress:9000;`
- **The WordPress container is up but php-fpm crashed inside it.** `docker compose exec wordpress ps aux`.
- **Missing `fastcgi_params`.** nginx needs `include fastcgi_params;` and `SCRIPT_FILENAME` set, or php-fpm receives a request it can't route.

---

## 3. "Error establishing a database connection"

```bash
docker compose exec wordpress ping -c2 mariadb        # DNS?
docker compose exec wordpress nc -zv mariadb 3306     # port open?
docker compose logs mariadb                            # is it initialized?
```

- **`ping` fails** → networking. Both services on the same network? Using the service name? See [`05_NETWORKING.md`](05_NETWORKING.md).
- **`ping` works, `nc` fails** → MariaDB isn't listening. Check `bind-address` in `my.cnf`: `127.0.0.1` restricts it to the container's own loopback. It needs `0.0.0.0`.
- **Both work, auth fails** → credentials. The classic cause is a **trailing newline** in a secret file ([`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md)). Verify:

```bash
docker compose exec mariadb sh -c 'xxd /run/secrets/db_password | tail -1'
```

If it ends `0a`, that newline is part of your password.

- **Worked on first build, fails after `down`/`up`** → your entrypoint isn't idempotent. It ran `CREATE USER` against a volume that already has the user, failed, and left initialization half-done. See §7.

---

## 4. WordPress directory is empty / nginx serves 403

Almost always the volume-shadowing behaviour from [`06_STORAGE.md`](06_STORAGE.md).

```bash
docker compose exec nginx ls -la /var/www/html
docker compose exec wordpress ls -la /var/www/html
```

- **Both empty** → WordPress was never downloaded, or was downloaded to a different path than the volume mount.
- **Populated in `wordpress`, empty in `nginx`** → they're not sharing the same named volume. Check both service blocks reference the identical volume name.
- **You added files to the image and they don't appear** → the volume already has content and wins over the image. The image-to-volume copy only happens when the volume is **empty**. Remove it and rebuild: `docker compose down -v && docker compose up -d --build`.

403 specifically usually means nginx found the directory but no index file, or lacks permission. Check `index index.php;` is in your config and that ownership inside the volume is sane.

---

## 5. Volume / mount failures

**`failed to mount local volume: no such file or directory`**

The `driver_opts` bind requires the host directory to **exist first**:

```bash
mkdir -p /home/<login>/data/wordpress /home/<login>/data/mariadb
```

Put that in the Makefile's `up` target, not in your shell history ([`07_COMPOSE.md`](07_COMPOSE.md)).

**Data disappeared after a rebuild** — check whether you ran `down -v`. `-v` destroys volumes; `down` alone doesn't. Check what your Makefile targets actually invoke.

**`ls /home/<login>/data` is empty on macOS** — expected, not a bug. That path resolves inside the Linux VM. See [`09_MAC_AND_LINUX.md`](09_MAC_AND_LINUX.md).

---

## 6. TLS problems

**`curl: (35) error / handshake failure`**

```bash
openssl s_client -connect <login>.42.fr:443 -tls1_2    # must work
openssl s_client -connect <login>.42.fr:443 -tls1_1    # must FAIL (rule V1)
```

If 1.2 fails too, your nginx is likely restricting to 1.3 only, or the certificate is missing/unreadable.

**`curl: (60) SSL certificate problem: self signed certificate`** — expected. Self-signed certificates are what the subject asks for. Use `curl -k`, and in a browser accept the warning. This is not a bug to fix.

**Browser shows a warning** — same thing. Be ready to explain why: a real certificate requires a public CA validating a domain you own, and `<login>.42.fr` is a local hosts entry.

---

## 7. Works the first time, breaks on the second `up`

The single most-tested failure mode, because evaluators run `down` then `up`.

Your entrypoint runs on **every** start, against a volume that may already be initialized. Unconditional setup fails the second time:

```sh
# ✗ fails once the DB exists
mariadb -e "CREATE DATABASE $MYSQL_DATABASE;"

# ✓ idempotent
mariadb -e "CREATE DATABASE IF NOT EXISTS $MYSQL_DATABASE;"
```

The general pattern — check for an initialization marker and skip:

```sh
if [ ! -d /var/lib/mysql/mysql ]; then
    # first run only: install the system tables, create DB and users
fi
exec "$@"
```

Same for WordPress: `wp core install` should be guarded by `wp core is-installed`.

**Test it deliberately, every time you touch an entrypoint:**

```bash
docker compose down && docker compose up -d && docker compose logs -f
```

---

## 8. Port and permission problems on the host

**`bind: address already in use`**

```bash
sudo lsof -i :443          # what has it?
```

Usually a host nginx/apache, or a previous run that didn't come down. `docker compose down` first.

**`permission denied while trying to connect to the Docker daemon socket`**

```bash
sudo usermod -aG docker $USER && newgrp docker
```

**`no space left on device`**

```bash
docker system df
docker system prune -a          # careful: removes unused images
```

Image layers accumulate quickly across rebuilds. A 20GB VM disk fills faster than you'd think.

---

## 9. Build problems

**`apk add` fails: `unable to select packages`** — the package name doesn't exist in that Alpine release. Names change between versions, and PHP packages are versioned (`php82-fpm`, `php83-fpm`). Search inside the image:

```bash
docker run --rm alpine:3.XX sh -c 'apk update && apk search php | grep fpm'
```

**A change to a config file doesn't take effect** — the build cache reused the layer, or the file is overridden by a volume. Force it:

```bash
docker compose build --no-cache <service>
```

**The build is slow every time** — your `COPY . .` sits above the package install, so every edit invalidates everything below. Reorder ([`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md)).

---

## 10. When you're properly stuck

1. **`docker compose config`** — is compose seeing what you think? Empty `${VAR}` interpolations show up here.
2. **Get inside the image, not the container**: `docker run -it --entrypoint sh <image>`. This gives you the built filesystem without your entrypoint running, which separates "the image is wrong" from "the startup is wrong".
3. **Bisect the stack.** Comment out two services and get one working alone. Three unknowns and one symptom is guesswork.
4. **Read the logs of the container that's actually failing**, not the one showing the error. A 502 from nginx is usually a WordPress problem.
5. **Rebuild from truly clean** — `docker compose down -v --rmi all` — to check you're not depending on state that isn't in the repository. Do this before declaring anything finished, because it's what the evaluator's machine does by definition.

---

**Next:** [`13_DEFENSE.md`](13_DEFENSE.md) — the questions you'll be asked.
