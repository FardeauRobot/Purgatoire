# 00 — Roadmap: what to learn, in what order

> The order matters more than the content. Almost everyone who struggles with Inception struggles because they opened `docker-compose.yml` on day one. Compose is an *orchestration* layer — it's meaningless until you know what it's orchestrating.

Each phase below has three parts:

- **Read** — the page in this library.
- **Do** — hands on keyboard. Non-negotiable; reading about containers teaches you nothing.
- **🚪 Gate** — say the answer **out loud, without notes**. If you can't, don't advance. This is also what the subject's Chapter IV is asking of you: only keep what you can take responsibility for.

Rough total: **10–14 days** at a normal pace. Phases 0–7 are learning (~5 days), 8–10 are building (~5 days), 11 is polish.

---

## Phase 0 — Get a container running (½ day)

Before any theory. You need something to poke at.

**Do:**
```bash
docker run hello-world
docker run -it alpine sh          # you are now "inside". look around.
ps aux                            # ← how many processes?
ls /                              # ← whose filesystem is this?
exit                              # ← where did the container go?
docker ps -a                      # ← there it is, dead
```

**Read:** [`09_MAC_AND_LINUX.md`](09_MAC_AND_LINUX.md) — install first, on whichever machine you're at.

**🚪 Gate:** Inside that Alpine shell, `ps aux` showed 2 processes and `ls /` showed a filesystem that isn't yours. What did you actually connect to? Where did those files come from?

---

## Phase 1 — The model: a container is a process (1 day)

The single most important phase. Everything downstream is a consequence of this.

**Read:** [`01_PHILOSOPHY.md`](01_PHILOSOPHY.md), then [`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md).

**Connect it to what you know:** you already built most of this by hand. In minishell you did `fork()` + `execve()` — a container is `fork()` + `execve()` with extra flags that hand the child its own PID table, its own mount table, its own network stack. `chroot` gave you a fake `/`. Namespaces are chroot generalized to *every* kind of resource. There is no magic here, only syscalls you haven't met yet (`clone`, `unshare`, `setns`).

**Do:** on a Linux box, `docker run -d alpine sleep 300`, then from the **host** run `ps aux | grep sleep`. Your container's process is right there in your host's process list, with a normal host PID. Now `docker exec <id> ps aux` — inside, the same process is PID 1. Same process, two different numbers. Sit with that until it's obvious.

**🚪 Gate:** Why can a container start in 50ms when a VM takes 30 seconds? Answer in terms of what each one has to boot.

---

## Phase 2 — Images and the Dockerfile (1½ days)

**Read:** [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md).

**Do:** write a Dockerfile from scratch that serves a static file with nginx on Alpine. Not for the project — as a finger exercise. Then:
```bash
docker build -t test .            # watch the layers
docker build -t test .            # again — everything CACHED
# change one line near the top, rebuild, watch the cache collapse
docker history test               # every layer, and what it cost you
```

**The lesson to extract:** instruction order is not style, it's performance. Things that change rarely go at the top; things that change every build go at the bottom.

**🚪 Gate:** You edited a config file and rebuilt. Which layers were reused, which were recomputed, and why exactly those? And: what's the difference between `COPY` and a volume mount, given that both put files in the container?

---

## Phase 3 — PID 1, or why your container keeps dying (½ day — but the highest-value half-day here)

This is the phase that separates people who pass from people who patch. The subject bans `tail -f`, `sleep infinity`, `while true`, and `bash` as entrypoint — and then tells you, pointedly, to go read about PID 1.

**Read:** [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md) (the PID 1 section) and [`04_CONTAINERS.md`](04_CONTAINERS.md).

**Do:** deliberately break it. Write a Dockerfile whose `CMD` starts nginx the normal daemon way (`nginx`), build it, run it, watch the container exit instantly. Then fix it with `nginx -g "daemon off;"`. Feel the difference.

**🚪 Gate:** Why does `CMD ["nginx"]` exit immediately while `CMD ["nginx", "-g", "daemon off;"]` doesn't? And *why* is `tail -f /dev/null` a bad fix rather than just an ugly one? (Hint: what happens to your service when Docker sends SIGTERM?)

---

## Phase 4 — Networking (½ day)

**Read:** [`05_NETWORKING.md`](05_NETWORKING.md).

**Do:**
```bash
docker network create testnet
docker run -d --name alice --network testnet alpine sleep 300
docker run --rm --network testnet alpine ping -c2 alice   # works. nothing configured it.
docker run --rm alpine ping -c2 alice                     # fails. why?
```

**🚪 Gate:** Nothing in your config maps the name `alice` to an IP. What resolved it? Why did the second command fail? And why does Inception forbid `network_mode: host`?

---

## Phase 5 — Storage (½ day)

**Read:** [`06_STORAGE.md`](06_STORAGE.md).

**Do:** run a container, write a file, `docker rm` it, run a fresh one — the file is gone. Now redo it with a named volume attached. Then find that volume on the host with `docker volume inspect`.

**🚪 Gate:** Where, exactly, on the host filesystem, do the bytes of a named volume live? What's the difference between that and a bind mount, given that both survive the container? And: Inception forbids bind mounts but demands the data land in `/home/<login>/data` — how is that not a contradiction?

---

## Phase 6 — Compose (½ day)

Only now. By this point compose should read as a boring YAML transcription of things you can already do by hand — which is exactly what it is.

**Read:** [`07_COMPOSE.md`](07_COMPOSE.md).

**Do:** take your Phase 2 nginx image and the Phase 4 network and write a two-service compose file that reproduces them. Then `docker compose up`, `down`, `logs`, `ps`.

**🚪 Gate:** For each block in your compose file, what's the equivalent `docker run` flag? If you can't map it back, you're copying, not writing.

---

## Phase 7 — Secrets and environment (½ day)

**Read:** [`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md).

