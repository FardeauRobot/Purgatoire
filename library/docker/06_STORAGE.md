# 06 — Storage: volumes, bind mounts, and the Inception pattern

> The container's writable layer dies with the container. Everything you want to keep has to live somewhere else. This page is about *where*, and about the one pattern Inception forces you into.

---

## 1. The default: everything is temporary

A container gets a thin writable layer on top of the image's read-only layers ([`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md)). Writes land there. `docker rm` deletes it.

Prove it:

```bash
docker run --name tmp1 alpine sh -c 'echo hello > /data.txt'
docker rm tmp1
docker run --rm alpine cat /data.txt      # ← no such file
```

For a database this is catastrophic and for a WordPress upload directory it's merely infuriating. Hence the three mount types.

---

## 2. The three mount types

| | Named volume | Bind mount | tmpfs |
|---|---|---|---|
| **Lives** | Docker-managed area | A host path you pick | RAM only |
| **Survives container** | Yes | Yes | No |
| **Managed by Docker** | Yes — `docker volume …` | No | No |
| **Portable** | Yes | No — tied to a host layout | Yes |
| **Use for** | Databases, uploads, app state | Live-editing source in dev | Secrets, scratch |

### Named volumes

```bash
docker volume create mydata
docker run -v mydata:/var/lib/mysql mariadb-img
docker volume inspect mydata     # ← shows Mountpoint on the host
```

With the default `local` driver the bytes end up under `/var/lib/docker/volumes/mydata/_data`. Docker created it, named it, and tracks it as an object with its own lifecycle: it exists before the container, survives it, and is only removed by an explicit `docker volume rm` or `docker compose down -v`.

### Bind mounts

```bash
docker run -v /home/me/site:/var/www/html nginx-img
```

Whatever is at that host path appears inside. Nothing is managed, nothing is tracked. Excellent for development — edit in your editor, the container sees it immediately, no rebuild. Poor for anything portable, because the config now encodes one machine's directory layout.

### tmpfs

```bash
docker run --tmpfs /run/secrets alpine
```

