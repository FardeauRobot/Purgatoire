# 03 — Subnetting

> Given a network, slice it into smaller ones. Or: given an IP/mask, figure out exactly which addresses are valid for hosts.

---

## 1. The five numbers of any subnet

For any `IP/mask`, five derived values describe it completely:

```
  ┌─────────────────────────────────────────────────────┐
  │  network address     ← first  address (host bits all 0)
  │  first usable host   ← network + 1
  │  last  usable host   ← broadcast − 1
  │  broadcast address   ← last   address (host bits all 1)
  │  total addresses     ← 2 ^ (32 − prefix)
  └─────────────────────────────────────────────────────┘
```

**Usable hosts = total − 2** (exceptions: `/31` and `/32`).

### Worked example: `192.168.1.42 /26`

Step 1 — mask binary: `/26` → last octet = 192 → block size = 64.
Step 2 — network address: floor last octet to multiple of 64. `42 // 64 = 0 → 0`. → `192.168.1.0`.
Step 3 — broadcast: `192.168.1.0 + 64 − 1 = 192.168.1.63`.
Step 4 — first usable: `192.168.1.1`. Last usable: `192.168.1.62`.
Step 5 — total: `64`. Usable: `62`.

| Field | Value |
|---|---|
| Network | `192.168.1.0` |
| First usable | `192.168.1.1` |
| Last usable | `192.168.1.62` |
| Broadcast | `192.168.1.63` |
| Total / Usable | 64 / 62 |

This is the calculation you'll do dozens of times in NetPractice. Get fast at it.

---

## 2. Picking a valid host IP

When NetPractice asks for an IP on a given subnet, the rules are:

1. Must be **within the subnet's range** (between network and broadcast, exclusive).
2. Must **not equal another interface's IP** on the same wire.
3. Must **match the subnet** of any pre-set gateway it depends on.
4. The IP **must not be** the network address or the broadcast address.

If any of these fails, the interface lights up red.

---

## 3. Splitting a network — VLSM at the kitchen table

Suppose you have `10.0.0.0/24` (256 addresses) and need to split it into pieces. Adding **one bit to the mask halves the network** every time:

```
  /24  →   1 × 256 addresses           10.0.0.0/24
  /25  →   2 × 128 addresses           10.0.0.0/25     10.0.0.128/25
  /26  →   4 ×  64 addresses           .0  .64  .128  .192
  /27  →   8 ×  32 addresses           .0  .32  .64  .96  .128 .160 .192 .224
  /28  →  16 ×  16 addresses           .0, .16, .32, ..., .240
```

Each step **doubles the number of subnets** and **halves the addresses per subnet**.

### Example: split `10.0.0.0/24` to give each of 4 departments their own subnet

You need 4 subnets → 2 bits added → `/26`.

| Department | Subnet | Network | Broadcast | Hosts |
|---|---|---|---|---|
| Dept A | `10.0.0.0/26`   | `.0`   | `.63`  | `.1` – `.62` |
| Dept B | `10.0.0.64/26`  | `.64`  | `.127` | `.65` – `.126` |
| Dept C | `10.0.0.128/26` | `.128` | `.191` | `.129` – `.190` |
| Dept D | `10.0.0.192/26` | `.192` | `.255` | `.193` – `.254` |

In NetPractice, this kind of layout shows up when one router connects multiple LANs — each LAN must be its own subnet, and the router has one interface (one IP) in each.

---

## 4. Router interfaces — one foot per subnet

A router with three LAN interfaces:

```
                       ┌─────────────┐
        LAN A          │             │           LAN B
   10.0.0.0/26    ─────┤  R1         ├─────  10.0.0.64/26
                       │             │
                       │             │
                       └──────┬──────┘
                              │
                              ▼
                         LAN C
                      10.0.0.128/26
```

Each `R1` interface has its own IP **in its respective LAN's subnet**:
- `R1-A` could be `10.0.0.62/26` (last usable in LAN A)
- `R1-B` could be `10.0.0.126/26` (last usable in LAN B)
- `R1-C` could be `10.0.0.190/26` (last usable in LAN C)