**Do:** set up `.gitignore` **before** you write a single credential. Then `git check-ignore -v srcs/.env` to prove it's covered. Do this now, not later — a password in a single commit is a failed project even if you delete it in the next one.

**🚪 Gate:** Trace one password from the file on disk to the running MariaDB process. How many places does it exist along the way, and which of them can another container read?

---

## Phase 8 — Build Inception, bottom-up (3–4 days)

**Read:** [`10_INCEPTION.md`](10_INCEPTION.md) fully before starting.

Build in dependency order, and **get each service green before starting the next**. Resist building all three and debugging at the end — with three unknowns and one symptom you'll be guessing.

| Step | Service | Done when |
|---|---|---|
| 8.1 | Skeleton — dirs, Makefile, compose, `.env`, `.gitignore` | `docker compose config` prints a valid resolved file |
| 8.2 | **MariaDB** | Container stays up; you can log in from inside it and the WP database + both users exist |
| 8.3 | **WordPress + php-fpm** | Container stays up, listening on 9000, connects to `mariadb` by name |
| 8.4 | **nginx** | `curl -k https://localhost` returns WordPress HTML |
| 8.5 | TLS + domain | `<login>.42.fr` resolves and serves; TLS 1.2/1.3 only, verified |
| 8.6 | Persistence | `docker compose down` then `up` — your post is still there |

**🚪 Gate per step:** the service restarts cleanly from scratch (`down -v`, `up --build`) without you touching anything by hand. If it needs a manual nudge, it's not done.

---

## Phase 9 — Harden and verify against the subject (½ day)

Walk [`10_INCEPTION.md`](10_INCEPTION.md)'s rule list line by line against your actual files. Every rule, checked, not remembered. The dashboard artifact has this as a tick-list.

**🚪 Gate:** Every box ticked, each one verified by running something — not by recalling that you probably did it.

---

## Phase 10 — The graded documentation (½ day)

`README.md`, `USER_DOC.md`, `DEV_DOC.md`. These are scored deliverables with mandatory sections, not an afterthought. Source material: [`11_COMPARISONS.md`](11_COMPARISONS.md) and [`10_INCEPTION.md`](10_INCEPTION.md).

Keep the AI-usage log current as you go — the README must state which tasks and which parts of the project used AI, and reconstructing that from memory at the end is how people end up writing something vague and getting questioned on it.

**🚪 Gate:** Hand `USER_DOC.md` to someone who has never seen the project. Can they start it and log into the admin panel with only that file?

---

## Phase 11 — Defense prep (½ day)

**Read:** [`13_DEFENSE.md`](13_DEFENSE.md), then [`12_TROUBLESHOOTING.md`](12_TROUBLESHOOTING.md).

**Do:** the best rehearsal is sabotage. Have someone break one thing — comment out a line in nginx's config, kill the DB container, corrupt an env var — and diagnose it live. Evaluators sometimes ask for exactly this kind of small on-the-spot modification.

**🚪 Gate:** Someone points at a random line in any of your files and asks "what happens if I delete this?" — and you know.

---

## The honest shortcut warning

You can copy a working Inception off GitHub in twenty minutes. It'll run. You will then fail the defense, because every question above is asked about *your* reasoning, and there won't be any.

The reverse is also true, and worth more: the four hours you spend on Phases 1–3 are the ones that make the remaining ten days feel mechanical.
