# Level 5 — First real routing: the default gateway

> Randomized per student. Representative, self-verified figures below.

---

## What this level looks like

One router, two hosts, **on two different subnets**:

```
Machine A ── A1 ──[ R1 | The Mighty Router | R2 ]── B1 ── Machine B
              (subnet 1)                    (subnet 2)
```

Goals: **A↔R, B↔R, A↔B.** A and B are now in *different* subnets, so for the first time a host must send packets to something **not on its own wire** — it needs a **route**.

---

## The concept that makes this level (and the rest of NetPractice)

A host only knows how to reach its **own** subnet directly. For anything else it consults its **routing table**:

```
destination/mask    →    gateway (next hop)
```

The **default route** `0.0.0.0/0` means "everything not local → send to this gateway." The single hard rule:

> **The gateway must be an IP on the host's own subnet, and it must belong to the router.**

A packet can't teleport to a gateway in another subnet — the host has to be able to *hand it over the local wire* first.

Why no router routing table here? `R` is **directly connected** to *both* subnets, so it already knows how to reach each. Only the two **hosts** need a route pointing at their local router foot.

---

## Worked instance (representative numbers)

Fixed anchors (router masks/IPs locked):

| Interface | IP | Mask | Subnet |
|-----------|----|------|--------|
| R1 | `42.10.20.126` | `255.255.255.128` (`/25`) | `42.10.20.0/25` → hosts `.1–.126` |
| R2 | `150.30.40.254` | `255.255.192.0` (`/18`) | `150.30.0.0/18` → `150.30.0.1–150.30.63.254` |

Editable: A1 (IP+mask), B1 (IP+mask), A's route, B's route gateway.

### Step 1 — put each host in its router foot's subnet

- **A1** (linked to R1) → IP `42.10.20.1`, mask `255.255.255.128` (`/25`). In `42.10.20.0/25` ✓, not `.0`/`.127`.
- **B1** (linked to R2) → IP `150.30.0.1`, mask `255.255.192.0` (`/18`). In `150.30.0.0/18` ✓.

### Step 2 — give each host a default route via its local router foot

- **A's route** → `default` (`0.0.0.0/0`), gateway **`42.10.20.126`** (= R1, which is on A's subnet ✓).
- **B's route** → `default`, gateway **`150.30.40.254`** (= R2, on B's subnet ✓ — `.40` falls inside `150.30.0.0/18`).

### Trace A → B

A: dest `150.30.0.1` is not local → default route → hand to `42.10.20.126` (R1). R is directly connected to `150.30.0.0/18` → delivers to B. Reverse path symmetric via B's default route. ✓

---

## Solution summary

| Field | Set to | Why |
|-------|--------|-----|
| A1 | `42.10.20.1 /25` | join R1's subnet |
| B1 | `150.30.0.1 /18` | join R2's subnet |
| A route | `default` via `42.10.20.126` | reach anything non-local through R1 |
| B route | `default` via `150.30.40.254` | reach anything non-local through R2 |

---

## Traps

- **Gateway in the wrong subnet.** A's gateway *must* be `42.10.20.x` (R1's side). Pointing A at R2's IP is unreachable — it's not on A's wire.
- The default-route placeholders the level ships with (e.g. `192.168.0.254`) are **decoys** — they belong to no interface here.
- Watch the odd masks: `/18` means the subnet spans `150.30.0.0`–`150.30.63.255`, so `150.30.40.254` *is* inside it.

---

## Checklist

- [ ] A host reaches non-local subnets only via a **route**.
- [ ] `0.0.0.0/0` (default) = "everything else → this gateway."
- [ ] **Gateway must live on the host's own subnet** and belong to the router.
- [ ] A directly-connected router needs no manual route for those subnets.
- [ ] Every `reach` goal is **bidirectional** — check the return path exists too.
