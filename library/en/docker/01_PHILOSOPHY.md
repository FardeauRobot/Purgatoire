# 01 — Philosophy: what Docker is for

> *"It works on my machine."* — every developer, once, before being asked to prove it.

Docker is not a virtualization product that happens to be fast. It's a **packaging format** with a runtime attached. Understanding which problem it was built to solve makes the rest of its design read as obvious rather than arbitrary.

---

## 1. The problems it actually solved

### Problem 1 — Environment drift

Software doesn't run on code. It runs on code *plus* a specific OpenSSL, a specific glibc, a particular PHP with a particular set of extensions, an environment variable someone set in 2019, and a config file in `/etc` that a colleague edited by hand and never wrote down.

Your machine has one set of those. The server has another. Your teammate has a third. The code is identical; the behaviour isn't. Every hour spent on "but it works locally" is this problem.

Docker's answer: **ship the environment with the application.** The image contains the userland — libraries, binaries, config, the lot — so the only thing that varies between machines is the kernel. It's not "here's my app, hope you have the right PHP"; it's "here's my app *and* its PHP."

**You've felt this already.** libft compiles on the school's Linux and on your Mac because you were careful about portability. Now imagine that program needed a specific `libxml2` minor version. That's the class of problem Docker is aimed at.

### Problem 2 — Dependency hell

Two applications on one server. One needs PHP 7.4, the other PHP 8.2. System package managers install *one* version of a package system-wide. Your options used to be: two servers, or an evening with `update-alternatives` and regret.

Containers make dependency conflicts structurally impossible. Each container carries its own userland, so PHP 7.4 and PHP 8.2 coexist without either being aware the other exists. This is the direct reason Inception insists on one service per container — it's not a stylistic preference, it's the property that makes the whole model work.

### Problem 3 — Slow, unrepeatable provisioning

Before containers, "give me a machine like the production one" meant a VM image measured in gigabytes, minutes to boot, and a wiki page of manual post-install steps that was out of date the moment it was written.

Docker replaces the wiki page with a **Dockerfile**: an executable, version-controlled, reviewable description of the environment. If it's not in the Dockerfile, it doesn't exist. Setup becomes a diff you can read in a pull request.

### Problem 4 — The gap between dev, staging, and production

The same image artifact runs on a laptop, in CI, and in production. Not "the same recipe" — the *same bytes*, identified by the same content hash. When something breaks in production, you can pull that exact image locally and reproduce it.

---

## 2. What it deliberately did *not* solve

Worth knowing, because overclaiming is how defenses go wrong.

- **It is not a security boundary against untrusted code.** Containers share the host kernel. A kernel exploit escapes. For genuinely hostile workloads you want a VM, or something like gVisor or Firecracker. Docker's isolation is about *tidiness and conflict avoidance* first, security second.
- **It does not make software portable across architectures.** An image built for arm64 does not run on amd64 without emulation. Directly relevant to you on Apple Silicon — see [`09_MAC_AND_LINUX.md`](09_MAC_AND_LINUX.md).
- **It does not manage state well on its own.** Databases in containers work, but persistence, backup, and migration are still your problem. Volumes are a mechanism, not a strategy.
- **It is not orchestration.** Docker runs containers on one host. Restarting failed containers across a fleet, rolling updates, scheduling — that's Swarm, Kubernetes, Nomad. Compose is a convenience for one machine, not a production control plane.

---

## 3. The shipping container metaphor, taken seriously

The name isn't decoration, and the analogy is unusually exact.

Before standardized shipping containers, cargo was **break-bulk**: barrels, sacks, crates, each a different shape, each loaded by hand. Every combination of cargo and vessel and port was a bespoke problem. Loading a ship took days.

The intermodal container standardized the **interface**, not the contents. A box of fixed dimensions with fixed corner fittings. Ports stopped caring what was inside. The same crane, the same truck chassis, the same ship handles every box. Loading took hours.

The mapping:

| Shipping | Docker |
|---|---|
| Standard container dimensions | The image format |
| Crane, truck, ship — indifferent to contents | The Docker Engine — indifferent to your app |
| Port | Host machine |
| Bill of lading | Image manifest / tag |
| Don't open the box to move it | Don't need to know a container's internals to run it |

