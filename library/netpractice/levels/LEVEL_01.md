# Level 1 — Two hosts on a wire, mind the mask width

> NetPractice randomizes numbers per student. The **layout** and **lessons** are stable; exact octets differ. The figures below are representative, verified to form a consistent solution — not your literal screen.

---

## What this level looks like

Two independent two-host wires (no router, no switch):

```
my PC ── A1 ─────── B1 ── my little brother's computer     (Goal 1: A ↔ B)
my Mac ── C1 ─────── D1 ── my little sister's computer       (Goal 2: C ↔ D)
```

Each goal is solved in isolation. The twist vs. later levels: the two pairs use **different mask widths** (`/24` on the left, `/16` on the right), so the "which octets are the host part" question is front and centre.

---

## The one concept to grasp

> Two interfaces on the same wire talk **only if** `IP AND mask` gives the **same network address**, with the **same mask**.

The mask tells you **which octets are fixed (network) and which are free (host)**:

- `/24` = `255.255.255.0` → first **three** octets are the network, last octet is the host. Same subnet ⇔ first three octets identical.
- `/16` = `255.255.0.0` → first **two** octets are the network, last **two** are host. Same subnet ⇔ first two octets identical.

---

## Worked instance (representative numbers)

| Interface | IP | Mask | Editable |
|-----------|----|------|----------|
| A1 | `104.93.23.50` | `255.255.255.0` (`/24`) | **IP only** |
| B1 | `104.94.23.12` | `255.255.255.0` (`/24`) | locked |
| C1 | `211.191.30.75` | `255.255.0.0` (`/16`) | locked |
| D1 | `211.190.40.42` | `255.255.0.0` (`/16`) | **IP only** |

### Left pair (A ↔ B, `/24`)

B1 is locked at `104.94.23.12/24` → its network is `104.94.23.0`. A1 currently reads `104.93.23.50` → network `104.93.23.0`. **The third octet differs (`93` vs `94`)** → different subnets → no talk.

Fix: drag A1 into B1's `/24`. Set **A1 → `104.94.23.11`** (any `104.94.23.1`–`.254` except `.12`; avoid `.0`/`.255`).

### Right pair (C ↔ D, `/16`)

C1 is locked at `211.191.30.75/16` → network `211.191.0.0` (only first two octets count). D1 reads `211.190.40.42` → network `211.190.0.0`. **Second octet differs (`191` vs `190`)**.

Fix: set **D1 → `211.191.40.42`** (second octet must be `191`; the third/fourth are free host bits). Avoid the all-zero-host `211.191.0.0` and all-ones `211.191.255.255`.

---

## Solution summary

| Field | From | To | Why |
|-------|------|----|-----|
| A1 IP | `104.93.23.50` | `104.94.23.11` | match B1's `/24` (first 3 octets) |
| D1 IP | `211.190.40.42` | `211.191.40.42` | match C1's `/16` (first 2 octets) |

---

## Traps

- **Don't change the wrong octet.** Under `/16`, fixing the third octet does nothing — only the first two define the subnet.
- Network (`host bits all 0`) and broadcast (`host bits all 1`) are never valid host IPs.
- When only the IP is editable, you have **no choice of subnet** — you must conform to the locked partner.

---

## Checklist

- [ ] The mask says which octets are network vs host — read it first.
- [ ] `/24` → match 3 octets; `/16` → match 2 octets; `/8` → match 1 octet.
- [ ] Edit the unlocked field to land inside the locked partner's subnet.
- [ ] Avoid network/broadcast.
