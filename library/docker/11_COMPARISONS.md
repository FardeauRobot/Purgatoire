# 11 — The Four Comparisons

> The subject (Chapter VI) requires your `README.md` to compare four pairs. This page is the source material — written long here so you can write it short there, and so you can defend it out loud when the evaluator picks one and says "explain."

For each pair: **the one-sentence answer**, the mechanism underneath, an honest table, and **the trap** — the follow-up question that catches people who memorized the table without understanding it.

---

## 1. Virtual Machines vs Docker

### The one-sentence answer

A virtual machine virtualizes **hardware** and boots its own kernel; a container virtualizes the **operating system** and borrows the host's kernel.

### The mechanism

A hypervisor presents fake hardware — CPU, disk, NIC — to a guest OS, which boots a full kernel and believes it owns a machine. Everything is duplicated: kernel, init system, system libraries, the lot.

A container is a **process on the host kernel** that has been given a restricted view of the world. Linux namespaces isolate what it can *see* (PIDs, mounts, network interfaces, hostnames, users), cgroups limit what it can *use* (CPU, memory, I/O). Nothing is emulated and nothing is booted. `docker run alpine sh` is closer to `execve()` than to powering on a computer.

```
      VIRTUAL MACHINES                      CONTAINERS
  ┌──────┬──────┬──────┐              ┌──────┬──────┬──────┐
  │ App  │ App  │ App  │              │ App  │ App  │ App  │
  │ libs │ libs │ libs │              │ libs │ libs │ libs │
  ├──────┼──────┼──────┤              └──────┴──────┴──────┘
  │Guest │Guest │Guest │  ← 3 kernels  ┌────────────────────┐
  │kernel│kernel│kernel│               │  Docker Engine     │
  ├──────┴──────┴──────┤               ├────────────────────┤
  │    Hypervisor      │               │   HOST KERNEL      │ ← one, shared
  ├────────────────────┤               ├────────────────────┤
  │    Host kernel     │               │    Hardware        │
  ├────────────────────┤               └────────────────────┘
  │     Hardware       │
  └────────────────────┘
```

### The comparison

| | Virtual Machine | Container |
|---|---|---|
| **Kernel** | Its own, booted from scratch | The host's, shared with every other container |
| **Isolation boundary** | Hardware-enforced by the hypervisor | Kernel features (namespaces, cgroups) |
| **Startup** | Seconds to minutes — it's a boot | Milliseconds — it's a process spawn |
| **Size** | Gigabytes (whole OS) | Megabytes (just your app + its deps) |
| **Density** | A handful per host | Hundreds per host |
| **Guest OS choice** | Any OS, any kernel | Must share the host's kernel type |
| **Overhead** | Full OS running per instance | Essentially the process itself |
| **Isolation strength** | Stronger — escape means breaking the hypervisor | Weaker — escape means breaking the kernel |

### When each wins

**VM:** you need a different kernel or OS than the host; you need hard multi-tenant isolation for untrusted code; you're running something that wants to be a machine (a full desktop, a legacy appliance).

**Container:** you're packaging an application and its dependencies; you want fast, repeatable, identical environments; you want density and quick restarts.

**They're not rivals.** Almost every container in production is running inside a VM — your cloud provider gives you a VM, and you pack containers into it. Inception itself has you run Docker inside a VM, and that's not a contradiction: the VM is the isolation boundary between you and the school's machine, the containers are the boundary between the three services.

### 🪤 The trap

> *"So Docker is just a lightweight VM?"*

No — and the giveaway is that a container **cannot run a different kernel**. You cannot run a Windows container on a Linux host, or an arbitrary Linux distro's kernel on macOS. What you *can* do is run a different **userland**: Alpine's userland on a Debian host works fine, because both speak to the same kernel through the same syscall interface. The distro is just files.

Which is exactly why Docker on macOS quietly runs a Linux VM — there's no Linux kernel on a Mac to borrow. See [`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md) and [`09_MAC_AND_LINUX.md`](09_MAC_AND_LINUX.md).

---

## 2. Secrets vs Environment Variables

### The one-sentence answer

Environment variables are **inherited, inspectable, and leaky by design**; secrets are **files, mounted at runtime, scoped to the service that needs them**.

### The mechanism

An environment variable set on a container is stored in the container's configuration and injected into the process environment. That means it is visible:

- in `docker inspect <container>` — in plain text, to anyone who can talk to the daemon
- in `/proc/<pid>/environ` inside the container
- **to every child process**, forever, whether they need it or not
- in `docker compose config` output
- frequently in crash dumps, error pages, and logging frameworks that helpfully dump the environment
- and if set via `ENV` in a Dockerfile, **baked into the image layer** — anyone who pulls the image has it