Hosts on each LAN use their R1 interface IP as their **default gateway**.

`★ Insight ─────────────────────────────────────`
"First usable" and "last usable" are by convention often given to gateways (so hosts get the middle range). NetPractice doesn't enforce this — pick any valid host IP. But evaluators will likely ask "why this IP?" — answering "I picked the first/last usable for the gateway, leaving the rest for hosts" is a clean, defensible answer.
`─────────────────────────────────────────────────`

---

## 5. The `/30` link — the workhorse of router-to-router

Two routers connected by a serial link don't need 254 host addresses — they need **two**. `/30` gives you exactly 2 usable IPs:

```
  10.0.0.0/30
    ├── .0   network address
    ├── .1   router A
    ├── .2   router B
    └── .3   broadcast
```

You'll see `/30` (or `/31` per RFC 3021) on **point-to-point links** in higher NetPractice levels. It signals "this is a router-to-router link, nothing else lives here."

```
  ┌────────┐         /30 link         ┌────────┐
  │   R1   ├───── 10.0.0.0/30 ────────┤   R2   │
  │  .1    │                          │  .2    │
  └────────┘                          └────────┘
```

---

## 6. Hidden trap: overlapping subnets

NetPractice won't always warn you when two interfaces define **overlapping** subnets. Example:

```
  Host H1:  10.0.0.5    /24    →  subnet  10.0.0.0/24    (covers 10.0.0.0 – 10.0.0.255)
  Host H2:  10.0.0.5    /16    →  subnet  10.0.0.0/16    (covers 10.0.0.0 – 10.0.255.255)
```

Both think they're on different subnets, but H1's subnet is contained within H2's. Packets between them may or may not work depending on which way they go — this is a misconfiguration the project tests you on.

**Rule:** all interfaces on the **same broadcast domain** (same wire / switch) must agree on the same subnet — same network address AND same mask. Both halves matter.

---

## 7. Two hosts on different subnets through the same switch — broken

```
   Switch
   ┌──┴──┐
   │     │
  H1     H2
  10.0.0.1/24    10.0.1.1/24
```

These two **cannot** communicate directly. Even though they're physically on the same switch, their masks place them on different logical subnets. H1 thinks H2 is "far" and will look for a gateway.

Fix in NetPractice: either change one mask to make them match, or accept that they need to go through a router — but you can't put a router in the middle of a switch in NetPractice; you'd have to redesign the topology.

If you see this in a level, it's almost always a **wrong mask**, not a wrong topology.

---

## 8. Recipe — "I have an IP, give me the subnet"

A repeatable procedure you can run in your head:

```
INPUT:   IP   = a.b.c.d
         mask = /n   (or dotted)

  1. Find the interesting octet i  (the one that the mask doesn't fully cover).
     If /n is 8,16,24 → interesting octet is the next one (host portion start).

  2. block = 256 − mask[i]

  3. floor = (IP[i] / block) * block       // integer division

  4. network   = IP[0..i-1] . floor . 0 . 0  (zero remaining octets)
     broadcast = IP[0..i-1] . (floor + block − 1) . 255 . 255 ...
                                              (fill remaining octets with 255)

  5. first usable = network + 1
     last usable  = broadcast − 1
```

Try it on `172.20.130.200/19`:

- `/19` = 16 + 3 ones in 3rd octet → mask = `255.255.224.0`, block on octet 2 = 256 − 224 = 32.
- IP[2] = 130 → floor = (130 / 32) * 32 = 4 * 32 = **128**.
- Network = `172.20.128.0`. Broadcast = `172.20.(128+32−1).255` = `172.20.159.255`.
- Usable: `172.20.128.1` – `172.20.159.254`. Hosts = 8190.

You can do this on paper in under a minute. By level 6 of NetPractice, you'll do it without paper.

---

## Next

→ [`04_ROUTING.md`](04_ROUTING.md): once you've assigned valid IPs/masks, packets still need to **find their way** across multiple subnets. That's routing.