The insight both share: **you get leverage by standardizing the interface and letting the contents vary.** Nginx, a Go binary, and a Postgres cluster are wildly different things, and `docker run` treats them identically.

Where the analogy breaks: physical containers are sealed and inert. Docker containers share a kernel with their host and with each other. The box is standardized, but the floor underneath is common — which is exactly the isolation caveat above.

---

## 4. A short lineage — none of this is new

Docker invented remarkably little. Its contribution was **ergonomics**: taking a decade of Unix isolation primitives and making them usable by someone who isn't a kernel engineer.

| Year | What | Contribution |
|---|---|---|
| 1979 | `chroot` | Change a process's apparent root directory. First "your `/` is not the real `/`." |
| 2000 | FreeBSD Jails | chroot plus isolated networking, users, and process visibility |
| 2005 | Solaris Zones | Full OS-level virtualization with resource controls |
| 2006 | cgroups (Google) | Limit and account CPU/memory/IO per process group. Merged into Linux 2.6.24. |
| 2002–2013 | Linux namespaces | Isolate mounts, PIDs, networks, IPC, hostnames, users — added one at a time over a decade |
| 2008 | LXC | Combined namespaces + cgroups into usable "Linux containers". Powerful, and painful to use. |
| 2013 | **Docker** | LXC's power plus an image format, a build file, a registry, and a one-line CLI |
| 2015 | OCI | Vendor-neutral standards for image format and runtime, so Docker isn't the only implementation |

The lesson: Docker's real invention is the **image** — a layered, content-addressed, shareable filesystem with a build recipe — plus the registry that makes sharing trivial. The isolation was already in the kernel and had been for years. Nobody was using it because the interface was miserable.

Say this at defense if the chance arises. "Docker didn't invent containers, it made them usable — the kernel primitives predate it by a decade" is the kind of sentence that shows you read past the tutorial.

---

## 5. Alpine and musl — why we chose the harder road

Inception lets you pick Alpine or Debian. We picked Alpine, and the reasoning is worth being able to state.

**Debian would teach you less.** You know `apt`. You know `/etc/init.d`. Building Inception on Debian, you'd lean on familiar Linux-admin reflexes and pass without your container intuition changing much.

**Alpine removes the crutches.** It's ~8MB, uses `musl` libc instead of `glibc`, `busybox` instead of GNU coreutils, and `apk` instead of `apt`. There is no init system. There is no service manager. If a daemon needs to keep running, *you* have to know how to keep it in the foreground — which is exactly the PID 1 lesson the subject is pushing you toward in [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md).

**The friction is the curriculum.** Alpine's known rough edges are real and worth meeting deliberately:

- **musl vs glibc.** Binaries compiled against glibc won't run. Anything distributed as a pre-built `.so` or vendor binary may simply refuse.
- **DNS resolution differences.** musl's resolver historically handled multiple `nameserver` entries and large responses differently from glibc's — a classic source of "it resolves on my Ubuntu image but not on Alpine."
- **Slimmer everything.** BusyBox utilities lack flags their GNU equivalents have. Scripts silently behave differently.

**The honest counterweight:** Alpine has not won outright. Plenty of production stacks deliberately use `debian-slim` because these differences caused real incidents — Python in particular saw significant performance regressions on musl, and the ecosystem's pre-built wheels assume glibc. "Alpine because it's smaller" is a shallow answer. "Alpine because I wanted the container-native constraints, and here's what it cost me" is a good one.

---

## 6. The three ideas to carry forward

1. **Immutability.** You don't patch a running container; you rebuild the image and replace it. Containers are cattle, not pets. This is why "I fixed it with `docker exec`" is not a fix — the next `up` reverts it.
2. **One responsibility per container.** Not one process — nginx forks workers, php-fpm forks children — but one *job*. It's what makes independent scaling, restarting, and reasoning possible, and it's why Inception mandates three containers instead of one convenient box.
3. **Declarative over imperative.** The Dockerfile and compose file describe the *desired state*. You don't script the steps to get there; you describe the destination and let the engine converge. Every time you're tempted to fix something by hand, you're breaking this.

---

**Next:** [`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md) — how the kernel actually pulls this off.