In memory, never written to disk, gone when the container stops. This is how Docker exposes secrets at `/run/secrets` ([`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md)).

---

## 3. The behaviour that catches everyone

Named volumes and bind mounts do **opposite things** when the container directory already has content in the image.

**Empty named volume onto a populated directory → Docker copies the image's content into the volume.**

```
image has /var/www/html/{index.php, wp-admin/, …}
mount empty volume at /var/www/html
→ volume is populated from the image. Files are visible.
```

**Bind mount onto a populated directory → the host path shadows it.**

```
image has /var/www/html/{index.php, wp-admin/, …}
bind-mount empty /home/me/empty at /var/www/html
→ directory appears EMPTY. Image content is hidden, not deleted.
```

Same syntactic position, opposite outcomes. This is the source of the classic *"I installed WordPress in the Dockerfile and now the directory is empty"* bug, and it's a favourite evaluator question.

Two follow-on details worth knowing:

- The copy happens **only if the volume is empty**. Once it has content, the volume always wins — which is why editing files in your image and rebuilding appears to do nothing until you remove the volume.
- The shadowing is a normal Linux mount. The underlying files are still there, just covered. Unmount and they reappear.

---

## 4. The Inception pattern

The subject requires three things that look mutually exclusive:

1. Two **named volumes** — one for the database, one for the WordPress files
2. **Bind mounts are not allowed** for them
3. Both must store their data in **`/home/<login>/data`** on the host

Requirement 3 sounds exactly like a bind mount. The resolution is `driver_opts` on the `local` driver, which lets a *named* volume be backed by a host path you choose:

```yaml
volumes:
  wordpress:
    driver: local
    driver_opts:
      type: none
      device: /home/<login>/data/wordpress
      o: bind
  mariadb:
    driver: local
    driver_opts:
      type: none
      device: /home/<login>/data/mariadb
      o: bind
```

Read literally, these are arguments to the `mount(2)` syscall:

- `type: none` — no filesystem type; we're not mounting a device
- `o: bind` — the mount option, i.e. perform a bind mount
- `device: …` — the source path

So: **yes, there is a bind mount underneath.** Do not claim otherwise at defense; a sharp evaluator will ask and confident wrongness is worse than the honest answer.

### The honest answer

What makes this a named volume rather than a bind mount is everything around the storage:

| | Short-form bind (forbidden) | Named volume with `driver_opts` (required) |
|---|---|---|
| Declared as | `- /host/path:/container/path` in the service | A named entry under top-level `volumes:` |
| Referenced by | Host path | Name — `wordpress:/var/www/html` |
| Appears in `docker volume ls` | No | Yes |
| Has a Docker lifecycle | No | Yes — created, inspected, removed via the API |
| Backup / migration | Manual, host-specific | Through the volume API |

The subject forbids the *short form in a service block*. It requires a *declared, named, Docker-managed volume object* — while also pinning where its bytes land. The kernel mechanism is shared; the management model is not. That distinction is the answer.

### The directory must exist first

`type: none` + `o: bind` does **not** create the directory. If `/home/<login>/data/wordpress` is missing, the mount fails and the container won't start. That's what the Makefile is for:

```make
DATA_DIR = /home/$(USER)/data

up:
	mkdir -p $(DATA_DIR)/wordpress $(DATA_DIR)/mariadb
	docker compose -f srcs/docker-compose.yml up -d --build
```

This is also the most common "works for me, fails on the evaluator's machine" cause: the directory existed on yours because you made it by hand weeks ago, and nothing in the repo recreates it.

---

## 5. Which volume goes where

```yaml
services:
  mariadb:
    volumes:
      - mariadb:/var/lib/mysql            # MariaDB's data directory

  wordpress:
    volumes:
      - wordpress:/var/www/html           # the site files

  nginx:
    volumes:
      - wordpress:/var/www/html           # the SAME volume, read by nginx
```

The WordPress volume is mounted in **two** containers. That's deliberate and it's how PHP-FPM setups work: nginx serves static assets straight off disk and only forwards `.php` requests to php-fpm over port 9000. Both processes need the same files.

Not a mistake, not a violation — just two containers sharing one volume, which is a normal thing to do and a good thing to be able to explain.

---

## 6. Commands you'll use

```bash
docker volume ls                       # what exists
docker volume inspect <name>           # driver, options, real mountpoint
docker volume rm <name>                # delete (fails if in use)
docker volume prune                    # delete all unused — careful

docker compose down                    # containers + networks. VOLUMES SURVIVE.
docker compose down -v                 # ← -v destroys the volumes. know this.
```

**Know which of these your Makefile targets call.** `make clean` running `down -v` when the evaluator expected `down` is a live demonstration of data loss. Name your targets so the destructive one is obviously destructive — `fclean` for `down -v`, `down` for the safe one.

Verify persistence properly:

```bash
docker compose down && docker compose up -d
# → your WordPress post is still there
ls /home/<login>/data/mariadb           # → actual database files, on the host
```

That second command is the one that proves rule N4, and — see below — it only really works on Linux.

---

## 7. The macOS caveat

`device: /home/<login>/data/wordpress` is resolved **inside the Linux VM** that Docker Desktop or colima runs, not on your Mac's filesystem. There is no `/home/<login>` on macOS; user directories live at `/Users/<login>`.

So on your Mac, the stack may come up fine while `ls /home/<login>/data` shows nothing — because the path exists in the VM, invisibly. **Rule N4 can only be demonstrated on a real Linux host**, which is one of the better reasons the subject insists on a VM.

Details in [`09_MAC_AND_LINUX.md`](09_MAC_AND_LINUX.md).

---

## 8. 🚪 Gate

1. Where do the bytes of a `local`-driver named volume live on the host?
2. You mount an empty named volume onto a directory that has files in the image. What do you see? And with a bind mount instead?
3. Inception bans bind mounts but demands data in `/home/<login>/data`. Explain the resolution — including what's genuinely bind-like about it.
4. `docker compose down` — is your data gone? What about `down -v`?
5. Why is the same WordPress volume mounted into two different containers?

---

**Next:** [`07_COMPOSE.md`](07_COMPOSE.md) — declaring all of this in one file.
