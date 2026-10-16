# 10 — Inception: the subject, distilled

> Source: `~/Documents/INCEPTION.pdf`, version 5.3. This page is a working index of every rule, not a replacement for reading the PDF — read it once in full, then use this to check yourself.

---

## 1. What you're building

A small infrastructure of three services, each in its own container, wired by one Docker network, with two persistent volumes, reachable from the outside world through exactly one door.

```
                                   WWW
                                    │
                                    │ 443 (TLS 1.2 / 1.3 only)
   ┌────────────────────────────────┼──────────────────────────────┐
   │ HOST (a virtual machine)       │                              │
   │  ┌─────────────────────────────┼───────────────────────────┐  │
   │  │ docker network              ▼                           │  │
   │  │  ┌──────────┐  3306  ┌────────────┐  9000  ┌─────────┐  │  │
   │  │  │ mariadb  │◄──────►│ wordpress  │◄──────►│  nginx  │  │  │
   │  │  │          │        │  + php-fpm │        │         │  │  │
   │  │  └────┬─────┘        └──────┬─────┘        └────┬────┘  │  │
   │  └───────┼─────────────────────┼───────────────────┼───────┘  │
   │          ▼                     ▼                   │          │
   │     ┌─────────┐         ┌─────────────┐            │          │
   │     │ volume  │         │   volume    │◄───────────┘          │
   │     │   db    │         │  wordpress  │  (nginx reads the     │
   │     └─────────┘         └─────────────┘   same site files)    │
   │                                                               │
   │     both backed by /home/<login>/data/                        │
   └───────────────────────────────────────────────────────────────┘
```

Two things in that diagram surprise people:

- **nginx and WordPress share the WordPress volume.** nginx serves the static files (CSS, images) directly off disk and forwards only `.php` requests to php-fpm over port 9000. Both containers need to see the same files.
- **php-fpm is not a web server.** It speaks FastCGI, not HTTP. You cannot `curl` it. nginx translates HTTP into FastCGI and back. This is why the WordPress container "does nothing" when you test it alone, and why the subject says the WordPress container must not contain nginx.

---

## 2. The mandatory rules

Every one of these is checkable, and evaluators check them. Grouped by where they bite.

### Structure

| # | Rule |
|---|---|
| S1 | The whole project runs **in a virtual machine** |
| S2 | All configuration files live in a `srcs/` folder |
| S3 | A `Makefile` at the project root builds everything via `docker-compose.yml` |
| S4 | One `Dockerfile` per service, written by you |
| S5 | Each Docker **image name matches its service name** |
| S6 | Each service runs in its **own dedicated container** |

### Images

| # | Rule |
|---|---|
| I1 | Base images must be the **penultimate stable** version of Alpine or Debian |
| I2 | The `latest` tag is **prohibited** |
| I3 | You **build your own images** — pulling ready-made ones from DockerHub is forbidden (Alpine/Debian base excepted) |
| I4 | **No password may appear in a Dockerfile** |

### Services

| # | Rule |
|---|---|
| V1 | nginx, with **TLSv1.2 or TLSv1.3 only** |
| V2 | WordPress + php-fpm, installed and configured, **without nginx** |
| V3 | MariaDB, **without nginx** |
| V4 | nginx is the **only entrypoint**, via **port 443 only** |
| V5 | Containers **restart on crash** |
| V6 | The WordPress database has **two users**, one an administrator |
| V7 | The admin username must **not contain** `admin` / `administrator` in any casing or combination (`admin`, `Admin-123`, `administrator`, … all rejected) |

### Storage & network

| # | Rule |
|---|---|
| N1 | One volume for the WordPress **database** |
| N2 | One volume for the WordPress **site files** |
| N3 | Both must be **named volumes** — bind mounts are **not allowed** |
| N4 | Both must store their data in **`/home/<login>/data`** on the host |
| N5 | A **docker-network** connects the containers, and the `networks` line must be present in `docker-compose.yml` |
| N6 | `network: host`, `--link`, and `links:` are **forbidden** |
| N7 | The domain `<login>.42.fr` must point to your local IP |

### Runtime hygiene

| # | Rule |
|---|---|
| R1 | Containers must **not** be started with an infinite loop — this covers the `CMD`, the `ENTRYPOINT`, *and* anything inside entrypoint scripts |
| R2 | Explicitly banned: `tail -f`, `bash`, `sleep infinity`, `while true` |
| R3 | Environment variables are **mandatory**, and a `.env` file is **mandatory** to store them |
| R4 | Docker **secrets** are strongly recommended for confidential data |
| R5 | Any credential, API key, or password found in your git repository = **project failure** |
| R6 | *(derived, not literal subject text)* Entrypoints must be **idempotent** — the second `up` must work as well as the first. Implied by R1 + the way evaluations are run; see §7. |

