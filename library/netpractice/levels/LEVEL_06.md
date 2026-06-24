# Level 6 — Reaching the Internet (and the return route)

> Randomized per student. Representative, self-verified figures below.

---

## What this level looks like

A host, its router, and the **Internet** as a destination:

```
webserv ── A1 ── Switch ── R1 [ gate ] R2 ── I1 [ Internet ] ── 8.8.8.8
           (your LAN)                  (the link to the ISP)   ("Somewhere on the Net")
```

Goal: **A reaches `8.8.8.8`** ("Somewhere on the Net").

---

## The concepts to grasp

1. **The Internet is just "every subnet you don't know about."** You reach it with a **default route** `0.0.0.0/0` pointing at the next router toward the ISP.
2. **Reachability is bidirectional.** A packet reaching `8.8.8.8` is useless if the reply can't come back. So the **Internet itself needs a route back to your LAN**. This is the trap that makes Level 6 more than "add a default route."

The chain of trust:
```
A  --default-->  R1  --default-->  Internet gateway   (forward)
Internet  --route to A's subnet-->  R2  -->  A          (return)
```

---

## Worked instance (representative numbers)

Fixed anchors:

| Interface | IP | Mask | Note |
|-----------|----|------|------|
| R1 | `50.60.70.254` | `255.255.255.128` (`/25`) | mask locked; your LAN's gateway |
| R2 | `163.172.250.12` | `255.255.255.240` (`/28`) | locked; faces the Internet |
| I1 (Internet) | `163.172.250.1` | `/28` | locked; the ISP gateway |
| A1 | `50.60.70.227` | mask editable | IP locked at `.227` |

R1 `.254 //25` → LAN subnet is **`50.60.70.128/25`** (hosts `.129–.254`). `R2` and `I1` share `163.172.250.0/28` ✓ (already correct, that's the LAN↔ISP link).

### The four edits

1. **A1 mask** → `255.255.255.128` (`/25`). `A1 = .227` then sits in `50.60.70.128/25` with R1 ✓.
2. **A's default route** → `0.0.0.0/0` via **`50.60.70.254`** (R1, on A's subnet). *(The shipped placeholder gateway `…​.1` is wrong — it isn't R1.)*
3. **R1's route** → change `10.0.0.0/8` to **`0.0.0.0/0`** (default), gateway `163.172.250.1` (the Internet gateway I1, fixed). Now R1 forwards Internet-bound traffic to the ISP.
4. **The Internet's return route** → it ships as `50.60.70.0/31 via 163.172.250.12`, which **does not cover your LAN** `50.60.70.128/25`. Widen it to **`50.60.70.128/25`** (or `50.60.70.0/24`) via `163.172.250.12` (R2). Now replies find their way back.

### Trace

Forward: A → R1 (`.254`) → R1 default → `163.172.250.1` (Internet) → `8.8.8.8`.
Return: `8.8.8.8` → Internet → route `50.60.70.128/25` via `163.172.250.12` (R2) → R1 → A. ✓

---

## Solution summary

| Field          | Set to                                 | Why                                       |
| -------------- | -------------------------------------- | ----------------------------------------- |
| A1 mask        | `/25`                                  | join R1's LAN subnet                      |
| A route        | `0.0.0.0/0` via `50.60.70.254`         | send Internet traffic to local gateway R1 |
| R1 route       | `0.0.0.0/0` via `163.172.250.1`        | R1 forwards to the ISP                    |
| Internet route | `50.60.70.128/25` via `163.172.250.12` | **return path** to your LAN               |

---

## Traps

- **Forgetting the return route.** Forward-only config still shows KO — the reply dies at the Internet. This is *the* lesson of Level 6.
- **Too-narrow return route.** `/31` covers 2 addresses; your `/25` LAN has 126. The Internet's route must *contain* your whole subnet.
- A's gateway must be R1's LAN IP, not the placeholder.

---

## Checklist

- [ ] Internet = default route `0.0.0.0/0`.
- [ ] Host default → local router; router default → ISP gateway.
- [ ] **The Internet needs a route back to your subnet** (right network *and* wide-enough mask).
- [ ] Every hop forward needs a matching hop back.
