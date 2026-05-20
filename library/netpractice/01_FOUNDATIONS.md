# 01 — Foundations: What is an IP Address?

> Before you solve a single level, you need the vocabulary. This page is the dictionary.

---

## 1. The IPv4 address — 32 bits, four bytes

An IPv4 address is a **32-bit unsigned integer**, written for humans as four decimal octets separated by dots.

```
       192   .    168   .     1    .   42
    ┌─────────┬─────────┬─────────┬─────────┐
    │11000000 │10101000 │00000001 │00101010 │   ← 32 bits, 4 octets
    └─────────┴─────────┴─────────┴─────────┘
       byte0     byte1     byte2     byte3
```

- Each octet ranges **0–255** (one byte, 8 bits).
- The whole address fits in `uint32_t` — `192.168.1.42` is `0xC0A8012A`.
- Hosts and routers don't see the dots. The dots are a human convenience.

**Why this matters in NetPractice:** every check you'll perform (subnet match, range membership, broadcast) happens in binary. The dotted-decimal form is just for display.

---

## 2. Special / reserved ranges

You won't pick these in NetPractice, but you must **recognize** them.

| Range | Name | Purpose |
|---|---|---|
| `0.0.0.0/8` | "This network" | Source-only, or "any address" in routing |
| `127.0.0.0/8` | Loopback | `127.0.0.1` is always your own machine |
| `10.0.0.0/8` | Private (RFC 1918) | LANs, NAT'd networks |
| `172.16.0.0/12` | Private (RFC 1918) | Corporate LANs |
| `192.168.0.0/16` | Private (RFC 1918) | Home networks |
| `169.254.0.0/16` | Link-local | DHCP failure auto-assignment |
| `224.0.0.0/4` | Multicast | One-to-many delivery |
| `255.255.255.255` | Limited broadcast | All hosts on local segment |

In NetPractice you'll mostly stay in the private ranges — that's normal for a lab.

---

## 3. Address classes (historical — still useful as intuition)

Before CIDR (1993), addresses had **fixed class-based masks**:

| Class | First bits | Range | Default mask | Hosts per net |
|---|---|---|---|---|
| A | `0xxxxxxx` | `0.0.0.0` – `127.255.255.255` | `/8` (`255.0.0.0`) | ~16.7M |
| B | `10xxxxxx` | `128.0.0.0` – `191.255.255.255` | `/16` (`255.255.0.0`) | ~65 K |
| C | `110xxxxx` | `192.0.0.0` – `223.255.255.255` | `/24` (`255.255.255.0`) | 254 |
| D | `1110xxxx` | `224.0.0.0` – `239.255.255.255` | (multicast) | — |
| E | `1111xxxx` | `240.0.0.0` – `255.255.255.255` | (reserved) | — |

**Classes are obsolete** — modern networks (and NetPractice) use **CIDR**, where the mask length is arbitrary. But if you see `192.x.x.x` with a `/24` mask, you're seeing a "class C habit."

---

## 4. CIDR notation — the mask, finally

CIDR (*Classless Inter-Domain Routing*) writes an address with a **prefix length**:

```
   192.168.1.42 / 24
   └────┬─────┘  └┬┘
     address    prefix length (in bits)
```

`/24` means: **the first 24 bits identify the network, the remaining 8 bits identify the host.**

Equivalent forms for `/24`:
- Prefix: `/24`
- Dotted mask: `255.255.255.0`
- Binary: `11111111.11111111.11111111.00000000`

The mask is **always a contiguous run of 1s followed by a contiguous run of 0s** in IPv4. `255.255.0.255` is **not a valid mask** — NetPractice will reject it.

`★ Insight ─────────────────────────────────────`
The prefix length is just "how many leading 1 bits the mask has." `/24` → 24 ones. `/27` → 27 ones. `/30` → 30 ones. Counting bits is faster than memorizing the dotted form once you've done it 5 times.
`─────────────────────────────────────────────────`

---

## 5. Common masks — memorize this table

This is the single most useful table in the project. Print it out.

| CIDR | Dotted mask | Host bits | Total addresses | Usable hosts |
|---|---|---|---|---|
| `/24` | `255.255.255.0`   | 8 | 256 | 254 |
| `/25` | `255.255.255.128` | 7 | 128 | 126 |
| `/26` | `255.255.255.192` | 6 | 64  | 62  |
| `/27` | `255.255.255.224` | 5 | 32  | 30  |
| `/28` | `255.255.255.240` | 4 | 16  | 14  |
| `/29` | `255.255.255.248` | 3 | 8   | 6   |
| `/30` | `255.255.255.252` | 2 | 4   | 2   |
| `/31` | `255.255.255.254` | 1 | 2   | 2*  |
| `/32` | `255.255.255.255` | 0 | 1   | 1   |

\* `/31` is special — RFC 3021 allows it for point-to-point links with **no network or broadcast address**, both addresses are usable.

**Usable hosts = total − 2** (one network address, one broadcast) — except `/31` and `/32`.

**The mask octet values you'll see**: `0, 128, 192, 224, 240, 248, 252, 254, 255`. Memorize these nine numbers. They are the only values a mask octet can ever take.

```
  bits    0   1   2   3   4   5   6   7   8
  value   0  128 192 224 240 248 252 254 255
```

Each step doubles: `128 → 192 (+64) → 224 (+32) → 240 (+16) → 248 (+8) → 252 (+4) → 254 (+2) → 255 (+1)`.

---

## 6. Network address, host address, broadcast

For any IP/mask pair, three derived values exist:

```
        Network address    First         Last      Broadcast
              │           usable        usable          │
              ▼              ▼            ▼             ▼
         192.168.1.0   192.168.1.1  192.168.1.254  192.168.1.255
         └──────────── 192.168.1.0/24 ─────────────────────────┘
                            254 usable host addresses
```

- **Network address**: all host bits = 0. Identifies the subnet, not a host. Cannot be assigned.
- **Broadcast**: all host bits = 1. A packet to this address goes to every host on the subnet. Cannot be assigned.
- **Usable hosts**: everything between.

**NetPractice will reject** an interface IP equal to the network or broadcast address of its own subnet. If a level shows a red interface and the IP looks valid, check this first.

---

## 7. Subnet membership — the one question that matters

Given a destination IP and your own (IP, mask), there is exactly one question:

> **Is the destination in my subnet?**

Computed by: `(destIP AND mask) == (myIP AND mask)`.

- **Yes** → send directly out the local interface (ARP, then the frame).
- **No** → consult the routing table to find a gateway that *is* in my subnet, and send the frame there. (Covered in [`04_ROUTING.md`](04_ROUTING.md).)

Everything in NetPractice reduces to this question, repeated at each hop.

---

## Next

→ [`02_BINARY_AND_MASKS.md`](02_BINARY_AND_MASKS.md): The binary math behind the AND. Doing it by hand in under 10 seconds.
