# 📖 GLOSSAIRE — Every technical term in Inception

> Source: subject `LEVEL5/Inception/INCEPTION.pdf`, version 5.3. Every term that appears in the subject (or that you must understand to satisfy it): **definition**, then **use in Inception**. Each section points to the library chapter that goes deeper.
>
> 🇫🇷 French version: [`../../fr/docker/GLOSSAIRE.md`](../../fr/docker/GLOSSAIRE.md).

---

## 🗂️ Contents

1. [[#1. Virtualization & system|Virtualization & system]]
2. [[#2. Docker — the building blocks|Docker — the building blocks]]
3. [[#3. Dockerfile — the instructions|Dockerfile — the instructions]]
4. [[#4. Docker Compose|Docker Compose]]
5. [[#5. Processes, PID 1 & daemons|Processes, PID 1 & daemons]]
6. [[#6. Networking|Networking]]
7. [[#7. Storage|Storage]]
8. [[#8. Secrets & configuration|Secrets & configuration]]
9. [[#9. Web & TLS|Web & TLS]]
10. [[#10. The services|The services]]
11. [[#11. Bonus|Bonus]]
12. [[#12. Submission & evaluation|Submission & evaluation]]

---

## 1. Virtualization & system

> Deeper: [`01_PHILOSOPHY.md`](01_PHILOSOPHY.md), [`02_UNDER_THE_HOOD.md`](02_UNDER_THE_HOOD.md)

### System administration (sysadmin)
- **Definition**: the job of installing, configuring and maintaining machines and services (servers, network, databases, backups).
- **Use**: the project's category (*"System Administration related exercise"*). Nothing compiles: you deliver **configuration**, and you must justify every line.

### Virtual machine (VM)
- **Definition**: a whole computer emulated by a **hypervisor** (VirtualBox, UTM, KVM…) — with its own kernel, its own OS, its own virtual hardware.
- **Use**: the subject requires **the whole project to run inside a VM**. It is the containers' host. The README must compare *VM vs Docker*.

### Hypervisor
- **Definition**: the software that creates and runs VMs by sharing the real hardware between them.
- **Use**: what runs your Inception VM (on Apple Silicon: UTM, or the hidden VM of Docker Desktop / OrbStack / Colima).

### Host
- **Definition**: the machine Docker runs on — the one whose kernel all containers share.
- **Use**: here, host = your VM. Volumes must live in `/home/<login>/data` **on the host**.

### Kernel
- **Definition**: the core of the OS, managing processes, memory, files, network.
- **Use**: the key VM/container difference: a VM has **its own kernel**, a container **shares the host's**. That is why a container starts in milliseconds and why macOS needs a Linux VM for Docker.

### Namespaces
- **Definition**: Linux kernel mechanism that gives a process an **isolated view** of a resource (PIDs, network, mounts, hostname, users…).
- **Use**: *what makes* a container. Your container's PID 1 is PID 1 only inside its PID namespace.

### cgroups (control groups)
- **Definition**: kernel mechanism that **limits and accounts** resources (CPU, RAM, I/O) for a group of processes.
- **Use**: invisible in the subject, but worth citing at defense: namespaces = *what the container sees*, cgroups = *what it may consume*.

### Distribution (Alpine / Debian)
- **Definition**: a packaged flavour of Linux with its package manager. **Alpine** is tiny (~5 MB, `musl`, `apk`); **Debian** is heavier but more standard (`glibc`, `apt`).
- **Use**: the only allowed base images. Containers must be built from **the penultimate stable version** of one of them.

### Penultimate stable version
- **Definition**: the stable release *just before* the latest one (e.g. if Debian 13 is the latest stable, the penultimate is Debian 12 `bookworm`).
- **Use**: forces an **explicit** tag (`alpine:3.21`, `debian:bookworm`). Check the version when you write the code and be able to justify it: it moves over time.

### Package manager (`apk` / `apt`)
- **Definition**: the tool that installs software on a distribution.
- **Use**: how you install nginx, php-fpm, mariadb in your Dockerfiles (`RUN apk add --no-cache nginx`).

---

## 2. Docker — the building blocks

> Deeper: [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md), [`04_CONTAINERS.md`](04_CONTAINERS.md)

### Docker
- **Definition**: platform that builds, ships and runs applications inside **containers**.
- **Use**: the project's central tool. The README must explain *the use of Docker* in your project.

### Docker Engine / `dockerd` daemon
- **Definition**: the background service that does the real work (build, run, network, volumes). The `docker` command is just a client talking to it over a socket.
- **Use**: must be running in the VM. Check with `systemctl status docker`.

### Image
- **Definition**: a **read-only** template, a stack of layers holding a filesystem and metadata (start command, ports, variables…).
- **Use**: one image **per service**, built **by you** from a Dockerfile. Each image must have **the same name as its service** (`nginx`, `wordpress`, `mariadb`).

### Container
- **Definition**: a **running** instance of an image: an isolated process + a writable layer on top of the image.
- **Use**: each service runs in **its own dedicated container**. The subject insists: *a container is not a VM*.

### Layer
- **Definition**: a filesystem diff produced by a Dockerfile instruction (`RUN`, `COPY`…). Layers are stacked through an **overlay filesystem**.
- **Use**: explains the **build cache** and why you chain `RUN`s (fewer layers, lighter image).

### Writable layer
- **Definition**: the per-container layer where all its writes go. It **disappears** with the container.
- **Use**: *the reason* the database and the WordPress files must live in **volumes**.

### Tag
- **Definition**: an image's version label (`name:tag`, e.g. `debian:bookworm`).
- **Use**: **the `latest` tag is forbidden**. It moves: the same Dockerfile would give a different image tomorrow.

### `latest`
- **Definition**: the default tag when none is given — not "the newest", just "whatever was tagged latest".
- **Use**: forbidden, implicitly too (`FROM alpine` with no tag = `alpine:latest`).

### Registry / Docker Hub
- **Definition**: a server that stores and distributes images. Docker Hub is the default public registry.
- **Use**: **pulling ready-made images is forbidden** (no `image: wordpress` or `nginx:x`). Only exception: the Alpine/Debian base images.

### `pull` / `build`
- **Definition**: `pull` downloads an existing image; `build` makes one from a Dockerfile.
- **Use**: you **build** everything. The only allowed pull is the implicit one in your `FROM alpine:X` / `debian:X`.

### Build cache
- **Definition**: Docker reuses a layer if the instruction and its inputs haven't changed.
- **Use**: order your Dockerfiles from least-changing (package install) to most-changing (copying scripts) to rebuild fast.

### Build context
- **Definition**: the directory sent to the daemon at `build` time; only its files are reachable by `COPY`.
- **Use**: each `srcs/requirements/<service>/` is its image's context.

### `.dockerignore`
- **Definition**: list of files **excluded from the build context** (like `.gitignore`).
- **Use**: present in every service folder in the example tree. Keeps useless — or sensitive — files out of the image.

---

## 3. Dockerfile — the instructions

> Deeper: [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md)

### Dockerfile
- **Definition**: a text recipe describing, instruction by instruction, how to build an image.
- **Use**: **one Dockerfile per service, written by you**, called from `docker-compose.yml`, itself run by the Makefile. **No password** inside.

| Instruction | Definition | Use in Inception |
|---|---|---|
| `FROM` | Base image | `FROM alpine:3.21` / `debian:bookworm` — never untagged |
| `RUN` | Runs a command **at build time**, creates a layer | Install packages |
| `COPY` | Copies a file from the context into the image | Copy `conf/` and `tools/` |
| `ADD` | Like `COPY` + URLs and archives | Prefer `COPY`, more explicit |
| `WORKDIR` | Working directory for following instructions | `/var/www/html` for WordPress |
| `EXPOSE` | **Documents** a listening port (publishes nothing) | `9000`, `3306`, `443` |
| `ENV` | Env var baked into the image | Never for a secret |
| `ARG` | Variable available **only at build time** | A package version, not a password (visible in `docker history`) |
| `ENTRYPOINT` | Program launched at start | Your init script (`tools/entrypoint.sh`) |
| `CMD` | Default arguments (or the command if no ENTRYPOINT) | `["nginx", "-g", "daemon off;"]` |
| `USER` | User for following instructions / runtime | Optional, good security habit |
| `HEALTHCHECK` | Command testing the container's health | Useful for `depends_on: condition: service_healthy` |

### Exec form vs shell form
- **Definition**: `CMD ["nginx","-g","daemon off;"]` (exec, JSON) runs the program **directly**; `CMD nginx -g "daemon off;"` (shell) runs it through `/bin/sh -c`.
- **Use**: exec form makes your program **PID 1** and lets it receive `SIGTERM`. Shell form puts `sh` at PID 1.

### Entrypoint script
- **Definition**: shell script run at container start to prepare the environment (create the database, install WordPress…) before launching the service.
- **Use**: it **must end with `exec <service>`** to hand PID 1 to the real daemon. No infinite loop allowed in it.

---

## 4. Docker Compose

> Deeper: [`07_COMPOSE.md`](07_COMPOSE.md)

### Docker Compose (`docker compose`)
- **Definition**: tool that describes and drives **a multi-container application** from a YAML file.
- **Use**: mandatory. `docker compose` (v2, plugin) — the old `docker-compose` (v1, Python) is obsolete.

### `docker-compose.yml`
- **Definition**: the YAML file declaring services, networks, volumes, secrets.
- **Use**: in `srcs/`. The Makefile calls it (`docker compose -f srcs/docker-compose.yml up -d --build`).

### YAML
- **Definition**: human-readable data format where **indentation (spaces, never tabs)** defines structure.
- **Use**: the compose format. Wrong indentation = parse error.

### Service
- **Definition**: in compose, the definition of a container (image, build, ports, volumes, networks…).
- **Use**: `nginx`, `wordpress`, `mariadb` (+ bonus). Service name = image name = **DNS name** on the network.

| Compose key | Definition | Use in Inception |
|---|---|---|
| `build` | Context/Dockerfile path | `build: requirements/nginx` |
| `image` | Name given to the built image | `image: nginx` — **same name as the service** |
| `container_name` | Fixed container name | Handy for `docker exec -it mariadb` |
| `ports` | **Publishes** a container port on the host | Only `"443:443"` on nginx |
| `expose` | Port reachable **on the internal network only** | `9000`, `3306` |
| `networks` | Networks joined | **Mandatory**: the network line must be present |
| `volumes` | Mounts | Named volumes only |
| `env_file` | Loads a variables file | `env_file: .env` |
| `environment` | Explicit variables | Non-secret only |
| `secrets` | Secrets mounted under `/run/secrets/` | `db_password`, `db_root_password`… |
| `restart` | Restart policy | `unless-stopped` or `always` |
| `depends_on` | Start order | `wordpress` depends on `mariadb` |

### Restart policy
- **Definition**: rule telling Docker when to relaunch a stopped container: `no`, `on-failure`, `always`, `unless-stopped`.
- **Use**: *"Your containers have to restart in case of a crash"* → `restart: unless-stopped` (or `always`). Testable at defense by killing the process.

### `depends_on`
- **Definition**: orders container start-up. By default it waits for the container to **start**, not to be **ready**.
- **Use**: WordPress must wait for MariaDB. Pair it with a `healthcheck` (`condition: service_healthy`) or a bounded wait loop in the entrypoint (one that *waits* then `exec`s — not an infinite loop).

### Makefile
- **Definition**: rules file read by `make` to automate commands.
- **Use**: mandatory **at the root**. It must **set up everything**: create `/home/<login>/data/...`, build the images through compose, start the stack. Usual targets: `all`, `down`, `clean`, `fclean`, `re`.

---

## 5. Processes, PID 1 & daemons

> Deeper: [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md) (PID 1 section)

### Process
- **Definition**: a running program, identified by a PID.
- **Use**: a container **is** a process (and its children). It lives exactly as long as its PID 1.

### PID 1
- **Definition**: the first process in a PID namespace (`init` on a normal machine). It has two special duties: **receive the signals** aimed at the container and **reap zombies**.
- **Use**: the subject says *"Read about PID 1"*. Your PID 1 must be **the real service** (nginx, php-fpm, mysqld), in the foreground. When it dies the container dies — and the restart policy brings it back.

### `exec` (shell)
- **Definition**: replaces the current shell process with the given program, **without creating a new process** (same PID).
- **Use**: the last line of every entrypoint: `exec php-fpm83 -F`. The script vanishes, the service becomes PID 1.

### Daemon
- **Definition**: a service program running in the background. Classically it **detaches** (forks) from the terminal.
- **Use**: the central trap. If nginx daemonizes, PID 1 returns and the container stops. Run every service **in the foreground**: `nginx -g 'daemon off;'`, `php-fpm -F`, `mysqld` (or `mariadbd`).

### Foreground / background
- **Definition**: in the foreground the process keeps control; in the background it returns immediately.
- **Use**: inside a container, the main service must be in the **foreground**.

### Forbidden hacky patches
- **Definition**: commands that keep a container artificially alive: `tail -f`, `bash`, `sleep infinity`, `while true`.
- **Use**: **forbidden**, including in the entrypoint and its scripts. They hide the real service: if it crashes the container stays "up" and the restart never fires.

### Signal (`SIGTERM`, `SIGKILL`)
- **Definition**: message the kernel delivers to a process. `docker stop` sends `SIGTERM`, then `SIGKILL` after 10 s.
- **Use**: if your PID 1 is a shell that doesn't forward `SIGTERM`, every `docker stop` takes 10 s and MariaDB is killed uncleanly (corruption risk).

### Zombie process
- **Definition**: a finished process whose parent hasn't read its exit status (`wait`).
- **Use**: PID 1 must reap them. Option: `init: true` in compose (adds `tini`).

---

## 6. Networking

> Deeper: [`05_NETWORKING.md`](05_NETWORKING.md)

### docker-network / bridge network
- **Definition**: a private virtual network created by Docker; containers attached to it can see each other. `bridge` is the default driver.
- **Use**: **mandatory**, declared in compose (`networks: inception: driver: bridge`). All three containers join it.

### Docker's embedded DNS
- **Definition**: on a user-defined network, Docker resolves **a service name** to its container's IP.
- **Use**: WordPress reaches the database at `mariadb:3306`, nginx reaches `wordpress:9000`. No hard-coded IPs.

### Host network (`network_mode: host`)
- **Definition**: the container shares the host's network stack directly, with no isolation.
- **Use**: **forbidden**. To compare in the README (*Docker Network vs Host Network*).

### `--link` / `links:`
- **Definition**: legacy (deprecated) mechanism to connect containers by name.
- **Use**: **forbidden**. The bridge network's DNS does the same thing cleanly.

### Port
- **Definition**: number (0–65535) identifying a service on an IP address.
- **Use**: **443** (HTTPS, nginx, only one open to the outside), **9000** (php-fpm, internal), **3306** (MariaDB, internal).

### Publishing a port (port mapping)
- **Definition**: `ports: "443:443"` binds a host port to a container port, making it reachable from outside.
- **Use**: **only nginx publishes a port**, and only 443. Don't publish 3306 or 9000.

### Infrastructure entrypoint
- **Definition**: *in the network sense*: the single door into your stack from outside. (Not to be confused with the Dockerfile `ENTRYPOINT`.)
- **Use**: nginx is the **only** entrypoint, via 443, over TLS 1.2/1.3.

### Local IP address / `localhost`
- **Definition**: `127.0.0.1` means the machine itself; the VM's local IP is its address on the network.
- **Use**: your domain name must **point to your local IP**.

### Domain name / `/etc/hosts`
- **Definition**: a readable name that resolves to an IP. `/etc/hosts` forces that resolution locally, before DNS.
- **Use**: `login.42.fr` → add `127.0.0.1 login.42.fr` to the VM's `/etc/hosts`. The `DOMAIN_NAME` variable in `.env` carries it.

---

## 7. Storage

> Deeper: [`06_STORAGE.md`](06_STORAGE.md)

### Persistence
- **Definition**: data **surviving** the container being stopped/removed.
- **Use**: the database and the site files must survive `docker compose down` then `up`. A defense classic.

### Volume (named volume)
- **Definition**: storage managed by Docker, referenced by a **name**, independent of container lifecycles.
- **Use**: **two named volumes required**: one for the WordPress database (`/var/lib/mysql`), one for the site files (`/var/www/html`).

### Bind mount
- **Definition**: mounting **a host path directly** into the container (`./data:/var/lib/mysql`). Docker doesn't manage it.
- **Use**: **forbidden** for these two volumes. To compare in the README (*Docker Volumes vs Bind Mounts*).

### `driver_opts` (`local` driver, `type: none`, `o: bind`)
- **Definition**: options of the `local` volume driver that let a **named** volume store its data at a precise host location.
- **Use**: the way to reconcile "named volume" with "data in `/home/<login>/data`":
  ```yaml
  volumes:
    db:
      driver: local
      driver_opts: { type: none, o: bind, device: /home/login/data/mariadb }
  ```
  The directory must exist **beforehand** (`mkdir -p` in the Makefile).

### Mount point
- **Definition**: the path inside the container where a volume appears.
- **Use**: `/var/lib/mysql` (mariadb), `/var/www/html` (wordpress **and** nginx, which share the same volume to serve static files).

### `/home/login/data`
- **Definition**: the host folder imposed by the subject.
- **Use**: where both volumes actually store their data. Replace `login` with yours.

---

## 8. Secrets & configuration

> Deeper: [`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md)

### Environment variable
- **Definition**: a `NAME=value` pair passed to a process and readable by it (`getenv`, `$NAME`).
- **Use**: **mandatory** to configure the services (domain name, database name, user…).

### `.env`
- **Definition**: `KEY=value` file read by Docker Compose (`${VAR}` substitution in the YAML, or injected through `env_file`).
- **Use**: **mandatory**, in `srcs/`. Holds `DOMAIN_NAME`, `MYSQL_USER`, etc. **Git-ignored**.

### Secret (Docker secrets)
- **Definition**: sensitive data mounted **as a read-only file** under `/run/secrets/<name>`, instead of an environment variable.
- **Use**: **strongly recommended** for passwords. Files in root `secrets/` (`db_password.txt`, `db_root_password.txt`, `credentials.txt`), declared in compose, read by scripts (`$(cat /run/secrets/db_password)`). The README compares *Secrets vs Environment Variables* (an env var leaks into `docker inspect`, logs, child processes).

### Credentials / API keys
- **Definition**: anything that authenticates: passwords, API keys, tokens.
- **Use**: **none in the git repo** (outside properly configured secrets) → **immediate project failure**. No password in any Dockerfile.

### `.gitignore`
- **Definition**: list of files git must not track.
- **Use**: must contain `.env` and `secrets/`. Check with `git status` *before* the first commit — a committed secret stays in history.

---

## 9. Web & TLS

> Deeper: [`10_INCEPTION.md`](10_INCEPTION.md)

### HTTP / HTTPS
- **Definition**: HTTP is the web's protocol (plaintext, port 80). HTTPS = HTTP **encrypted with TLS** (port 443).
- **Use**: only HTTPS on 443 is allowed. No port 80.

### TLS (Transport Layer Security)
- **Definition**: protocol that encrypts and authenticates a connection. Successor of SSL (the "SSL" name survives in configs).
- **Use**: **TLSv1.2 or TLSv1.3 only** → `ssl_protocols TLSv1.2 TLSv1.3;` in nginx. Testable with `openssl s_client -connect login.42.fr:443 -tls1_1` (must fail).

### Certificate (SSL/TLS) & private key
- **Definition**: the certificate proves the server's identity and carries its public key; the secret private key decrypts.
- **Use**: nginx needs both for HTTPS (`ssl_certificate`, `ssl_certificate_key`).

### Self-signed certificate
- **Definition**: a certificate signed by its own key, with no certificate authority. Browsers show a warning.
- **Use**: the norm for Inception. Generated **at build or start** with `openssl req -x509 -nodes -newkey rsa:2048 ...` — never committed.

### OpenSSL
- **Definition**: command-line cryptography toolkit.
- **Use**: generate the certificate; test the TLS version.

### Web server
- **Definition**: program that receives HTTP requests and returns responses (files or generated content).
- **Use**: nginx is the **only** web server in the stack.

### Reverse proxy
- **Definition**: server that receives client requests and **relays** them to a backend server.
- **Use**: nginx relays `.php` requests to php-fpm (`fastcgi_pass wordpress:9000;`) and serves static files itself.

### CGI / FastCGI
- **Definition**: protocols through which a web server delegates script execution to another program. FastCGI keeps processes alive instead of spawning one per request.
- **Use**: nginx ↔ php-fpm speak **FastCGI** on port 9000. It's not HTTP: you can't `curl` php-fpm.

---

## 10. The services

### NGINX
- **Definition**: light, fast web server and reverse proxy.
- **Use**: `nginx` container: TLS 1.2/1.3, port 443, serves `/var/www/html`, forwards PHP to `wordpress:9000`. Run with `daemon off;`.

### WordPress
- **Definition**: CMS (content management system) written in PHP, storing its data in a MySQL/MariaDB database.
- **Use**: `wordpress` container **with php-fpm, without nginx**. Must be **installed and configured** automatically on first start.

### PHP
- **Definition**: server-side scripting language WordPress is written in.
- **Use**: installed in the wordpress container (+ extensions: `mysqli`, etc.).

### php-fpm (FastCGI Process Manager)
- **Definition**: PHP's FastCGI implementation: a pool of PHP processes executing the scripts the web server sends.
- **Use**: PID 1 of the wordpress container, in the foreground (`php-fpm -F`), listening on `0.0.0.0:9000` (set in `www.conf`; by default it listens on `127.0.0.1` → unreachable from nginx).

### WP-CLI
- **Definition**: command-line tool to install and manage WordPress (`wp core download`, `wp config create`, `wp core install`, `wp user create`).
- **Use**: the standard way to automate installation in the entrypoint, without the web form.

### `wp-config.php`
- **Definition**: WordPress's configuration file (database connection, salt keys…).
- **Use**: generated by WP-CLI with `DB_HOST=mariadb:3306` and credentials read from the secrets.

### WordPress users / administrator
- **Definition**: the site's accounts; the administrator has every right.
- **Use**: **two users** required, **one admin**, whose name **must not contain** `admin`/`Admin`/`administrator`/`Administrator`.

### Administration panel (`wp-admin`)
- **Definition**: WordPress's management interface, at `https://login.42.fr/wp-admin`.
- **Use**: to document in `USER_DOC.md` (how to reach it).

### MariaDB
- **Definition**: relational DBMS (SQL database), a free, MySQL-compatible fork.
- **Use**: `mariadb` container **without nginx**, listens on 3306, data in `/var/lib/mysql` (volume). Run in the foreground (`mysqld` / `mariadbd`).

### Database / DBMS / SQL
- **Definition**: a database stores structured data in tables; the DBMS is the software managing it; SQL is the language to query it.
- **Use**: the entrypoint creates the WordPress database and its user (`CREATE DATABASE`, `CREATE USER`, `GRANT`) on first start only.

### Root user (MariaDB)
- **Definition**: the DBMS superuser.
- **Use**: password in `secrets/db_root_password.txt`. WordPress uses a **dedicated user** (`db_password.txt`), never root.

### `bind-address`
- **Definition**: the IP address MariaDB accepts connections on.
- **Use**: defaults to `127.0.0.1` → WordPress can't connect. Set `0.0.0.0` in `conf/`.

### Idempotence (initialization)
- **Definition**: an operation you can replay without changing the result.
- **Use**: your entrypoints must detect whether the database / WordPress already exists (files in the volume) and **not reinstall** on every restart.

---

## 11. Bonus

### Redis (cache)
- **Definition**: **in-memory** key-value database, used as a cache.
- **Use**: dedicated container, plugged into WordPress (*Redis Object Cache* plugin) to avoid querying MariaDB on every page.

### FTP (File Transfer Protocol) / FTP server
- **Definition**: file transfer protocol (port 21 + passive ports). Common server: `vsftpd`.
- **Use**: container pointing at **the WordPress volume**, to upload/edit site files.

### Static website
- **Definition**: site made of fixed files (HTML/CSS/JS), no server-side code executed.
- **Use**: bonus **in any language except PHP** (showcase, résumé).

### Adminer
- **Definition**: web database management interface, in a single PHP file.
- **Use**: container to administer MariaDB from the browser.

### Service of your choice
- **Definition**: any useful service (monitoring like cAdvisor, Uptime Kuma, etc.).
- **Use**: to **justify at defense**. Bonuses may open other ports. Graded **only if the mandatory part is perfect**.

---

## 12. Submission & evaluation

### Imposed directory tree
- **Definition**: the expected repository structure.
- **Use**:
  ```
  Makefile
  secrets/      credentials.txt, db_password.txt, db_root_password.txt
  srcs/         docker-compose.yml, .env, requirements/
  srcs/requirements/<service>/   Dockerfile, .dockerignore, conf/, tools/
  ```
  `conf/` = configuration files (nginx.conf, www.conf, 50-server.cnf); `tools/` = scripts (entrypoints).

### `README.md`
- **Definition**: the repository's entry documentation.
- **Use**: **in English**, italic first line (*This project has been created as part of the 42 curriculum by <login>*), sections **Description**, **Instructions**, **Resources** (including AI usage), **Project description** with the 4 comparisons: VM vs Docker, Secrets vs Env Vars, Docker Network vs Host Network, Docker Volumes vs Bind Mounts.

### `USER_DOC.md`
- **Definition**: user / administrator documentation.
- **Use**: services provided, start/stop, reach the site and the admin panel, manage credentials, check everything runs.

### `DEV_DOC.md`
- **Definition**: developer documentation.
- **Use**: set up the environment from scratch, build/launch via Makefile + Compose, container/volume management commands, where data is stored and how it persists.

### Markdown (`.md`)
- **Definition**: lightweight text format with simple markup (`#`, `**`, lists…).
- **Use**: required format of the three documents.

### Peer-evaluation / defense / live modification
- **Definition**: grading by a peer; a small change to the project may be requested on the spot.
- **Use**: be ready to change a port, a name, a variable in a few minutes — hence understanding every line. See [`13_DEFENSE.md`](13_DEFENSE.md).

---

## 🧰 Useful commands (cheat sheet)

| Command | Role |
|---|---|
| `docker compose -f srcs/docker-compose.yml up -d --build` | Build and start the stack in the background |
| `docker compose -f srcs/docker-compose.yml down` | Stop and remove containers + network (volumes kept) |
| `docker compose ... down -v` | Same **+ removes volumes** |
| `docker ps` / `docker ps -a` | Running / all containers |
| `docker images` | Local images (check the names) |
| `docker logs -f <c>` | Follow a container's logs |
| `docker exec -it <c> sh` | Open a shell in a running container |
| `docker inspect <c>` | A container's full config (network, mounts, restart…) |
| `docker network ls` / `inspect` | Networks and attached containers |
| `docker volume ls` / `inspect` | Volumes and their `device` on the host |
| `docker system prune -a --volumes` | Clean everything (⚠️ destructive) |
| `openssl s_client -connect login.42.fr:443 -tls1_2` | Test the TLS version |
| `docker exec -it mariadb mariadb -u root -p` | Open the SQL console |
