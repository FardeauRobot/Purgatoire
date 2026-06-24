# Level 7 — Two routers in series: carve one block into subnets

> Randomized per student. Representative, self-verified figures below. This is the hardest *design* level — take it slowly.

---

## What this level looks like

Host A — Router 1 — Router 2 — Host C, three wires in a chain:

```
Host A ── A1 ─[ R1 ]─ R12 ──── R21 ─[ R2 ]─ R22 ── C1 ── Host C
        (link 1)   (link 2 = router-to-router)   (link 3)
```

Goal: **A ↔ C.** Everything is editable (IPs and masks), and the level ships every interface in the **same** `/24` (e.g. `100.198.14.x`). That's the whole problem.

---

## The concept to grasp: subnetting one address block

Three separate wires need **three distinct subnets**. And a router **cannot have two feet in the same subnet** (R1's `R11` and `R12`; R2's `R21` and `R22`). With everything left at `/24` they'd all collapse into one subnet on three different wires — illegal and broken.

So you **carve `100.198.14.0/24` into non-overlapping pieces** by lengthening the mask. A `/26` (block size 64) gives four subnets: `.0`, `.64`, `.128`, `.192` — more than enough.

> Two fixed IPs constrain the carving: `R11 = .1` and `R12 = .254`. Choose blocks that contain them, and that keep R1's two feet apart.

---

## Worked instance (representative numbers, all `/26`)

Fixed-IP anchors: `R11 = 100.198.14.1`, `R12 = 100.198.14.254` (masks editable). Everything else IP+mask editable.

| Wire | Subnet | Interfaces |
|------|--------|-----------|
| Link 1 (A–R1) | `100.198.14.0/26` (`.0–.63`) | `R11 = .1`, `A1 = .2` |
| Link 2 (R1–R2) | `100.198.14.192/26` (`.192–.255`) | `R12 = .254`, `R21 = .193` |
| Link 3 (R2–C) | `100.198.14.64/26` (`.64–.127`) | `R22 = .65`, `C1 = .66` |

Check the router feet: R1 has `.1` (in `.0/26`) and `.254` (in `.192/26`) → **different** ✓. R2 has `.193` (in `.192/26`) and `.65` (in `.64/26`) → **different** ✓. No subnet reused across wires ✓.

Set **every** involved interface's mask to `255.255.255.192` (`/26`), and the editable IPs as above.

### Build the routing tables

| Host/Router | Route | Gateway | Meaning |
|-------------|-------|---------|---------|
| A | `0.0.0.0/0` | `100.198.14.1` (R11) | all traffic → R1 |
| C | `0.0.0.0/0` | `100.198.14.65` (R22) | all traffic → R2 |
| R1 | `0.0.0.0/0` | `100.198.14.193` (R21) | unknowns → R2 |
| R2 | `0.0.0.0/0` | `100.198.14.254` (R12) | unknowns → R1 |

### Trace A → C

A → R11. R1: dest `.66` not in its connected `.0/26` or `.192/26` → default → R21 (`.193`). R2: `.66` is in its connected `.64/26` → delivers to C. Return path mirrors it. ✓

---

## Solution summary

1. Carve `…14.0/24` into `/26` blocks; give each of the **three wires its own block**.
2. Keep each router's two feet in **different** blocks (the fixed `.1` and `.254` already sit in different `/26`s).
3. Set every interface mask to `/26` and pick distinct host IPs.
4. Default-route A→R1, C→R2, and point the two routers at **each other** so unknown destinations flow down the chain.

---

## Traps

- **Leaving masks at `/24`.** Then all three wires are "the same subnet" → routers reject two feet in one subnet, packets never leave.
- **Reusing a block.** Each wire must be a *different* subnet; don't put link 1 and link 3 both on `.0/26`.
- **Forgetting a router's route.** R1 knows its two local subnets but not C's — it needs the route toward R2 (and R2 toward R1).
- An interface IP that lands on its block's network/broadcast (`.0`, `.63`, `.64`, `.127`, …).

---

## Checklist

- [ ] N wires ⇒ N distinct subnets — carve the block by lengthening masks.
- [ ] A router's interfaces are always in **different** subnets.
- [ ] Honour fixed IPs when choosing block boundaries.
- [ ] Each router needs a route toward every network it isn't directly attached to.
- [ ] Verify forward **and** return paths end-to-end.
