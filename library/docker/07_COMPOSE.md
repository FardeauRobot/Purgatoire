# 07 — Docker Compose

> Compose is a YAML transcription of `docker run` flags you already know. If a block in your compose file doesn't map back to something you could do by hand, you're copying rather than writing — and that gap is exactly where defense questions land.

---

## 1. What it does

`docker run` per container means long flag lists, remembered order, and a network to create by hand. Compose declares the whole stack in one file and reconciles reality against it.

```bash
docker compose up -d --build
```

That one command builds every image, creates the network, creates the volumes, starts everything in dependency order, and applies restart policies.

> Write `docker compose` (v2, Go plugin), not `docker-compose` (v1, Python, deprecated).

---

## 2. Anatomy

```yaml
services:          # the containers
volumes:           # named volumes, declared at top level
networks:          # networks, declared at top level
secrets:           # secret definitions
```

> **No `version:` key.** The old `version: "3.8"` at the top is obsolete in Compose v2 and now emits a warning. Old tutorials all have it; leave it out.

---

## 3. The service block, field by field

Each field maps to a `docker run` flag. That mapping is the thing to know.

```yaml
services:
  nginx:
    build:
      context: ./requirements/nginx     # ← docker build ./requirements/nginx
      dockerfile: Dockerfile
    image: nginx                        # ← -t nginx   (rule S5: image name == service name)
    container_name: nginx
    ports:
      - "443:443"                       # ← -p 443:443
    volumes:
      - wordpress:/var/www/html         # ← -v wordpress:/var/www/html
    networks:
      - inception                       # ← --network inception
    depends_on:
      - wordpress
    env_file:
      - .env                            # ← --env-file .env
    environment:
      - DOMAIN_NAME=${DOMAIN_NAME}      # ← -e DOMAIN_NAME=...
    restart: unless-stopped             # ← --restart unless-stopped
```

**`build:` vs `image:`.** With both, compose builds from `context` and *tags the result* with `image`. That's how Inception rule S5 (image name matches service name) is satisfied. With `image:` alone, compose pulls a ready-made image — which the subject forbids for anything but the Alpine base.

**`container_name:`** fixes the container's name instead of letting compose generate `<project>-<service>-1`. Convenient for typing; it also prevents scaling that service beyond one replica, which is irrelevant here.

**`env_file:` vs `environment:`.** `env_file` loads a file wholesale; `environment` sets values explicitly, and `${VAR}` interpolates from the `.env` sitting next to the compose file. The interpolation source and `env_file` are separate mechanisms that happen to point at the same file in this project — worth knowing they're distinct.

---

## 4. Volumes and networks

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

networks:
  inception:
    driver: bridge
```

Full explanation of the `driver_opts` pattern — and why it's a named volume rather than a bind mount — in [`06_STORAGE.md`](06_STORAGE.md). Networking in [`05_NETWORKING.md`](05_NETWORKING.md).

---

## 5. Secrets

```yaml
secrets:
  db_password:
    file: ../secrets/db_password.txt

services:
  mariadb:
    secrets:
      - db_password
```

The file's contents appear in the container at `/run/secrets/db_password`. The service reads it as a file rather than receiving it as an environment variable. Details, and the honest limits of what this buys you outside Swarm, in [`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md).

---

## 6. A skeleton for Inception

The shape, with the bodies left for you:

```yaml
services:
  mariadb:
    build: ./requirements/mariadb
    image: mariadb
    container_name: mariadb
    volumes:
      - mariadb:/var/lib/mysql
    networks:
      - inception
    env_file:
      - .env
    secrets:
      - db_password
      - db_root_password
    restart: unless-stopped

  wordpress:
    build: ./requirements/wordpress
    image: wordpress
    container_name: wordpress
    volumes:
      - wordpress:/var/www/html
    networks:
      - inception
    depends_on:
      - mariadb
    env_file:
      - .env
    secrets:
      - db_password
      - credentials
    restart: unless-stopped

  nginx:
    build:
      context: ./requirements/nginx
      args:
        DOMAIN_NAME: ${DOMAIN_NAME}   # ← build-time; RUN can't see env_file
    image: nginx
    container_name: nginx
    ports:
      - "443:443"
    volumes:
      - wordpress:/var/www/html
    networks:
      - inception
    depends_on:
      - wordpress
    env_file:
      - .env
    restart: unless-stopped

volumes:
  mariadb:
    driver: local
    driver_opts: { type: none, device: /home/<login>/data/mariadb, o: bind }
  wordpress:
    driver: local
    driver_opts: { type: none, device: /home/<login>/data/wordpress, o: bind }

networks:
  inception:
    driver: bridge

secrets:
  db_password:
    file: ../secrets/db_password.txt
  db_root_password:
    file: ../secrets/db_root_password.txt
  credentials:
    file: ../secrets/credentials.txt
```