A Docker secret is instead exposed as a **file inside the container**, at `/run/secrets/<name>`, on an in-memory filesystem. It isn't in the image, isn't in the environment block, isn't printed by `docker inspect`, and isn't inherited by child processes — a process has to deliberately open the file.

### The honest caveat (say this at defense and you'll stand out)

With plain `docker compose` — which is what Inception uses — `secrets:` reads a **local file on the host** and mounts it into the container. It is *not* encrypted at rest. The "encrypted in the cluster's Raft log, transmitted over mutual TLS, never written to disk on the node" story belongs to **Docker Swarm** and Kubernetes, not to compose on your laptop.

So what do you actually gain in Inception? Real, but narrower than the buzzword suggests:

1. The value never appears in the environment, so it can't leak through `inspect`, child processes, or an environment-dumping error page.
2. It's not in an image layer, so it can't ship with the image.
3. It's a file with its own permissions, read at the moment it's needed, rather than an ambient global.
4. The secret file lives in `secrets/`, one obvious directory to gitignore — which is the failure mode the subject actually cares about.

### The comparison

| | Environment variable | Docker secret |
|---|---|---|
| **Exposed as** | Process environment | File at `/run/secrets/<name>` |
| **Visible in `docker inspect`** | Yes, in plain text | No (only the mount) |
| **Inherited by child processes** | Yes, automatically | No — must be read deliberately |
| **Can end up in an image layer** | Yes, via `ENV` or `ARG` | No |
| **Encrypted at rest** | No | Only in Swarm/K8s, **not** in plain compose |
| **Scope** | Whole container | Only services that declare it |
| **Good for** | Config: hostnames, ports, usernames, paths | Credentials: passwords, API keys, tokens |

### What Inception requires

Both, for different jobs. The subject **mandates** a `.env` file for environment variables and **strongly recommends** secrets for anything confidential. The clean split:

- `.env` → `DOMAIN_NAME`, `MYSQL_DATABASE`, `MYSQL_USER`, `WP_ADMIN_USER` — things that are configuration, not credentials
- `secrets/` → `db_password.txt`, `db_root_password.txt`, `credentials.txt` — things that would matter if leaked

