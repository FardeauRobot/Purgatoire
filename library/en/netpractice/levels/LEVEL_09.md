# Level 9 — Capstone: switch + two routers + Internet, six goals

> Randomized per student. **Method-first** note — large topology, pre-filled routing tables, no green-OK oracle. Learn the procedure; the numbers are yours to verify.

---

## What this level looks like

```
A (meson) ─┐
           ├─ Switch ── R11 [ R1: proton ] R12 ── I1 [ Internet ]
B (ion) ───┘                 R13
                              │
                             R21 [ R2: boson ] R22 ── C (cation)
                                   R23 ── D (gluon)
```

Goals: **A↔B, C↔D, A↔Internet, A↔D, B↔C, C↔Internet** — six paths crossing both routers and the switch.

---

## What's new vs. earlier levels

- **A and B share a switch** → they must be in the **same subnet** as each other *and* as R1's switch-facing foot (`R11`). (Level 3 idea, now embedded.)
- **The routers ship with multiple pre-filled routes**, some fixed, some editable. You're repairing a routing *table*, and **longest-prefix match** decides which entry wins.
- Six goals means several **return paths** to keep consistent at once.

---

## Longest-prefix match (the new rule to internalise)

When several routes match a destination, the router uses the **most specific** one — the **longest mask**. Example table:

```
10.0.0.0/27   via X     ← wins for 10.0.0.5 (matches, /27 is longer)
10.0.0.0/8    via Y
0.0.0.0/0     via Z      ← only used when nothing else matches
```

So a `0.0.0.0/0` default is the *fallback*; a specific `…/27` overrides it for its range. When you add or fix a route, make sure its mask is specific enough to win where you need it, and that you're not accidentally shadowed by a broader fixed entry.

---

## Diagnostic order

1. **Switch subnet first.** Put A, B, and R11 in one subnet (same mask, distinct host IPs, none on network/broadcast). A's and B's masks ship mismatched (`/24` vs `/16`) — unify to R11's mask.
2. **Each point-to-point link:** R13↔R21, R22↔C1, R23↔D1 — same subnet + same mask on both ends. Several ship with a deliberately wrong mask or a foreign network.
3. **Each host's default gateway** = its local router foot (on its own subnet). Fix any pointing elsewhere.
4. **Router tables:** confirm each router has a path to every destination network — directly connected, via a specific route, or via default. Respect the **fixed** entries (often the Internet default and the LAN-return routes) and fill the **editable** gateways to match real interfaces.
5. **Internet return routes** must cover the LAN networks the goals require (A's, C's), via R1's Internet foot — with a mask wide enough to include them.
6. **Trace all six goals forward and back.**

---

## Recurring traps (the families that appear here)

- **Switch members with different masks.** A `/24` and a `/16` on the same switch disagree on subnet size → broken even with matching network octets.
- **A gateway that isn't on the host's wire.** Always an instant fail; check each host's route first.
- **A specific fixed route shadowing your default**, or your too-broad route shadowing a needed specific one — read the *whole* table, longest-prefix wins.
- **One-directional success.** A goal `X↔Y` needs Y→X to work too; a missing return route on a far router or the Internet leaves it KO.
- **Network/broadcast as a host IP** in the smaller subnets (`/26`, `/27`, `/30`).

---

## Checklist

- [ ] A, B, R11 in **one** switch subnet, identical mask.
- [ ] Every link: same subnet + same mask both ends.
- [ ] Every host gateway is a **local** router foot.
- [ ] Each router reaches every needed network; **longest-prefix** picks the right entry.
- [ ] Internet return routes cover the LANs, wide enough mask.
- [ ] All six goals verified **both directions**.
