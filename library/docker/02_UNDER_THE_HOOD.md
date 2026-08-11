# 02 — Under the hood: a container is a process

> If you take one page from this library, take this one. Everything else is a consequence.

---

## 1. The claim

**A container is a normal Linux process that has been lied to.**

There is no container object in the kernel. `grep -r container` in the Linux source finds nothing that corresponds to what Docker calls a container. What exists is a set of independent kernel features:

- **namespaces** — control what a process can *see*
- **cgroups** — control what a process can *use*
- **union filesystems** — assemble a root directory out of stacked layers
- **capabilities / seccomp / LSMs** — control what a process is *allowed to do*

Docker picks values for all of these, calls `clone()`, and gives the result a name. That bundle is the container.

**Prove it to yourself** on any Linux host:

```bash
docker run -d --name proof alpine sleep 300
ps aux | grep "sleep 300"       # ← on the HOST. there it is. a normal process.
docker exec proof ps aux        # ← inside, the same process is PID 1
```

One process. Two PIDs. Nothing is emulated, nothing was booted. Compare with a VM, where the guest's processes are invisible to the host — the host sees one process, `qemu`, and the guest's process table is inside a simulated machine.

**Connect it to what you know:** in minishell you called `fork()` then `execve()`. Docker calls `clone()` — `fork()`'s more configurable sibling — with flags like `CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET`, then `execve()`. Same two steps you already wrote. The flags are the entire difference.

---

## 2. Namespaces — controlling what a process sees

A namespace wraps a global kernel resource so that processes inside it see their own private instance. Linux has several; each is independent, and a process can be in any combination.

| Namespace | Flag | Isolates | Container consequence |
|---|---|---|---|
| **PID** | `CLONE_NEWPID` | Process IDs | Your process is PID 1; can't see or signal host processes |
| **Mount** | `CLONE_NEWNS` | Mount table | Its own filesystem tree — `/` is the image, not yours |
| **Network** | `CLONE_NEWNET` | Interfaces, routes, ports, iptables | Own IP, own port space — two containers can both bind 443 |
| **UTS** | `CLONE_NEWUTS` | Hostname, domain name | `hostname` returns the container ID, not the host's |
| **IPC** | `CLONE_NEWIPC` | Shared memory, semaphores, queues | Can't reach another container's SysV IPC |
| **User** | `CLONE_NEWUSER` | UID/GID mappings | Can be root inside, unprivileged outside |
| **Cgroup** | `CLONE_NEWCGROUP` | cgroup root view | Hides the host's cgroup layout |

### The PID namespace, concretely

The PID namespace is the one you'll actually reason about, because it's where the Inception rules come from.

```
HOST view                          CONTAINER view
─────────                          ──────────────
PID 1     systemd
PID 843   dockerd
PID 9022  nginx: master     ←→     PID 1   nginx: master
PID 9051  nginx: worker     ←→     PID 6   nginx: worker
```

The same processes, renumbered. The container's PID 1 is an ordinary process on the host, and this has consequences that Docker's entire lifecycle model rests on:

- **PID 1 is special to the kernel.** It's meant to be an init: it reaps orphaned children, and it gets different default signal handling.
- **When PID 1 exits, the namespace is torn down** and every other process in it is killed. This is *the* reason a container ends when its main process ends — not a Docker policy, a kernel behaviour.
- **PID 1 doesn't get default signal handlers.** A normal process that ignores SIGTERM still dies, because the kernel's default action applies. PID 1 has no default action — a process that doesn't explicitly handle SIGTERM will simply *ignore* it. This is why `docker stop` on a badly-built container hangs for 10 seconds and then gets SIGKILL'd.

That last point is the real reason the subject bans `tail -f /dev/null`. It's not that it's ugly — it's that `tail` becomes PID 1, doesn't handle SIGTERM, doesn't forward signals to your actual service, and doesn't reap zombies. Your database gets SIGKILL'd mid-write on every shutdown. Full treatment in [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md).

### The mount namespace and the root filesystem

The container's `/` isn't `chroot` — it's `pivot_root` into a filesystem Docker assembled from the image's layers. That's why `ls /` inside Alpine shows Alpine's tree while your host is running something else entirely. The distro *is* just files; the kernel is shared.

---

## 3. Cgroups — controlling what a process uses

Namespaces handle visibility. They do nothing about consumption: a process in its own PID namespace can still eat every core and all your RAM.

**Control groups** (cgroups) are hierarchical resource limits, enforced by the kernel:

| Controller | Limits | Docker flag |
|---|---|---|
| `cpu` | CPU shares and hard quotas | `--cpus`, `--cpu-shares` |
| `memory` | RAM ceiling; OOM-kills on breach | `--memory` |
| `blkio` | Block I/O bandwidth | `--device-read-bps` |
| `pids` | Max number of processes | `--pids-limit` |

Without cgroups, one runaway container takes down every other container on the host. Inception doesn't require you to set limits, but knowing they exist — and that this is the "cgroups" half of the namespaces-and-cgroups pair — is a defense-grade answer.

Historical note worth having: cgroups came from Google in 2006, built to pack many workloads onto one machine safely. Container density was the goal from the start.

