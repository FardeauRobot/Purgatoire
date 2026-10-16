# Level 8 — Two routers + Internet: masks, local gateways, return paths

> Randomized per student. This note is **method-first**: the exact octets vary and there's no green-OK oracle here, so it teaches *how to find and fix* the broken fields rather than handing you numbers. The structural bugs below are real and stable across instances.

---

## What this level looks like

```
office (C) ── C1 ── R22 [ R2: transit ] R23 ── D1 ── home (D)
                          R21
                           │
                          R13 [ R1: gate ] R12 ── I1 [ Internet ] ── 8.8.8.8
                           (10.0.0.x link)   (163.x.250.x link)
```

Goals: **C↔D, C↔Internet, D↔Internet.** Three things must hold at once, so work the diagnostic in order.

---

## Diagnostic order (use this every multi-router level)

1. **Every link: same subnet + same mask on both ends.**
2. **Every host: default gateway = the local router foot** (an IP on the host's own subnet).
3. **Every router: a route toward each network it isn't directly attached to**, plus a default toward the Internet.
4. **The Internet: a return route back to each LAN.**
5. **Walk each goal forward and back**, hop by hop.

---

## The bugs planted in this level (find these)

### Mask/subnet mismatches on a link
- **`R13` and `R21`** (the R1↔R2 link, the `10.0.0.x` wire) ship with **different masks** (e.g. `/24` vs `/16`). Two ends of one wire must share the mask → unify them (e.g. both `255.255.255.0`, `R13=10.0.0.1`, `R21=10.0.0.2`).
- **`R23` and `D1`** (the home link): `D1`'s mask is **locked** (e.g. `/28`) and its IP fixes the subnet (e.g. `7.9.10.0/28`). `R23` ships in a *different* network with a *different* mask → drag `R23` into D1's subnet with D1's mask (e.g. `7.9.10.1/28`, avoiding D1's own IP and `.0`/`.15`).

> The `R22`↔`C1` link (e.g. `192.168.0.x/24`) and the `R12`↔`I1` link (e.g. `163.…250.0/28`) usually ship **already consistent** — verify, don't blindly change.

### Wrong host gateways (default routes pointing nowhere local)
- **C's default route** ships via something like `10.0.0.254` — but C lives on the `192.168.0.x` wire, so its gateway **must be `R22`** (e.g. `192.168.0.254`). A gateway not on the host's subnet is unreachable.
- **D's default route** ships via a bogus address (e.g. `9.9.9.9`). Point it at **`R23`** (D's local foot, after you fix R23 above).

### Routes between the routers and to/from the Internet
- **R1** needs: the route to reach the C/D LANs **via R2** (next hop = R2's foot on the `10.0.0.x` link), and its **default** `0.0.0.0/0` toward the Internet gateway (often shipped fixed — keep it).
- **R2** needs a **default** route toward R1 so C and D can reach the Internet.
- **The Internet's return route** must cover the LANs (C's and D's networks) via R1's Internet-facing foot — check its mask is wide enough.

---

## Why each rule bites here

- **Mask must match on a link:** if one end thinks `/24` and the other `/16`, they disagree on who's "local" → forwarding breaks even though the network numbers look related.
- **Gateway must be local:** a host can only physically hand a packet to something on its own wire. `10.0.0.254` is not on C's `192.168.0.x` wire.
- **Return path:** `C↔Internet` is two goals' worth of direction. The Internet must know a route back to `192.168.0.0/…` and to D's network, or replies vanish.

---

## Checklist

- [ ] Walk **every link** and confirm identical mask + same network on both ends.
- [ ] Each host's gateway is an IP **on its own subnet**, owned by the attached router.
- [ ] Each router can reach every LAN (direct or via a route) **and** the Internet (default).
- [ ] The Internet has a **return route** to each LAN, with a wide-enough mask.
- [ ] Trace all three goals forward **and** back before clicking Check.