> On R5: "in your repository" includes history. `git rm` after the fact does not remove the blob. If you ever commit one, the fix is rewriting history and rotating the credential — treat the `.gitignore` as something you write *before* the first credential exists. See [`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md).

---

## 3. Choosing the base image

**We're using Alpine.** Not because it's easier — because it teaches container thinking. Debian's `apt` is Linux-admin knowledge you already have from the piscine, so building on Debian would get you through the project without moving your understanding much. Alpine has no init system to lean on, no assumed service manager, and forces every dependency to be explicit. The friction is the lesson. (Where industry practice differs — Debian-slim still dominates plenty of stacks, and `musl` vs `glibc` is a real reason why — [`01_PHILOSOPHY.md`](01_PHILOSOPHY.md) covers it.)

### "Penultimate stable" — look it up, don't trust a number

This is a live evaluation question and any version written down here goes stale within months. The rule, not the answer:

- **Penultimate stable = N-1.** Not the newest stable release, the one before it.
- Check on the day at **alpinelinux.org/releases/**. Take the current stable series, step back one.
- Pin it explicitly in your Dockerfile: `FROM alpine:3.XX`. Never `FROM alpine` and never `FROM alpine:latest` — both resolve to whatever is newest today, which breaks reproducibility and violates I2.

Be ready to say *why* the subject asks for N-1: the newest release is where packaging bugs live, and pinning an explicit older tag is what makes a build reproducible six months from now. That reasoning is the point of the rule.

---

## 4. Directory structure

Straight from the subject, adapted to our location:

```
LEVEL5/Inception/
├── Makefile
├── README.md               ← graded
├── USER_DOC.md             ← graded
├── DEV_DOC.md              ← graded
├── .gitignore
├── secrets/
│   ├── credentials.txt
│   ├── db_password.txt
│   └── db_root_password.txt
└── srcs/
    ├── docker-compose.yml
    ├── .env
    └── requirements/
        ├── mariadb/
        │   ├── Dockerfile
        │   ├── .dockerignore
        │   ├── conf/
        │   └── tools/
        ├── nginx/
        │   ├── Dockerfile
        │   ├── .dockerignore
        │   ├── conf/
        │   └── tools/
        ├── wordpress/
        │   ├── Dockerfile
        │   ├── .dockerignore
        │   ├── conf/
        │   └── tools/
        └── bonus/
```

`conf/` holds configuration files copied into the image. `tools/` holds entrypoint and setup scripts. Keep to this layout — evaluators are told what to look for, and an unfamiliar tree costs you goodwill before you've said a word.

### A note on "the root of your repository"

The subject says the graded docs go "at the root of your Git repository." Inception lives inside Purgatoire at `LEVEL5/Inception/`, so for us that means the **project root** — `LEVEL5/Inception/README.md`.

This is normal and standard practice for a monorepo, but be ready to say it in one sentence if an evaluator raises an eyebrow: *"the repository holds my whole 42 cursus; this project's root is `LEVEL5/Inception/`, and everything the subject asks for is there."* If you'd rather not have the conversation at all, the alternative is submitting Inception as its own repository — worth deciding before you're standing at the defense.

---

## 5. The graded documentation

Three Markdown files, listed in Chapters VI and VII as **prerequisites for validation**. They are not optional and not an afterthought — a working stack with missing docs does not pass. Write them from [`11_COMPARISONS.md`](11_COMPARISONS.md), [`07_COMPOSE.md`](07_COMPOSE.md) and [`06_STORAGE.md`](06_STORAGE.md).

| # | Rule |
|---|---|
| D1 | `README.md` — italicized first line, **Description**, **Instructions**, **Resources** (incl. AI usage), **Project description** with the four comparisons. English. |
| D2 | `USER_DOC.md` — services provided, start/stop, site + admin panel, credentials, health check |
| D3 | `DEV_DOC.md` — setup from scratch, build via Makefile + compose, container/volume commands, data persistence |
| D4 | An **AI-usage log** kept current, condensed into the README's Resources section (§6) |

### `README.md`

**Must be written in English.** Required contents:

- **The very first line, italicized, verbatim:**
  *This project has been created as part of the 42 curriculum by `<login>`.*
- **Description** — what the project is, its goal, a brief overview
- **Instructions** — installation, build, execution
- **Resources** — the references you used (docs, articles, tutorials) **and a description of how AI was used: for which tasks, and which parts of the project**
- **Project description** — the use of Docker, the sources included in the project (i.e. what's in `srcs/`), the main design choices, and the four comparisons:
  - Virtual Machines vs Docker
  - Secrets vs Environment Variables
  - Docker Network vs Host Network
  - Docker Volumes vs Bind Mounts

### `USER_DOC.md`

For an end user or administrator. Must explain how to:

- understand what services the stack provides
- start and stop the project
- access the website and the administration panel
- locate and manage credentials
- check that the services are running correctly

### `DEV_DOC.md`

For a developer. Must describe how to:

- set up the environment from scratch — prerequisites, configuration files, secrets
- build and launch the project using the Makefile and Docker Compose
- use the relevant commands to manage containers and volumes
- identify where project data is stored and how it persists

---

## 6. The AI-usage log — start it now

The README must state which tasks and which parts of the project used AI. Reconstructing that at the end produces something vague, and vague is what gets questioned. Keep this table current as you work; it gets condensed into the README's Resources section at the end.

| Date | What | How AI was used | What I verified myself |
|---|---|---|---|
| 2026-08-11 | This documentation set (`library/docker/`) | Generated the study notes, the subject breakdown, and the four comparisons from the PDF | *(to fill: which claims you checked against official docs, what you rewrote, what you found wrong)* |

Chapter IV is explicit that you may only use AI-generated content you fully understand and can take responsibility for. The 🚪 gates in [`00_ROADMAP.md`](00_ROADMAP.md) are the mechanism for that — each one is a checkpoint where you confirm the understanding transferred. The right-hand column above is the one that matters at defense.

---

## 7. Design choices you have to make

The subject leaves these open. Each is a likely defense question, so decide deliberately rather than by copying.

| Choice | Options | Notes |
|---|---|---|
| **Base image** | Alpine / Debian | Decided: Alpine. Be able to justify it. |
| **TLS certificate** | Self-signed via `openssl` | Generate at build time or in the entrypoint; browsers will warn, that's expected — `curl -k` for testing |
| **WordPress setup** | `wp-cli` / manual SQL / the web installer | `wp-cli` is the clean route: scriptable, idempotent, creates both users non-interactively. The web installer can't satisfy V6/V7 reproducibly. |
| **PHP version** | Whatever the Alpine release packages | Pin the package; don't assume a version number |
| **Startup ordering** | `depends_on` / retry loop / healthcheck | `depends_on` alone only waits for *start*, not readiness — WordPress will race MariaDB. See [`04_CONTAINERS.md`](04_CONTAINERS.md) |
| **Restart policy** | `unless-stopped` / `always` / `on-failure` | V5 requires restart on crash; pick one and know the difference |
| **Where the DB is seeded** | Entrypoint script vs SQL file in `/docker-entrypoint-initdb.d` | You're building the image yourself, so it's your entrypoint script — and it must be idempotent, since it runs on every start, not just the first |

That last one deserves emphasis: your entrypoint runs **every time the container starts**, and the volume may already contain a fully initialized database. "Create the database and users" must therefore mean "create them if they don't already exist." Getting this wrong is the classic cause of a stack that works on first build and breaks on the second `up` — which is precisely the sequence an evaluator will run.

---

## 8. Bonus

Assessed **only if the mandatory part is flawless**. Not "mostly working" — flawless. If any mandatory requirement fails, the bonus isn't looked at.

Each bonus service needs its own Dockerfile, its own container, and its own volume if relevant. You may open extra ports for these.

- redis cache for WordPress
- an FTP server pointing at the WordPress volume
- a static website in any language **except PHP**
- Adminer
- a service of your choice — **you will have to justify it** at defense

---

## 9. Evaluation notes

- Only what's in the git repository is evaluated. Check your folder and file names against §4.
- A **small live modification** may be requested — a behaviour change, a few lines to rewrite, an easy feature. It's a test of real understanding, doable in a few minutes. [`00_ROADMAP.md`](00_ROADMAP.md) Phase 11 is how you rehearse for it.
- Expect the evaluator to run `docker compose down` and `up` again, and to reboot the VM. Both must work with no manual intervention.

---

## Where to go next

| You need | Page |
|---|---|
| The learning sequence | [`00_ROADMAP.md`](00_ROADMAP.md) |
| Why containers work at all | [`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md) |
| Writing the Dockerfiles, and PID 1 | [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md) |
| The `driver_opts` volume pattern | [`06_STORAGE.md`](06_STORAGE.md) |
| The four comparisons in full | [`11_COMPARISONS.md`](11_COMPARISONS.md) |
| It's broken | [`12_TROUBLESHOOTING.md`](12_TROUBLESHOOTING.md) |
