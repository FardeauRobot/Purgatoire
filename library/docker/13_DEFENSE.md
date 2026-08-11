# 13 — Defense cheatsheet

> The page you skim 15 minutes before evaluation. Short answers only — the long versions are elsewhere in this library, and if you need the long version now, you needed it last week.

---

## The 60-second project summary

*"Three containers on one private Docker network: nginx terminating TLS on 443, WordPress with php-fpm on 9000, and MariaDB on 3306. nginx is the only thing exposed to the host. Two named volumes hold the database and the site files, both backed by `/home/<login>/data`. Everything is built from Alpine — no pre-made images — orchestrated by docker compose, driven by a Makefile."*

Have this ready. It sets the frame and answers three questions before they're asked.

---

## Conceptual questions

**What is a container?**
A process on the host kernel with a restricted view of the world — namespaces control what it sees, cgroups what it uses. Not a machine, not a VM.

**Container vs VM?**
A VM boots its own kernel on virtualized hardware. A container shares the host's kernel. That single difference produces every other one: milliseconds instead of minutes, megabytes instead of gigabytes, and weaker isolation because the kernel is shared. → [`11_COMPARISONS.md`](11_COMPARISONS.md)

**So it's a lightweight VM?**
No — a container can't run a different kernel. Windows containers need a Windows kernel. What varies is the userland; the distro is just files.

**Why does Docker on macOS need a VM?**
Namespaces and cgroups are Linux kernel features. macOS's kernel is Darwin and doesn't have them. Docker Desktop runs a Linux VM and puts the daemon inside it.

**What's the difference between an image and a container?**
An image is a stack of read-only layers plus metadata. A container is a running instance with a writable layer on top. Class and instance.

**What's a layer?**
A filesystem diff produced by one Dockerfile instruction. They're content-addressed, cached, and shared between images — which is why builds are fast and containers are cheap.

---

## Dockerfile questions

**Why not `latest`?**
It resolves to whatever is newest at build time, so the build isn't reproducible — today's image and next month's differ with no change to your files. The subject bans it.

**What's "penultimate stable" and why?**
N-1: the release before the current one. The newest release is where packaging bugs live; an explicit older tag is what makes a build reproducible.

**`CMD` vs `ENTRYPOINT`?**
`ENTRYPOINT` is the fixed executable, `CMD` its default arguments. `CMD` alone is easily overridden by `docker run image <something>`; `ENTRYPOINT` isn't.

**Why exec form (`["a","b"]`) rather than shell form?**
Shell form wraps everything in `/bin/sh -c`, making the shell PID 1. Signals then hit the shell, which doesn't forward them, and your service never shuts down cleanly.

**Does `EXPOSE` open a port?**
No. It's documentation. Only `ports:` / `-p` publishes anything.

**You deleted a file in a later `RUN` — is it gone from the image?**
No. The delete writes a whiteout marker; the bytes remain in the earlier layer and are extractable. This is why passwords must never appear in a Dockerfile at any point.

---

## PID 1 — expect at least one of these

**Why does your container stay running?**
Because PID 1 is the service itself, in the foreground: `nginx -g "daemon off;"`, `php-fpm82 -F`, `mariadbd`. The container's lifetime is PID 1's lifetime.

**Why not `tail -f /dev/null`?**
Four reasons, any two will do: signals sent to PID 1 reach `tail`, which doesn't forward them, so the service never shuts down cleanly; PID 1 ignores signals it doesn't explicitly handle, so it's not even a slow shutdown; Docker can't see the real service, so it can't restart it when it dies; and zombies never get reaped.

**Why `exec "$@"` at the end of an entrypoint script?**
`exec` replaces the shell with the service, so the service becomes PID 1 and receives signals directly. Without it the shell stays PID 1 and you've reproduced the `tail -f` problem.

**Why `mariadbd` and not `mariadbd_safe`?**
`_safe` is a wrapper that restarts the server on crash. That's the restart policy's job — a supervisor inside the container hides failures from Docker.

---

## Networking questions

**How does WordPress find the database?**
The compose service name `mariadb` is a DNS name on the user-defined network. Docker runs an embedded DNS server at `127.0.0.11` that resolves it. Nothing configures the mapping.

**Why not `network_mode: host`?**
It removes the container's network namespace, so MariaDB's 3306 and php-fpm's 9000 would bind directly on the host and be reachable from outside. nginx would stop being the only entrypoint. Also forbidden by the subject, and meaningless on macOS.

**Prove MariaDB isn't exposed.**
```bash
curl telnet://localhost:3306                          # fails from the host
docker compose exec wordpress nc -zv mariadb 3306     # succeeds from inside
```

**Why can three containers each use port 443?**
Each has its own network namespace and therefore its own port space.

---

## Storage questions