Both are gitignored. **Credentials found in your git repository are an immediate project failure**, and that includes a commit you later reverted — the blob is still in the history. See [`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md).

### 🪤 The trap

> *"Your `.env` isn't in git — so it's a secret too, right?"*

No. Gitignoring is about **repository** exposure. Secrets-vs-env is about **runtime** exposure — who can read the value on a running system. A gitignored `.env` still lands in `docker inspect` in plain text. They defend against two different threats, which is precisely why the subject asks for both.

---

## 3. Docker Network vs Host Network

### The one-sentence answer

A user-defined Docker network gives each container **its own network stack** plus name-based DNS between containers; host networking **deletes that separation** and puts the container directly on the host's stack.

### The mechanism

**User-defined bridge network.** Each container gets its own network namespace: its own interfaces, routing table, iptables rules, port space. Docker creates a virtual ethernet pair — one end inside the container, one end plugged into a bridge on the host — and runs an **embedded DNS server at `127.0.0.11`** that resolves *service names to container IPs*. Nothing from the outside reaches in unless you explicitly publish a port.

This is why, in Inception, WordPress connects to the host `mariadb` and no file anywhere configures that mapping. The service name in `docker-compose.yml` becomes a resolvable hostname on the network. (This is a property of *user-defined* networks; the legacy default `bridge` network doesn't do it, which is why the old `--link` flag existed — and why it's now deprecated and forbidden by the subject.)

**Host network** (`network_mode: host`). The container gets no network namespace of its own — it shares the host's. Binding port 80 inside the container *is* binding port 80 on the host. No port mapping happens because none is needed. No DNS-by-name, because there's no Docker network to serve it.

### The comparison

| | User-defined Docker network | Host network |
|---|---|---|
| **Network namespace** | Its own, isolated | The host's, shared |
| **DNS between containers** | Yes — service name resolves | No |
| **Port publishing** | Explicit, via `ports:` | None — ports are already on the host |
| **Port conflicts** | Impossible between containers | Two containers wanting 443 collide |
| **External exposure** | Only what you publish | Every port the process opens |
| **Performance** | One NAT/veth hop | Marginally faster, no NAT |
| **Portability** | Same everywhere | Linux only — meaningless on macOS/Windows |

The performance argument is the one people reach for, and it's mostly irrelevant. The bridge hop costs microseconds and a little throughput at very high packet rates. For a WordPress site serving a handful of requests, it is unmeasurable. Don't offer performance as your reason for anything in this project.

### Why Inception forbids host networking

`network_mode: host`, `--link`, and `links:` are all explicitly banned, and the `networks` line **must** be present in your compose file. The reason is the architecture the subject is teaching:

**nginx must be the only way in, on port 443, over TLS.** With host networking, MariaDB's 3306 and php-fpm's 9000 would be bound directly on the host — reachable from outside, bypassing nginx entirely. The single controlled entrypoint would be a fiction. The isolated network is what makes the guarantee true rather than aspirational: 3306 and 9000 exist only inside the Docker network, and the only port crossing the boundary is the one you published.

### 🪤 The trap

> *"Show me that MariaDB isn't reachable from outside."*

From the host: `curl telnet://localhost:3306` fails, because nothing is published. From the WordPress container: `docker compose exec wordpress nc -zv mariadb 3306` succeeds. Same port, two vantage points — that's the isolation, demonstrated rather than asserted. Have this ready.

---

## 4. Docker Volumes vs Bind Mounts

### The one-sentence answer

A bind mount maps a **path you choose on the host** into the container; a named volume is a storage object **Docker creates, names, and manages** for you.

### The mechanism

**Bind mount.** You point at an existing host path: `/home/me/site:/var/www`. The container sees whatever is at that path right now. It depends entirely on the host's directory layout and file permissions, and Docker manages nothing about it.

**Named volume.** Docker creates a storage area (with the default `local` driver, under `/var/lib/docker/volumes/<name>/_data`), gives it a name, and tracks it as an object you can `ls`, `inspect`, `rm`, back up, or move to another host. Its lifecycle is independent of any container.

**The behavioural difference that actually bites**, and that nobody expects: mount an **empty named volume** onto a container directory that already has content in the image, and Docker **copies the image's content into the volume** on first use. A bind mount does the opposite — it *shadows* the directory, and the image content becomes invisible. Same syntax position in the compose file, opposite outcomes. This is a favourite evaluator question, and it's the source of the classic "my WordPress directory is empty" bug.

### The comparison

| | Named volume | Bind mount |
|---|---|---|
| **Created by** | Docker | You — the path must exist |
| **Location** | Docker-managed (`/var/lib/docker/volumes/…`) | Anywhere you point |
| **Lifecycle** | Independent object; survives `down`, removed by `down -v` | None — it's just a host directory |
| **Managed via CLI** | `docker volume ls / inspect / rm` | Not tracked at all |
| **Empty-mount behaviour** | Image content is **copied in** | Image content is **hidden** |
| **Permissions** | Docker initializes them | Inherited from the host, frequent UID/GID pain |
| **Portability** | Same config works on any host | Tied to one machine's layout |
| **Best for** | Persistent application data — databases, uploads | Development: live-editing source from your editor |

### What Inception requires — and the apparent contradiction

The subject says two things that look incompatible:

1. You **must** use named volumes. Bind mounts are **not allowed** for the two persistent stores.
2. Both volumes **must** store their data in `/home/<login>/data` on the host.

Requirement 2 sounds like a bind mount. The resolution is the `local` driver's `driver_opts`, which let a *named* volume be backed by a specific host path:

```yaml
volumes:
  wordpress:
    driver: local
    driver_opts:
      type: none
      device: /home/<login>/data/wordpress
      o: bind
```

**Be honest about this at defense**, because a sharp evaluator will press on it: yes, `o: bind` means the kernel performs a bind mount underneath. What makes it a named volume rather than a bind mount is everything *around* the storage — it's a declared, named object with a Docker-managed lifecycle, it appears in `docker volume ls`, services reference it by name rather than by host path, and it can be inspected, backed up, and removed through the volume API. What the subject forbids is the short-form `- /host/path:/container/path` in a service's `volumes:` block; what it wants is a named volume whose backing location you chose deliberately. Claiming "this isn't a bind mount at all" is the wrong answer. Explaining the layering is the right one.

Note also the ordering constraint: `/home/<login>/data/wordpress` must **exist before** `docker compose up`, or the mount fails. That's what the `Makefile` is for. Full detail in [`06_STORAGE.md`](06_STORAGE.md).

### 🪤 The trap

> *"You ran `docker compose down`. Is your data gone?"*

No — `down` removes containers and networks; volumes survive. `down -v` is what destroys them, and that `-v` is the difference between a redeploy and a catastrophe. Know which one your `Makefile`'s targets call, because you will be asked, and "I think `make clean` does the safe one" is not an answer.

---

## Writing these into the README

The README wants these **short** — a paragraph or a small table each, inside the **Project description** section, alongside your explanation of Docker's role and the sources in `srcs/`. This page is the long form you read from; don't paste it wholesale.

The test of whether your README version is good enough: an evaluator reads your three sentences, asks one follow-up, and you answer without hedging. Everything above the 🪤 markers is what you write; everything at them is what you'll be asked.
