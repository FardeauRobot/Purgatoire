# 09 — Testing on macOS and Linux

> Nothing Docker-related is installed on this Mac yet. This page starts from zero on both platforms, then covers the two places where they genuinely diverge — because one of them will bite you at evaluation if you don't plan for it.

---

## 1. The short version

**Develop on the Mac, validate in a Linux VM.**

The Mac gives you a fast edit-build-test loop with no VM management. But two Inception requirements *cannot be honestly demonstrated* on macOS, and the evaluator will be looking at a Linux machine. So: iterate natively, then run the full checklist in the VM before you call anything done.

---

## 2. Installing on macOS (Apple Silicon)

Three viable options. All of them run a Linux VM underneath ([`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md)) — the difference is who manages it.

### Docker Desktop — the default

```bash
brew install --cask docker
```

Then launch Docker.app once to start the VM and install the CLI. GUI included, works out of the box, uses Apple's Virtualization framework. Licensing is free for personal use and small companies, paid for large ones — irrelevant for you, worth knowing it exists.

### colima — lightweight, CLI-only

```bash
brew install colima docker docker-compose
colima start --cpu 4 --memory 8
```

No GUI, no background app, starts and stops when you say so. Uses Lima under the hood. Good if Docker Desktop's resource appetite annoys you.

### OrbStack — fastest

```bash
brew install --cask orbstack
```

Noticeably lighter and faster than Docker Desktop, particularly on file I/O. Free for personal use.

**Recommendation: Docker Desktop.** Not because it's best, but because it's what the documentation and every tutorial assumes, and you don't need one more variable while learning. Switch later if it irritates you.

### Verify

```bash
docker --version
docker compose version          # note: "compose", not "docker-compose"
docker run --rm hello-world
```

> `docker-compose` (hyphen, v1, Python) is deprecated. `docker compose` (space, v2, Go plugin) is current. Both may work; write the v2 form.

---

## 3. Installing on Debian

On the VM, or on a school machine. **Don't use the distro's `docker.io` package** — it lags badly and often lacks the compose plugin. Use Docker's own repository:

```bash
# prerequisites
sudo apt-get update
sudo apt-get install -y ca-certificates curl gnupg

# Docker's GPG key
sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/debian/gpg \
  | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
sudo chmod a+r /etc/apt/keyrings/docker.gpg

# the repository
echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/debian $(. /etc/os-release && echo "$VERSION_CODENAME") stable" \
  | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io \
                        docker-buildx-plugin docker-compose-plugin
```

Then, so you don't type `sudo` for everything:

```bash
sudo usermod -aG docker $USER
newgrp docker           # or log out and back in
```

> Adding yourself to the `docker` group is **equivalent to root access** — the daemon runs as root and anyone who can talk to its socket can mount the host filesystem. Fine on your own VM; understand what you did.

And so the stack survives a reboot, which the evaluator may test:

```bash
sudo systemctl enable --now docker
```

---

## 4. The VM, on Apple Silicon

The subject requires the project to run in a VM. On an M-series Mac:

| Tool | Verdict |
|---|---|
| **UTM** | Recommended. Free, native Apple Silicon, runs arm64 Debian well. `brew install --cask utm` |
| **Lima** | Good CLI alternative — `limactl start template://debian` |
| **VMware Fusion** | Works, free for personal use, heavier |
| **Parallels** | Best performance, paid |
| **VirtualBox** | ✗ Don't. Apple Silicon support is experimental at best. This is the 42 default on Intel machines and it does not transfer. |

Install Debian arm64 in UTM, install Docker via §3, and clone the project in. Give it at least 2 CPUs, 4GB RAM, and 20GB disk — images add up faster than you expect.

**Note the architecture:** an arm64 Debian VM on an arm64 Mac builds arm64 images. That's consistent with your host but *not* necessarily with the school's Intel machines. See §6.

---

## 5. Divergence 1 — `/home/<login>/data` doesn't exist on your Mac

Inception rule N4: both named volumes must store their data in `/home/<login>/data` on the host.

On macOS:

- there is no `/home/<login>` — user directories are `/Users/<login>`
- `driver_opts: device: /home/<login>/data/wordpress` is resolved **inside the Linux VM**, where `/home/<login>` may not exist either
- so the mount either fails, or silently succeeds against a path inside the VM that you cannot inspect from Finder

**Consequence:** `ls /home/<login>/data` on your Mac will never show your database files, and no amount of configuration changes that. Rule N4 is only demonstrable on a real Linux host.

**How to work anyway:** develop on the Mac with a compose override that points `device:` at a path under `/Users/<login>/…`, keeping the committed compose file correct for Linux. Or just accept that persistence testing happens in the VM. Either way — **the committed `docker-compose.yml` must have the `/home/<login>/data` paths**, because that's what's graded.

---

## 6. Divergence 2 — arm64 vs amd64

Your Mac is arm64. `docker build` produces arm64 images. The evaluation machine is very likely amd64.

This mostly doesn't matter for Inception (Alpine, nginx, MariaDB, PHP all publish both architectures), but it can:

- a package that exists for amd64 and not arm64, or at a different version
- anything downloading a pre-built binary in a `RUN` — architecture-specific, and it will fail on the other one
- performance differences under emulation that make a timing-dependent bug appear on one and not the other

**Check what you built:**

```bash
docker image inspect <image> --format '{{.Architecture}}'
```

**Build for the other architecture when you want certainty:**

```bash
docker build --platform linux/amd64 -t test .
```

This uses QEMU emulation — correct, and slow. Don't do it every build; do it once before you consider the project finished.

**The reliable answer** is to do your final validation in the VM, and to make sure nothing in your Dockerfiles hardcodes an architecture. Prefer `apk add <package>` over downloading a binary release.

---

## 7. Things that differ but matter less

| | macOS | Linux |
|---|---|---|
| **Bind mount speed** | Slower — goes through a file-sharing layer | Native |
| **`network_mode: host`** | Near-useless — means the VM's network | Works as documented |
| **Container IPs from the host** | Not routable | Directly reachable |
| **`/var/lib/docker`** | Inside the VM | On your filesystem |
| **File ownership** | Remapped by the sharing layer | Real UIDs, real permission errors |

That last one produces a genuinely confusing class of bug: a container that runs as a non-root user may work on macOS (where the sharing layer papers over ownership) and fail on Linux with permission denied. Another reason for the VM check.

---

## 8. The `/etc/hosts` entry

Rule N7: `<login>.42.fr` must point at your local IP. That's a hosts-file entry, and it's per-machine — it does not travel with the repo.

**Linux / macOS host:**

```bash
sudo sh -c 'echo "127.0.0.1 <login>.42.fr" >> /etc/hosts'
```

Verify:

```bash
ping -c1 <login>.42.fr
curl -k https://<login>.42.fr
```

Document this in `USER_DOC.md`. The evaluator's machine won't have the entry, and "it works on mine" is exactly the sentence Docker was invented to eliminate.

---

## 9. The two-platform workflow

**On the Mac, continuously:**

```bash
docker compose -f srcs/docker-compose.yml up -d --build
docker compose logs -f
curl -k https://localhost
```

Fast, no VM to babysit. Good for Dockerfile iteration, config debugging, and getting each service to stay up.

**In the VM, before declaring anything done:**

```bash
make                                       # full build from clean
curl -k https://<login>.42.fr              # the real domain
ls -la /home/<login>/data/{mariadb,wordpress}   # ← rule N4, actually verified
docker compose down && docker compose up -d     # persistence
sudo reboot                                 # then check it all came back
openssl s_client -connect <login>.42.fr:443 -tls1_1   # must FAIL
openssl s_client -connect <login>.42.fr:443 -tls1_2   # must succeed
```

Those last two are the TLS rule (V1), tested rather than assumed. `-tls1_1` failing is the proof.

---

## 10. 🚪 Gate

1. Why is there no native Docker on macOS, and what actually runs your containers there?
2. Why can't you prove rule N4 on your Mac?
3. Your image builds fine locally. Why is that not evidence it builds on the evaluator's machine?
4. Which parts of the workflow must happen in the VM, and why each?

---

**Next:** [`10_INCEPTION.md`](10_INCEPTION.md) — the subject, rule by rule.
