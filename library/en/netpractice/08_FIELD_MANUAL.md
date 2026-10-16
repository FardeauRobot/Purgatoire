# 08 — Field Manual (Solve & Defend)

> The merge of [`06_TROUBLESHOOTING.md`](06_TROUBLESHOOTING.md) (the diagnostic tree) and [`07_EVAL_CHEATSHEET.md`](07_EVAL_CHEATSHEET.md) (the compressed reference), reorganized around **doing the exercise**: compute → place → route → trace → defend. This is the one page to keep open while you solve.

---

## 0. The three invariants (everything reduces to these)

1. **Same wire ⇒ same subnet.** Two interfaces on one wire can talk directly ⇔ `IP & mask` is identical for both **and** the masks are identical.
2. **Gateway lives in the host's subnet.** `gateway & mask == host_IP & mask`. A host can only hand a frame to a next-hop it can reach without routing.
3. **Routes go both ways.** For every (source, destination) the level tests, every router on the path needs a **forward** route *and* the return path needs a **return** route.

> **Why "masks identical," not just "same network"?** Reachability is bidirectional. A can reach B only if B is inside A's subnet, *and* B can reach A only if A is inside B's subnet. With different masks those two tests can disagree — one side goes green, the other red. Matching masks is the defensible way to guarantee both at once.

---

## 1. The one computation that matters — the block method

Every level is gated on one skill: given `a.b.c.d/n`, produce **network, broadcast, first usable, last usable**. Do this cold, on paper, and 80% of levels fall out.

### Step 1 — find the *interesting octet*

The interesting octet is the one where the mask is **neither 0 nor 255** — it's where the network/host boundary cuts through.

```
  /24  →  255.255.255.0     interesting octet = 4th
  /26  →  255.255.255.192   interesting octet = 4th
  /19  →  255.255.224.0     interesting octet = 3rd
  /12  →  255.240.0.0       interesting octet = 2nd
```

### Step 2 — the block size (a.k.a. the magic number)

```
  block = 256 − (mask value of the interesting octet)
```

`/26` → `256 − 192 = 64`.  `/19` → `256 − 224 = 32`.

### Step 3 — floor the IP's interesting octet to a multiple of the block

```
  floor = (IP's interesting octet ÷ block, integer)  ×  block
```

### Step 4 — assemble (this is where the cheatsheet was sloppy)

```
  octets BEFORE the interesting one  →  copied from the IP, unchanged
  the interesting octet              →  floor            (network)
                                        floor + block−1  (broadcast)
  octets AFTER the interesting one   →  0  (network)   /   255  (broadcast)
```

`★ Correction ────────────────────────────────────`
The old cheatsheet wrote `network = …floor.0.0…` and `broadcast = …(floor+block−1).255.255…`, which silently assumes the **2nd** octet is interesting — yet its own example used the **3rd**. The rule above is position-independent: *before = copy, at = floor, after = 0 or 255.*
`──────────────────────────────────────────────────`

### Worked: `172.20.130.200/19`

```
  interesting octet = 3rd        (mask 255.255.224.0)
  block  = 256 − 224 = 32
  floor  = (130 ÷ 32) × 32 = 4 × 32 = 128

  network      = 172.20.128.0        (before: 172.20 | at: 128 | after: 0)
  broadcast    = 172.20.159.255      (128 + 32 − 1 = 159 | after: 255)
  first usable = 172.20.128.1
  last usable  = 172.20.159.254
```

### Worked: `192.168.1.130/26` (last-octet case, the common one)

```
  interesting octet = 4th
  block  = 256 − 192 = 64
  floor  = (130 ÷ 64) × 64 = 2 × 64 = 128

  network   = 192.168.1.128
  broadcast = 192.168.1.191          (128 + 64 − 1)
  usable    = .129 … .190
```

---

## 2. Mask table (memorize the usable column)

| /n  | Dotted          | Block (increment) | Usable hosts |
|-----|-----------------|-------------------|--------------|
| /24 | 255.255.255.0   | 256               | 254          |
| /25 | 255.255.255.128 | 128               | 126          |
| /26 | 255.255.255.192 | 64                | 62           |
| /27 | 255.255.255.224 | 32                | 30           |
| /28 | 255.255.255.240 | 16                | 14           |
| /29 | 255.255.255.248 | 8                 | 6            |
| /30 | 255.255.255.252 | 4                 | 2            |

