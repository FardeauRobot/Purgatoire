# 🐳 Docker & Inception — Support Library

> *"You never step into the same river twice — unless you built it from a Dockerfile."*

Everything you need to **understand Docker, build Inception, and defend it** at evaluation.

Inception is the first 42 project where the deliverable isn't a binary. Nothing compiles. You write configuration, and the grade depends on whether you can explain *why* every line of that configuration is there. The evaluator's questions are not "does it work" — they're "what happens if I `docker compose down` and back up", "why is this a volume and not a bind mount", "why doesn't your container die immediately".

This library exists so those questions have answers you actually own.

---

## 🗺️ Reading order

Start at the roadmap — it sequences everything below into phases with a gate at each step.

| # | File | What you learn |
|---|---|---|
| 🧭 | [`00_ROADMAP.md`](00_ROADMAP.md) | **The learning checklist.** Phases, in order, each with a "can you explain this without notes?" gate. Start here. |
| 1 | [`01_PHILOSOPHY.md`](01_PHILOSOPHY.md) | Why Docker exists. The problems it actually solved, the ones it didn't, and the shipping-container metaphor taken seriously. |
| 2 | [`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md) | Namespaces, cgroups, overlay filesystems. Why a container is a *process*, not a machine — and why that means macOS runs a hidden Linux VM. |
| 3 | [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md) | Layers, the build cache, every Dockerfile instruction. **PID 1** and why the subject bans `tail -f`. |
| 4 | [`04_CONTAINERS.md`](04_CONTAINERS.md) | Lifecycle and the CLI verbs you'll live in. Restart policies, healthchecks, startup ordering. |
| 5 | [`05_NETWORKING.md`](05_NETWORKING.md) | Bridge networks, embedded DNS, published ports. Why `network_mode: host` is forbidden. |
| 6 | [`06_STORAGE.md`](06_STORAGE.md) | Volumes vs bind mounts vs tmpfs — and the `driver_opts` pattern Inception forces you into. |
| 7 | [`07_COMPOSE.md`](07_COMPOSE.md) | `docker-compose.yml` field by field, and how the Makefile drives it. |
| 8 | [`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md) | `.env` vs Docker secrets, `/run/secrets`, and the git hygiene that stops you failing the project outright. |
| 9 | [`09_MAC_AND_LINUX.md`](09_MAC_AND_LINUX.md) | Installing and testing on both, from zero. Where the two platforms genuinely diverge. |
| 10 | [`10_INCEPTION.md`](10_INCEPTION.md) | **The subject, distilled.** Every mandatory rule, every trap, the directory tree, the design choices to make. |
| 11 | [`11_COMPARISONS.md`](11_COMPARISONS.md) | The four comparisons the README must contain, written to be defended out loud. |
| 12 | [`12_TROUBLESHOOTING.md`](12_TROUBLESHOOTING.md) | "It's broken, what now?" — symptom → diagnosis decision tree. |
| 13 | [`13_DEFENSE.md`](13_DEFENSE.md) | The one-pager you skim 15 minutes before defense. |

---

## 🎯 The mental model

Four ideas underpin everything. Get these and the rest is detail.

1. **A container is a process, not a machine.** It's a normal Linux process that has been *lied to* about what it can see — its own PID space, its own filesystem root, its own network stack. The kernel underneath is your kernel. Nothing is emulated. That single fact explains the startup speed, the tiny footprint, the security caveats, and why you need a Linux VM on macOS.

2. **An image is a stack of read-only diffs; a container is a writable layer on top.** Build once, run many. When the container dies, the writable layer dies with it. Anything you want to survive must live in a volume — this is the whole reason Inception has two of them.

3. **One service per container.** Not "one process" — nginx forks workers — but one *responsibility*. Three services means three containers, three Dockerfiles, three images. Bundling them is the most common way to fail this project.

4. **The container lives exactly as long as its PID 1.** Your `CMD` *is* PID 1. When it exits, the container exits. This is why daemons must run in the foreground, and why `tail -f` "fixes" are both forbidden and a sign you didn't understand the model.

---

## ✅ Ready-to-defend checklist

You're ready to evaluate when you can, unprompted:

- [ ] Explain the difference between a VM and a container in terms of **kernels**, not vibes
- [ ] Draw the Inception architecture from memory — three containers, one network, two volumes, port 443
- [ ] Explain what each line of your `docker-compose.yml` does, including the ones you copied
- [ ] Explain why your container doesn't exit, pointing at the actual PID 1
- [ ] Show where a password lives at every stage: file on disk → compose → container → application
- [ ] Prove your data survives `docker compose down` and explain *where* on the host it lives
- [ ] Explain why WordPress reaches the database by the name `mariadb` and nothing configures that
- [ ] Justify Alpine over Debian, and say what you'd lose by switching

If any of these is shaky, the roadmap tells you which page to reread.

---

## 📌 Project facts

| Thing | Value |
|---|---|
| Project location | `LEVEL5/Inception/` |
| Base image | Alpine (penultimate stable — [look it up](10_INCEPTION.md), never `latest`) |
| Services | nginx · wordpress + php-fpm · mariadb |
| Entry point | port **443** only, TLS 1.2/1.3 only, nginx only |
| Domain | `<login>.42.fr` |
| Volumes | two named, backed by `/home/<login>/data` |
| Graded docs | `README.md`, `USER_DOC.md`, `DEV_DOC.md` at the project root |

> The original brief is kept in [`start.txt`](start.txt). Live progress tracker: the Inception dashboard artifact.
