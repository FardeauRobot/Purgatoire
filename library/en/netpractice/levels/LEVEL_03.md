# Level 3 — Three hosts on one switch: same subnet for everyone

> NetPractice randomizes the numbers per student. The **layout** and the **lessons** are stable; the exact octets you see will differ. The worked figures below are one concrete instance.

---

## What this level looks like

Three hosts, all plugged into a single **switch**, and every host must reach every other host:

```
        Host_B ── B1
                    │
Host_C ── C1 ──── Switch_1 ──── A1 ── Host_A
```

Goals: **A↔B, A↔C, B↔C** — full mesh, everyone talks to everyone.

---

## The one concept to grasp: a switch is transparent

A **switch works at layer 2** — it just copies frames to every port (you can see this in the *Get my config* trace: *"on switch S: pass to all connections"*). It has **no IP, no routing, no subnet logic.** In the source it's literally `0.0.0.0/32`, `type: hidden`.

The consequence is the whole level:

> Every interface connected to the **same switch** is on the **same wire**. So **all of them must share one subnet** — identical mask, and `IP AND mask` giving the same network address — exactly like two hosts on a cable, just with N hosts instead of 2.

This is the first level where **more than two** interfaces sit in one broadcast domain. The rule doesn't change; it just applies three times at once.

---

## Worked instance (the screenshot)

| Interface | IP | Mask | Editable |
|-----------|----|------|----------|
| A1 | `104.198.158.125` | `255.255.255.128` (`/25`) | **mask only** (IP locked) |
| B1 | `104.198.158.126` | `255.255.255.128` (`/25`) | **IP + mask** (both free) |
| C1 | `104.198.158.124` | `255.255.255.128` (`/25`) | **IP only** (mask locked) |

### Find the anchors (the locked fields)

Two fields can't move — they define the answer:
- **A1 IP is locked** at `104.198.158.125`
- **C1 mask is locked** at `/25` (`255.255.255.128`)

So the subnet *must* be the `/25` that contains `.125`. `/25` → block size `256 − 128 = 128`. `.125` is in the **0** block:

```
Network   : 104.198.158.0
First host: 104.198.158.1
Last host : 104.198.158.126
Broadcast : 104.198.158.127
```

### Fit the other two into it

- **A1 mask** → `255.255.255.128` (it started at `/24`; must match the others' `/25`).
- **C1 IP** → any host `.1`–`.126`, distinct from the rest → `104.198.158.124`.
- **B1** started as `127.168.42.42 / 255.255.0.0` — a *completely different* network (the trap). Drag it into the shared subnet: IP `104.198.158.126`, mask `255.255.255.128`.

Final state: all three on `104.198.158.0/25`, masks all `/25`, hosts `.124 / .125 / .126` — distinct, none is `.0` or `.127`. Full mesh works.

---

## Solution summary

| Field | Action | Why |
|-------|--------|-----|
| A1 mask | `/24` → `255.255.255.128` | masks on one wire must be **identical**; C1's `/25` is locked |
| B1 IP | `127.168.42.42` → `104.198.158.126` | wrong network entirely → move into the shared `/25` |
| B1 mask | `255.255.0.0` → `255.255.255.128` | match the others |
| C1 IP | → `104.198.158.124` | a usable host in `…158.0/25`, distinct from A and B |

---

## Traps that bite here

- **Mismatched masks break it even when the network looks right.** If A1 stays `/24` while C1 is `/25`, they disagree on where the subnet ends → no communication. The masks must be *the same*.
- A switch port shows an IP field in some versions — **ignore it**, the switch is L2 (here it's hidden `0.0.0.0/32`).
- Don't hand `.0` or `.127` to a host — network and broadcast of this `/25`.

---

## Checklist to internalize (defend at eval)

- [ ] A switch is **layer 2 / transparent** — no IP, no subnet of its own.
- [ ] **All hosts on one switch = one subnet**: same mask + same `IP AND mask`.
- [ ] Solve around the **locked fields** — they pin down the subnet, the rest must conform.
- [ ] `/25` → 128-block, **126 usable**, broadcast at `.127`. (full table in [`../07_EVAL_CHEATSHEET.md`](../07_EVAL_CHEATSHEET.md))