---

## 4. Union filesystems — how layers work

The image is not a disk image. It's a **stack of read-only directories**, merged into one view by a union filesystem (`overlay2` on modern Docker).

```
    ┌──────────────────────────┐
    │ container writable layer │ ← yours, dies with the container
    ├──────────────────────────┤
    │ COPY ./conf /etc/nginx   │ ┐
    ├──────────────────────────┤ │
    │ RUN apk add nginx        │ ├─ read-only, shared between containers
    ├──────────────────────────┤ │
    │ FROM alpine:3.XX         │ ┘
    └──────────────────────────┘
              ↓ merged view
             /  ← what the process sees
```

Rules that follow from this design, all of them things you'll hit:

- **Layers are read-only and shared.** Ten containers from one image share the same layer files on disk. This is why containers are cheap to start.
- **Writes go to the container's own layer** via **copy-on-write**: modifying a file from a lower layer copies it up first. Large files are expensive to modify for this reason.
- **Deleting doesn't shrink anything.** Removing a file in a later layer writes a "whiteout" marker hiding it. The bytes are still in the lower layer, still in the image, still downloadable. **This is why `RUN echo secret > /tmp/f && rm /tmp/f` does not remove the secret** — it's a real exfiltration path, and it's part of why the subject bans passwords in Dockerfiles.
- **The writable layer dies with the container.** Anything that must survive goes in a volume. See [`06_STORAGE.md`](06_STORAGE.md).

---

## 5. Why there is no Docker on macOS

Now the piece that matters for your machine.

Containers are **Linux kernel features**. Namespaces, cgroups, overlayfs — none exist in Darwin, the macOS kernel. There is nothing on a Mac for a Linux container to share.

So Docker Desktop, colima, OrbStack, Rancher — all of them do the same thing underneath: **run a Linux virtual machine** and put the Docker daemon inside it. The `docker` command on your Mac is a client that talks over a socket to a daemon in that VM.

```
   Your Mac
   ┌───────────────────────────────────────┐
   │  docker CLI ──────┐                   │
   │                   │ socket            │
   │  ┌────────────────▼─────────────────┐ │
   │  │ Linux VM (Apple Virtualization)  │ │
   │  │  ┌────────────────────────────┐  │ │
   │  │  │ dockerd                    │  │ │
   │  │  │  ┌────────┐  ┌──────────┐  │  │ │
   │  │  │  │ nginx  │  │ mariadb  │  │  │ │
   │  │  │  └────────┘  └──────────┘  │  │ │
   │  │  └────────────────────────────┘  │ │
   │  │      Linux kernel  ← the shared one│ │
   │  └──────────────────────────────────┘ │
   └───────────────────────────────────────┘
```

This is not a detail — it produces concrete, surprising behaviour:

1. **"The host" means the VM, not your Mac.** When a compose file says `device: /home/<login>/data`, that path is resolved *inside the Linux VM*. It is not a folder on your Mac. This is why Inception's `/home/<login>/data` requirement can only be genuinely demonstrated on Linux.
2. **Bind mounts from macOS go through a file-sharing layer** (virtiofs and friends) and are noticeably slower than native.
3. **`network_mode: host` means the VM's network**, not your Mac's — which is part of why it's near-useless on macOS.
4. **Apple Silicon builds arm64 images by default.** The evaluation VM is probably amd64. An image that builds cleanly on your Mac is not evidence it builds for the evaluator.

Details and workarounds in [`09_MAC_AND_LINUX.md`](09_MAC_AND_LINUX.md).

**On Linux, none of this applies.** `dockerd` runs directly on your kernel; containers are processes in your process table. That's why the subject wants the project done in a VM — the VM is a real Linux host, and the behaviour you observe there is the behaviour the evaluator will see.

---

## 6. What Docker itself adds

The kernel gives isolation. Docker supplies the ergonomics:

| Piece | Job |
|---|---|
| **Docker CLI** | What you type. Talks HTTP to the daemon over a Unix socket. |
| **dockerd** | The daemon. Images, networks, volumes, the API. |
| **containerd** | Container lifecycle supervisor. Also what Kubernetes uses directly. |
| **runc** | The thing that actually calls `clone()` with the right flags. OCI-compliant, ~small. |
| **Image format** | Layered, content-addressed, shareable |
| **Registry** | `docker pull` — the piece that made the whole thing spread |

The chain: `docker run` → dockerd → containerd → runc → `clone()` + `execve()`.

Worth knowing that `runc` is a few thousand lines and swappable. The isolation isn't Docker's; Docker's contribution is everything around it.

---

## 7. 🚪 Gate

Before moving on, say these out loud:

1. Why does a container start in milliseconds when a VM takes 30 seconds?
2. Why can you run Alpine userland on a Debian host, but not a Windows container on Linux?
3. Why does the container die when its main process exits — Docker policy, or kernel behaviour?
4. Why does `RUN echo secret > /f && rm /f` still leak the secret?
5. Why can't you fully test Inception's `/home/<login>/data` requirement on your Mac?

If any answer is fuzzy, reread the matching section — everything downstream assumes these.

---

**Next:** [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md) — building the layers you just learned about.
