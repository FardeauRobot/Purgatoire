# 07 — Evaluation Cheatsheet

> Skim this 15 minutes before evaluation. Everything compressed to the bone.

---

## The three invariants

1. **Same subnet, same wire** — two interfaces on the same wire ⇔ `IP AND mask` identical, masks identical.
2. **Gateway in host's subnet** — `gateway AND mask == host_IP AND mask`.
3. **Routes both ways** — for every (source, destination) pair, every router along the path must have a forward route AND a return route.

---

## Mask table (memorize the right column)

| /n   | Dotted          | Block | Total | Usable |
|------|-----------------|-------|-------|--------|
| /24  | 255.255.255.0   | 256   | 256   | 254    |
| /25  | 255.255.255.128 | 128   | 128   | 126    |
| /26  | 255.255.255.192 | 64    | 64    | 62     |
| /27  | 255.255.255.224 | 32    | 32    | 30     |
| /28  | 255.255.255.240 | 16    | 16    | 14     |
| /29  | 255.255.255.248 | 8     | 8     | 6      |
| /30  | 255.255.255.252 | 4     | 4     | 2      |

**Mask octet values (the only legal ones):** `0, 128, 192, 224, 240, 248, 252, 254, 255`.

---

## "Where does this IP fall?" — the recipe

```
  block = 256 − mask[interesting_octet]
  floor = (IP[interesting_octet] / block) * block
  network   = ...floor.0.0...
  broadcast = ...(floor + block − 1).255.255...
  first usable = network + 1
  last  usable = broadcast − 1
```

Try mentally: `172.20.130.200/19`. Block = 32. Floor = 128. Subnet `172.20.128.0`, broadcast `172.20.159.255`.

---

## Routing in one paragraph

A packet for destination D walks the table. Any row where `(D AND row.mask) == row.network` matches. **Longest prefix wins.** The default route `0.0.0.0/0` always matches but has prefix 0 — used only when no other row applies. The chosen row's **next-hop must be in a directly-connected subnet** of the router.

---

## Defending your answers

When an evaluator points at any cell, be ready to say:

| They ask | You answer |
|---|---|
| "Why this IP?" | "It's in the subnet `X.Y.Z.0/n`, between network and broadcast, and not used elsewhere." |
| "Why this mask?" | "It matches the mask of every other interface on this wire, and the subnet size fits the hosts on this LAN." |
| "Why this gateway?" | "It's the router's interface on this host's subnet — `gateway AND mask == host AND mask`." |
| "Why this route?" | "Router R needs to reach LAN X. The next hop toward X is router N, whose interface on R's link is `N_IP`. So the entry is `X.0/mask → N_IP`." |
| "Why a default route here?" | "Because all non-local traffic exits this router via the same link, a single default `0.0.0.0/0 → next` is enough." |
| "Could you use a different IP?" | "Yes — any usable host IP in the subnet works. I chose `.X` because [first/last usable was free, gateways conventionally take edges, etc.]." |

---

## Common red-flag patterns (and the fix in one line)

| Symptom | Fix |
|---|---|
| Interface red, IP looks "round" | Check it's not network or broadcast address |
| Host can't reach gateway | Gateway must be in host's subnet — re-derive |
| One ping direction works | Return route missing on some router |
| All green but level not validated | Read the test description — far hosts may be involved |
| Two hosts on same switch fail | Masks or subnets disagree — make them identical |

---

## Process for each level

```
  1. PAPER — draw topology, label every interface and link.
  2. SUBNETS — for each wire, compute (network, broadcast, usable range).
  3. IPS — pick host IPs in usable range, avoid duplicates.
  4. GATEWAYS — set each host's gateway to a router IP in the host's subnet.
  5. ROUTES — for each remote LAN, add a route on each transit router.
  6. TRACE — walk a ping in both directions. Find the first failure.
  7. FIX one thing. Re-trace.
```

---

## Final checklist before clicking "validate"

- [ ] Every host has an IP, a mask, and (where needed) a gateway.
- [ ] Every router has IPs on each interface, all in different subnets.
- [ ] Every router's routes' next-hops are in directly-connected subnets.
- [ ] No host uses the network or broadcast address of its subnet.
- [ ] No two interfaces on the same wire have different masks.
- [ ] No two interfaces have the same IP.
- [ ] You can explain, on paper, why every field has the value it does.

If all seven are yes, click. If any is no, fix it before clicking — clicking the wrong thing repeatedly is how students waste evaluation time.

---

## Vocabulary that may come up in defense

| Term | One-line definition |
|---|---|
| Subnet mask | A 32-bit value with N leading 1s indicating which bits of the address are the network portion. |
| CIDR | Notation `a.b.c.d/n` combining address and prefix length. |
| Network address | First address of a subnet (host bits all 0). Not assignable to a host. |
| Broadcast address | Last address of a subnet (host bits all 1). Goes to all hosts on the subnet. |
| Default gateway | The next-hop for any destination not on a directly-connected subnet. |
| Routing table | Per-host (and per-router) list of rules mapping destination subnets to next-hops. |
| Longest-prefix match | The rule that the routing entry with the longest mask wins when multiple match. |
| ARP | Layer-2 protocol that maps an IP to a MAC address on the local subnet. (Out of scope for NetPractice, but evaluators sometimes ask.) |
| Loopback | `127.0.0.1` — always the local machine. |
| RFC 1918 | The private IPv4 ranges (`10.x`, `172.16–31.x`, `192.168.x`). |

---

## Last thought

You will not be evaluated on whether you got the answer fastest. You'll be evaluated on whether you understand it. **Slow is smooth, smooth is fast.** Take a breath, draw the topology, and explain your reasoning. The rest follows.

Good luck.
