# 05 — Networking

> Two questions: how do containers reach each other, and how does the outside world reach in? Inception's answers are "by name, on a private network" and "through nginx on 443, and nowhere else."

You built a webserv. You know sockets, `bind()`, `listen()`, ports. This page is mostly about *whose* port space you're binding into.

---

## 1. Network namespaces, briefly

Each container gets its own network namespace: its own interfaces, routing table, iptables rules, and **its own port space**. That last point has a consequence worth stating explicitly — three containers can all bind port 443 simultaneously without conflict, because there are three separate port spaces.

Docker wires a container to a network with a **veth pair**: a virtual cable with one end inside the container (as `eth0`) and the other plugged into a bridge on the host.

```
   HOST
   ┌──────────────────────────────────────────┐
   │  eth0 (real NIC)                         │
   │    │                                     │
   │  ┌─┴──────────────┐   docker0 / br-xxxx  │
   │  │  bridge        │                      │
   │  └──┬──────────┬──┘                      │
   │   veth       veth                        │
   │     │          │                         │
   │  ┌──┴───┐   ┌──┴────┐                    │
   │  │ eth0 │   │ eth0  │  ← inside containers│
   │  │nginx │   │mariadb│                     │
   │  └──────┘   └───────┘                    │
   └──────────────────────────────────────────┘
```

---

## 2. Network drivers

| Driver | What it does |
|---|---|
| **bridge** | Default. Private network on the host; containers get internal IPs. |
| **host** | No namespace of its own — shares the host's stack. **Forbidden by Inception.** |
| **none** | No networking at all. |
| **overlay** | Multi-host networking for Swarm. |
| **macvlan** | Container gets a MAC address on the physical LAN. |

You'll use `bridge`, which is what compose creates for you by default.

---

## 3. The default bridge vs a user-defined bridge

This distinction is the single most useful thing on this page.

**The default `bridge` network** (the one containers land on with no configuration) has **no DNS**. Containers can reach each other by IP, and IPs change on every restart. This is why Docker once had a `--link` flag — it injected `/etc/hosts` entries. It was fragile, and it's deprecated.

**A user-defined bridge network** runs an **embedded DNS server at `127.0.0.11`** that resolves **container and service names to their current IPs**, automatically, with no configuration.

```bash
docker network create testnet
docker run -d --name alice --network testnet alpine sleep 300
docker run --rm --network testnet alpine ping -c2 alice   # ✓ resolves
docker run --rm alpine ping -c2 alice                     # ✗ default bridge, no DNS
```

**This is why WordPress reaches the database at the hostname `mariadb` and nothing in any config file defines that mapping.** The service name in `docker-compose.yml` becomes a DNS name on the network. Docker Compose always creates a user-defined network, so you get this for free — which is exactly why people don't notice it's happening and can't explain it when asked.

Other properties of user-defined networks worth knowing: containers can be attached and detached while running, and containers on *different* user-defined networks cannot reach each other at all. Network membership is the isolation boundary.

---

## 4. Publishing ports

Internal networking is invisible from outside the host. To let the world in, publish:

```yaml
ports:
  - "443:443"      # host:container
```

```bash
docker run -p 8080:80 nginx-img     # host 8080 → container 80
docker run -p 127.0.0.1:8080:80 …   # bind only to loopback
```

Docker implements this with an iptables DNAT rule plus a userland proxy. Traffic hitting the host on the left-hand port is forwarded to the right-hand port in the container.

Two things to be precise about:

- **`EXPOSE` in a Dockerfile publishes nothing.** It's documentation. Only `ports:` / `-p` actually opens anything.
- **`ports:` vs `expose:` in compose:** `ports:` publishes to the host; `expose:` only documents. Containers on the same network can reach *any* port on each other regardless of either — network membership grants access, not port declarations.

That last point is what makes Inception's architecture work. MariaDB never publishes 3306. php-fpm never publishes 9000. They're reachable from inside the network anyway, and unreachable from outside. **Only nginx publishes anything, and only 443.**

---

## 5. Inception's network rules

### `networks` must be present

Rule N5. Compose creates a default network whether or not you write one, but the subject wants it declared explicitly:

```yaml
networks:
  inception:
    driver: bridge

services:
  nginx:
    networks:
      - inception
  wordpress:
    networks:
      - inception
  mariadb:
    networks:
      - inception
```

### `host`, `--link`, and `links:` are forbidden

Rule N6.

`network_mode: host` removes the container's network namespace entirely. Binding port 3306 inside would bind port 3306 **on the host**, exposing MariaDB to the outside world. The "nginx is the only entrypoint" guarantee (V4) would become untrue in the most direct way possible. It would also make the three containers fight over ports, and it doesn't meaningfully work on macOS anyway.

`--link` and `links:` are the deprecated pre-DNS mechanism from §3. Using them signals you didn't understand that user-defined networks solved this in 2016.

### The resulting shape

```
   WWW ──443──► [nginx] ──9000──► [wordpress/php-fpm] ──3306──► [mariadb]
                   │                       │                        │
                   └───────────────────────┴────────────────────────┘
                              inception network (bridge)
```

Only the first arrow crosses the host boundary. Ports 9000 and 3306 exist only inside.

---

## 6. Ports 9000 and 3306

**9000 — FastCGI.** php-fpm does not speak HTTP. It speaks FastCGI, a binary protocol. nginx receives an HTTP request for a `.php` file, translates it into a FastCGI request, sends it to `wordpress:9000`, gets the response, and translates it back into HTTP.

You cannot `curl` port 9000 and get anything useful. This surprises people testing the WordPress container in isolation — it looks broken when it's working correctly. Test it with `nc -zv wordpress 9000` (is anything listening?) rather than `curl`.

In nginx's config this is the `fastcgi_pass wordpress:9000;` line — the service name resolving via §3's embedded DNS.

**3306 — MySQL protocol.** WordPress's PHP connects directly. In `wp-config.php` the database host is `mariadb`, or `mariadb:3306`. Again, a service name resolved by Docker's DNS.

---

## 7. Debugging the network

```bash
docker network ls
docker network inspect inception          # which containers, which IPs
docker compose exec wordpress ping -c2 mariadb    # DNS + reachability
docker compose exec wordpress nc -zv mariadb 3306 # is the port open?
docker compose exec wordpress cat /etc/resolv.conf # should show 127.0.0.11
```

If `ping mariadb` fails, work through it in order:

1. Are both containers actually on the same network? (`docker network inspect`)
2. Is the target running? (`docker compose ps` — a stopped container has no DNS entry)
3. Are you using the **service** name from the compose file, not the container name?
4. Are you on the default bridge instead of a user-defined network? (No DNS there.)

If `ping` works but `nc` fails, DNS is fine and the service isn't listening — that's an application problem, not a network one. Useful split: it tells you which container to read the logs of.

---

## 8. 🚪 Gate

1. Nothing in your configuration maps `mariadb` to an IP. What resolves it?
2. Why can three containers each bind port 443 without conflict?
3. Why is MariaDB unreachable from the host but reachable from the WordPress container?
4. Does `EXPOSE 9000` make port 9000 reachable from your browser?
5. Precisely what breaks about Inception's security model under `network_mode: host`?

---

**Next:** [`06_STORAGE.md`](06_STORAGE.md) — making the data survive.