Points to notice, each of which is a possible question:

- Only nginx has `ports:`. Rule V4 — single entrypoint.
- The `wordpress` volume is mounted in **two** services. Deliberate; see [`06_STORAGE.md`](06_STORAGE.md).
- Every service has `restart:`. Rule V5.
- `image:` matches the service name. Rule S5.
- Secret paths are `../secrets/…` because they're resolved **relative to the compose file**, which lives in `srcs/` while `secrets/` sits at the project root.
- `depends_on` here controls order only, not readiness — see [`04_CONTAINERS.md`](04_CONTAINERS.md) for the healthcheck fix.
- nginx uses the **long-form `build:`** with `args:`, because its Dockerfile needs `DOMAIN_NAME` at *build* time for the TLS certificate's `CN`. `env_file:` and `environment:` are runtime-only and invisible to `RUN` — see [`03_IMAGES_AND_DOCKERFILES.md`](03_IMAGES_AND_DOCKERFILES.md). The other two services need nothing at build time, so the short form is fine for them.

---

## 7. Driving it from the Makefile

Rule S3: a Makefile at the project root that builds everything via `docker-compose.yml`.

```make
NAME     = inception
COMPOSE  = docker compose -f srcs/docker-compose.yml
DATA_DIR = /home/$(USER)/data

all: up

up:
	mkdir -p $(DATA_DIR)/wordpress $(DATA_DIR)/mariadb
	$(COMPOSE) up -d --build

down:
	$(COMPOSE) down

re: down up

clean:
	$(COMPOSE) down --rmi all

fclean: clean
	$(COMPOSE) down -v --rmi all
	sudo rm -rf $(DATA_DIR)/wordpress $(DATA_DIR)/mariadb

logs:
	$(COMPOSE) logs -f

ps:
	$(COMPOSE) ps

.PHONY: all up down re clean fclean logs ps
```

Two things this gets right, both of which matter:

- **`mkdir -p` before `up`.** The `driver_opts` bind fails if the directory doesn't exist ([`06_STORAGE.md`](06_STORAGE.md)). Without this line the project works on your machine — where you created the directory by hand once — and fails on the evaluator's.
- **The destructive target is the one named `fclean`.** `down -v` deletes your volumes. Keep it out of `clean`, and know which target does what, because you will be asked and answering wrong in front of a running stack is expensive.

---

## 8. Commands

```bash
docker compose up -d --build      # build and start
docker compose down               # stop, remove containers + networks (volumes survive)
docker compose down -v            # ← also destroys volumes
docker compose ps                 # status
docker compose logs -f <service>
docker compose exec <service> sh
docker compose restart <service>
docker compose build --no-cache <service>
docker compose config             # ← resolved file, with variables substituted
```

**`docker compose config` is the debugging tool people forget.** It prints the fully resolved file — every `${VAR}` substituted, every path expanded. When a variable is empty or a path is wrong, this shows you what compose actually sees, rather than what you think you wrote. Run it before your first `up`.

---

## 9. 🚪 Gate

1. For every block in your compose file, what's the equivalent `docker run` flag?
2. Why does only nginx have a `ports:` entry?
3. Why is the same volume mounted into two services?
4. Why are the secret paths `../secrets/…` rather than `./secrets/…`?
5. Which Makefile target destroys your data, and what exactly does it run?

---

**Next:** [`08_SECRETS_AND_ENV.md`](08_SECRETS_AND_ENV.md) — the credentials that must never reach git.