- **Usable = block − 2** (subtract network + broadcast). The dropped "Total" column from the old table was just the block size restated — same number whenever the mask lives in the last octet.
- **Only legal mask-octet values:** `0, 128, 192, 224, 240, 248, 252, 254, 255`. Anything else (e.g. `255.255.255.100`) is instantly wrong — the 1-bits must be contiguous from the left.
- **/30 is the point-to-point mask.** Exactly 2 usable addresses = the two router feet on a router-to-router wire. When you see two routers cabled directly, `/30` is almost always the intended link size. (NetPractice avoids `/31`; treat `/30` as the smallest link you'll build.)

---

## 3. Process — the order to fill fields in

Never solve fields in the order the UI lists them. Solve in **dependency order**, because each step constrains the next.

```
  1. PAPER    Redraw the topology. Label every interface, every wire.
              Mark which fields are EDITABLE vs LOCKED (grayed).
  2. SUBNETS  For each wire, compute (network, broadcast, usable range)
              using §1. Do the LOCKED interfaces first — they dictate
              the subnet everyone else on that wire must join.
  3. IPS      Pick host IPs from the MIDDLE of the usable range.
              No duplicates on a wire.
  4. GATEWAYS Each host's gateway = a router interface IN the host's
              own subnet (invariant 2).
  5. ROUTES   For each remote LAN, add a route on each transit router:
              destination = remote LAN's network/mask,
              next-hop    = the next router toward it (must be a
              directly-connected neighbor).
  6. TRACE    Walk a ping in BOTH directions. Stop at the first failure.
  7. FIX one thing, re-trace. Never batch-guess.
```

`★ NetPractice-specific ───────────────────────────`
**Read the locked fields as the ground truth.** Later levels (esp. 9–10) hand you a mostly-filled routing table or a fixed IP and expect you to *conform* to it, not redesign around it. If a field is gray, it's a constraint — derive the subnet from it rather than fighting it. The most common late-level mistake is inventing a clean subnet that contradicts a value already locked on the wire.
`──────────────────────────────────────────────────`

---

## 4. When it's red — the five questions

Run these **in order**, stop at the first "no." That's your bug.

```
  1. Is the IP a VALID HOST for its subnet?
     └ not the network addr, not broadcast, inside range, not a duplicate.
  2. Do all interfaces on the SAME WIRE share subnet AND mask?
  3. Is the GATEWAY in the SAME SUBNET as the host?
  4. Does every router on the path have a route to the destination LAN —
     in BOTH directions?
  5. Any OVERLAPPING subnets or routing LOOPS?
```

~90% of red lights die at 1–3. The interesting failures live at 4–5 — see §5.

**Mask-mismatch trap (question 2).** Both ends can compute the *same network* yet differ on mask — one silently rejects:

```
  H1: 10.0.0.10 /24  → network 10.0.0.0   ✅
  H2: 10.0.0.20 /16  → network 10.0.0.0   ✅   …but masks differ → BROKEN
```

**Gateway trap (question 3).**

```
  Host:    192.168.1.50 /24   → subnet 192.168.1.0/24
  Gateway: 192.168.2.1        → subnet 192.168.2.0/24   ❌ not reachable
  Fix: gateway must be the router's foot ON 192.168.1.0/24.
```

---

## 5. The three topics that actually cost points

These are where evaluations are won or lost. Everything above is mechanical; this is judgment.

### 5.1 Asymmetric routing (the #1 post-L5 killer)

A ping is a **request + a response**. If the request arrives but the response can't get home, the level is still red — and the UI won't tell you which half failed.

```
  A ──[R1]────[R2]── B

  R1 knows how to reach B's LAN  ✅  (request gets there)
  R2 has NO route back to A's LAN ❌  (reply is dropped)
  → looks like "B unreachable," is really "reply lost."
```

**Discipline:** for every tested pair, trace forward *and* back, and confirm each transit router has a route for *both* endpoints' LANs.

### 5.2 Longest-prefix match & the default route

A packet for destination `D` scans the routing table. A row matches when `(D & row.mask) == row.network`. **When several match, the longest mask (most specific) wins.**

```
  Table on R:
    10.1.2.0/24    → next-hop N1     (prefix 24)
    0.0.0.0/0      → next-hop N2     (prefix 0, the default)

  Packet to 10.1.2.9  → both match → /24 wins → exits toward N1.
  Packet to 8.8.8.8   → only the default matches → exits toward N2.
```

- The default route `0.0.0.0/0` matches *everything* but has the shortest prefix, so it's the fallback of last resort.
- Every chosen next-hop **must be in a directly-connected subnet** of that router. A route pointing at an unreachable next-hop is dead.
- **Design rule to avoid loops:** in a chain of routers, only the one nearest the outside/Internet carries a default pointing outward. Inner routers use **specific** routes pointing *inward* toward their leaf LANs.

### 5.3 Overlaps & loops

```
  Overlap:  R1 foot 10.0.0.1  /24  → covers 10.0.0.0–10.0.0.255
            R2 foot 10.0.0.130/25  → covers 10.0.0.128–10.0.0.255
            → both claim 10.0.0.200. Ambiguous. Fix: narrow/realign masks.

  Loop:     R1 default → R2  AND  R2 default → R1
            → any non-local packet ping-pongs forever. Fix: break the
              symmetry (one default out, specific routes in).
```

---

## 6. Defensive habits (prevent the bug before it exists)

- **Subnet first, IP second.** Define network + mask, *then* pick a host inside it. Choosing the IP first makes you reverse-engineer the mask blindly.
- **Never leave a gateway blank** when the level crosses subnets. Blank is only correct on a single-wire level.
- **Sketch the topology** — including the router-to-router wire everyone forgets to give its own subnet.
- **Middle-of-range IPs.** Avoids network/broadcast edges and survives a later mask change (`.130` is still valid if a `/24` becomes `/25`; `.1` may get stranded).
- **One gateway convention:** always `.1` (or always `.254`) across every LAN. Zero lookup, no collisions.

---

## 7. Pre-validate checklist

- [ ] Every host has IP, mask, and (where crossing subnets) a gateway.
- [ ] Every router interface is in a **different** subnet from its siblings.
- [ ] Every route's next-hop sits in a **directly-connected** subnet.
- [ ] No host uses its subnet's network or broadcast address.
- [ ] No two interfaces on one wire disagree on mask.
- [ ] No duplicate IPs.
- [ ] Forward **and** return path traced for each tested pair.
- [ ] You can explain, on paper, why every field holds its value.

All yes → click validate. Any no → fix first. Clicking-and-hoping is how evaluation time evaporates.

---

## 8. Defense Q&A (what the evaluator will point at)

| They ask | You answer |
|---|---|
| "Why this IP?" | "It's inside `X.Y.Z.0/n`, between network and broadcast, unused elsewhere on the wire." |
| "Why this mask?" | "It matches every other interface on this wire, and the block size fits the hosts this LAN needs." |
| "Why this gateway?" | "It's the router's foot in this host's subnet — `gateway & mask == host & mask`." |
| "Why this route?" | "Router R must reach LAN X; the next hop toward X is neighbor N, whose foot on R's link is `N_IP`, so: `X/mask → N_IP`." |
| "Why a default here?" | "All non-local traffic exits this router by the same link, so one `0.0.0.0/0 → next` covers it." |
| "Could the IP differ?" | "Yes — any usable host in the subnet. I chose `.X` because [middle of range / gateway took the edge]." |

---

## 9. Vocabulary (for the oral)

| Term | One line |
|---|---|
| Subnet mask | 32-bit value, N leading 1s marking the network portion. |
| CIDR | `a.b.c.d/n` — address + prefix length. |
| Network address | First address (host bits all 0). Not assignable. |
| Broadcast | Last address (host bits all 1). Reaches all hosts on the subnet. |
| Default gateway | Next-hop for any destination not directly connected. |
| Longest-prefix match | Most-specific matching route wins. |
| Default route | `0.0.0.0/0` — matches all, lowest priority. |
| RFC 1918 | Private ranges: `10.x`, `172.16–31.x`, `192.168.x`. |

---

> **Slow is smooth, smooth is fast.** Draw the topology, compute the subnets, trace both directions, then explain. The green lights follow the reasoning — not the other way around.

→ Deeper dives: [`03_SUBNETTING.md`](03_SUBNETTING.md) · [`04_ROUTING.md`](04_ROUTING.md) · per-level notes in [`levels/`](levels/).