**Volume vs bind mount?**
A volume is a Docker-managed, named object with its own lifecycle. A bind mount is a host path you choose, managed by nobody. → [`11_COMPARISONS.md`](11_COMPARISONS.md)

**The subject bans bind mounts but wants data in `/home/<login>/data`. Explain.**
A named volume with `driver_opts` on the `local` driver: `type: none`, `o: bind`, `device: /home/<login>/data/x`. Yes, there's a bind mount underneath at the kernel level — the difference is the management model: it's a declared, named object with a Docker lifecycle, referenced by name, visible in `docker volume ls`, rather than a raw host path in a service block.

*(Don't claim there's no bind mount involved. Explaining the layering is the answer that lands.)*

**`docker compose down` — is data lost?**
No. `down` removes containers and networks. `down -v` destroys volumes. In my Makefile, `down` is safe and `fclean` is the destructive one.

**Mount an empty volume onto a populated directory — what happens?**
With a named volume, the image's content is copied into the volume. With a bind mount, the host path shadows it and the directory looks empty. Opposite behaviours, same syntax position.

**Why is the WordPress volume in two containers?**
nginx serves static files directly off disk and forwards only `.php` to php-fpm. Both need the same files.

---

## Secrets questions

**Secrets vs environment variables?**
Env vars land in the process environment: visible in `docker inspect`, inherited by children, potentially baked into an image. Secrets are files at `/run/secrets/<name>` on tmpfs — none of those paths apply.

**Are Docker secrets encrypted?**
In Swarm and Kubernetes, yes. **Under plain `docker compose`, no** — the file is read from the host and mounted in. The gain here is that the value never enters the environment and never lands in an image layer.

*(Saying this precisely is a differentiator. Most people assert "encrypted" and can't defend it.)*

**Why does `wp-config.php` contain a plaintext password?**
WordPress requires it; it's not configurable. It's mitigated by living in a volume, not the repository, readable only inside the container.

**Where's your `.env`, and why isn't it a secret?**
Gitignored, in `srcs/`. It holds configuration — domain, database name, usernames — not credentials. Gitignoring protects the repository; secrets protect the runtime. Different threats.

---

## Architecture questions

**Why three containers?**
One responsibility each: independent restart, independent rebuild, no dependency conflicts. Required by the subject, and the reason containers are useful at all.

**Why isn't nginx in the WordPress container?**
The subject forbids it, and it would break one-service-per-container. php-fpm speaks FastCGI, nginx speaks HTTP; they're separate concerns that happen to sit next to each other.

**Can you curl php-fpm?**
No — it speaks FastCGI, a binary protocol, not HTTP. Test it with `nc -zv wordpress 9000`.

**How do containers restart on crash?**
`restart: unless-stopped` on every service. It restarts on failure and survives a host reboot, but respects a container I stopped by hand.

**`depends_on` guarantees MariaDB is ready?**
No — only that it's *started*. Readiness needs a healthcheck with `condition: service_healthy`, or a wait loop in the entrypoint.

---

## Live-modification prep

The subject allows a small on-the-spot change. Know where these live in your files:

| Asked to… | You edit |
|---|---|
| Change the admin username | `srcs/.env`, then rebuild |
| Add a second static page | nginx `conf/` + the WordPress volume |
| Change the TLS version | nginx `conf/` — `ssl_protocols` |
| Add an environment variable | `.env` + the service's `env_file`/`environment` |
| Change the restart policy | `docker-compose.yml`, per service |
| Add a healthcheck | Dockerfile `HEALTHCHECK` or compose `healthcheck:` |
| Change where volumes live | `driver_opts: device:` + the Makefile's `mkdir` |

---

## Final pre-defense checklist

Run these, don't recall them:

```bash
make fclean && make                         # builds from nothing
docker compose ps                            # 3 up, none restarting
curl -k https://<login>.42.fr | head         # WordPress HTML
openssl s_client -connect <login>.42.fr:443 -tls1_1   # must FAIL
ls -la /home/<login>/data/{mariadb,wordpress}         # data on the host
docker compose down && docker compose up -d  # post still there
sudo reboot                                  # then verify it all came back
git log -p | grep -iE 'password|secret'      # nothing
grep -rn "latest" srcs/                      # nothing
grep -rnE "tail -f|sleep infinity|while true" srcs/   # nothing
```

Plus: two WordPress users exist, the admin's name contains no form of `admin`, and `README.md` / `USER_DOC.md` / `DEV_DOC.md` are all present with their required sections.

---

## If you don't know

Say so, then reason out loud. *"I'm not certain — but based on how volumes work, I'd expect…"* Evaluators are assessing whether you understand the system, and visible reasoning from correct principles scores better than a confident wrong answer. Guessing with certainty is the thing that actually costs you.
